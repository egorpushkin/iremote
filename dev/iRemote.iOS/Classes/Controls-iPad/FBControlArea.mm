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

#import "FBControlArea.h"
#import "FBSurfaceHolder.h"

// Dependent controls.
#import "MagicCursor.h"

// Application settings.
#import "../State/UISettings.h"

// Precompiled header.
#include "Common/Common.h"
// Connector tools.
#include "Connector/Server.h"

static const float Setting_MagicCursorLifetime = 3.0f;
static const float Setting_MagicCursorAppearance = 0.5f;
static const float Setting_MagicCursorDisappearance = 0.7f;

@implementation FBControlArea

#pragma mark ISettingsListener

- (void)settingsChanged {
	// Update local settings store.
	absoluteCursor_ = [[UISettings instance] asBool:@"areaAbsolute"];	
	// Repaint the control if necessary.
	if ( !self.hidden && 0.0f != self.alpha ) {
		[self drawView];
	}
}

#pragma mark Helpers

- (CGPoint)controlToSurface:(CGPoint)point {
	return CGPointMake(
		( point.x - offset_.x ) * zoomLevel_,
		( point.y - offset_.y ) * zoomLevel_	
	);		
}

- (CGPoint)surfaceToControl:(CGPoint)point {
	return CGPointMake(
		point.x / zoomLevel_ + offset_.x,
		point.y / zoomLevel_ + offset_.y	
	);		
}

- (CGRect)visibleSurfaceRect:(const RemotePC::SFB::SurfacePtr&)surface {
	CGPoint controlTopLeft = CGPointMake(0.0f, 0.0f);
	CGPoint controlBottomRight = CGPointMake(self.frame.size.width - 1.0f, self.frame.size.height - 1.0f);	
	CGPoint surfaceTopLeft = [self controlToSurface:controlTopLeft];
	CGPoint surfaceBottomRight = [self controlToSurface:controlBottomRight];
	surfaceTopLeft.x = floor(surfaceTopLeft.x);
	surfaceTopLeft.y = floor(surfaceTopLeft.y);	
	surfaceBottomRight.x = ceil(surfaceBottomRight.x);
	surfaceBottomRight.y = ceil(surfaceBottomRight.y);	
	CGRect surfaceControl = CGRectMake(surfaceTopLeft.x, surfaceTopLeft.y, 
		surfaceBottomRight.x - surfaceTopLeft.x + 1.0f, surfaceBottomRight.y - surfaceTopLeft.y + 1.0f);
	CGRect surfaceEntire = CGRectMake(0.0f, 0.0f, surface->screenWidth(), surface->screenHeight());
	CGRect surfaceVisible = CGRectIntersection(surfaceControl, surfaceEntire);
	return surfaceVisible;
}

- (CGPoint)surfacePointToTilePosition:(CGPoint)point surface:(const RemotePC::SFB::SurfacePtr&)surface {
	size_t textureSize = surface->textureSize();
	return CGPointMake((size_t)point.x / textureSize, (size_t)point.y / textureSize);
}

#pragma mark Magic cursor control

- (void)magicCursorHidden {
	magicCursor_.hidden = YES;
}

- (void)hideMagicCursor {
	if ( !magicTimer_ )
		return;
	// Animate disappearance.	
	if ( !magicCursor_.hidden ) {
		[UIView beginAnimations:nil context:nil];
		[UIView setAnimationDuration:Setting_MagicCursorDisappearance];
		[UIView setAnimationDelegate:self];
		[UIView setAnimationDidStopSelector:@selector(magicCursorHidden)];
		magicCursor_.alpha = 0.0f;
		[UIView commitAnimations];	
	}
	// Cleanup timer.
	[magicTimer_ invalidate];	
	[magicTimer_ release];
	magicTimer_ = nil;
}

