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

#include "DeviceInfo.h"

namespace RemotePC
{
namespace SFB
{		    
    // {27F8E303-239B-4613-86DB-20922F267B59}
    const mc::Guid DeviceInfo::messageId_ =    
        { 0x27F8E303, 0x239B, 0x4613, { 0x86, 0xDB, 0x20, 0x92, 0x2F, 0x26, 0x7B, 0x59 } };
    
    DeviceInfo::DeviceInfo()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICEID, mc::TypeInfo< mc::ICommon >::GetGuid());        
        mc::MessageImpl::PutProperty(PROP_WIDTH, (int)0);        
        mc::MessageImpl::PutProperty(PROP_HEIGHT, (int)0);        
        mc::MessageImpl::PutProperty(PROP_DESCR, "");                
    }
    
    DeviceInfo::DeviceInfo(const mc::Guid& id, size_t width, size_t height, const std::string& descr)
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICEID, id);                
        mc::MessageImpl::PutProperty(PROP_WIDTH, width);
        mc::MessageImpl::PutProperty(PROP_HEIGHT, height);                
        mc::MessageImpl::PutProperty(PROP_DESCR, descr);                
    }
    
    // IMessage section
    mc::IMessage::Size_ DeviceInfo::GetDataSize()
    {
        return
            // Size of device id property
            sizeof( mc::Guid )
            // Size of width property        
            + sizeof( int ) 
            // Size of height property        
            + sizeof( int )  
            // Size of descr property        
            + sizeof( size_t ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_DESCR).toString() );
    }
    
    // MessageImpl section
    mc::result DeviceInfo::WriteBody(std::ostream& stream)
    {
        mc::Guid id = mc::MessageImpl::GetProperty(PROP_DEVICEID).toGuid();
        size_t width = mc::MessageImpl::GetProperty(PROP_WIDTH).toInt();
        size_t height = mc::MessageImpl::GetProperty(PROP_HEIGHT).toInt();
        std::string descr = mc::MessageImpl::GetProperty(PROP_DESCR).toString();
        
        mc::MessageImpl::Write(stream, id);
        mc::MessageImpl::Write(stream, width);
        mc::MessageImpl::Write(stream, height);
        mc::MessageImpl::Write(stream, descr);        
        
        return mc::_S_OK;
    }
    
    mc::result DeviceInfo::ReadBody(std::istream& stream)
    {
        mc::Guid id;
        size_t width = 0;
        size_t height = 0;
        std::string descr;
        
        mc::MessageImpl::Read(stream, id);		
        mc::MessageImpl::Read(stream, width);		
        mc::MessageImpl::Read(stream, height);		
        mc::MessageImpl::Read(stream, descr);		
                
        mc::MessageImpl::PutProperty(PROP_DEVICEID, id);                
        mc::MessageImpl::PutProperty(PROP_WIDTH, width);                
        mc::MessageImpl::PutProperty(PROP_HEIGHT, height);                
        mc::MessageImpl::PutProperty(PROP_DESCR, descr);          
        
        return mc::_S_OK;
    }    
}
}
