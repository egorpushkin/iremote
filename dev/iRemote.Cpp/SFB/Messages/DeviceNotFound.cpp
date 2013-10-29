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

#include "DeviceNotFound.h"

namespace RemotePC
{
namespace SFB
{		    
    // {51D64984-4632-40F7-A225-9B5068A4157A}
    const mc::Guid DeviceNotFound::messageId_ =
        { 0x51D64984, 0x4632, 0x40F7, { 0xA2, 0x25, 0x9B, 0x50, 0x68, 0xA4, 0x15, 0x7A } };
    
    DeviceNotFound::DeviceNotFound()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICEID, mc::TypeInfo< mc::ICommon >::GetGuid());        
    }
    
    DeviceNotFound::DeviceNotFound(const mc::Guid& id)
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICEID, id);                
    }
    
    // IMessage section
    mc::IMessage::Size_ DeviceNotFound::GetDataSize()
    {
        return
            // Size of device id property
            sizeof ( mc::Guid );
    }
    
    // MessageImpl section
    mc::result DeviceNotFound::WriteBody(std::ostream& stream)
    {
        mc::Guid id = mc::MessageImpl::GetProperty(PROP_DEVICEID).toGuid();
        
        mc::MessageImpl::Write(stream, id);
        
        return mc::_S_OK;
    }
    
    mc::result DeviceNotFound::ReadBody(std::istream& stream)
    {
        mc::Guid id;
        
        mc::MessageImpl::Read(stream, id);		
        
        mc::MessageImpl::PutProperty(PROP_DEVICEID, id);		
        
        return mc::_S_OK;
    }    
}
}
