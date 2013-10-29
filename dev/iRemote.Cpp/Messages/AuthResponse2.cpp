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

#include "AuthResponse.h"
#include "AuthResponse2.h"

#include "../Versions/Versions.h"

namespace RemotePC
{

	// {56B9B1B4-8A01-4bd1-B3FF-068CF76C4B72}
	const mc::Guid AuthResponse2::messageId_ =
		{ 0x56b9b1b4, 0x8a01, 0x4bd1, { 0xb3, 0xff, 0x6, 0x8c, 0xf7, 0x6c, 0x4b, 0x72 } };

	AuthResponse2::AuthResponse2()
		: AuthResponse()
	{
        mc::MessageImpl::SetCode(AuthResponse2::messageId_);
		mc::MessageImpl::PutProperty(PROP_APP, mc::TypeInfo< mc::ICommon >::GetGuid());
		mc::MessageImpl::PutProperty(PROP_HOST_ID, mc::TypeInfo< mc::ICommon >::GetGuid());
		mc::MessageImpl::PutProperty(PROP_VER_MAJOR, 0);
		mc::MessageImpl::PutProperty(PROP_VER_MINOR, 0);
		mc::MessageImpl::PutProperty(PROP_VER_REV, 0);
	}

	AuthResponse2::AuthResponse2(mc::RefGuid hostId)
		: AuthResponse()
	{
        mc::MessageImpl::SetCode(AuthResponse2::messageId_);
		mc::MessageImpl::PutProperty(PROP_APP, RemotePCApplicationGuid);
		mc::MessageImpl::PutProperty(PROP_HOST_ID, hostId);
		mc::MessageImpl::PutProperty(PROP_VER_MAJOR, REMOTEPC_VER_MAJOR);
		mc::MessageImpl::PutProperty(PROP_VER_MINOR, REMOTEPC_VER_MINOR);
		mc::MessageImpl::PutProperty(PROP_VER_REV, REMOTEPC_VER_REV);
	}

	// IMessage section
	mc::IMessage::Size_ AuthResponse2::GetDataSize()
	{
		return 
			AuthResponse::GetDataSize() + 
			// Size of app property
			sizeof( mc::Guid ) +
			// Size of host id property
			sizeof( mc::Guid ) + 
			// Size of major version property
			sizeof( int ) +
			// Size of minor version property
			sizeof( int ) +
			// Size of revision property
			sizeof( int );	
	}

	// MessageImpl section
	mc::result AuthResponse2::WriteBody(std::ostream& stream)
	{
		if ( mc::Error::IsFailed( AuthResponse::WriteBody(stream) ) )
			return mc::_E_FAIL;

		mc::Guid app = mc::MessageImpl::GetProperty(PROP_APP).toGuid();
		mc::Guid hostId = mc::MessageImpl::GetProperty(PROP_HOST_ID).toGuid();
		int verMajor = mc::MessageImpl::GetProperty(PROP_VER_MAJOR).toInt();
		int verMinor = mc::MessageImpl::GetProperty(PROP_VER_MINOR).toInt();
		int verRev = mc::MessageImpl::GetProperty(PROP_VER_REV).toInt();
		
		mc::MessageImpl::Write(stream, app);
		mc::MessageImpl::Write(stream, hostId);
		mc::MessageImpl::Write(stream, verMajor);
		mc::MessageImpl::Write(stream, verMinor);
		mc::MessageImpl::Write(stream, verRev);

		return mc::_S_OK;
	}

	mc::result AuthResponse2::ReadBody(std::istream& stream)
	{
		if ( mc::Error::IsFailed( AuthResponse::ReadBody(stream) ) )
			return mc::_E_FAIL;

		mc::Guid app;
		mc::Guid hostId;
		int verMajor;
		int verMinor;
		int verRev;
		
		mc::MessageImpl::Read(stream, app);
		mc::MessageImpl::Read(stream, hostId);        
		mc::MessageImpl::Read(stream, verMajor); 
		mc::MessageImpl::Read(stream, verMinor);        
		mc::MessageImpl::Read(stream, verRev); 

		mc::MessageImpl::PutProperty(PROP_APP, app);
		mc::MessageImpl::PutProperty(PROP_HOST_ID, hostId);
		mc::MessageImpl::PutProperty(PROP_VER_MAJOR, verMajor);
		mc::MessageImpl::PutProperty(PROP_VER_MINOR, verMinor);
		mc::MessageImpl::PutProperty(PROP_VER_REV, verRev);
		
		return mc::_S_OK;
	}

}
