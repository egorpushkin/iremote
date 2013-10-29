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

#ifndef IUIEVENTS_H__REMOTEPC__INCLUDED_
#define IUIEVENTS_H__REMOTEPC__INCLUDED_

namespace RemotePC
{

	interface IUIEvents : public mc::ICommon
	{
        
        virtual void OnConnected() = 0;
        
        virtual void OnApproved() = 0;
        
        virtual void OnPasswordRequested(bool wrongPass) = 0;
        
        virtual void OnError(const std::string& message) = 0;
        
        virtual void OnDisconnected() = 0;
        
		virtual void AttachGLContext() = 0;
		
		virtual void ShowScreenshot(mc::IMessageRef message) = 0;		
        
        virtual void UpdateVolume(unsigned long volume) = 0;
		
		virtual void WarnOnOldRemotePC() = 0;
        
        virtual void showShellResponse(const std::string& response) = 0;
		
		virtual void updateZoomParams(float minZoom, float maxZoom, float currentZoom) = 0;

	};

	typedef mc::Strong< IUIEvents > IUIEventsPtr;
	typedef const IUIEventsPtr& IUIEventsRef;

}

#endif // !IUIEVENTS_H__REMOTEPC__INCLUDED_
