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

namespace RemotePC
{
    
    // {89C02B34-CC92-46f0-8C95-B7A5F61BD9AD}
	const mc::Guid MoveMessage::messageId_ =  
        { 0x89c02b34, 0xcc92, 0x46f0, { 0x8c, 0x95, 0xb7, 0xa5, 0xf6, 0x1b, 0xd9, 0xad } };
        
    MoveMessage::MoveMessage()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DX, 0.0f);
        mc::MessageImpl::PutProperty(PROP_DY, 0.0f);
    }
    
    MoveMessage::MoveMessage( float dx, float dy )
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DX, dx);
        mc::MessageImpl::PutProperty(PROP_DY, dy);
    }
    
    // IMessage section
    mc::IMessage::Size_ MoveMessage::GetDataSize()
    {
        return sizeof( int ) + ( sizeof( float ) * 2 );
    }

    // MessageImpl section
    mc::result MoveMessage::WriteBody(std::ostream& stream)
    {
        float dx = mc::MessageImpl::GetProperty(PROP_DX).toFloat();
        float dy = mc::MessageImpl::GetProperty(PROP_DY).toFloat();
        
        mc::MessageImpl::Write(stream, dx);
        mc::MessageImpl::Write(stream, dy);
        
        return mc::_S_OK;
    }

    mc::result MoveMessage::ReadBody(std::istream& stream)
    {
        float dx = 0.0f;
        float dy = 0.0f;
        
        mc::MessageImpl::Read(stream, dx);
        mc::MessageImpl::Read(stream, dy);        
        
        mc::MessageImpl::PutProperty(PROP_DX, dx);
        mc::MessageImpl::PutProperty(PROP_DY, dy);
        
        return mc::_S_OK;
    }

}
