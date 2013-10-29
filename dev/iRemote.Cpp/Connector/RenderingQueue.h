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

#ifndef RENDERINGQUEUE_H__REMOTEPC__INCLUDED_
#define RENDERINGQUEUE_H__REMOTEPC__INCLUDED_

namespace RemotePC
{		
	class RenderingQueue
		: public mc::CommonImpl< mc::IRunnable >
	{
	public:
		
		RenderingQueue();
		
		void start(IUIEventsRef events);
		
		void stop();
		
		void handle(mc::IMessageRef message);
		
		// IRunnable section
		virtual mc::result Run();
		
	private:
		
		void handlePendingMessage();
		
		void setPriority();
		
	private:
		
		IUIEventsPtr events_;
		
		mc::IThreadPtr thread_;
				
		mc::IMutexPtr pendingGuard_;		
		
		mc::IMessagePtr pendingMessage_;
		
		mc::ISemaphorePtr decodingTasks_;
		
		bool cancelled_;
		
	};
	
	typedef mc::Strong< RenderingQueue > RenderingQueuePtr;			
}

#endif // RENDERINGQUEUE_H__REMOTEPC__INCLUDED_
