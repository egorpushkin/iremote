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

#ifndef RASTERFORMAT_H__SFB__REMOTEPC__INCLUDED_
#define RASTERFORMAT_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{
    class RasterFormat
    {
    public:
        typedef enum tagFormat_
        {
            UNKNOWN = -1,
            RGB = 0,
            BGR,
            RGBA,
            BGRA,
            ARGB,
            ABGR,
            GRAYSCALE
        }
        Format_;

		typedef enum tagMethod_
		{
			JPEG = 1,
			PNG
		}
		Method_;

        typedef enum tagOrientation_
        {
            NORMAL = 0,
            UPSIDEDOWN
        }
        Orientation_;

    public:

        RasterFormat();

    public:

        // Pixel properties.
        Format_ format_;
        size_t bytesPerPixel_;

        // Grid properties.
        size_t cellSize_;
        size_t matrixWidth_;
        size_t matrixHeight_;

        // Raster properties.
        Orientation_ orientation_;

        size_t originalWidth_;
        size_t originalHeight_;
        size_t rasterWidth_;
        size_t rasterHeight_;

        size_t rowPixelBytes_;
        size_t rowTotalBytes_;

		// Transmission properties.
		Method_ method_;
		unsigned char quality_;
    };

    typedef boost::shared_ptr< RasterFormat > RasterFormatPtr;
}
}

#endif // RASTERFORMAT_H__SFB__REMOTEPC__INCLUDED_
