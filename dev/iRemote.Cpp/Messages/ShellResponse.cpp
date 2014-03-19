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

#include "ShellResponse.h"

namespace RemotePC
{

	// {889A0B20-6418-4dd5-8D11-29C557010D87}
	const mc::Guid ShellResponse::messageId_ =  
		{ 0x889a0b20, 0x6418, 0x4dd5, { 0x8d, 0x11, 0x29, 0xc5, 0x57, 0x1, 0xd, 0x87 } };

    ShellResponse::ShellResponse()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_RESPONSE, "");
		mc::MessageImpl::PutProperty(PROP_STATUS, mc::_E_FAIL);
    }
    
    ShellResponse::ShellResponse( const std::string & response, mc::result status )
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_RESPONSE, response);
		mc::MessageImpl::PutProperty(PROP_STATUS, status);
    }

    // IMessage section
    mc::IMessage::Size_ ShellResponse::GetDataSize()
    {
        return 
			// Size of response property
            sizeof( int ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_RESPONSE).toString() ) +
			// Size of status property
            sizeof( int );
    }

    // MessageImpl section
    mc::result ShellResponse::WriteBody(std::ostream& stream)
    {
        std::string response = mc::MessageImpl::GetProperty(PROP_RESPONSE).toString();
		mc::result status = mc::MessageImpl::GetProperty(PROP_STATUS).toLong();
        
		mc::MessageImpl::Write(stream, response);
        mc::MessageImpl::Write(stream, (int)status);
        
        return mc::_S_OK;
    }

    mc::result ShellResponse::ReadBody(std::istream& stream)
    {
        std::string response;
        int status;

		mc::MessageImpl::Read(stream, response);
		mc::MessageImpl::Read(stream, status);

		mc::MessageImpl::PutProperty(PROP_RESPONSE, response);
        mc::MessageImpl::PutProperty(PROP_STATUS, (mc::result)status);
        
        return mc::_S_OK;
    }

}
