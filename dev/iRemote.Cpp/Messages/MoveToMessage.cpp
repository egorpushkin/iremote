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

#include "MoveMessage.h"
#include "MoveToMessage.h"

namespace RemotePC
{
    // {8D05496E-A73F-416D-BDD3-68A9CD43CAF4}
    const mc::Guid MoveToMessage::messageId_ =
        { 0x8D05496E, 0xA73F, 0x416D, { 0xBD, 0xD3, 0x68, 0xA9, 0xCD, 0x43, 0xCA, 0xF4 } };

    MoveToMessage::MoveToMessage()
        : MoveMessage()
    {
        mc::MessageImpl::SetCode(MoveToMessage::messageId_);
    }

    MoveToMessage::MoveToMessage( float x, float y )
        : MoveMessage( x, y )
    {
        mc::MessageImpl::SetCode(MoveToMessage::messageId_);
    }
}
