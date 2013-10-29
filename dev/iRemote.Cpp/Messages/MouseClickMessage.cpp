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

#include "MouseClickMessage.h"

namespace RemotePC
{

    // {55964DA0-074F-49FE-8934-449A4894F6D1}
    const mc::Guid MouseClickMessage::messageId_ =
        { 0x55964DA0, 0x074F, 0x49FE, { 0x89, 0x34, 0x44, 0x9A, 0x48, 0x94, 0xF6, 0xD1 } };

    MouseClickMessage::MouseClickMessage()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_X, 0.0f);
        mc::MessageImpl::PutProperty(PROP_Y, 0.0f);
    }

    MouseClickMessage::MouseClickMessage( float x, float y )
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_X, x);
        mc::MessageImpl::PutProperty(PROP_Y, y);
    }

    // IMessage section
    mc::IMessage::Size_ MouseClickMessage::GetDataSize()
    {
        return
            // Size of position property.
            sizeof( float ) * 2;
    }

    // MessageImpl section
    mc::result MouseClickMessage::WriteBody(std::ostream& stream)
    {
        float x = mc::MessageImpl::GetProperty(PROP_X).toFloat();
        float y = mc::MessageImpl::GetProperty(PROP_Y).toFloat();

        mc::MessageImpl::Write(stream, x);
        mc::MessageImpl::Write(stream, y);

        return mc::_S_OK;
    }

    mc::result MouseClickMessage::ReadBody(std::istream& stream)
    {
        float x = 0.0f;
        float y = 0.0f;

        mc::MessageImpl::Read(stream, x);
        mc::MessageImpl::Read(stream, y);

        mc::MessageImpl::PutProperty(PROP_X, x);
        mc::MessageImpl::PutProperty(PROP_Y, y);

        return mc::_S_OK;
    }

}