- (void)showMagicCursor:(CGPoint)controlPoint {
	// Animate appearance.	
	if ( magicCursor_.hidden ) {
		[UIView beginAnimations:nil context:nil];
		[UIView setAnimationDuration:Setting_MagicCursorAppearance];
		magicCursor_.alpha = 1.0f;
		[UIView commitAnimations];	
	}
	magicCursor_.center = controlPoint;			
	magicCursor_.hidden = NO;	
	// Schedule disappearance. 
	if ( nil != magicTimer_ ) {
		[magicTimer_ invalidate];
		[magicTimer_ release];		
	}
	magicTimer_ = [[NSTimer scheduledTimerWithTimeInterval:Setting_MagicCursorLifetime
		target:self selector:@selector(hideMagicCursor) userInfo:nil repeats:NO] retain];
}

- (void)showMagicCursor {
	RemotePC::SFB::SurfacePtr surface = [[FBSurfaceHolder instance] surface];
    if ( !surface || !surface->cursor()->isSet() )    
		return;	
	
	CGPoint cursorSurfacePoint = CGPointMake(surface->cursor()->x(), surface->cursor()->y());
	CGPoint cursorControlPoint = [self surfaceToControl:cursorSurfacePoint];
	
	[self showMagicCursor:cursorControlPoint];
}

/**
 * Server cursor updates are surpressed in absolute mode, 
 * when user pans the screen. 
 */
- (void)moveCursorTo:(CGPoint)surfacePoint atControl:(CGPoint)controlPoint {	
	RemotePC::SFB::SurfacePtr surface = [[FBSurfaceHolder instance] surface];
	if ( !surface ) 
		return;
	// Update local cursor position right away. 
	surface->cursor()->set(surfacePoint.x, surfacePoint.y);	
	// Notify on local cursor movement.
	[[FBSurfaceHolder instance] updateCursor:NO]; 
	// Update magic cursor forcibly.
	[self showMagicCursor:controlPoint];	
}

/**
 * Relative movement should not cause local cursor changes.
 * It causes huge inaccuracy of local cursor position after a while.	
 * Only server cursor updates are considered, in relative mode.
 */
- (void)moveCursorBy:(CGPoint)translation atControl:(CGPoint)controlPoint {	
	// Update magic cursor forcibly.
	[self showMagicCursor:controlPoint];
}

#pragma mark Mode control

