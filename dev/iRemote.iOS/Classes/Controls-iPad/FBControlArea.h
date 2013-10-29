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
#import "../../Tools/OpenGL/EAGLView.h"
#import "IFBControl.h"
#import "../State/ISettingsListener.h"

enum {
	FBControlAreaModeNone              = 0,
	FBControlAreaModeScaleToFit        = 1 << 0,	
	FBControlAreaModeScaleToFill       = 1 << 1,	
	FBControlAreaModeFreeform          = 1 << 2		
};
typedef NSUInteger FBControlAreaMode;	

@class MagicCursor;

@interface FBControlArea : EAGLView< IFBControl, UIGestureRecognizerDelegate, ISettingsListener > {

	FBControlAreaMode mode_;	
	BOOL internalPanningAllowed_;
	BOOL internalZoomingAllowed_;		
	BOOL userPanningAllowed_;
	BOOL userZoomingAllowed_;	
	
	CGPoint offset_;
    CGFloat zoomLevel_;
	CGRect offsetRange_;
	CGSize zoomRange_;
	
	BOOL panning_;
	NSUInteger maxTouches_;
	
	IBOutlet MagicCursor *magicCursor_;
	NSTimer *magicTimer_;
	
	BOOL absoluteCursor_;
	
}

#pragma mark Surface geometry

- (CGPoint)controlToSurface:(CGPoint)point;
- (CGPoint)surfaceToControl:(CGPoint)point;

#pragma mark Mode control

- (void)setMode:(FBControlAreaMode)mode;
- (FBControlAreaMode)mode;
- (void)setPanningAllowed:(BOOL)allowed;
- (BOOL)panningAllowed;
- (void)setZoomingAllowed:(BOOL)allowed;
- (BOOL)zoomingAllowed;

#pragma mark Rendering section

- (void)draw;
- (void)drawBackground;
- (void)drawSurface;
- (void)drawSurfaceCore;
- (void)drawCursor;
- (void)drawCursor:(CGPoint)point;

#pragma mark IFBControl section

- (void)initControl;
- (void)updateControl;

@end
