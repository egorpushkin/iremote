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

#import "FBSurfaceHolder.h"

static const float Setting_ServerCursorSupressionInterval = 5.0f;

@interface SurfaceInitializer : NSObject {            
	RemotePC::SFB::SurfacePtr surface_;
}

- (id)initWithSurface:(const RemotePC::SFB::SurfacePtr&)surface;
- (RemotePC::SFB::SurfacePtr)surface;

@end

@implementation SurfaceInitializer

- (id)initWithSurface:(const RemotePC::SFB::SurfacePtr&)surface {
    if ((self = [super init])) {
        surface_ = surface;
    }    
    return self;
}

- (RemotePC::SFB::SurfacePtr)surface {
	return surface_;
}

- (void)dealloc {
    [super dealloc];    
}

@end

@implementation FBSurfaceHolder

- (void)subscribe:(id<IFBControl>)control {
	[controls_ addObject:control];
	[control initControl];
}

- (void)unsubscribe:(id<IFBControl>)control {
	[controls_ removeObject:control];
}

- (void)unsubscribeSafely:(id<IFBControl>)control {
	[self performSelectorOnMainThread:@selector(unsubscribe:) withObject:control waitUntilDone:NO];	
}

- (RemotePC::SFB::SurfacePtr)surface {
	return surface_;
}

- (void)loadResources {
	background_ = RemotePC::SFB::TexturePtr(new RemotePC::SFB::Texture("Background.png"));
	cursor_ = RemotePC::SFB::TexturePtr(new RemotePC::SFB::Texture("MouseCursorGL.png"));
}

- (void)releaseResources {
	background_ = RemotePC::SFB::TexturePtr();
	cursor_ = RemotePC::SFB::TexturePtr();
}

- (RemotePC::SFB::TexturePtr)background {
	return background_;
}

- (RemotePC::SFB::TexturePtr)cursor {
	return cursor_;
}

#pragma mark IFBControl section

- (void)initControlsCore:(SurfaceInitializer *)initializer {
	surface_ = [initializer surface];
	[initializer release];
	for ( id<IFBControl> control in controls_ ) {
		[control initControl];
	}	
}

- (void)updateControlsCore {
	for ( id<IFBControl> control in controls_ ) {
		[control updateControl];
	}		
}

- (void)updateCursorLocal:(BOOL)server {
	if ( server ) {
		// Surpress server notifications sent shortly after local cursor  
		// modifications.
		NSTimeInterval intervalSinceLastUserChange = -[lastUserCursorChangeTime_ timeIntervalSinceNow];
		if ( intervalSinceLastUserChange < Setting_ServerCursorSupressionInterval )
			return;
		// Update cursor visible to UI controls.
		surface_->cursor()->set(surface_->internalCursor());
	} else {
		[lastUserCursorChangeTime_ release];
		lastUserCursorChangeTime_ = [[NSDate alloc] init];
	}	 
	// Notify controls.
	for ( id<IFBControl> control in controls_ ) {
		[control updateCursor:server];
	}			
}

- (void)updateCursorCore:(NSNumber *)server {
	[self updateCursorLocal:[server boolValue]];
	[server release];
}

- (void)initControls:(const RemotePC::SFB::SurfacePtr&)surface {
	SurfaceInitializer *initializer = [[SurfaceInitializer alloc] initWithSurface:surface];
	[self performSelectorOnMainThread:@selector(initControlsCore:) withObject:initializer waitUntilDone:NO];	
}

- (void)updateControls {
	[self performSelectorOnMainThread:@selector(updateControlsCore) withObject:nil waitUntilDone:NO];
}

- (void)updateCursor:(BOOL)server {
	if ( server ) {
		NSNumber *serverNum = [[NSNumber alloc] initWithBool:server];
		[self performSelectorOnMainThread:@selector(updateCursorCore:) withObject:serverNum waitUntilDone:NO];
	} else {
		// Local notifications are always sent from UI thread already.
		// There is no need to dispatch it. 
		[self updateCursorLocal:server];
	}
}

#pragma mark Life cycle section

- (id)init {
    if ((self = [super init])) {
		controls_ = [[NSMutableArray alloc] initWithCapacity:0];
		lastUserCursorChangeTime_ = [[NSDate alloc] initWithTimeIntervalSinceNow:-10.0];
		[self loadResources];
	}
	return self;
}

- (void)dealloc {
	[self releaseResources];
	[lastUserCursorChangeTime_ release];
	[controls_ release];
    [super dealloc];
}

#pragma mark Singleton section

static FBSurfaceHolder *globalFBSurfaceHolder = nil;

+ (FBSurfaceHolder *)instance {
    @synchronized(self) {
        if ( globalFBSurfaceHolder == nil ) {
            [[self alloc] init]; // assignment not done here
        }
    }
    return globalFBSurfaceHolder;
}

+ (id)allocWithZone:(NSZone *)zone {
    @synchronized(self) {
        if ( globalFBSurfaceHolder == nil ) {
            globalFBSurfaceHolder = [super allocWithZone:zone];
            return globalFBSurfaceHolder;  // assignment and return on first allocation
        }
    }
    return nil; // on subsequent allocation attempts return nil
}

@end