- (void)applyMode {
	RemotePC::SFB::SurfacePtr surface = [[FBSurfaceHolder instance] surface];
    if ( !surface )    
        return;
	
	float controlWidth = self.frame.size.width;
	float controlHeight = self.frame.size.height;
	float surfaceWidth = (float)surface->screenWidth();
	float surfaceHeight = (float)surface->screenHeight();	
	
	if ( FBControlAreaModeScaleToFit == mode_ ) {
		internalPanningAllowed_ = true;
		internalZoomingAllowed_ = false;
		// Calculate zoom level and offset.
        float controlRatio = controlWidth / controlHeight;
        float surfaceRatio = surfaceWidth / surfaceHeight;
		if ( controlRatio < surfaceRatio ) {
			// Vertical fit (surface touches left and right control borders).
			float newSurfaceWidth = controlWidth;
			float newSurfaceHeight = newSurfaceWidth / surfaceRatio;
			offset_ = CGPointMake(0.0f, ( controlHeight - newSurfaceHeight ) / 2.0f);
			zoomLevel_ =  surfaceWidth / newSurfaceWidth;					
			// Calculate offset range.
			offsetRange_ = CGRectMake(0.0f, 0.0f, 0.0f, controlHeight - newSurfaceHeight);			
		} else {
			// Horizontal fit (surface touches top and bottom control borders).
			float newSurfaceHeight = controlHeight;			
			float newSurfaceWidth = newSurfaceHeight * surfaceRatio;
			offset_ = CGPointMake(( controlWidth - newSurfaceWidth ) / 2.0f, 0.0f);        
			zoomLevel_ =  surfaceHeight / newSurfaceHeight;					
			// Calculate offset range.
			offsetRange_ = CGRectMake(0.0f, 0.0f, controlWidth - newSurfaceWidth, 0.0f);			
		}		
	} else if ( FBControlAreaModeScaleToFill == mode_ ) {
		internalPanningAllowed_ = true;
		internalZoomingAllowed_ = false;
		// Calculate zoom level and offset.
        float controlRatio = controlWidth / controlHeight;
        float surfaceRatio = surfaceWidth / surfaceHeight;
		if ( controlRatio > surfaceRatio ) {
			// Vertical fit (control touches left and right surface borders).
			float newSurfaceWidth = controlWidth;
			float newSurfaceHeight = newSurfaceWidth / surfaceRatio;
			offset_ = CGPointMake(0.0f, ( controlHeight - newSurfaceHeight ) / 2.0f);
			zoomLevel_ =  surfaceWidth / newSurfaceWidth;				
			// Calculate offset range.
			offsetRange_ = CGRectMake(0.0f, controlHeight - newSurfaceHeight, 0.0f, 0.0f);						
		} else {
			// Horizontal fit (control touches top and bottom surface borders).
			float newSurfaceHeight = controlHeight;				
			float newSurfaceWidth = newSurfaceHeight * surfaceRatio;		
			offset_ = CGPointMake(( controlWidth - newSurfaceWidth ) / 2.0f, 0.0f);        
			zoomLevel_ =  surfaceHeight / newSurfaceHeight;					
			// Calculate offset range.
			offsetRange_ = CGRectMake(controlWidth - newSurfaceWidth, 0.0f, 0.0f, 0.0f);			
		}		
	} else if ( FBControlAreaModeFreeform == mode_ ) {
		offsetRange_ = CGRectNull;
		internalPanningAllowed_ = true;
		internalZoomingAllowed_ = true;		
	}
}

- (void)applyOffsetRange {
	if ( CGRectIsNull(offsetRange_) )
		return;
	if ( offset_.x < offsetRange_.origin.x )
		offset_.x = offsetRange_.origin.x;
	else if ( offset_.x > offsetRange_.size.width )
		offset_.x = offsetRange_.size.width;		
	if ( offset_.y < offsetRange_.origin.y )
		offset_.y = offsetRange_.origin.y;
	else if ( offset_.y > offsetRange_.size.height )
		offset_.y = offsetRange_.size.height;		
}

- (void)applyZoomRange {
	if ( zoomLevel_ < zoomRange_.width )
		zoomLevel_ = zoomRange_.width;
	if ( zoomLevel_ > zoomRange_.height )
		zoomLevel_ = zoomRange_.height;
}

- (void)setMode:(FBControlAreaMode)mode {	
	mode_ = mode;
	[self applyMode];
	[self drawView];		
}

- (FBControlAreaMode)mode {
	return mode_;	
}

- (void)setPanningAllowed:(BOOL)allowed {
	userPanningAllowed_ = allowed;	
}

- (BOOL)panningAllowed {
	return userPanningAllowed_;
}

- (void)setZoomingAllowed:(BOOL)allowed {
	userZoomingAllowed_ = allowed;
}

- (BOOL)zoomingAllowed {
	return userZoomingAllowed_;
}

#pragma mark Guesture recornizers section

- (void)handleTap:(UITapGestureRecognizer *)sender {     
	if ( UIGestureRecognizerStateEnded == sender.state ) {         
		// Notify server on click.
		CGPoint tapPoint = [sender locationInView:self];
		CGPoint surfacePoint = [self controlToSurface:tapPoint];
		RemotePC::Holder::Instance().OnClicked(surfacePoint.x, surfacePoint.y);
		// Update local cursor position right away. 
		[self moveCursorTo:surfacePoint atControl:tapPoint];
	} 
}

- (void)handleLongPress:(UITapGestureRecognizer *)sender {     
	if ( UIGestureRecognizerStateEnded == sender.state ) {   
		// TODO: Move cursor to tap position. 
		RemotePC::Holder::Instance().OnRightDown();
		RemotePC::Holder::Instance().OnRightUp();
	}
}

