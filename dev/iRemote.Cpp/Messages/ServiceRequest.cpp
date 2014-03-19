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

#include "ServiceRequest.h"

namespace RemotePC
{
	// {178978D7-CFE5-47F7-BF10-54040768D78F}
	const mc::Guid ServiceRequest::messageId_ =
		{ 0x178978D7, 0xCFE5, 0x47F7, { 0xBF, 0x10, 0x54, 0x04, 0x07, 0x68, 0xD7, 0x8F } };
	
    ServiceRequest::ServiceRequest()
		: mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_ID, mc::Guid());
        mc::MessageImpl::PutProperty(PROP_VER_MAJOR, 0);
		mc::MessageImpl::PutProperty(PROP_VER_MINOR, 0);
		mc::MessageImpl::PutProperty(PROP_DETAILS, "");
    }
	
    ServiceRequest::ServiceRequest( const mc::Guid& id, int verMajor, int verMinor, const std::string& details )
	: mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_ID, id);
        mc::MessageImpl::PutProperty(PROP_VER_MAJOR, verMajor);
		mc::MessageImpl::PutProperty(PROP_VER_MINOR, verMinor);
		mc::MessageImpl::PutProperty(PROP_DETAILS, details);
    }
	
    // IMessage section
    mc::IMessage::Size_ ServiceRequest::GetDataSize()
    {
        return
			// Size of id property
			sizeof( mc::Guid ) +
			// Size of verMajor property
			sizeof( int ) +
			// Size of verMinor property
			sizeof( int ) +
			// Size of details property
            sizeof( int ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_DETAILS).toString() );
    }
	
    // MessageImpl section
    mc::result ServiceRequest::WriteBody(std::ostream& stream)
    {
        mc::Guid id = mc::MessageImpl::GetProperty(PROP_ID).toGuid();
		int verMajor = mc::MessageImpl::GetProperty(PROP_VER_MAJOR).toInt();
		int verMinor = mc::MessageImpl::GetProperty(PROP_VER_MINOR).toInt();		
        std::string details = mc::MessageImpl::GetProperty(PROP_DETAILS).toString();
		
        mc::MessageImpl::Write(stream, id);
        mc::MessageImpl::Write(stream, verMajor);
        mc::MessageImpl::Write(stream, verMinor);		
        mc::MessageImpl::Write(stream, details);
		
        return mc::_S_OK;
    }
	
    mc::result ServiceRequest::ReadBody(std::istream& stream)
    {
        mc::Guid id;
		int verMajor;
		int verMinor;		
        std::string details;
		
        mc::MessageImpl::Read(stream, id);
        mc::MessageImpl::Read(stream, verMajor);
        mc::MessageImpl::Read(stream, verMinor);		
        mc::MessageImpl::Read(stream, details);				
		
        mc::MessageImpl::PutProperty(PROP_ID, id);
        mc::MessageImpl::PutProperty(PROP_VER_MAJOR, verMajor);
        mc::MessageImpl::PutProperty(PROP_VER_MINOR, verMinor);
        mc::MessageImpl::PutProperty(PROP_DETAILS, details);		
		
        return mc::_S_OK;
    }
}
