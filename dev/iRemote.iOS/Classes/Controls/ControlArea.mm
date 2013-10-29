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

#import "ControlArea.h"
#import "Content.h"

#import "../Main/iRemoteAppDelegate.h"
#import "Controls.h"
#import "ScreenshotProvider.h"

// Rendering tools.
#import "../../Tools/OpenGL/Texture.h"
#import "../../Tools/OpenGL/Painter2d.h"

#include "Connector/Server.h"
#include "../State/UIConfig.h"

@implementation ControlArea

const CGFloat scrollInterval = 10.0f;
const CGFloat maximumPinchDistanceForScroll = 3.0f;
const CGFloat zoomCoefficient = 0.01f;
const CGFloat zoomInOutCoefficient = 1.2f;

#pragma mark Properties

@synthesize zoomEnabled;
@synthesize zoomSupported;

#pragma mark Zooming tool

- (BOOL)checkRemotePCZoomingSupport {
	if ( !self.zoomSupported ) {
		[Controls showOldRemotePCAlert];
	}
	return self.zoomSupported;
}

/** 
 * Adjusts zoom level to bounds from UIConfig.
 */
- (void)adjustZoom {
	if ( zoomLevel > RemotePC::UIConfig::maxZoom_ )
		zoomLevel = RemotePC::UIConfig::maxZoom_;
	if ( zoomLevel < RemotePC::UIConfig::minZoom_ )
		zoomLevel = RemotePC::UIConfig::minZoom_;		
}

#pragma mark Public interface

- (IBAction)zoomIn {
	if ( !self.zoomEnabled )
		return;
	if ( ![self checkRemotePCZoomingSupport] )
		return;	
	zoomLevel /= zoomInOutCoefficient;
	[self adjustZoom];
	RemotePC::Holder::Instance().OnZoom( zoomLevel );
}

- (IBAction)zoomOut {
	if ( !self.zoomEnabled )
		return;
	if ( ![self checkRemotePCZoomingSupport] )
		return;		
	zoomLevel *= zoomInOutCoefficient;
	[self adjustZoom];	
	RemotePC::Holder::Instance().OnZoom( zoomLevel );
}

- (IBAction)zoom1to1 {
	if ( !self.zoomEnabled )
		return;
	if ( ![self checkRemotePCZoomingSupport] )
		return;		
	zoomLevel = 1.0f;
	[self adjustZoom];	
	RemotePC::Holder::Instance().OnZoom( zoomLevel );
}

#pragma mark Geometry tools

- (CGFloat)eucledianDistanceFromPoint:(CGPoint)from toPoint:(CGPoint)to {
    float dx = to.x - from.x;
    float dy = to.y - from.y;    
    return sqrt(dx * dx + dy * dy);
}

#pragma mark Helpers

- (CGFloat)pinchDitanceForEvent:(UIEvent *)event {
    NSArray *localTouches = [[event touchesForView:self] allObjects];
    CGPoint touchPoint1 = [[localTouches objectAtIndex:0] locationInView:self];
    CGPoint touchPoint2 = [[localTouches objectAtIndex:1] locationInView:self];        
    return [self eucledianDistanceFromPoint:touchPoint1 toPoint:touchPoint2];    
}

- (CGSize)moveToOffset:(CGPoint)from to:(CGPoint)to {
	CGSize offset;
	if ( RemotePC::UIConfig::Instance().PanningMode() )
		offset = CGSizeMake(to.x - from.x, to.y - from.y);
	else
		offset = CGSizeMake(from.x - to.x, from.y - to.y);
	return offset;
}

#pragma mark Misc

- (void)layoutSubviews {
    [super layoutSubviews];
    cursor.hidden = YES;
    [self setNeedsDisplay];
}

- (void)update:(BOOL)hasValidScreenshot {
	// Show cursor only if there is valid screenshot to display.
    cursor.hidden = !hasValidScreenshot;
	// Redraw content.
	[self setNeedsDisplay];
}

- (void)draw {
    CGRect rect = self.frame;    
	CGSize offset = CGSizeZero;
    CGRect backRect;
	if ( rect.size.width > rect.size.height ) {
		// Rectangle for landscape view
		float hoffset = ( rect.size.height - rect.size.width ) / 2;    
		backRect = CGRectMake(- offset.width + 0.0, - offset.height + hoffset, rect.size.width, rect.size.width);
	} else {
		// Rectangle for portrait view.
		float woffset = ( rect.size.width - rect.size.height ) / 2;    
		backRect = CGRectMake(- offset.width + woffset, - offset.height + 0.0, rect.size.height, rect.size.height);
	}
	// Display screenshot.
	Texture *texture = [[ScreenshotProvider instance] texture];
	[Painter2d drawTexture:texture inRect:backRect];
}

