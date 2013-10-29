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

#include "RequestDevice.h"

namespace RemotePC
{
namespace SFB
{		    
    // {64151A43-583E-4067-9AFE-876FEDD582A7}
    const mc::Guid RequestDevice::messageId_ =
        { 0x64151A43, 0x583E, 0x4067, { 0x9A, 0xFE, 0x87, 0x6F, 0xED, 0xD5, 0x82, 0xA7 } };
	
	// {01E09D08-5492-424B-821B-0A7F253C447F}
	const mc::Guid RequestDevice::primaryDisplayId_ =
		{ 0x01E09D08, 0x5492, 0x424B, { 0x82, 0x1B, 0x0A, 0x7F, 0x25, 0x3C, 0x44, 0x7F } };		
    
    RequestDevice::RequestDevice()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICEID, mc::TypeInfo< mc::ICommon >::GetGuid());        
    }
    
    RequestDevice::RequestDevice(const mc::Guid& id)
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICEID, id);                
    }
    
    // IMessage section
    mc::IMessage::Size_ RequestDevice::GetDataSize()
    {
        return
            // Size of device id property
            sizeof ( mc::Guid );
    }
    
    // MessageImpl section
    mc::result RequestDevice::WriteBody(std::ostream& stream)
    {
        mc::Guid id = mc::MessageImpl::GetProperty(PROP_DEVICEID).toGuid();
        
        mc::MessageImpl::Write(stream, id);
        
        return mc::_S_OK;
    }
    
    mc::result RequestDevice::ReadBody(std::istream& stream)
    {
        mc::Guid id;
        
        mc::MessageImpl::Read(stream, id);		
        
        mc::MessageImpl::PutProperty(PROP_DEVICEID, id);		
        
        return mc::_S_OK;
    }    
}
}
