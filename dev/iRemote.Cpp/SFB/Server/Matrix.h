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

#ifndef MATRIX_H__SFB__REMOTEPC__INCLUDED_
#define MATRIX_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{
    class Matrix
    {
    public:

        typedef struct tagCell_
        {
            size_t x_;
            size_t y_;
            size_t width_;
            size_t height_;

            ByteBufferPtr buffer_;

            bool modified_;
        }
        Cell_;

        typedef struct tagCellLocation_
        {
            tagCellLocation_(size_t x, size_t y);

            size_t x_;
            size_t y_;
        }
        CellLocation_;

        typedef std::vector< CellLocation_ > ChangesList_;

        Matrix(const RawImagePtr& image, size_t width, size_t height);
        ~Matrix();

        inline Cell_ ** cells() const
        {
            return cells_;
        }

        inline size_t width() const
        {
            return width_;
        }

        inline size_t height() const
        {
            return height_;
        }

        inline ChangesList_& changes()
        {
            return changesList_;
        }

        inline ChangesList_& allCells()
        {
            return allCells_;
        }

        inline RawImagePtr image() const
        {
            return image_;
        }

        bool isChanged() const;

    private:

        void init();

        void destroy();

        Matrix( const Matrix& );
        const Matrix& operator=( const Matrix& );

    private:

        RawImagePtr image_;

        Cell_ ** cells_;

        size_t width_;

        size_t height_;

        ChangesList_ changesList_;

        ChangesList_ allCells_;
    };

    typedef boost::shared_ptr< Matrix > MatrixPtr;
}
}

#endif // MATRIX_H__SFB__REMOTEPC__INCLUDED_
