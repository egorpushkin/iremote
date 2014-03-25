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

#import <UIKit/UIKit.h>

#import "../Main/iRemoteAppDelegate.h"
#import "../Views/ControlViewController.h"
#import "../Controls/ControlArea.h"
#import "../../Tools/OpenGL/Texture.h"
#import "../Controls/ScreenshotProvider.h"
#import "../Controls/IndicatorsProvider.h"
#import "../../Tools/OpenGL/GLContext.h"

// Precompiled header.
#include "Common/Common.h"
// iRemote object model control.
#include "Connector/Server.h"
// Public interface.
#include "UIEvents.h"
// User interface configuration.
#include "../State/UIConfig.h"
// Screenshot messages.
#include "Messages/IScreenshotMessage.h"
#include "Messages/ScreenshotMessage.h"

namespace MinCOM
{
	// {E671318E-B04F-4387-B7FC-0AAF60CE7138}
	template<> const Guid TypeInfo< RemotePC::IUIEvents >::iid_ = 
		{ 0xe671318e, 0xb04f, 0x4387, { 0xb7, 0xfc, 0xa, 0xaf, 0x60, 0xce, 0x71, 0x38 } };
}

namespace RemotePC
{
	UIEvents::UIEvents()
		: mc::CommonImpl< IUIEvents >()
	{
	}

	// IUIEvents section
    void UIEvents::OnConnected()
    {
        [[iRemoteAppDelegate delegate] performSelectorOnMainThread:@selector(onConnected) withObject:nil waitUntilDone:NO];
    }
    
    void UIEvents::OnApproved()
    {
        [[iRemoteAppDelegate delegate] performSelectorOnMainThread:@selector(onApproved) withObject:nil waitUntilDone:NO];
    }
    
    void UIEvents::OnPasswordRequested(bool wrongPass)
    {
        // This magic was required to prevent popup appearance in the wrong place (in the middle of the screen).
        // It is not used any more due to forced popup dismissing with turned off animation.
        // [NSThread sleepForTimeInterval:0.8];
        
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];				
        [[iRemoteAppDelegate delegate] performSelectorOnMainThread:@selector(onPasswordRequested:) withObject:[NSNumber numberWithBool:wrongPass] waitUntilDone:NO];
		[pool release];
    }
    
    void UIEvents::OnError(const std::string& message)
    {
        // Perform cleanup after unsuccessful attempt.
        Holder::Instance().Stop();
        // Notify UI.
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];						
        [[iRemoteAppDelegate delegate] performSelectorOnMainThread:@selector(onError:) withObject:[NSString stringWithUTF8String:message.c_str()] waitUntilDone:NO];
		[pool release];
    }
    
    void UIEvents::OnDisconnected()
    {
        // Perform cleanup after unsuccessful attempt.
        Holder::Instance().Stop();
        // Notify UI.
        [[iRemoteAppDelegate delegate] performSelectorOnMainThread:@selector(onDisconnected) withObject:nil waitUntilDone:NO];
    }
	
	void UIEvents::AttachGLContext()
	{
		[[GLContext instance] setBackgroundContext];
	}
	
	void UIEvents::ShowScreenshot(mc::IMessageRef message)
	{
		if ( ScreenshotMessage::messageId_ != message->GetCode() )
			return;
		IScreenshotMessagePtr screenshot(message);
		[[ScreenshotProvider instance] updateScreenshotHost:(unsigned char *)screenshot->GetData() ofLength:screenshot->GetDataLength()];
	}
    
    void UIEvents::UpdateVolume(unsigned long volume)
    {
        // Spread volume changes.
        [[IndicatorsProvider instance] volumeChanged:volume];
    }
	
	void UIEvents::WarnOnOldRemotePC()
	{
		[[iRemoteAppDelegate delegate] performSelectorOnMainThread:@selector(warnOnOldRemotePC) withObject:nil waitUntilDone:NO];
	}
    
    void UIEvents::ShowShellResponse(const std::string& response)
    {
		// This method is called from outside of any memory pool.
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];		
        NSString *responseString = [NSString stringWithCString:response.c_str() encoding:[NSString defaultCStringEncoding]];
        [[iRemoteAppDelegate delegate] performSelectorOnMainThread:@selector(showShellResponse:) withObject:responseString waitUntilDone:NO];        
		[pool release];
    }
	
	void UIEvents::UpdateZoomParams(float minZoom, float maxZoom, float currentZoom)
	{
		// Save parameters locally to provide access for ControlArea. 
		UIConfig::minZoom_ = minZoom;
		UIConfig::maxZoom_ = maxZoom;
		UIConfig::currentZoom_ = currentZoom;		
	}

}
