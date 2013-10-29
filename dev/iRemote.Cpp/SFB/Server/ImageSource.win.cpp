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

// QT provides display wnd id.
#include <QtGui/QtGui>

namespace RemotePC
{
namespace SFB
{
	class ImageSource::Impl_
	{
	public:

		Impl_()
			: buffer_(NULL)
		    , length_(0)
		{
		}

		~Impl_()
		{
			delete [] buffer_;
		}

		unsigned char *buffer(size_t length)
		{
			if ( NULL != buffer_ )
			{
				if ( length == length_ )
				{
					return buffer_;					
				}
				delete [] buffer_;
			}
			buffer_ = new unsigned char[length];
			length_ = length;
			return buffer_;
		}

	private:

		unsigned char *buffer_;
		size_t length_;

	};

	ImageSource::ImageSource()
		: impl_(new Impl_())
	{
	}

	ImageSource::~ImageSource()
	{
		delete impl_;
	}

	RawImagePtr ImageSource::nextImage(const RasterFormatPtr& format, const MemoryCachePtr& cache)
	{
		HWND winId = QApplication::desktop()->winId();

		RECT r;
		::GetClientRect(winId, &r);
		size_t x = 0;
		size_t y = 0;
		size_t w = r.right - r.left;
		size_t h = r.bottom - r.top;

        // Retrieve display info.
        format->originalWidth_ = w;
        format->originalHeight_ = h;
        // Make the image dimensions are multiples of the size of SFB cell size.
        format->matrixWidth_ = Math<size_t>::ceil(format->originalWidth_, format->cellSize_);
        format->matrixHeight_ = Math<size_t>::ceil(format->originalHeight_, format->cellSize_);
        format->rasterWidth_ = format->matrixWidth_ * format->cellSize_;
        format->rasterHeight_ = format->matrixHeight_ * format->cellSize_;
        format->rowPixelBytes_ = format->bytesPerPixel_ * format->originalWidth_;
        format->rowTotalBytes_ = format->bytesPerPixel_ * format->rasterWidth_;

		// Create and setup bitmap
		HDC display_dc = ::GetDC(0);
		HDC bitmap_dc = ::CreateCompatibleDC(display_dc);
		HBITMAP bitmap = ::CreateCompatibleBitmap(display_dc, w, h);
		HGDIOBJ null_bitmap = ::SelectObject(bitmap_dc, bitmap);

		// Copy data
		HDC window_dc = ::GetDC(winId);
		::BitBlt(bitmap_dc, 0, 0, w, h, window_dc, x, y, SRCCOPY);

		// Clean up all but bitmap
		::ReleaseDC(winId, window_dc);
		::SelectObject(bitmap_dc, null_bitmap);
		::DeleteDC(bitmap_dc);

		// Prepare bits extraction
		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(bmi));
		bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth       = w;
		bmi.bmiHeader.biHeight      = -(int)h;
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biBitCount    = 24;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage   = w * h * 3;

		// Get bitmap bits
		unsigned char *buffer = impl_->buffer(bmi.bmiHeader.biSizeImage);

		RawImagePtr image;
		if ( GetDIBits(display_dc, bitmap, 0, h, buffer, &bmi, DIB_RGB_COLORS) ) 
		{
			// Post process image.
			size_t length = format->rowTotalBytes_ * format->rasterHeight_;
			unsigned char *data = cache->request(length);

			if ( RasterFormat::GRAYSCALE == format->format_ )
			{
				unsigned char *src = buffer;
				unsigned char *dst = data;
				size_t offset = ( format->rasterWidth_ - format->originalWidth_ ) * 3;
				for ( size_t y = 0 ; y < h ; ++y )
				{
					for ( size_t x = 0 ; x < w ; ++x )
					{
						int color = *src++;
						color += *src++;
						color += *src++;
						*dst++ = color / 3;
					}
					dst += offset;
				}
			}
			else if ( RasterFormat::BGR == format->format_ )
			{
				unsigned char *src = buffer;
				unsigned char *dst = data;
				for ( size_t y = 0 ; y < h ; ++y )
				{
					memcpy(dst, src, format->originalWidth_ * format->bytesPerPixel_);
					dst += format->rowTotalBytes_;
					src += w * 3;
				}
			}
			else
				return RawImagePtr();

			image = RawImagePtr(new RawImage(format, (unsigned char *)data, bmi.bmiHeader.biSizeImage, cache));
		}

		// Clean up bitmap
		::DeleteObject(bitmap);
		::ReleaseDC(0, display_dc);

		return image;
	}
}
}
