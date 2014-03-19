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

#ifndef BLOCKMESSAGE_H__MINCOM__INCLUDED_
#define BLOCKMESSAGE_H__MINCOM__INCLUDED_

#include "Messages/IScreenshotMessage.h"

namespace RemotePC
{
namespace SFB
{

    class BlockMessage
        : public mc::MessageImpl
        , public mc::CommonImpl< IScreenshotMessage >
    {
    public:

        typedef enum tagProperties_
        {
            PROP_X = 1,
            PROP_Y = 2,
            PROP_WIDTH = 3,
            PROP_HEIGHT = 4
        }
        Properties_;

        typedef mc::CommonImpl< IScreenshotMessage > ClassRoot_;

        static const mc::Guid messageId_;

        BlockMessage();

        BlockMessage(float x, float y, float width, float height, size_t dataLength, const char * data);

        virtual ~BlockMessage();

        // IScreenshotMessage section
        virtual int GetDataLength();

        virtual const char * GetData();

        // IMessage section
        virtual mc::IMessage::Size_ GetDataSize();

        // MessageImpl section
        virtual mc::result WriteBody(std::ostream& stream);

        virtual mc::result ReadBody(std::istream& stream);

    private:

        size_t dataLength_;

        char * data_;

    };

}
}

#endif // BLOCKMESSAGE_H__MINCOM__INCLUDED_
