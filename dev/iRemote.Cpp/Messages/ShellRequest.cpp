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

#include "ShellRequest.h"

namespace RemotePC
{

	// {D968EE57-B4A2-473b-A0F6-FDA73FEB728F}
	const mc::Guid ShellRequest::messageId_ =  
		{ 0xd968ee57, 0xb4a2, 0x473b, { 0xa0, 0xf6, 0xfd, 0xa7, 0x3f, 0xeb, 0x72, 0x8f } };

    ShellRequest::ShellRequest()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_COMMAND, "");
    }
    
    ShellRequest::ShellRequest( const std::string & command )
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_COMMAND, command);
    }

    // IMessage section
    mc::IMessage::Size_ ShellRequest::GetDataSize()
    {
        return 
			// Size of command property
			sizeof( size_t ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_COMMAND).toString() );
    }

    // MessageImpl section
    mc::result ShellRequest::WriteBody(std::ostream& stream)
    {
        std::string command = mc::MessageImpl::GetProperty(PROP_COMMAND).toString();
        
		mc::MessageImpl::Write(stream, command);
        
        return mc::_S_OK;
    }

    mc::result ShellRequest::ReadBody(std::istream& stream)
    {
        std::string command;

		mc::MessageImpl::Read(stream, command);

		mc::MessageImpl::PutProperty(PROP_COMMAND, command);
        
        return mc::_S_OK;
    }

}
