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

#import "GLContext.h"

@implementation GLContext

// Context tools
- (void)setCurrentContext {
    if ( [EAGLContext currentContext] != context )
        [EAGLContext setCurrentContext:context];
}

- (EAGLContext *)context {
    return context;
}

// Internal life cycle tools
static GLContext *globalGLContext = nil;

- (id)init {	
	if ( self = [super init] ) {			        
        // Create context and make it current for caller thread.
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
        if ( !context || ![EAGLContext setCurrentContext:context] ) {
            [self release];
            return nil;
        }        
    }	
	return self;	
}

- (void)dealloc {
    if ( [EAGLContext currentContext] == context ) {
        [EAGLContext setCurrentContext:nil];
    }
    [context release];
	[super dealloc];	
}

// Singleton section
+ (GLContext *)instance {
    @synchronized(self) {
        if ( globalGLContext == nil ) {
            [[self alloc] init]; // assignment not done here
        }
    }
    return globalGLContext;
}

+ (id)allocWithZone:(NSZone *)zone {
    @synchronized(self) {
        if ( globalGLContext == nil ) {
            globalGLContext = [super allocWithZone:zone];
            return globalGLContext;  // assignment and return on first allocation
        }
    }
    return nil; //on subsequent allocation attempts return nil
}

@end
