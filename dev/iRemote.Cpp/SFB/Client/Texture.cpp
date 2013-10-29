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

#include "Common/Common.h"
#include "Texture.h"

#include <OpenGLES/ES1/gl.h>
#include <CoreGraphics/CoreGraphics.h>

namespace RemotePC
{
namespace SFB
{
	
	Texture::Texture(size_t width, size_t height)
		: id_(wrongTexture_) 
		, width_(width)
		, height_(height)
	    , bytesPerPixel_(4)
		, sync_(mc::Library::Mutex())
	    , initialized_(false)
		, buffer_(NULL)
		, length_(0)
	{
		generate();
		
		// For now, textures are not filled with white by default. 
		// Texture is not rendered, untill it is filled with real data.
		// createEmpty();
	}
	
	Texture::Texture(unsigned char *data, size_t length)
		: id_(wrongTexture_) 
		, width_(0)
		, height_(0)
		, bytesPerPixel_(4)
		, sync_(mc::Library::Mutex())
	    , initialized_(false)
		, buffer_(NULL)
		, length_(0)
	{
		generate();
		updateFromData(data, length);
	}
	
	Texture::~Texture()
	{
		glDeleteTextures(1, &id_);
		delete [] buffer_;
	}
	
	size_t Texture::width() const
	{
		return width_;
	}
	
	size_t Texture::height() const
	{
		return height_;
	}
	
	void Texture::draw(float x, float y, float width, float height)
	{
		if ( !initialized_ )
			return;
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
		glColor4ub(255, 255, 255, 255);	 
		
		GLfloat	coordinates[] = { 
			0.0f,	0.0f,
			1.0f,   0.0f, 
			0.0f,	1.0f,
			1.0f,	1.0f };
		
		GLfloat	vertices[] = {	
			x,			y,						
			x + width,	y,						
			x,			y + height,	
			x + width,	y + height };
		
		glBindTexture(GL_TEXTURE_2D, id_);
		{
			// Check whether there is data to be rendered into the texture.
			if ( buffer_ )
			{
				mc::MutexScope locker(sync_);				
				if ( buffer_ )
				{
					GLint format = ( 1 == bytesPerPixel_ ) ? ( GL_LUMINANCE ) : ( GL_RGBA );
					glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, buffer_);       				
					delete [] buffer_;
					buffer_ = NULL;
					length_ = 0;
				}
			}
		}
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	
		
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);	
	}
	
	void Texture::updateFromRawData(unsigned char *data, size_t length)
	{
		mc::MutexScope locker(sync_);
		// Check whether previous buffer has not been processed yet. 
		if ( buffer_ )
		{
			// Check whether it is possible to reuse the buffer.
			if ( length == length_ )
			{
				// Reuse old buffer. The work is done below.
			}
			else
			{
				// Free old buffer.
				delete [] buffer_;
				buffer_ = NULL;
			}
		}
		// Allocate new buffer if necessary and copy data into it.
		length_ = length;
		if ( !buffer_ )
		{
			buffer_ = new unsigned char[length_];
		}
		memcpy(buffer_, data, length_);
		
		// Mark that texture has data to draw. 
		initialized_ = true;
		
		// The code below is not thread safe. Sometimes it crashes inside glTexImage2D,
		// when other thread calls glDrawArrays or swaps rendering buffers.
        // glBindTexture(GL_TEXTURE_2D, id_);	
		// GLint format = ( 1 == bytesPerPixel_ ) ? ( GL_LUMINANCE ) : ( GL_RGBA );
		// glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, data);        
	}
	
	void Texture::updateFromData(unsigned char *data, size_t length)
	{
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
        
		size_t bitsPerComponent = CGImageGetBitsPerComponent(image);
		bytesPerPixel_ = CGImageGetBitsPerPixel(image) / bitsPerComponent;
		width_ = CGImageGetWidth(image);
		height_ = CGImageGetHeight(image);
		size_t rawLength = width_ * height_ * bytesPerPixel_;
		GLubyte* rawData = new GLubyte[rawLength];
		
		CGContextRef context = CGBitmapContextCreate(
			rawData, width_, height_, bitsPerComponent, width_ * bytesPerPixel_, CGImageGetColorSpace(image), 
			( 1 == bytesPerPixel_ ) ? ( kCGImageAlphaNone ) : ( kCGImageAlphaPremultipliedLast ));
		if ( !context ) 
		{ 
			delete []rawData; 
			CFRelease(image);
			return; 
		}
		
		CGContextClearRect(context, CGRectMake(0.0, 0.0, width_, height_));
		CGContextDrawImage(context, CGRectMake(0.0, 0.0, (CGFloat)width_, (CGFloat)height_), image);	
		CGContextRelease(context);
		
		updateFromRawData(rawData, rawLength);
		
		delete [] rawData;
		
        CFRelease(image);		
	}
	
	void Texture::generate()
	{
		glGenTextures(1, &id_);
		glBindTexture(GL_TEXTURE_2D, id_);			
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		
	}
	
	void Texture::createEmpty()
	{
		size_t length = width_ * height_ * bytesPerPixel_;
		unsigned char *data = new unsigned char[length];
		memset(data, 0xff, length);
		updateFromRawData(data, length);
		delete[]data;
	}
	
}
}

		