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

#ifndef MATH_H__SFB__REMOTEPC__INCLUDED_
#define MATH_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{
    template <class T>
    class Math
    {
    public:
        static inline T nextHigherPowerOf2(T k)
        {
            --k;
            for ( int i = 1 ; i < (int)sizeof(T)*CHAR_BIT ; i <<= 1 )
                k = k | k >> i;
            return k + 1;
        }

        static inline T power2Value(T k)
        {
            T value = 0;
            while ( k > 1 )
            {
                k = k >> 1;
                ++value;
            }
            return value;
        }

        static inline T ceil(T dividend, T divider)
        {
            return
                ( dividend / divider ) + ( ( 0 == ( dividend % divider ) ) ? ( 0 ) : ( 1 ) );
        }
    };
}
}

#endif // MATH_H__SFB__REMOTEPC__INCLUDED_
