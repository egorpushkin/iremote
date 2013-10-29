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

#import "MagicCursor.h"
#import "FBSurfaceHolder.h"

// Connector tool.
#include "Connector/Server.h"

@implementation MagicCursor

#pragma mark User action handlers

- (IBAction)leftButtonClick {
	RemotePC::Holder::Instance().OnLeftDown();
	RemotePC::Holder::Instance().OnLeftUp();
}

- (IBAction)rightButtonClick {
	RemotePC::Holder::Instance().OnRightDown();
	RemotePC::Holder::Instance().OnRightUp();
}

- (IBAction)leftButtonToggle {
	RemotePC::SFB::SurfacePtr surface = [[FBSurfaceHolder instance] surface];
	if ( !surface )
		return;
	if ( surface->cursor()->leftPressed() ) {
		surface->cursor()->pressLeft(false);
		RemotePC::Holder::Instance().OnLeftUp();								 
	} else {
		surface->cursor()->pressLeft(true);		
		RemotePC::Holder::Instance().OnLeftDown();
	}
}

- (IBAction)rightButtonToggle {
	RemotePC::SFB::SurfacePtr surface = [[FBSurfaceHolder instance] surface];
	if ( !surface )
		return;
	if ( surface->cursor()->rightPressed() ) {
		surface->cursor()->pressRight(false);
		RemotePC::Holder::Instance().OnRightUp();
	} else {
		surface->cursor()->pressRight(true);		
		RemotePC::Holder::Instance().OnRightDown();
	}	
}

#pragma mark UIView section

/**
 * Current implementation discards all events targeted to this 
 * particular view, but allows to handle events sent to subviews.
 */
- (UIView *)hitTest:(CGPoint)point withEvent:(UIEvent *)event {
	UIView * view = [super hitTest:point withEvent:event];
	return ( self == view ) ? nil : view;	
}

#pragma mark Life cycle section

- (void)dealloc {
    [super dealloc];
}

@end
