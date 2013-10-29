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
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"
#import "GLContext.h"

#define USE_DEPTH_BUFFER 0

// A class extension to declare private methods
@interface EAGLView ()

- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;

@end

@implementation EAGLView

// You must implement this method
+ (Class)layerClass {
    return [CAEAGLLayer class];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder {    
    updateFrameBuffer = YES;
    viewFramebuffer = 0;
    viewRenderbuffer = 0;    
    if ((self = [super initWithCoder:coder])) {
        // Get and configure the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
            [NSNumber numberWithBool:NO], 
            kEAGLDrawablePropertyRetainedBacking,
            kEAGLColorFormatRGBA8, 
            kEAGLDrawablePropertyColorFormat, nil];
        // Configure context
        [[GLContext instance] setCurrentContext];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame {    
    updateFrameBuffer = YES;
    viewFramebuffer = 0;
    viewRenderbuffer = 0;    
    if ((self = [super initWithFrame:frame])) {
        // Get and configure the layer
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;        
        eaglLayer.opaque = YES;
        eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
            [NSNumber numberWithBool:NO], 
            kEAGLDrawablePropertyRetainedBacking, 
            kEAGLColorFormatRGBA8, 
            kEAGLDrawablePropertyColorFormat, nil];
        // Configure context
        [[GLContext instance] setCurrentContext];    
    }
    return self;
}

- (void)drawViewInit {
	[[GLContext instance] setCurrentContext];
	
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glViewport(0, 0, backingWidth, backingHeight);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0.0f, backingWidth, backingHeight, 0.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();    
    
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
}

- (void)draw {
}

- (void)drawViewFinal {    
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [[[GLContext instance] context] presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void)drawView {
    [self drawViewInit];    
    [self draw];
    [self drawViewFinal];    
}

- (void)setNeedsDisplay {
    if ( 0 == viewFramebuffer || 0 == viewRenderbuffer )
        return;
    [self drawView];    
}

- (void)startUpdatingFrameBuffer {
    updateFrameBuffer = YES;
}

- (void)stopUpdatingFrameBuffer {
    updateFrameBuffer = NO;    
}

- (void)layoutSubviews {
    [super layoutSubviews];
    if ( updateFrameBuffer ) {
        [self destroyFramebuffer];
        [self createFramebuffer];
    }
}

- (BOOL)createFramebuffer {    
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [[[GLContext instance] context] renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    if ( USE_DEPTH_BUFFER ) {
        glGenRenderbuffersOES(1, &depthRenderbuffer);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    }
    
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
        NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
        return NO;
    }    
    return YES;
}

- (void)destroyFramebuffer {    
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;    
    if(depthRenderbuffer) {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}

- (void)refreshFrameBuffer {
    if ( updateFrameBuffer ) {
        [self destroyFramebuffer];
        [self createFramebuffer];
    }	
}

- (void)dealloc {
    [super dealloc];
}

@end
