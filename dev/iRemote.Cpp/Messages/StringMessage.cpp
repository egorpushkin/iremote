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

#include "IStringMessage.h"
#include "StringMessage.h"

namespace MinCOM
{    
    // {F3E945B1-B1D2-4C38-A9B3-2D777CCB8F50}
	template<> const Guid TypeInfo< RemotePC::IStringMessage >::iid_ =
        { 0xF3E945B1, 0xB1D2, 0x4C38, { 0xA9, 0xB3, 0x2D, 0x77, 0x7C, 0xCB, 0x8F, 0x50 } };        
}

namespace RemotePC
{
    StringMessage::StringMessage()
        : mc::MessageImpl()
        , mc::CommonImpl< IStringMessage >()
    {
        mc::MessageImpl::PutProperty(PROP_METADATA, "");
        mc::MessageImpl::PutProperty(PROP_VALUE, "");        
    }
    
    StringMessage::StringMessage(const mc::Guid& code, const std::string& metadata, const std::string& value)
        : mc::MessageImpl( code )
        , mc::CommonImpl< IStringMessage >()
	{
        mc::MessageImpl::PutProperty(PROP_METADATA, metadata);
        mc::MessageImpl::PutProperty(PROP_VALUE, value);        
	}
        
	/**
     * IStringMessage section
     */
    
    std::string StringMessage::GetMetadata()
    {
        return mc::MessageImpl::GetProperty(PROP_METADATA).toString();
    }
    
    std::string StringMessage::GetValue()   
    {
        return mc::MessageImpl::GetProperty(PROP_VALUE).toString();        
    }
    
    /**
     * IMessage section
     */
    
	mc::IMessage::Size_ StringMessage::GetDataSize()
    {
        return 
            // Size of metadata property
			sizeof( size_t ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_METADATA).toString() ) +
            // Size of value property
            sizeof( size_t ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_VALUE).toString() );
    }
    
    /**
     * MessageImpl section
     */
    
	mc::result StringMessage::WriteBody(std::ostream& stream)
    {
        std::string metadata = mc::MessageImpl::GetProperty(PROP_METADATA).toString();
        std::string value = mc::MessageImpl::GetProperty(PROP_VALUE).toString();        
		
        mc::MessageImpl::Write(stream, metadata);
        mc::MessageImpl::Write(stream, value);

        return mc::_S_OK;
    }
    
	mc::result StringMessage::ReadBody(std::istream& stream)
    {
        std::string metadata;
        std::string value;        
		
        mc::MessageImpl::Read(stream, metadata);
        mc::MessageImpl::Read(stream, value);
        
        mc::MessageImpl::PutProperty(PROP_METADATA, metadata);
        mc::MessageImpl::PutProperty(PROP_VALUE, value);
        
        return mc::_S_OK;
    }
    
}

