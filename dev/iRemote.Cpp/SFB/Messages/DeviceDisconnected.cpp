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

#include "DeviceDisconnected.h"

namespace RemotePC
{
namespace SFB
{		    
    // {F9E142DF-06D1-401F-BD31-5FDAD4A70CF8}
    const mc::Guid DeviceDisconnected::messageId_ =
        { 0xF9E142DF, 0x06D1, 0x401F, { 0xBD, 0x31, 0x5F, 0xDA, 0xD4, 0xA7, 0x0C, 0xF8 } };    
    
    DeviceDisconnected::DeviceDisconnected()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICEID, mc::TypeInfo< mc::ICommon >::GetGuid());        
    }
    
    DeviceDisconnected::DeviceDisconnected(const mc::Guid& id)
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICEID, id);                
    }
    
    // IMessage section
    mc::IMessage::Size_ DeviceDisconnected::GetDataSize()
    {
        return
            // Size of device id property
            sizeof ( mc::Guid );
    }
    
    // MessageImpl section
    mc::result DeviceDisconnected::WriteBody(std::ostream& stream)
    {
        mc::Guid id = mc::MessageImpl::GetProperty(PROP_DEVICEID).toGuid();
        
        mc::MessageImpl::Write(stream, id);
        
        return mc::_S_OK;
    }
    
    mc::result DeviceDisconnected::ReadBody(std::istream& stream)
    {
        mc::Guid id;
        
        mc::MessageImpl::Read(stream, id);		
        
        mc::MessageImpl::PutProperty(PROP_DEVICEID, id);		
        
        return mc::_S_OK;
    }    
}
}
