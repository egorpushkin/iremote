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

#include "PasswordMessage.h"

namespace RemotePC
{

	// {E213ACA9-9526-463d-A594-1B7DDBF92FE4}
	const mc::Guid PasswordMessage::messageId_ =  
		{ 0xe213aca9, 0x9526, 0x463d, { 0xa5, 0x94, 0x1b, 0x7d, 0xdb, 0xf9, 0x2f, 0xe4 } };

    PasswordMessage::PasswordMessage()
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_USERNAME, "");
        mc::MessageImpl::PutProperty(PROP_PASSWORD, "");
    }
    
    PasswordMessage::PasswordMessage( const std::string & password )
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_USERNAME, "");
		mc::MessageImpl::PutProperty(PROP_PASSWORD, password);
    }

    // IMessage section
    mc::IMessage::Size_ PasswordMessage::GetDataSize()
    {
        return 
			// Size of username property
            sizeof( int ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_USERNAME).toString() ) +
			// Size of password property
            sizeof( int ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_PASSWORD).toString() );
    }

    // MessageImpl section
    mc::result PasswordMessage::WriteBody(std::ostream& stream)
    {
        std::string username = mc::MessageImpl::GetProperty(PROP_USERNAME).toString();
		std::string password = mc::MessageImpl::GetProperty(PROP_PASSWORD).toString();
        
		mc::MessageImpl::Write(stream, username);
		mc::MessageImpl::Write(stream, password);
        
        return mc::_S_OK;
    }

    mc::result PasswordMessage::ReadBody(std::istream& stream)
    {
        std::string username;
		std::string password;

		mc::MessageImpl::Read(stream, username);
        mc::MessageImpl::Read(stream, password);

		mc::MessageImpl::PutProperty(PROP_USERNAME, username);
        mc::MessageImpl::PutProperty(PROP_PASSWORD, password);
        
        return mc::_S_OK;
    }

}