#pragma mark High-level events handling

- (void)handleMove:(CGPoint)from to:(CGPoint)to {
    CGSize offset = [self moveToOffset:from to:to];
    RemotePC::Holder::Instance().OnMovedBy(offset.width, offset.height);
}

- (void)handleScroll:(CGPoint)newPoint {    
    // Handle vertical scroll.
    vscrollLength += newPoint.y - lastPoint.y;
    if ( fabs(vscrollLength) > scrollInterval ) {
        RemotePC::Holder::Instance().OnScrolled( true, ( vscrollLength < 0 ) ? ( 120 ) : ( -120 ) );
        vscrollLength -= ( vscrollLength / fabs(vscrollLength) ) * scrollInterval;
    }
    // Handle horizontal scroll.
    hscrollLength += newPoint.x - lastPoint.x;
    if ( fabs(hscrollLength) > scrollInterval ) {
        RemotePC::Holder::Instance().OnScrolled( false, ( hscrollLength < 0 ) ? ( -120 ) : ( 120 ) );
        hscrollLength -= ( hscrollLength / fabs(hscrollLength) ) * scrollInterval;
    }       
}

- (void)handleZoom:(CGFloat)distance {    
    // Calculate new zoom level.
    CGFloat distanceDifference = pinchDistance - distance;
    zoomLevel = zoomLevel + distanceDifference * zoomCoefficient;
	[self adjustZoom];
    // Notify server on new zoom level.
    RemotePC::Holder::Instance().OnZoom( zoomLevel );
}

#pragma mark Low-level events handling

// Handles the start of a touch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [[event touchesForView:self] anyObject];
    lastPoint = [touch locationInView: self];
	
    // Update area flags.
    moveGestureOccured = NO;
    if ( [[event allTouches] count] > 1 ) {
        singleTouchEvent = NO;
    }
    
    // Get local touches and calculate distance between two touches, if any.
    // This is required to distinguish zoom and scroll events.
    NSSet *localTouches = [event touchesForView:self];
    if ( 2 == [localTouches count] ) {
        pinchDistance = [self pinchDitanceForEvent:event];
    }
}

// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {  
    UITouch *touch = [[event touchesForView:self] anyObject];
    CGPoint newPoint = [touch locationInView:self];
    
    // Update area flags.    
    moveGestureOccured = YES;
    
    int localTouches = [[event touchesForView:self] count];
    if ( 1 == localTouches ) {    
        [self handleMove:lastPoint to:newPoint];
    } else if ( 2 == localTouches ) {    
        CGFloat distance = [self pinchDitanceForEvent:event];
        if ( ( -1.0f == pinchDistance ) || 
            ( fabs(distance - pinchDistance) < maximumPinchDistanceForScroll ) ||
			(!self.zoomEnabled) || 
			(!self.zoomSupported) ) {
            // Handle scroll gesture.
            [self handleScroll:newPoint];
        } else {
            // Handle zoom gesture.
            [self handleZoom:distance];
        }        
        // Remember pinch distance.
        pinchDistance = distance;        
    }
	
	lastPoint = newPoint;    
}

// Handles the end of a touch event when the touch is a tap.
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    pinchDistance = -1.0f;
    
    if ( [touches count] == [[event allTouches] count] ) {
        // Event seems to be finished.
        if ( singleTouchEvent && !moveGestureOccured ) {
            // Click event occured.
            RemotePC::Holder::Instance().OnLeftDown();    
            RemotePC::Holder::Instance().OnLeftUp();    
        }
        
        // Update area flags.
        moveGestureOccured = NO;           
        singleTouchEvent = YES;
		
		lastPoint = CGPointZero;
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    pinchDistance = -1.0f;  
	
    if ( [touches count] == [[event allTouches] count] ) {	
		lastPoint = CGPointZero;		
	}
}
	
#pragma mark Life cycle section

- (id)initWithCoder:(NSCoder*)coder {
    if ((self = [super initWithCoder:coder])) {
        // Configure initial settings.
        vscrollLength = 0.0f;
        hscrollLength = 0.0f;
        zoomLevel = 1.0f;
        pinchDistance = -1.0f;
        singleTouchEvent = YES;
        moveGestureOccured = NO;        
		zoomEnabled = NO;
		zoomSupported = NO;
        // Configure control.
        self.multipleTouchEnabled = YES;
    }
    return self;
}

- (void)dealloc {
    [cursor release];
    [super dealloc];
}

@end
