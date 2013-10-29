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

#include "KeyboardMessage.h"

namespace RemotePC
{
    
	// {274FD65E-0ED7-4a86-9C59-22A920437BB6}
	const mc::Guid KeyboardMessage::messageId_ =  
        { 0x274fd65e, 0xed7, 0x4a86, { 0x9c, 0x59, 0x22, 0xa9, 0x20, 0x43, 0x7b, 0xb6 } };
        
    KeyboardMessage::KeyboardMessage()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_VK, (unsigned char)'\x0');
    }
    
    KeyboardMessage::KeyboardMessage(Type_ type, unsigned char vk, bool keydown)
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_TYPE, type);
        mc::MessageImpl::PutProperty(PROP_VK, vk);
        mc::MessageImpl::PutProperty(PROP_SCAN, (unsigned short)0);
        mc::MessageImpl::PutProperty(PROP_EVENT, keydown);
    }
    
    KeyboardMessage::KeyboardMessage(Type_ type, unsigned short scan)
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_TYPE, type);
        mc::MessageImpl::PutProperty(PROP_VK, (unsigned char)'\x0');
        mc::MessageImpl::PutProperty(PROP_SCAN, scan);
        mc::MessageImpl::PutProperty(PROP_EVENT, (bool)false);    
    }
    
    // IMessage section
    mc::IMessage::Size_ KeyboardMessage::GetDataSize()
    {
        return 
            // Size of type property
            sizeof( int )
            // Size of vk property
            + sizeof( unsigned char )
            // Size of scan property
            + sizeof( unsigned short )
            // Size of event property
            + sizeof( bool );
    }

    // MessageImpl section
    mc::result KeyboardMessage::WriteBody(std::ostream& stream)
    {
        int type = mc::MessageImpl::GetProperty(PROP_TYPE).toInt();
        unsigned char vk = mc::MessageImpl::GetProperty(PROP_VK).toUChar();
        unsigned short scan = mc::MessageImpl::GetProperty(PROP_SCAN).toUShort();
        bool event = mc::MessageImpl::GetProperty(PROP_EVENT).toBool();        
        
        mc::MessageImpl::Write(stream, type);
        mc::MessageImpl::Write(stream, vk);
        mc::MessageImpl::Write(stream, scan);
        mc::MessageImpl::Write(stream, event);
        
        return mc::_S_OK;
    }

    mc::result KeyboardMessage::ReadBody(std::istream& stream)
    {
        int type = 0;
        unsigned char vk = '\x0';
        unsigned short scan = 0;
        bool event = false;    
        
        mc::MessageImpl::Read(stream, type);
        mc::MessageImpl::Read(stream, vk);
        mc::MessageImpl::Read(stream, scan);
        mc::MessageImpl::Read(stream, event);       
        
        mc::MessageImpl::PutProperty(PROP_TYPE, type);
        mc::MessageImpl::PutProperty(PROP_VK, vk);
        mc::MessageImpl::PutProperty(PROP_SCAN, scan);
        mc::MessageImpl::PutProperty(PROP_EVENT, event);
        
        return mc::_S_OK;
    }

}