- (void)handlePan:(UIPanGestureRecognizer *)sender {     
	if ( ( UIGestureRecognizerStateEnded == sender.state ) 
		|| ( UIGestureRecognizerStateCancelled == sender.state )
		|| ( UIGestureRecognizerStateFailed == sender.state ) ) {
		panning_ = NO;
		maxTouches_ = 0;
	}	
	if ( UIGestureRecognizerStateChanged == sender.state ) {
		NSUInteger touchesCount = [sender numberOfTouches];		
		CGPoint translation = [sender translationInView:self];		
		if ( ( 1 == touchesCount ) && ( maxTouches_ <= 1 ) ) {
			maxTouches_ = 1;
			CGPoint panPoint = [sender locationInView:self];
			CGPoint surfacePoint = [self controlToSurface:panPoint];
			if ( absoluteCursor_ ) {
				// Send new absolute position.
				RemotePC::Holder::Instance().OnMovedTo(surfacePoint.x, surfacePoint.y);
				// Update local cursor position right away. 
				[self moveCursorTo:surfacePoint atControl:panPoint];				
			} else {
				// Send just diff.
				RemotePC::Holder::Instance().OnMovedBy(translation.x, translation.y);
				// Update local cursor position right away. 
				[self moveCursorBy:translation atControl:panPoint];				
			}
		} else if ( 2 == touchesCount ) {
			maxTouches_ = 2;
			RemotePC::Holder::Instance().OnScrolled2(true, ( translation.y ) * 3);
			RemotePC::Holder::Instance().OnScrolled2(false, ( translation.x ) * 3);			
		} else if ( 3 == touchesCount ) {
			maxTouches_ = 3;
			if ( internalPanningAllowed_ && userPanningAllowed_ ) { 
				CGPoint translation = [sender translationInView:self];
				offset_.x += ( translation.x );
				offset_.y += ( translation.y );			
				[self applyOffsetRange];
				[self drawView];				
				[self hideMagicCursor];				
			}
			panning_ = YES;
		}	
	}
	[sender setTranslation:CGPointMake(0.0f, 0.0f) inView:self];	
}

- (void)handlePinch:(UIPinchGestureRecognizer *)sender {     
	if ( !panning_ && internalZoomingAllowed_ && userZoomingAllowed_ ) {
		if ( UIGestureRecognizerStateBegan == sender.state ) {		
			[self hideMagicCursor];
		} else if ( UIGestureRecognizerStateChanged == sender.state ) {
			if ( 0.0f != sender.scale ) {			
				CGPoint pinchPoint = [sender locationInView:self];
				// Update zoom level.
				CGFloat prevZoomLevel = zoomLevel_;
				zoomLevel_ /= sender.scale;				
				sender.scale = 1.0f;
				[self applyZoomRange];
				// Update screen centering.
				offset_.x = pinchPoint.x - ( pinchPoint.x - offset_.x ) * prevZoomLevel / zoomLevel_;
				offset_.y = pinchPoint.y - ( pinchPoint.y - offset_.y ) * prevZoomLevel / zoomLevel_;
				[self applyOffsetRange];				
				// Repaint the control.
				[self drawView];
			}	
		}
	}
}

- (void)handleSwipe:(UISwipeGestureRecognizer *)sender {     
	
	
}

