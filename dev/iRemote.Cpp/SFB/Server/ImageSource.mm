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

#include "RasterFormat.h"
#include "MemoryCache.h"
#include "RawImage.h"
#include "ImageSource.h"
#include "Math.h"

#import <CoreFoundation/CoreFoundation.h>
#import <ApplicationServices/ApplicationServices.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>

namespace RemotePC
{
namespace SFB
{
    ImageSource::ImageSource()
        : impl_(NULL)
    {
    }

    ImageSource::~ImageSource()
    {
    }

    RawImagePtr ImageSource::nextImage(const RasterFormatPtr& format, const MemoryCachePtr& cache)
    {
        // Prepare display properties.
        CGLPixelFormatAttribute attribs[] =
        {
            kCGLPFAFullScreen,
            kCGLPFADisplayMask,
            (CGLPixelFormatAttribute)0,    /* Display mask bit goes here */
            (CGLPixelFormatAttribute)0
        };
        CGDirectDisplayID display = CGMainDisplayID();
        if ( display == kCGNullDirectDisplay )
            display = CGMainDisplayID();
        attribs[2] = (CGLPixelFormatAttribute)CGDisplayIDToOpenGLDisplayMask(display);

        // Build a full-screen GL context.
        CGLPixelFormatObj pixelFormatObj;
        GLint numPixelFormats;
        CGLChoosePixelFormat(attribs, &pixelFormatObj, &numPixelFormats);
        if ( pixelFormatObj == NULL )
            // No full screen context support
            return RawImagePtr();
        CGLContextObj glContextObj;
        CGLCreateContext(pixelFormatObj, NULL, &glContextObj);
        CGLDestroyPixelFormat(pixelFormatObj);
        if ( glContextObj == NULL )
            return RawImagePtr();

        // Set context.
        CGLSetCurrentContext(glContextObj);
        CGLSetFullScreen(glContextObj);

        // Retrieve display info.
        format->originalWidth_ = CGDisplayPixelsWide(display);
        format->originalHeight_ = CGDisplayPixelsHigh(display);
        // Make the image dimensions are multiples of the size of SFB cell size.
        format->matrixWidth_ = Math<size_t>::ceil(format->originalWidth_, format->cellSize_);
        format->matrixHeight_ = Math<size_t>::ceil(format->originalHeight_, format->cellSize_);
        format->rasterWidth_ = format->matrixWidth_ * format->cellSize_;
        format->rasterHeight_ = format->matrixHeight_ * format->cellSize_;
        format->rowPixelBytes_ = format->bytesPerPixel_ * format->originalWidth_;
        format->rowTotalBytes_ = format->bytesPerPixel_ * format->rasterWidth_;

        // Allocate buffer to store image raster.
        size_t length = format->rowTotalBytes_ * format->rasterHeight_;
        unsigned char *data = cache->request(length);
        unsigned char *pixelData = data
            + ( format->rasterHeight_ - format->originalHeight_ ) * format->rowTotalBytes_;

        // Read framebuffer into our bitmap
        glReadBuffer(GL_FRONT);
        glFinish(); // Finish all OpenGL commands
        glPixelStorei(GL_PACK_ALIGNMENT, 4); // Force 4-byte alignment
        glPixelStorei(GL_PACK_ROW_LENGTH, format->rasterWidth_);
        glPixelStorei(GL_PACK_SKIP_ROWS, 0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

        // Set RGB to GRAYSCALE color transformation matrix.
        if ( RasterFormat::GRAYSCALE == format->format_ )
        {
            GLfloat matrix[] =
                { 0.33f, 0.0f, 0.0f, 0.0f,
                  0.34f, 0.0f, 0.0f, 0.0f,
                  0.33f, 0.0f, 0.0f, 0.0f,
                  0.0f, 0.0f, 0.0f, 1.0f };
            glMatrixMode(GL_COLOR);
            glLoadMatrixf(matrix);
        }

        // Fetch the data in the requested format.
        GLenum glFormat;
        GLenum glType;
        if ( RasterFormat::RGB == format->format_ )
        {
            glFormat = GL_RGB;
            glType = GL_UNSIGNED_BYTE;
        }
        else if ( RasterFormat::GRAYSCALE == format->format_ )
        {
            glFormat = GL_LUMINANCE;
            glType = GL_UNSIGNED_BYTE;
        }
        else
            return RawImagePtr();

        glReadPixels(0, 0, format->originalWidth_, format->originalHeight_,
            glFormat, glType, pixelData);

        // Construct image based on data.
        RawImagePtr rawImage(new RawImage(format, data, length, cache));

        // Get rid of GL context.
        CGLSetCurrentContext(NULL);
        CGLClearDrawable(glContextObj); // disassociate from full screen
        CGLDestroyContext(glContextObj); // and destroy the context

        return rawImage;
    }
}
}
