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

#ifndef UIEVENTS_H__REMOTEPC__INCLUDED_
#define UIEVENTS_H__REMOTEPC__INCLUDED_

namespace RemotePC
{
	class UIEvents
		: public mc::CommonImpl< IUIEvents >
	{
	public:

		UIEvents();

		/**
		 * IUIEvents section
		 */
		
        virtual void OnConnected();
        
        virtual void OnApproved();
        
        virtual void OnPasswordRequested(bool wrongPass);
        
        virtual void OnError(const std::string& message);   
        
        virtual void OnDisconnected();        
        
		virtual void AttachGLContext();
		
		virtual void ShowScreenshot(mc::IMessageRef message);
        
        virtual void UpdateVolume(unsigned long volume);    
		
		virtual void WarnOnOldRemotePC();
        
        virtual void ShowShellResponse(const std::string& response);
		
		virtual void UpdateZoomParams(float minZoom, float maxZoom, float currentZoom);	
	};
}

#endif // !UIEVENTS_H__REMOTEPC__INCLUDED_
