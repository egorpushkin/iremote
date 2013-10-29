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

#include "ControlMessage.h"

namespace RemotePC
{
namespace SFB
{
		
	// {758BE3DD-1E9C-491C-BA47-F908822CB03A}
    const mc::Guid ControlMessage::messageId_ =
		{ 0x758BE3DD, 0x1E9C, 0x491C, { 0xBA, 0x47, 0xF9, 0x08, 0x82, 0x2C, 0xB0, 0x3A } };
	
    ControlMessage::ControlMessage()
		: mc::MessageImpl( messageId_ )
	{
		mc::MessageImpl::PutProperty(PROP_DEVICEID, mc::TypeInfo< mc::ICommon >::GetGuid());        
		mc::MessageImpl::PutProperty(PROP_COMMAND, (int)0);
	}
	
    ControlMessage::ControlMessage(const mc::Guid& id, int command)
		: mc::MessageImpl( messageId_ )
	{
		mc::MessageImpl::PutProperty(PROP_DEVICEID, id);                
		mc::MessageImpl::PutProperty(PROP_COMMAND, command);
	}
	
	// IMessage section
    mc::IMessage::Size_ ControlMessage::GetDataSize()
	{
		return
            // Size of device id property
            sizeof ( mc::Guid )
			// Size of command property
			+ sizeof( int );
	}
	
	// MessageImpl section
    mc::result ControlMessage::WriteBody(std::ostream& stream)
	{
        mc::Guid id = mc::MessageImpl::GetProperty(PROP_DEVICEID).toGuid();
		int command = mc::MessageImpl::GetProperty(PROP_COMMAND).toInt();
		
        mc::MessageImpl::Write(stream, id);
		mc::MessageImpl::Write(stream, command);
		
		return mc::_S_OK;
	}
	
    mc::result ControlMessage::ReadBody(std::istream& stream)
	{
        mc::Guid id;
		int command = 0;

		mc::MessageImpl::Read(stream, id);		
		mc::MessageImpl::Read(stream, command);

		mc::MessageImpl::PutProperty(PROP_DEVICEID, id);		
		mc::MessageImpl::PutProperty(PROP_COMMAND, command);
		
		return mc::_S_OK;
	}
		
}
}
