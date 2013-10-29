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

#include "InitMessage.h"

namespace RemotePC
{
namespace SFB
{

    // {9723C2E3-DA7C-453E-BA15-4E6B268403FB}
    const mc::Guid InitMessage::messageId_ =
        { 0x9723C2E3, 0xDA7C, 0x453E, { 0xBA, 0x15, 0x4E, 0x6B, 0x26, 0x84, 0x03, 0xFB } };

    InitMessage::InitMessage()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_CELLSIZE, (int)0);
        mc::MessageImpl::PutProperty(PROP_DEVICEIDS, "");
    }

    InitMessage::InitMessage(size_t cellSize, const std::string& ids)
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_CELLSIZE, cellSize);
        mc::MessageImpl::PutProperty(PROP_DEVICEIDS, ids);        
    }

    // IMessage section
    mc::IMessage::Size_ InitMessage::GetDataSize()
    {
        return
            // Size of cell size property
            sizeof( int )
            + sizeof( size_t ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_DEVICEIDS).toString() );
    }

    // MessageImpl section
    mc::result InitMessage::WriteBody(std::ostream& stream)
    {
        size_t size = mc::MessageImpl::GetProperty(PROP_CELLSIZE).toInt();
        std::string ids = mc::MessageImpl::GetProperty(PROP_DEVICEIDS).toString();

        mc::MessageImpl::Write(stream, size);
        mc::MessageImpl::Write(stream, ids);        

        return mc::_S_OK;
    }

    mc::result InitMessage::ReadBody(std::istream& stream)
    {
        size_t size = 0;
        std::string ids;

        mc::MessageImpl::Read(stream, size);
        mc::MessageImpl::Read(stream, ids);

		mc::MessageImpl::PutProperty(PROP_CELLSIZE, size);	
        mc::MessageImpl::PutProperty(PROP_DEVICEIDS, ids);

        return mc::_S_OK;
    }

}
}
