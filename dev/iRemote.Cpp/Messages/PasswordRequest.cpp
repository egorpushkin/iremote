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

#include "PasswordRequest.h"

namespace RemotePC
{

	// {F6C157C4-5ED4-4350-8FDC-0AA78AE1E430}
	const mc::Guid PasswordRequest::messageId_ =  
		{ 0xf6c157c4, 0x5ed4, 0x4350, { 0x8f, 0xdc, 0xa, 0xa7, 0x8a, 0xe1, 0xe4, 0x30 } };

    PasswordRequest::PasswordRequest()
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_HOST_ID, mc::TypeInfo< mc::ICommon >::GetGuid());
		mc::MessageImpl::PutProperty(PROP_REQUEST_AUTH, false);
    }
    
    PasswordRequest::PasswordRequest( mc::RefGuid hostId, bool requestAuth )
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_HOST_ID, hostId);
		mc::MessageImpl::PutProperty(PROP_REQUEST_AUTH, requestAuth);
    }

    // IMessage section
    mc::IMessage::Size_ PasswordRequest::GetDataSize()
    {
        return 
			// Size of host id property
			sizeof( mc::Guid ) +
			// Size of request auth property
			sizeof( bool );
    }

    // MessageImpl section
    mc::result PasswordRequest::WriteBody(std::ostream& stream)
    {
		mc::Guid hostId = mc::MessageImpl::GetProperty(PROP_HOST_ID).toGuid();
        bool requestAuth = mc::MessageImpl::GetProperty(PROP_REQUEST_AUTH).toBool();
        
		mc::MessageImpl::Write(stream, hostId);
		mc::MessageImpl::Write(stream, requestAuth);
        
        return mc::_S_OK;
    }

    mc::result PasswordRequest::ReadBody(std::istream& stream)
    {
		mc::Guid hostId;
        bool requestAuth;

		mc::MessageImpl::Read(stream, hostId);
		mc::MessageImpl::Read(stream, requestAuth);

		mc::MessageImpl::PutProperty(PROP_HOST_ID, hostId);
		mc::MessageImpl::PutProperty(PROP_REQUEST_AUTH, requestAuth);
        
        return mc::_S_OK;
    }

}
