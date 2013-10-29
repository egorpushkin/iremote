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

#ifndef JPEGCOMPRESSOR_H__SFB__REMOTEPC__INCLUDED_
#define JPEGCOMPRESSOR_H__SFB__REMOTEPC__INCLUDED_

extern "C" {
#ifdef WIN32
#define XMD_H
#endif
#include "jpeglib.h"
}

namespace RemotePC
{
namespace SFB
{
    class JpegCompressor
    {
    public:

        JpegCompressor();
        ~JpegCompressor();

        ByteBufferPtr compress(const RawImagePtr& image, size_t x, size_t y, size_t width, size_t height);

    private:

    public:
      void initDestination();
      bool emptyOutputBuffer();
      void termDestination();

    protected:
      static const int ALLOC_CHUNK_SIZE;

      struct jpeg_compress_struct m_cinfo;
      struct jpeg_error_mgr m_jerr;

      unsigned char *m_outputBuffer;
      size_t m_numBytesAllocated;
      size_t m_numBytesReady;

    };

    typedef boost::shared_ptr< JpegCompressor > JpegCompressorPtr;
}
}

#endif // JPEGCOMPRESSOR_H__SFB__REMOTEPC__INCLUDED_
