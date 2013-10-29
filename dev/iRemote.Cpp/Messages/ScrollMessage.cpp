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

#include "ScrollMessage.h"

namespace RemotePC
{

	// {4378A387-FA89-404a-B022-D040FDC3A32F}
	const mc::Guid ScrollMessage::messageId_ =  
        { 0x4378a387, 0xfa89, 0x404a, { 0xb0, 0x22, 0xd0, 0x40, 0xfd, 0xc3, 0xa3, 0x2f } };
        
    ScrollMessage::ScrollMessage()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_ORIENTATION, false);
		mc::MessageImpl::PutProperty(PROP_VALUE, (int)0);
    }
    
	ScrollMessage::ScrollMessage( bool vertical, int value )
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_ORIENTATION, vertical);
		mc::MessageImpl::PutProperty(PROP_VALUE, value);
    }
    
    // IMessage section
    mc::IMessage::Size_ ScrollMessage::GetDataSize()
    {
        return sizeof( bool ) + sizeof( int );
    }

    // MessageImpl section
    mc::result ScrollMessage::WriteBody(std::ostream& stream)
    {
		bool vertical = mc::MessageImpl::GetProperty(PROP_ORIENTATION).toBool();
		int value = mc::MessageImpl::GetProperty(PROP_VALUE).toInt();
        
        mc::MessageImpl::Write(stream, vertical);
		mc::MessageImpl::Write(stream, value);
        
        return mc::_S_OK;
    }

    mc::result ScrollMessage::ReadBody(std::istream& stream)
    {
		bool vertical = false;
		int value = 0;
        
        mc::MessageImpl::Read(stream, vertical);        
		mc::MessageImpl::Read(stream, value);        
        
        mc::MessageImpl::PutProperty(PROP_ORIENTATION, vertical);
		mc::MessageImpl::PutProperty(PROP_VALUE, value);
        
        return mc::_S_OK;
    }

}
