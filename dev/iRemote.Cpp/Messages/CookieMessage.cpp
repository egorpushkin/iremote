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

#include "CookieMessage.h"

namespace RemotePC
{

	// {2C3D38A7-F40C-4318-B2FE-34C9C569BD0E}
	const mc::Guid CookieMessage::messageId_ =
        { 0x2c3d38a7, 0xf40c, 0x4318, { 0xb2, 0xfe, 0x34, 0xc9, 0xc5, 0x69, 0xbd, 0xe } };

	CookieMessage::CookieMessage()
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_CODE, mc::TypeInfo< mc::ICommon >::GetGuid());
    }

	CookieMessage::CookieMessage(const mc::Guid& code)
        : mc::MessageImpl( messageId_ )
	{
		mc::MessageImpl::PutProperty(PROP_CODE, code);
	}

    // IMessage section
	mc::IMessage::Size_ CookieMessage::GetDataSize()
    {
        return 
			// Size of unique code 
			sizeof( mc::Guid );	
    }

    // MessageImpl section
	mc::result CookieMessage::WriteBody(std::ostream& stream)
    {
		mc::Guid code = mc::MessageImpl::GetProperty(PROP_CODE).toGuid();

		mc::MessageImpl::Write(stream, code);

        return mc::_S_OK;
    }

	mc::result CookieMessage::ReadBody(std::istream& stream)
    {
		mc::Guid code;
     
		mc::MessageImpl::Read(stream, code); 

		mc::MessageImpl::PutProperty(PROP_CODE, code);
        
        return mc::_S_OK;
    }

}
