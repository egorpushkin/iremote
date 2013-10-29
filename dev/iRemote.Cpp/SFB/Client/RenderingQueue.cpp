/**
 * Copyright (c) 2013 Egor Pushkin. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Common/Common.h"

#include "Matrix.h"
#include "Texture.h"
#include "Cursor.h"
#include "Surface.h"
#include "IFBEvents.h"
#include "RenderingQueue.h"

#include "../Messages/BlockMessage.h"
#include "../Messages/BatchHeader.h"

namespace MinCOM
{	
    // {56CB6EB0-BF19-46B5-A380-725EA8CAA07D}
    template<> const Guid TypeInfo< RemotePC::SFB::RenderingQueue >::iid_ =
		{ 0x56CB6EB0, 0xBF19, 0x46B5, { 0xA3, 0x80, 0x72, 0x5E, 0xA8, 0xCA, 0xA0, 0x7D } };	
}

namespace RemotePC
{
namespace SFB
{
		
	RenderingQueue::RenderingQueue()
		: mc::CommonImpl< mc::IRunnable >()
		, events_()
		, thread_()
	    , modeGuard_( mc::Library::Mutex() )
	    , mode_(MODE_COLLECTING)	
	    , bufferGuard_( mc::Library::Mutex() )	
		, bufferMatrix_()
		, batchSize_(0)
		, blocksReceived_(0)	
		, displayGuard_( mc::Library::Mutex() )
		, displayMatrix_()
	    , renderTasks_( mc::Library::Semaphore(0, std::numeric_limits< long >::max()) ) 
		, cancelled_(false)
	{
	}
	
	void RenderingQueue::start(IFBEventsRef events, SurfaceRef surface)
	{
		mc::MutexScope locker( modeGuard_ );
		
		if ( thread_ )
			return;
		
		// Remember new surface and events.
		events_ = events;
		surface_ = surface;
				
		// Create matrixes.
		size_t width = surface_->blockWidth();
		size_t height = surface_->blockHeight();		
		size_t cellSize = surface_->textureSize();
		{			
			mc::MutexScope locker( bufferGuard_ );			
			bufferMatrix_ = MatrixPtr(new Matrix(width, height, cellSize));
		}
		{			
			mc::MutexScope locker( displayGuard_ );			
			displayMatrix_ = MatrixPtr(new Matrix(width, height, cellSize));
		}
		
		cancelled_ = false;
		
		// Start worker thread.
		thread_ = mc::Library::Thread();
		thread_->SetContext(GetSelf());
		thread_->Start();
	}
	
	void RenderingQueue::stop()
	{
		mc::MutexScope locker( modeGuard_ );
		
		if ( !thread_ )
			return;		 
		
		cancelled_ = true;
		renderTasks_->Release(1);
		thread_->Join();
	}
	
	void RenderingQueue::handle(mc::IMessageRef message)
	{
		mc::Guid messageId = message->GetCode();
		
		if ( BatchHeader::messageId_ == messageId )
		{
			startBatch(message->GetProperty(BatchHeader::PROP_COUNT).toUInt());
		}
		else if ( BlockMessage::messageId_ == messageId )
		{
			receiveBlock(message);
			
			// Check whether batch is ready.
			if ( batchSize_ == blocksReceived_ )
				swapMatrixes();
		}		
	}
	
	void RenderingQueue::startBatch(size_t size)
	{
		mc::MutexScope locker( bufferGuard_ );
		batchSize_ = size;
		blocksReceived_ = 0;		
	}
	
	void RenderingQueue::receiveBlock(mc::IMessageRef message)
	{
		mc::MutexScope locker( bufferGuard_ );	
		
		++blocksReceived_;
		
		size_t blockSize = surface_->textureSize();
		float x = message->GetProperty(BlockMessage::PROP_X).toFloat();
		float y = message->GetProperty(BlockMessage::PROP_Y).toFloat();			
		size_t blockX = x / blockSize;
		size_t blockY = y / blockSize;
		
		Matrix::Cell_& cell = bufferMatrix_->cells()[blockY][blockX];
		cell.message_ = message;
		cell.modified_ = true;		
	}
	
	// Exchange tools
	////////////////////////////////////////////////////////////////////////////////
	
	void RenderingQueue::swapMatrixes()
	{
		mc::MutexScope locker1( bufferGuard_ );
		mc::MutexScope locker2( displayGuard_ );
		
		if ( ( batchSize_ == blocksReceived_ )
			&& ( 0 != batchSize_ )
			&& ( 0 == displayMatrix_->changes().size() ) )
		{
			// Update changes list.
			bufferMatrix_->buildChanges();
			// There is nothing to do, if no changes were found.
			if ( !bufferMatrix_->isChanged() )
				return;
			// Perform swap.
			std::swap(bufferMatrix_, displayMatrix_); 
			// Wake up background processing.
			renderTasks_->Release(displayMatrix_->changes().size());
		}		
	}
	
	// Mode tools
	////////////////////////////////////////////////////////////////////////////////
		
	void RenderingQueue::block()
	{
		mc::MutexScope locker( modeGuard_ );
		mode_ = MODE_BLOCKED;
	}
	
	void RenderingQueue::unblock()
	{		
		mc::MutexScope locker( modeGuard_ );
		mode_ = MODE_COLLECTING;
	}
	
	bool RenderingQueue::testBlocked()
	{
		mc::MutexScope locker( modeGuard_ );
		return ( MODE_BLOCKED == mode_ );
	}
	
	// Rendering tools
	////////////////////////////////////////////////////////////////////////////////
	
	// IRunnable section
	mc::result RenderingQueue::Run()
	{
		// There is no need in attaching context to current thread, as 
		// all OpenGL commands are executed on main thread only.
		// Register GL context to render textures from background. 		
		// events_->attachGLContext();
		
		while ( !cancelled_ )
		{
			// Wait for tasks.
            if ( mc::Error::IsFailed(renderTasks_->Wait()) )
                break;	
			
			// Check cancellation.
			if ( cancelled_ )
				break;
			
			// Check blocking.
			if ( testBlocked() )
			{
				// Increment count of pending rendering tasks, as it was decremented by
				// Wait routine.
				renderTasks_->Release(1);
				// Sleep for a second to avoid hangs.
				mc::IThread::Sleep(1000);
				continue;
			}
			
			// Extract next rendering block.
			{
				mc::MutexScope locker( displayGuard_ );				
				
				if ( 0 == displayMatrix_->changes().size() )
					continue;
				
				Matrix::CellLocation_ &blockLocation = displayMatrix_->changes().front();
				
				renderBlock(blockLocation.x_, blockLocation.y_);
				
				displayMatrix_->changes().erase(displayMatrix_->changes().begin());
				
				if ( 0 == displayMatrix_->changes().size() )
					// Update view.
					events_->update();
			}
		}
		
		return mc::_S_OK;
	}
	
	void RenderingQueue::renderBlock(size_t blockX, size_t blockY)
	{
		Matrix::Cell_ &block = displayMatrix_->cells()[blockY][blockX];
		IScreenshotMessagePtr blockMessage(block.message_);
		Texture *texture = surface_->texture(blockX, blockY);
		texture->updateFromData((unsigned char *)blockMessage->GetData(), blockMessage->GetDataLength());
		
		block.message_ = NULL;
		block.modified_ = false;
	}
	
}
}
