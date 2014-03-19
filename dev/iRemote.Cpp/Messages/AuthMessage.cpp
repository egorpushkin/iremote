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

#include "AuthMessage.h"

#include "../Versions/Versions.h"

namespace RemotePC
{

	// {1FE3A457-4369-42c7-878F-1075F70B44CF}
	const mc::Guid AuthMessage::messageId_ =  
        { 0x1fe3a457, 0x4369, 0x42c7, { 0x87, 0x8f, 0x10, 0x75, 0xf7, 0xb, 0x44, 0xcf } };
        
    AuthMessage::AuthMessage()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICE, 0);
        mc::MessageImpl::PutProperty(PROP_ID, "");
        mc::MessageImpl::PutProperty(PROP_NAME, "");
		mc::MessageImpl::PutProperty(PROP_HAS_CODE, false);
		mc::MessageImpl::PutProperty(PROP_CODE, mc::TypeInfo< mc::ICommon >::GetGuid());
		mc::MessageImpl::PutProperty(PROP_APP, iRemoteApplicationGuid);
		mc::MessageImpl::PutProperty(PROP_VER_MAJOR, iREMOTE_VER_MAJOR);
		mc::MessageImpl::PutProperty(PROP_VER_MINOR, iREMOTE_VER_MINOR);
		mc::MessageImpl::PutProperty(PROP_VER_REV, iREMOTE_VER_REV);
    }
    
    AuthMessage::AuthMessage( int device, const std::string & identifier, const std::string & name )
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_DEVICE, device);
        mc::MessageImpl::PutProperty(PROP_ID, identifier);
        mc::MessageImpl::PutProperty(PROP_NAME, name);
		mc::MessageImpl::PutProperty(PROP_HAS_CODE, false);
		mc::MessageImpl::PutProperty(PROP_CODE, mc::TypeInfo< mc::ICommon >::GetGuid());
		mc::MessageImpl::PutProperty(PROP_APP, iRemoteApplicationGuid);
		mc::MessageImpl::PutProperty(PROP_VER_MAJOR, iREMOTE_VER_MAJOR);
		mc::MessageImpl::PutProperty(PROP_VER_MINOR, iREMOTE_VER_MINOR);
		mc::MessageImpl::PutProperty(PROP_VER_REV, iREMOTE_VER_REV);
    }

	AuthMessage::AuthMessage( int device, const std::string & identifier, const std::string & name, const mc::Guid& code )
        : mc::MessageImpl( messageId_ )
	{
		mc::MessageImpl::PutProperty(PROP_DEVICE, device);
		mc::MessageImpl::PutProperty(PROP_ID, identifier);
		mc::MessageImpl::PutProperty(PROP_NAME, name);
		mc::MessageImpl::PutProperty(PROP_HAS_CODE, true);
		mc::MessageImpl::PutProperty(PROP_CODE, code);
		mc::MessageImpl::PutProperty(PROP_APP, iRemoteApplicationGuid);
		mc::MessageImpl::PutProperty(PROP_VER_MAJOR, iREMOTE_VER_MAJOR);
		mc::MessageImpl::PutProperty(PROP_VER_MINOR, iREMOTE_VER_MINOR);
		mc::MessageImpl::PutProperty(PROP_VER_REV, iREMOTE_VER_REV);
	}
    
    // IMessage section
    mc::IMessage::Size_ AuthMessage::GetDataSize()
    {
        return 
			// Size of device property
			sizeof( int ) + 
			// Size of identifier property
            sizeof( int ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_ID).toString() ) +
			// Size of name property
            sizeof( int ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_NAME).toString() ) +
			// Size of code presence flag
			sizeof( bool ) + 
			// Size of unique code 
			sizeof( mc::Guid ) +
			// Size of app id
			sizeof( mc::Guid ) +
			// Size of major version property
			sizeof( int ) +
			// Size of minor version property
			sizeof( int ) +
			// Size of revision property
			sizeof( int );			
    }

    // MessageImpl section
    mc::result AuthMessage::WriteBody(std::ostream& stream)
    {
		int device = mc::MessageImpl::GetProperty(PROP_DEVICE).toInt();
        std::string identifier = mc::MessageImpl::GetProperty(PROP_ID).toString();
		std::string name = mc::MessageImpl::GetProperty(PROP_NAME).toString();
		bool hasCode = mc::MessageImpl::GetProperty(PROP_HAS_CODE).toBool();
		mc::Guid code = mc::MessageImpl::GetProperty(PROP_CODE).toGuid();
		mc::Guid app = mc::MessageImpl::GetProperty(PROP_APP).toGuid();
		int verMajor = mc::MessageImpl::GetProperty(PROP_VER_MAJOR).toInt();
		int verMinor = mc::MessageImpl::GetProperty(PROP_VER_MINOR).toInt();
		int verRev = mc::MessageImpl::GetProperty(PROP_VER_REV).toInt();
        
		mc::MessageImpl::Write(stream, device);
		mc::MessageImpl::Write(stream, identifier);
        mc::MessageImpl::Write(stream, name);
		mc::MessageImpl::Write(stream, hasCode);
		mc::MessageImpl::Write(stream, code);
		mc::MessageImpl::Write(stream, app);
		mc::MessageImpl::Write(stream, verMajor);
		mc::MessageImpl::Write(stream, verMinor);
		mc::MessageImpl::Write(stream, verRev);
        
        return mc::_S_OK;
    }

    mc::result AuthMessage::ReadBody(std::istream& stream)
    {
		int device;
        std::string identifier;
		std::string name;
		bool hasCode;
		mc::Guid code;
		mc::Guid app;
		int verMajor;
		int verMinor;
		int verRev;

		mc::MessageImpl::Read(stream, device);
        mc::MessageImpl::Read(stream, identifier);
        mc::MessageImpl::Read(stream, name);        
		mc::MessageImpl::Read(stream, hasCode);        
		mc::MessageImpl::Read(stream, code);        
		mc::MessageImpl::Read(stream, app);        
		mc::MessageImpl::Read(stream, verMajor);        
		mc::MessageImpl::Read(stream, verMinor);        
		mc::MessageImpl::Read(stream, verRev);        

		mc::MessageImpl::PutProperty(PROP_DEVICE, device);
        mc::MessageImpl::PutProperty(PROP_ID, identifier);
        mc::MessageImpl::PutProperty(PROP_NAME, name);
		mc::MessageImpl::PutProperty(PROP_HAS_CODE, hasCode);
		mc::MessageImpl::PutProperty(PROP_CODE, code);
		mc::MessageImpl::PutProperty(PROP_APP, app);
		mc::MessageImpl::PutProperty(PROP_VER_MAJOR, verMajor);
		mc::MessageImpl::PutProperty(PROP_VER_MINOR, verMinor);
		mc::MessageImpl::PutProperty(PROP_VER_REV, verRev);
        
        return mc::_S_OK;
    }

}
