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

#import "Texture.h"

@interface Texture (PrivateMethods) 
- (void)loadTextureCore:(CGImageRef)textureImage;
- (void)unloadTextureCore;
@end

@implementation Texture

static const GLuint wrongTexture = -1;

@synthesize textureId;
@synthesize width;
@synthesize height;

// Internal life cycle tools
- (id)initWithSize:(CGSize)size {
    if ( self = [super init] ) {    	
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);			
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
		
		width = size.width;
		height = size.height;		
		size_t length = width * height * 4;
		unsigned char *data = new unsigned char[length];
		memset(data, 0xff, length);
		[self updateFromRawData:data];
		delete[]data;		
	}
	return self;
}

- (void)updateFromRawData:(unsigned char *)data {
	glBindTexture(GL_TEXTURE_2D, textureId);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);	
}

- (void)updateFromData:(unsigned char*)data ofLength:(size_t)length {
	// Construct data provider from data.
	CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, data, length, NULL);
	if ( !dataProvider ) 
		return;
	
	// Construct image from data.
	CGImageRef image = NULL;
	// Test png
	if ( !memcmp(data + 1, "PNG", 3) )
		image = CGImageCreateWithPNGDataProvider(dataProvider, NULL, true, kCGRenderingIntentDefault);
	// Test jpg
	else if ( !memcmp(data + 6, "JFIF", 3) )
		image = CGImageCreateWithJPEGDataProvider(dataProvider, NULL, true, kCGRenderingIntentDefault);
	CFRelease(dataProvider);
	if ( !image )
		return;
	
	width = CGImageGetWidth(image);
	height = CGImageGetHeight(image);
	GLubyte* rawData = new GLubyte[width * height * 4];
	
	CGContextRef context = CGBitmapContextCreate(
		rawData, width, height, CGImageGetBitsPerComponent(image), width * 4, CGImageGetColorSpace(image), kCGImageAlphaPremultipliedLast);
	if ( !context ) 
	{ 
		delete []rawData; 
		CFRelease(image);
		return; 
	}
	
	CGContextDrawImage(context, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), image);	
	CGContextRelease(context);
	
	[self updateFromRawData:rawData];
	
	delete [] rawData;
	
	CFRelease(image);
}

- (id)initWithFilename:(NSString *)filename {
    if ( self = [super init] ) {    
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFStringRef filepath = CFStringCreateWithCString(NULL, [filename UTF8String], kCFStringEncodingUTF8);
        CFURLRef fileurl = CFBundleCopyResourceURL(mainBundle, filepath, NULL, NULL);
        CFRelease(filepath);
        if ( !fileurl ) {
            [self release];
            return nil;
        }
        CGDataProviderRef provider = CGDataProviderCreateWithURL(fileurl);
        CFRelease(fileurl);
        if ( !provider ) {
            [self release];
            return nil;
        }
        CGImageRef image = CGImageCreateWithPNGDataProvider(provider, NULL, true, kCGRenderingIntentDefault);
        if ( !image ) {
            [self release];
            return nil;
        }        
        // Load texture.
		textureId = wrongTexture;
		width = 0;
		height = 0;
        [self loadTextureCore:image];       
        
        CFRelease(image);
    }
    return self;    
}

- (id)initWithData:(const char*)data ofLength:(size_t)length {
    if ( self = [super init] ) {
        // Construct data provider from data.
        CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, data, length, NULL);
        if ( !dataProvider ) {
            [self release];
            return nil;
        }
        
        // Construct image from data.
        CGImageRef image = NULL;
        // Test png
        if ( !memcmp(data + 1, "PNG", 3) ) {
            image = CGImageCreateWithPNGDataProvider(dataProvider, NULL, true, kCGRenderingIntentDefault);
        }
        // Test jpg
        else if ( !memcmp(data + 6, "JFIF", 3) ) {
            image = CGImageCreateWithJPEGDataProvider(dataProvider, NULL, true, kCGRenderingIntentDefault);
        }
        CFRelease(dataProvider);
        if ( !image ) {
            [self release];
            return nil;
        }
        
		textureId = wrongTexture;
		width = 0;
		height = 0;
        [self loadTextureCore:image];        

        CFRelease(image);
    }
    return self;
}

- (id)initDelayedWithData:(const char*)data ofLength:(size_t)length {
    if ( self = [super init] ) {
        dataLength = length;
        localData = new char[dataLength];
        memcpy(localData, data, dataLength);
    }
    return self;
}

- (void)loadTexture {
    // Construct data provider from data.
    CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, localData, dataLength, NULL);
    if ( !dataProvider ) {
        return;
    }
    
    // Construct image from data.
    CGImageRef image = NULL;
    // Test png
    if ( !memcmp(localData + 1, "PNG", 3) ) {
        image = CGImageCreateWithPNGDataProvider(dataProvider, NULL, true, kCGRenderingIntentDefault);
    }
    // Test jpg
    else if ( !memcmp(localData + 6, "JFIF", 3) ) {
        image = CGImageCreateWithJPEGDataProvider(dataProvider, NULL, true, kCGRenderingIntentDefault);
    }
    CFRelease(dataProvider);
    if ( !image ) {
        return;
    }
    
    textureId = wrongTexture;
    width = 0;
    height = 0;
    [self loadTextureCore:image];        
    
    CFRelease(image);    
    delete [] localData;
}

- (id)initWithCGImage:(CGImageRef)textureImage {
    if ( self = [super init] ) {			
		textureId = wrongTexture;
		width = 0;
		height = 0;
        [self loadTextureCore:textureImage];
	}	
	return self;
}

- (void)dealloc {
	[self unloadTextureCore];
	[super dealloc];	
}

- (void)loadTextureCore:(CGImageRef)textureImage {	
	// Generate identifier for new texture
	glGenTextures(1, &textureId);
	if ( GL_NO_ERROR != glGetError() ) return;
	
	// Bind texture image to newly generated id
    BOOL success = [Texture bindTexture:textureImage toId:textureId];
	if ( success ) {
		width = CGImageGetWidth(textureImage);
		height = CGImageGetHeight(textureImage);
	}
}

- (void)unloadTextureCore {
	glDeleteTextures(1, &textureId);
	textureId = wrongTexture;
}

+ (BOOL)bindTexture:(CGImageRef)textureImage toId:(GLuint)textureId {		
	size_t width = CGImageGetWidth(textureImage);
	size_t height = CGImageGetHeight(textureImage);
	GLubyte* screenData = new GLubyte[width * height * 4];
	
	CGContextRef textureContext = CGBitmapContextCreate(
        screenData, width, height, CGImageGetBitsPerComponent(textureImage), width * 4, CGImageGetColorSpace(textureImage), kCGImageAlphaPremultipliedLast);
	if ( !textureContext ) { delete []screenData; return NO; }
	
    CGContextClearRect(textureContext, CGRectMake(0.0, 0.0, width, height));
	CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), textureImage);	
	CGContextRelease(textureContext);
	
	glBindTexture(GL_TEXTURE_2D, textureId);
	if ( GL_NO_ERROR != glGetError() ) { delete []screenData; return NO; }
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenData);
	if ( GL_NO_ERROR != glGetError() ) { delete []screenData; return NO; }
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	if ( GL_NO_ERROR != glGetError() ) { delete []screenData; return NO; }
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	if ( GL_NO_ERROR != glGetError() ) { delete []screenData; return NO; }
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if ( GL_NO_ERROR != glGetError() ) { delete []screenData; return NO; }
    
	delete []screenData;    
	
	return YES;
}

@end
