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
#import <OpenGLES/ES1/gl.h>

@interface Texture : NSObject {
	GLuint textureId;
	size_t width;
	size_t height;
    
    char *localData;
    size_t dataLength;
}

@property (nonatomic, readonly) GLuint textureId; 
@property (nonatomic, readonly) size_t width; 
@property (nonatomic, readonly) size_t height; 

- (id)initWithSize:(CGSize)size;
- (id)initWithFilename:(NSString *)filename;
- (id)initWithCGImage:(CGImageRef)textureImage;
- (id)initWithData:(const char*)data ofLength:(size_t)length;

- (void)updateFromRawData:(unsigned char *)data;
- (void)updateFromData:(unsigned char*)data ofLength:(size_t)length;

/**
 * Does not bind texture imeediately. Subsequet call to loadTexture is required.
 */  
- (id)initDelayedWithData:(const char*)data ofLength:(size_t)length;
- (void)loadTexture;

+ (BOOL)bindTexture:(CGImageRef)textureImage toId:(GLuint)textureId;

@end
