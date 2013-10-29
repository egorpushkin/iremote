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

#ifndef EVENTS_H__REMOTEPC__INCLUDED_
#define EVENTS_H__REMOTEPC__INCLUDED_

/** 
 * Preliminary declaration of main application UI class.
 * It dispatches all events from server component.
 */
class Dialog;

namespace RemotePC
{

	class Events
		: public mc::CommonImpl< IUIEvents >
	{
	public:

		Events(Dialog * window);

		// IUIEvents section
		virtual void ServiceStarted();

		virtual void FailedToStartService();

		virtual void ProveAuthentication(size_t cookie);

		virtual mc::ISemaphorePtr GetAuthSem() const;

		virtual void OnNewClient(size_t cookie);

		virtual void CheckAuth();
		
		virtual void OnDisconnection(size_t cookie);

		virtual void OnOldiRemote(size_t cookie);

	private:
	
		Dialog * window_;

		mc::ISemaphorePtr authSem_;
	
	};

}

#endif // !EVENTS_H__REMOTEPC__INCLUDED_