- (void)setupGestureRecognizers {
	// Tap gesture.
	UITapGestureRecognizer *tapGestureRecognizer = [[UITapGestureRecognizer alloc]
		initWithTarget:self action:@selector(handleTap:)];
	tapGestureRecognizer.delegate = self;
    [self addGestureRecognizer:tapGestureRecognizer];
    [tapGestureRecognizer release];	
	
	// TODO: Double tap gesture. 
	
	// Long press gesture. 
	UILongPressGestureRecognizer *longPressGestureRecognizer = [[UILongPressGestureRecognizer alloc]
		initWithTarget:self action:@selector(handleLongPress:)];
	longPressGestureRecognizer.delegate = self;
    [self addGestureRecognizer:longPressGestureRecognizer];
    [longPressGestureRecognizer release];			
	// Pan gesture.
	UIPanGestureRecognizer *panGestureRecognizer = [[UIPanGestureRecognizer alloc]
		initWithTarget:self action:@selector(handlePan:)];
	panGestureRecognizer.delegate = self;	
	panGestureRecognizer.minimumNumberOfTouches = 1;	
	panGestureRecognizer.maximumNumberOfTouches = 3;
    [self addGestureRecognizer:panGestureRecognizer];
    [panGestureRecognizer release];
	// Pinch gesture.
	UIPinchGestureRecognizer *pinchGestureRecognizer = [[UIPinchGestureRecognizer alloc]
		initWithTarget:self action:@selector(handlePinch:)];
	pinchGestureRecognizer.delegate = self;	
    [self addGestureRecognizer:pinchGestureRecognizer];
    [pinchGestureRecognizer release];	
	// Swipe gesture.
	UISwipeGestureRecognizer *swipeGestureRecognizer = [[UISwipeGestureRecognizer alloc]
		initWithTarget:self action:@selector(handleSwipe:)];
	swipeGestureRecognizer.delegate = self;	
    [self addGestureRecognizer:swipeGestureRecognizer];
    [swipeGestureRecognizer release];		
	
	// [panGestureRecognizer requireGestureRecognizerToFail:pinchGestureRecognizer];
	// [pinchGestureRecognizer requireGestureRecognizerToFail:panGestureRecognizer];	
}

#pragma mark UIGestureRecognizerDelegate section

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer 
	shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer {
	/* if ( [otherGestureRecognizer isKindOfClass:[UIPanGestureRecognizer class]]
		&& [gestureRecognizer isKindOfClass:[UIPinchGestureRecognizer class]] )	
		return NO;	
	if ( [gestureRecognizer isKindOfClass:[UIPanGestureRecognizer class]]
		&& [otherGestureRecognizer isKindOfClass:[UIPinchGestureRecognizer class]] )	
		return NO; */
	return NO;	
}

#pragma mark Rendering section

- (void)draw {		
	// Draw the background.
	[self drawBackground];	
	// Draw the surface.
	[self drawSurface];
	// Draw the cursor.
	[self drawCursor];
}

- (void)drawBackground {
	RemotePC::SFB::TexturePtr background = [[FBSurfaceHolder instance] background];
	size_t backWidth = background->width();
	size_t backHeight = background->height();	
	size_t backsWidth = ceil( self.frame.size.width / (float)backWidth );
	size_t backsHeight = ceil( self.frame.size.height / (float)backHeight );	
	for ( size_t i = 0 ; i < backsHeight ; ++i ) {
		for ( size_t j = 0 ; j < backsWidth ; ++j ) {
			background->draw(			
				j * backWidth, i * backHeight,
				backWidth, backHeight);			
		}
	}	
}

- (void)drawSurface {
	// Skip drawing, if there is no surface. 
	RemotePC::SFB::SurfacePtr surface = [[FBSurfaceHolder instance] surface];
    if ( !surface )    
        return;	
	
	// Set scissor rect to cut non existing part of the surface.
	float surfaceWidth = (float)surface->screenWidth();
	float surfaceHeight = (float)surface->screenHeight();		
	float projectionWidth = surfaceWidth / zoomLevel_;		
	float projectionHeight = surfaceHeight / zoomLevel_;	
	float scissorY = self.frame.size.height - projectionHeight - offset_.y;
	glEnable(GL_SCISSOR_TEST);		
	glScissor((GLint)offset_.x, (GLint)scissorY, (GLint)projectionWidth, (GLint)projectionHeight);
	
	// Draw surface tiles.
	[self drawSurfaceCore];
	
	// Disable scissor to prevent side effects.
	glDisable(GL_SCISSOR_TEST);			
}

