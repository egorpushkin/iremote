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

#ifndef RENDERINGQUEUE_H__SFB__REMOTEPC__INCLUDED_
#define RENDERINGQUEUE_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{		
	class RenderingQueue
		: public mc::CommonImpl< mc::IRunnable >
	{
	public:
		
		RenderingQueue();
		
		void start(IFBEventsRef events, SurfaceRef surface);
		
		void stop();
		
		/**
		 * Should accept and handle both batch header and block message.
		 */
		void handle(mc::IMessageRef message);
		
		void block();
		
		void unblock();
		
		// IRunnable section
		virtual mc::result Run();
		
	private:
		
		bool testBlocked();
		
		void startBatch(size_t size);
		
		void receiveBlock(mc::IMessageRef message);
		
		void swapMatrixes();

		void renderBlock(size_t blockX, size_t blockY);
		
	private:
		
		IFBEventsPtr events_;
		
		SurfacePtr surface_;
		
		mc::IThreadPtr thread_;
		
		/** Protects mode property. */
		mc::IMutexPtr modeGuard_;			
		
		typedef enum tagMode_
		{
			MODE_BLOCKED = 0x1,
			MODE_COLLECTING = 0x2			
		}
		Mode_;
		
		Mode_ mode_;				
		
		/** Protects buffer matrix. */
		mc::IMutexPtr bufferGuard_;		
				
		MatrixPtr bufferMatrix_;
		
		size_t batchSize_;
		
		size_t blocksReceived_;
		
		/** Protects display matrix. */
		mc::IMutexPtr displayGuard_;				
		
		MatrixPtr displayMatrix_;
		
		mc::ISemaphorePtr renderTasks_;
		
		bool cancelled_;
		
	};
	
	typedef mc::Strong< RenderingQueue > RenderingQueuePtr;			
}
}

#endif // RENDERINGQUEUE_H__SFB__REMOTEPC__INCLUDED_