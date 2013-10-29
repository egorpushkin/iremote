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
#include "IUIEvents.h"
#include "RenderingQueue.h"

#include <boost/thread/thread.hpp>
#include <unistd.h>
#include <sched.h>

namespace MinCOM
{
	
    // {8F07BE8C-AA58-4566-9E20-D92C401F3268}
    template<> const Guid TypeInfo< RemotePC::RenderingQueue >::iid_ =
		{ 0x8F07BE8C, 0xAA58, 0x4566, { 0x9E, 0x20, 0xD9, 0x2C, 0x40, 0x1F, 0x32, 0x68 } };
	
}

namespace RemotePC
{
		
	RenderingQueue::RenderingQueue()
		: mc::CommonImpl< mc::IRunnable >()
		, events_()
		, thread_()
	    , pendingGuard_( mc::Library::Mutex() )
		, pendingMessage_()
	    , decodingTasks_( mc::Library::Semaphore(0, std::numeric_limits< long >::max()) ) 
		, cancelled_(false)
	{
	}
	
	void RenderingQueue::start(IUIEventsRef events)
	{
		events_ = events;
		
		cancelled_ = false;		
		thread_ = mc::Library::Thread();
		thread_->SetContext(GetSelf());
		thread_->Start();
	}
	
	void RenderingQueue::stop()
	{
		cancelled_ = true;		
		decodingTasks_->Release(1);
		thread_->Join();
	}
	
	void RenderingQueue::handle(mc::IMessageRef message)
	{
		mc::MutexScope locker( pendingGuard_ );
		mc::IMessagePtr prevMessage = pendingMessage_;
		pendingMessage_ = message;
		if ( NULL == prevMessage )
			decodingTasks_->Release(1);
	}
		
	// IRunnable section
	////////////////////////////////////////////////////////////////////////////////
	
	mc::result RenderingQueue::Run()
	{
		// Adjust background thread priority to avoid UI hangs due to decoding.
		setPriority();		
		
		// Register GL context to render textures from background. 		
		events_->AttachGLContext();
		
		while ( !cancelled_ )
		{
			// Wait for tasks.
            if ( mc::Error::IsFailed(decodingTasks_->Wait()) )
                break;	
			
			// Check cancellation.
			if ( cancelled_ )
				break;
			
			handlePendingMessage();
		}
		
		return mc::_S_OK;
	}
	
	void RenderingQueue::handlePendingMessage()
	{
		mc::IMessagePtr message;
		{
			mc::MutexScope locker( pendingGuard_ );
			message = pendingMessage_;
			pendingMessage_ = NULL;
		}
		if ( NULL == message )
			return;
		events_->ShowScreenshot(message);
	}
	
	void RenderingQueue::setPriority()
	{
		int priority = 0;
		int	policy;
		struct sched_param param;
		pthread_getschedparam(pthread_self(), &policy, &param);
		param.sched_priority = priority;
		pthread_setschedparam(pthread_self(), policy, &param);		
	}

}
