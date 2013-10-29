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

#import "Content.h"
#import "../../Tools/OpenGL/Texture.h"

@interface Content (PrivateMethods)

- (void)registerImage:(NSString *)filename;

@end

@implementation Content

// Fonts
@synthesize largeButtonFont;

static Content *globalContent = nil;

- (UIImage *)imageNamed:(NSString *)filename {
    return [images objectForKey:filename];
}

- (Texture *)textureNamed:(NSString *)filename {
    // Check whether texture presents in cache.
    Texture *texture = [textures objectForKey:filename];
    if ( texture )
        return texture;
    // Register new texture.
    texture = [[Texture alloc] initWithFilename:[NSString stringWithFormat:@"%@.png", filename]];
    [textures setObject:texture forKey:filename];
    return [texture autorelease];
}

// Internal life cycle tools
- (id)init {	
	if ( self = [super init] ) {			
        
        // Fonts 
        largeButtonFont = [UIFont boldSystemFontOfSize:20];
        
		// Images
		images = [[NSMutableDictionary dictionary] retain];
        
        // Misc
        [self registerImage:@"BackNormal"];
        [self registerImage:@"BackPressed"];
        
        // Hosts
        [self registerImage:@"AuthLocked"]; 
        [self registerImage:@"AuthUnlocked"]; 
		
		// Buttons
		[self registerImage:@"ButtonNormalLeft"];
		[self registerImage:@"ButtonNormalCenter"];
		[self registerImage:@"ButtonNormalRight"];
        [self registerImage:@"BackArrowLeftNormal"];
        [self registerImage:@"BackArrowRightNormal"];
		[self registerImage:@"ButtonPressedLeft"];
		[self registerImage:@"ButtonPressedCenter"];
   		[self registerImage:@"ButtonPressedRight"];
        [self registerImage:@"BackArrowLeftPressed"];
        [self registerImage:@"BackArrowRightPressed"];        
        
        // TouchPad
   		[self registerImage:@"MouseCursor"];        
        
        // Textures
        textures = [[NSMutableDictionary dictionary] retain];
    }	
	return self;	
}

// Singleton section
+ (Content *)instance {
    @synchronized(self) {
        if ( globalContent == nil ) {
            [[self alloc] init]; // assignment not done here
        }
    }
    return globalContent;
}

+ (id)allocWithZone:(NSZone *)zone {
    @synchronized(self) {
        if ( globalContent == nil ) {
            globalContent = [super allocWithZone:zone];
            return globalContent;  // assignment and return on first allocation
        }
    }
    return nil; //on subsequent allocation attempts return nil
}

- (void)dealloc {
    // Fonts
    [largeButtonFont release];
	// Images
	[images release];	
    // Textures
    [textures release];
	[super dealloc];	
}

// Private tools
- (UIImage *)createImage:(NSString *)name ofType:(NSString *)extension {	
	NSString *pathToImage = [[NSBundle mainBundle] pathForResource:name ofType:extension];	
	UIImage *image = [[UIImage alloc] initWithContentsOfFile:pathToImage];	
	return [image autorelease];	
}

- (void)registerImage:(NSString *)filename {	
	[images setObject:[self createImage:filename ofType:@"png"] forKey:filename];	
}

@end
