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

#import "ScreenshotProvider.h"
#import "../../Tools/OpenGL/Texture.h"

#include "Connector/Server.h"
#include "../State/UIConfig.h"

@implementation ScreenshotProvider

const CGFloat screenshotTimerInterval = 2.0f;
const CGFloat screenshotExpirationInterval = 3.0f;

- (BOOL)hasValidScreenshot {
    if ( !screenshotDate )
        return NO;	
    NSTimeInterval screenshotLifetime = [[NSDate date] timeIntervalSinceDate:screenshotDate];
    return ( screenshotLifetime < screenshotExpirationInterval );
}

- (Texture *)texture {
	return texture;
}

- (void)checkScreenhotExpiration:(NSTimer*)theTimer {
    if ( !screenshotDate )
        return;
    if ( ![self hasValidScreenshot] ) {
        // Screenshot has expired.
		// Invalidate last screenshot date.
		[screenshotDate release];
		screenshotDate = nil;
		// Specify what to draw.
		texture = background;
		// Notify screenshot host. 
		if ( screenshotHost )
			[screenshotHost update:NO];
    }
}

- (void)setScreenshotHost:(id<IScreenshotHost>)aScreenshotHost {
    // Check whether server is still alive.
	// TODO: Ensure that this check is necessary, as host is always set from
	// the main thread and it does not heavily depend on server state.
    RemotePC::IServerControlPtr server = RemotePC::Holder::Instance().GetServer();
    if ( !server )
        return;
    
    // Update screenshot host.
    if ( screenshotHost ) {
        [screenshotHost release];
        screenshotHost = nil;
    } 
	if ( aScreenshotHost ) {        
        screenshotHost = [aScreenshotHost retain];  
		[screenshotHost update:[self hasValidScreenshot]];
    }    
}

- (id<IScreenshotHost>)screenshotHost {
    return screenshotHost;
}

/**
 * This method should only be called if there is valid screenshot to render.
 */
- (void)updateScreenshotHostCore {
	// Update date.
	[screenshotDate release];
	screenshotDate = [[NSDate date] retain]; 	
	// Specify what to draw.
	texture = screenshot;
	// Notify host.
    if ( !screenshotHost )
		return;
	[screenshotHost update:YES];
}

- (void)updateScreenshotHost:(unsigned char*)data ofLength:(size_t)length {
	[screenshot updateFromData:data ofLength:length];	
	[self performSelectorOnMainThread:@selector(updateScreenshotHostCore) withObject:nil waitUntilDone:NO];
}

- (id)init {
    if ((self = [super init])) {
		screenshot = [[Texture alloc] initWithSize:
			CGSizeMake(RemotePC::UIConfig::screenshotWidth_, RemotePC::UIConfig::screenshotHeight_)];
		background = [[Texture alloc] initWithFilename:@"Background.png"];
		texture = background;
        // Register timer responsible for screeenshots expiration.
        screenshotTimer = [[NSTimer alloc] initWithFireDate:nil interval:screenshotTimerInterval target:self selector:@selector(checkScreenhotExpiration:) userInfo:nil repeats:YES];    
        [[NSRunLoop mainRunLoop] addTimer:screenshotTimer forMode:NSDefaultRunLoopMode];		
	}
	return self;
}

- (void)dealloc {
    [screenshotTimer invalidate];
    [screenshotTimer release];
    [screenshotDate release];	
	[screenshot release];
	[background release];
    [screenshotHost release];
    [super dealloc];
}

// Singleton section
static ScreenshotProvider *globalScreenshotProvider = nil;

+ (ScreenshotProvider *)instance {
    @synchronized(self) {
        if ( globalScreenshotProvider == nil ) {
            [[self alloc] init]; // assignment not done here
        }
    }
    return globalScreenshotProvider;
}

+ (id)allocWithZone:(NSZone *)zone {
    @synchronized(self) {
        if ( globalScreenshotProvider == nil ) {
            globalScreenshotProvider = [super allocWithZone:zone];
            return globalScreenshotProvider;  // assignment and return on first allocation
        }
    }
    return nil; //on subsequent allocation attempts return nil
}

@end
