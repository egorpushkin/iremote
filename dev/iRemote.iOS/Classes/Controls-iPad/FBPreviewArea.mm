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

#import <QuartzCore/QuartzCore.h>
#import "FBPreviewArea.h"
#import "FBSurfaceHolder.h"
#import "../State/UISettings.h"

@implementation FBPreviewArea

#pragma mark ISettingsListener

- (void)settingsChanged {
	zoomLevel_ = [[UISettings instance] asFloat:@"previewZoom"];
	[super settingsChanged];
}

#pragma mark Control positionning

- (void)moveControl:(CGPoint)translation {
	// Move control according to current translation.
	self.center = CGPointMake(self.center.x + translation.x, self.center.y + translation.y);
	// Change alignment depending on new control position.
	self.autoresizingMask = 0;
	if ( self.center.x < ( self.superview.frame.size.width / 2.0f ) )
		self.autoresizingMask = self.autoresizingMask | UIViewAutoresizingFlexibleRightMargin;
	else
		self.autoresizingMask = self.autoresizingMask | UIViewAutoresizingFlexibleLeftMargin;
	if ( self.center.y < ( self.superview.frame.size.height / 2.0f ) )
		self.autoresizingMask = self.autoresizingMask | UIViewAutoresizingFlexibleBottomMargin;
	else
		self.autoresizingMask = self.autoresizingMask | UIViewAutoresizingFlexibleTopMargin;	
}

#pragma mark Guesture recornizers section

- (void)handlePan:(UIPanGestureRecognizer *)sender {     
	if ( UIGestureRecognizerStateChanged == sender.state ) {
		// Get current translation.
		CGPoint translation = [sender translationInView:self];
		// Move control and control alignment.
		[self moveControl:translation];
		// Flush current translation. 
		[sender setTranslation:CGPointMake(0.0f, 0.0f) inView:self];
	}
}

- (void)setupGestureRecognizers {
	// Pan gesture.
	UIPanGestureRecognizer *panGestureRecognizer = [[UIPanGestureRecognizer alloc]
		initWithTarget:self action:@selector(handlePan:)];
	panGestureRecognizer.minimumNumberOfTouches = 1;
	panGestureRecognizer.maximumNumberOfTouches = 1;
    [self addGestureRecognizer:panGestureRecognizer];
    [panGestureRecognizer release];	
}

#pragma mark Rendering section

/**
 * drawSurface is overridden to adjust offset_ occording to current surface
 * cursor position and zoomLevel_.
 */
- (void)drawSurface {		
	RemotePC::SFB::SurfacePtr surface = [[FBSurfaceHolder instance] surface];
    if ( !surface || !surface->cursor()->isSet() )    
		return;	
	
	// Calculate offset to center preview control on mouse cursor.
	offset_ = CGPointMake(0.0f, 0.0f);
	CGPoint cursorPoint = CGPointMake(surface->cursor()->x(), surface->cursor()->y());
	CGPoint controlOffset = [self surfaceToControl:cursorPoint];	
	offset_ = CGPointMake(
		- controlOffset.x + ( self.frame.size.width / 2.0f ),
		- controlOffset.y + ( self.frame.size.height / 2.0f ));
	
	// Invoke original version to set scissor rectangle correctly and 
	// render tiles.
	[super drawSurface];
}

- (void)drawCursor {
	// Cursor is always shown at the very certer of the control. 
	[super drawCursor:CGPointMake(self.frame.size.width / 2.0f, self.frame.size.height / 2.0f)];
}

#pragma mark IFBControl section

- (void)updateCursor:(BOOL)server {
	[self drawView];
}

#pragma mark Life cycle section

- (id)initWithCoder:(NSCoder*)coder {
    if ((self = [super initWithCoder:coder])) {
		// Configure UIView.
        self.layer.cornerRadius = 10.0f;
		self.layer.masksToBounds = YES;
		self.layer.borderColor = [[UIColor grayColor] CGColor];
		self.layer.borderWidth = 3.0f;		
		// Configure FBControlArea.
		[self setMode:FBControlAreaModeFreeform];
		
		zoomLevel_ = 0.3f;
	}
	return self;
}

- (void)dealloc {
    [super dealloc];
}

@end
