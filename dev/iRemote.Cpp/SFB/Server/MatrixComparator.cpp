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
#include "Matrix.h"
#include "MatrixComparator.h"

namespace RemotePC
{
namespace SFB
{

    MatrixComparator::MatrixComparator()
    {
    }

    void MatrixComparator::compare(const MatrixPtr& current, const MatrixPtr& next)
    {
        // Check whether matrixes are equal.
        if ( ( current->width() != next->width() ) ||
             ( current->height() != next->height() ) )
            return;
        // Compare matrixes.
        Matrix::Cell_ **nextCells = next->cells();
        size_t width = current->width();
        size_t height = current->height();
        size_t blockSize = current->image()->format()->cellSize_;
        size_t blockWidthBytes = blockSize * current->image()->format()->bytesPerPixel_;
        for ( size_t i = 0 ; i < height ; ++i )
            for ( size_t j = 0 ; j < width ; ++j )
            {
                bool changed = false;
                for ( size_t line = 0 ; line < blockSize ; ++line )
                {
                    size_t imageX = j * blockSize;
                    size_t imageY = i * blockSize + line;
                    if ( RasterFormat::UPSIDEDOWN == current->image()->format()->orientation_ )
                    {
                        imageY = current->image()->format()->rasterHeight_ - imageY - 1;
                    }
                    size_t offset
                        = imageY * current->image()->format()->rowTotalBytes_
                        + imageX * current->image()->format()->bytesPerPixel_;
                    int isEquals = memcmp(
                        current->image()->data() + offset, next->image()->data() + offset,
                        blockWidthBytes);
                    if ( 0 != isEquals )
                    {
                        changed = true;
                        break;
                    }
                }
                if ( changed )
                {
                    // Mark cell as modified.
                    nextCells[i][j].modified_ = true;
                    // Add cell to the list of modified cells.
                    next->changes().push_back(Matrix::CellLocation_(j, i));
                }
            }
    }
}
}
