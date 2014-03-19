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

#include "VolumeMessage.h"

namespace RemotePC
{
    
	// {4B204C3F-6AB8-4191-BA21-7627618E15A4}
	const mc::Guid VolumeMessage::messageId_ =  
        { 0x4b204c3f, 0x6ab8, 0x4191, { 0xba, 0x21, 0x76, 0x27, 0x61, 0x8e, 0x15, 0xa4 } };

    VolumeMessage::VolumeMessage()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICE, DEVICE_MASTER);
		mc::MessageImpl::PutProperty(PROP_VALUE, (unsigned long)0);
    }
    
	VolumeMessage::VolumeMessage(Devices_ device, unsigned long value)
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_DEVICE, device);
		mc::MessageImpl::PutProperty(PROP_VALUE, value);
    }
    
    // IMessage section
    mc::IMessage::Size_ VolumeMessage::GetDataSize()
    {
        return 
            // Size of device property
            sizeof( int )
            // Size of value property
            + sizeof( int );
    }

    // MessageImpl section
    mc::result VolumeMessage::WriteBody(std::ostream& stream)
    {
        int device = mc::MessageImpl::GetProperty(PROP_DEVICE).toInt();
        unsigned long value = mc::MessageImpl::GetProperty(PROP_VALUE).toULong();
        
        mc::MessageImpl::Write(stream, device);
        mc::MessageImpl::Write(stream, (int)value);
        
        return mc::_S_OK;
    }

    mc::result VolumeMessage::ReadBody(std::istream& stream)
    {
        int device = DEVICE_MASTER;
        int value = 0;
        
        mc::MessageImpl::Read(stream, device);
        mc::MessageImpl::Read(stream, value);
        
        mc::MessageImpl::PutProperty(PROP_DEVICE, device);
        mc::MessageImpl::PutProperty(PROP_VALUE, (unsigned long)value);
        
        return mc::_S_OK;
    }

}
