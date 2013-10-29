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

#ifndef RAWIMAGE_H__SFB__REMOTEPC__INCLUDED_
#define RAWIMAGE_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{
    class RawImage
    {
    public:

        RawImage(const RasterFormatPtr& format, unsigned char *data, size_t length, const MemoryCachePtr& cache);
        ~RawImage();

        inline RasterFormatPtr format() const
        {
            return format_;
        }

        inline unsigned char *data() const
        {
            return data_;
        }

        inline size_t length() const
        {
            return length_;
        }

    private:

        RasterFormatPtr format_;

        unsigned char *data_;
        size_t length_;
        bool owns_;

		MemoryCachePtr cache_;

    };

    typedef boost::shared_ptr< RawImage > RawImagePtr;
}
}

#endif // RAWIMAGE_H__SFB__REMOTEPC__INCLUDED_
