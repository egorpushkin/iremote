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

namespace RemotePC
{
namespace SFB
{
    Matrix::tagCellLocation_::tagCellLocation_(size_t x, size_t y)
        : x_(x)
        , y_(y)
    {
    }

    Matrix::Matrix(const RawImagePtr& image, size_t width, size_t height)
        : image_(image)
        , cells_(NULL)
        , width_(width)
        , height_(height)
        , changesList_()
        , allCells_()
    {
        init();
    }

    Matrix::~Matrix()
    {
        destroy();
    }

    bool Matrix::isChanged() const
    {
        return ( changesList_.size() > 0 );
    }

    void Matrix::init()
    {
        // Allocate cells.
        cells_ = new Cell_*[height_];
        for ( size_t i = 0 ; i < height_ ; ++i )
            cells_[i] = new Cell_[width_];
        // Initialize cells.
        size_t blockSize = image_->format()->cellSize_;
        for ( size_t i = 0 ; i < height_ ; ++i )
            for ( size_t j = 0 ; j < width_ ; ++j )
            {
                cells_[i][j].x_ = j * blockSize;
                cells_[i][j].y_ = i * blockSize;
                cells_[i][j].width_ = blockSize;
                cells_[i][j].height_ = blockSize;
                cells_[i][j].modified_ = false;
                allCells_.push_back(CellLocation_(j, i));
            }
    }

    void Matrix::destroy()
    {
        for ( size_t i = 0 ; i < height_ ; ++i )
            delete [] cells_[i];
        delete [] cells_;
    }
}
}
