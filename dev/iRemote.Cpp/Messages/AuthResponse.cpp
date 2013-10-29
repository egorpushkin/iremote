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

namespace RemotePC
{

	// {7680E9AA-868F-4f84-8B4C-3D4121B48787}
	const mc::Guid AuthResponse::messageId_ =
        { 0x7680e9aa, 0x868f, 0x4f84, { 0x8b, 0x4c, 0x3d, 0x41, 0x21, 0xb4, 0x87, 0x87 } };

	AuthResponse::AuthResponse()
        : mc::MessageImpl( messageId_ )
    {
 		InitOSProperty();
 		InitCodeProperties();
    }

	void AuthResponse::InitOSProperty()
	{
		// Determine OS at compile time.
#if defined(_MSC_VER) || defined(__MINGW32__)
		mc::MessageImpl::PutProperty(PROP_HOSTOS, OS_WIN);
#elif defined(__APPLE__)
		mc::MessageImpl::PutProperty(PROP_HOSTOS, OS_MAC);
#elif defined(__LINUX__)
		mc::MessageImpl::PutProperty(PROP_HOSTOS, OS_LINUX);
#else
		mc::MessageImpl::PutProperty(PROP_HOSTOS, OS_UNKNOWN);
#endif
	}

	void AuthResponse::InitCodeProperties()
	{
		mc::MessageImpl::PutProperty(PROP_HAS_CODE, true);
		mc::MessageImpl::PutProperty(PROP_CODE, mc::Guid::Generate());
	}
    
    // IMessage section
	mc::IMessage::Size_ AuthResponse::GetDataSize()
    {
        return 
			// Size of os property
			sizeof( int ) +
			// Size of code presence flag
			sizeof( bool ) + 
			// Size of unique code 
			sizeof( mc::Guid );	
    }

    // MessageImpl section
	mc::result AuthResponse::WriteBody(std::ostream& stream)
    {
		int os = mc::MessageImpl::GetProperty(PROP_HOSTOS).toInt();
		bool hasCode = mc::MessageImpl::GetProperty(PROP_HAS_CODE).toBool();
		mc::Guid code = mc::MessageImpl::GetProperty(PROP_CODE).toGuid();
        
		mc::MessageImpl::Write(stream, os);
		mc::MessageImpl::Write(stream, hasCode);
		mc::MessageImpl::Write(stream, code);

        return mc::_S_OK;
    }

	mc::result AuthResponse::ReadBody(std::istream& stream)
    {
		int os;
		bool hasCode;
		mc::Guid code;
        
		mc::MessageImpl::Read(stream, os);
		mc::MessageImpl::Read(stream, hasCode);        
		mc::MessageImpl::Read(stream, code); 

		mc::MessageImpl::PutProperty(PROP_HOSTOS, os);
		mc::MessageImpl::PutProperty(PROP_HAS_CODE, hasCode);
		mc::MessageImpl::PutProperty(PROP_CODE, code);
        
        return mc::_S_OK;
    }

}
