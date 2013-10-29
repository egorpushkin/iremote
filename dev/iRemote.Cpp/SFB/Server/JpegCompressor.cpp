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
#include "ByteBuffer.h"
#include "JpegCompressor.h"

namespace RemotePC
{
namespace SFB
{
    const int JpegCompressor::ALLOC_CHUNK_SIZE = 65536;

    typedef struct
    {
        struct jpeg_destination_mgr pub;
        JpegCompressor *_this;
    }
    my_destination_mgr;

    static void
    init_destination(j_compress_ptr cinfo)
    {
      my_destination_mgr *dest_ptr = (my_destination_mgr *)cinfo->dest;
      dest_ptr->_this->initDestination();
    }

    static boolean
    empty_output_buffer (j_compress_ptr cinfo)
    {
      my_destination_mgr *dest_ptr = (my_destination_mgr *)cinfo->dest;
      return (boolean)dest_ptr->_this->emptyOutputBuffer();
    }

    static void
    term_destination (j_compress_ptr cinfo)
    {
      my_destination_mgr *dest_ptr = (my_destination_mgr *)cinfo->dest;
      dest_ptr->_this->termDestination();
    }

    JpegCompressor::JpegCompressor()
        : m_outputBuffer(0)
		, m_numBytesAllocated(0)
		, m_numBytesReady(0)
    {
        m_cinfo.err = jpeg_std_error(&m_jerr);
        jpeg_create_compress(&m_cinfo);

        my_destination_mgr *dest = new my_destination_mgr;
        dest->pub.init_destination = init_destination;
        dest->pub.empty_output_buffer = empty_output_buffer;
        dest->pub.term_destination = term_destination;
        dest->_this = this;
        m_cinfo.dest = (jpeg_destination_mgr *)dest;
        m_cinfo.dct_method = JDCT_FASTEST;
    }

    JpegCompressor::~JpegCompressor()
    {
        if ( m_outputBuffer )
            delete [] m_outputBuffer;

        delete m_cinfo.dest;
        m_cinfo.dest = NULL;

        jpeg_destroy_compress(&m_cinfo);
    }

    void JpegCompressor::initDestination()
    {
        if ( !m_outputBuffer )
        {
            size_t newSize = ALLOC_CHUNK_SIZE;
            m_outputBuffer = new unsigned char[newSize];
            m_numBytesAllocated = newSize;
        }
        m_numBytesReady = 0;
        m_cinfo.dest->next_output_byte = m_outputBuffer;
        m_cinfo.dest->free_in_buffer =  m_numBytesAllocated;
    }

    bool JpegCompressor::emptyOutputBuffer()
    {
        size_t oldSize = m_numBytesAllocated;
        size_t newSize = oldSize + ALLOC_CHUNK_SIZE;

        unsigned char *newOutputBuffer = new unsigned char[newSize];
        memcpy(newOutputBuffer, m_outputBuffer, oldSize);
        delete [] m_outputBuffer;
        m_outputBuffer = newOutputBuffer;

        m_numBytesAllocated = newSize;

        m_cinfo.dest->next_output_byte = &m_outputBuffer[oldSize];
        m_cinfo.dest->free_in_buffer = newSize - oldSize;

        return true;
    }

    void JpegCompressor::termDestination()
    {
        m_numBytesReady = m_numBytesAllocated - m_cinfo.dest->free_in_buffer;
    }

    ByteBufferPtr JpegCompressor::compress(const RawImagePtr& image, size_t x, size_t y, size_t width, size_t height)
    {
        RasterFormat::Format_ format = image->format()->format_;
        if ( RasterFormat::GRAYSCALE == format )
        {
            m_cinfo.input_components = 1;
            m_cinfo.in_color_space = JCS_GRAYSCALE;
        }
        else if ( RasterFormat::RGB == format )
        {
            m_cinfo.input_components = 3;
            m_cinfo.in_color_space = JCS_RGB;
        }
		else if ( RasterFormat::BGR == format )
		{
			m_cinfo.input_components = 3;
			m_cinfo.in_color_space = JCS_EXT_BGR;
		}
        else
            return ByteBufferPtr();
        jpeg_set_defaults(&m_cinfo);

        m_cinfo.image_width = width;
        m_cinfo.image_height = height;

		jpeg_set_quality(&m_cinfo, image->format()->quality_, true);

        jpeg_start_compress(&m_cinfo, TRUE);

        const char *src;
        size_t stride;
        if ( RasterFormat::NORMAL == image->format()->orientation_ )
        {
            src = (const char *)image->data()
                + ( y * image->format()->rowTotalBytes_ )
                + ( x * image->format()->bytesPerPixel_ );
            stride = image->format()->rowTotalBytes_;
        }
        else
        {
            src = (const char *)image->data()
                + ( image->format()->rowTotalBytes_ * ( image->format()->rasterHeight_ - y - 1 ) )
                + ( x * image->format()->bytesPerPixel_ );
            stride = -image->format()->rowTotalBytes_;
        }

        JSAMPLE *rgb = new JSAMPLE[width * m_cinfo.input_components * 8];
        JSAMPROW rowPointer[8];
        for (int i = 0; i < 8; i++)
            rowPointer[i] = &rgb[width * m_cinfo.input_components * i];

        while ( m_cinfo.next_scanline < m_cinfo.image_height )
        {
            int maxRows = m_cinfo.image_height - m_cinfo.next_scanline;
            if (maxRows > 8)
            {
                maxRows = 8;
            }
            for ( int dy = 0; dy < maxRows; dy++ )
            {
                memcpy(rowPointer[dy], src, width * m_cinfo.input_components);
                src += stride;
            }
            jpeg_write_scanlines(&m_cinfo, rowPointer, maxRows);
        }

        delete [] rgb;

        jpeg_finish_compress(&m_cinfo);

        ByteBufferPtr buffer(new ByteBuffer(m_outputBuffer, m_numBytesReady, true));
        m_outputBuffer = NULL;
        return buffer;
    }
}
}