- (void)drawSurfaceCore {
	// Skip drawing, if there is no surface. 
	RemotePC::SFB::SurfacePtr surface = [[FBSurfaceHolder instance] surface];
    if ( !surface )    
        return;		
	
	// Calculate surface visible rectangle.
	CGRect surfaceVisible = [self visibleSurfaceRect:surface];
	if ( CGRectEqualToRect(surfaceVisible, CGRectNull) ) 
		return;		
	
	// Translate visible rectangle corrdinates to tile indexes. 
	CGPoint topLeftPoint = CGPointMake(surfaceVisible.origin.x, surfaceVisible.origin.y);
	CGPoint bottomRightPoint = CGPointMake(
		surfaceVisible.origin.x + surfaceVisible.size.width - 1.0f, 
		surfaceVisible.origin.y + surfaceVisible.size.height - 1.0f);	
	CGPoint topLeftTile = [self surfacePointToTilePosition:topLeftPoint surface:surface];
	CGPoint bottomRightTile = [self surfacePointToTilePosition:bottomRightPoint surface:surface];	
	
	// Draw the surface.
	float textureSize = (float)surface->textureSize();	
	float zoomedTextureSize = textureSize / zoomLevel_;	
	for ( int i = (int)topLeftTile.y ; i <= (int)bottomRightTile.y ; ++i ) {
		for ( int j = (int)topLeftTile.x ; j <= (int)bottomRightTile.x ; ++j ) {	
			RemotePC::SFB::Texture *texture = surface->texture(j, i);
			texture->draw(
				( int )( offset_.x + ( j * zoomedTextureSize ) ),
				( int )( offset_.y + ( i * zoomedTextureSize ) ),
				( int )ceil(zoomedTextureSize),
				( int )ceil(zoomedTextureSize));
		}
	}				
}

- (void)drawCursor {
	RemotePC::SFB::SurfacePtr surface = [[FBSurfaceHolder instance] surface];	
	if ( !surface || !surface->cursor()->isSet() ) 
		return;
	
	CGPoint point = CGPointMake(surface->cursor()->x(), surface->cursor()->y());
	point = [self surfaceToControl:point];
	[self drawCursor:point];	
}

- (void)drawCursor:(CGPoint)point {
	RemotePC::SFB::TexturePtr cursor = [[FBSurfaceHolder instance] cursor];		
	size_t cursorWidth = cursor->width();
	size_t cursorHeight = cursor->height();	
	cursor->draw(
		point.x - ( cursorWidth >> 1 ), point.y - ( cursorHeight >> 1 ), 
		cursorWidth, cursorHeight );	
}

#pragma mark IFBControl section

- (void)initControl {
	[self applyMode];
	[self drawView];	
}

- (void)updateControl {
	[self drawView];
}

- (void)updateCursor:(BOOL)server {
	[self drawView];	
}

#pragma mark UIView section

- (void)layoutSubviews {
	[super layoutSubviews];
	[self applyMode];
	[self drawView];	
}

#pragma mark Life cycle section

- (id)initWithCoder:(NSCoder*)coder {
    if ((self = [super initWithCoder:coder])) {
        // Initialization code.
		[self setupGestureRecognizers];
		mode_ = FBControlAreaModeScaleToFit;	
		internalPanningAllowed_ = true;
		internalZoomingAllowed_ = true;			
		userPanningAllowed_ = true;
		userZoomingAllowed_ = true;					
		offset_ = CGPointMake(0.0f, 0.0f);
		zoomLevel_ = 1.0f;				
		panning_ = NO;
		maxTouches_ = 0;
		zoomRange_ = CGSizeMake(0.25f, 4.0f);
	}
	return self;
}

- (void)dealloc {
	[magicTimer_ invalidate];
	[magicTimer_ release];
    [super dealloc];
}

@end
