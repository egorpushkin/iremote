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

#include "IDataMessage.h"
#include "DataMessage.h"

namespace MinCOM
{

	// {650A0E1B-A2F4-4e02-810C-BFE76F4251B2}
	template<> const Guid TypeInfo< RemotePC::IDataMessage >::iid_ =
		{ 0x650a0e1b, 0xa2f4, 0x4e02, { 0x81, 0xc, 0xbf, 0xe7, 0x6f, 0x42, 0x51, 0xb2 } };

}

namespace RemotePC
{
    DataMessage::DataMessage()
        : mc::MessageImpl()
		, mc::CommonImpl< IDataMessage >()
		, dataLength_( 0 )
		, data_( NULL )
    {
        mc::MessageImpl::PutProperty(PROP_METADATA, "");        
    }
    
	DataMessage::DataMessage(mc::RefGuid code, const std::string& metadata, size_t dataLength, const char * data)
		: mc::MessageImpl( code )
		, mc::CommonImpl< IDataMessage >()
		, dataLength_( dataLength )
		, data_( NULL )
	{
        mc::MessageImpl::PutProperty(PROP_METADATA, metadata);                
		if ( dataLength > 0 && data )
		{
			data_ = new char[ dataLength ];
			memcpy( data_, data, dataLength );
		}
		else
			throw std::exception();
	}

	DataMessage::~DataMessage()
	{
		if ( data_ )
			delete [] data_;
	}

	/**
     * IDataMessage section
     */
    
    std::string DataMessage::GetMetadata()
	{
        return mc::MessageImpl::GetProperty(PROP_METADATA).toString();
	}

	size_t DataMessage::GetDataLength() const
	{
		return dataLength_;
	}

	const char * DataMessage::GetData() const
	{
		return data_;
	}
    
    /**
     * IMessage section
     */
    
	mc::IMessage::Size_ DataMessage::GetDataSize()
    {
        return 
			// Size of metadata property
			sizeof( size_t ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_METADATA).toString() ) +
			// Size of data length property
			sizeof( size_t ) +
			// Size of data
			dataLength_;
    }

    /**
     * MessageImpl section
     */
    
	mc::result DataMessage::WriteBody(std::ostream& stream)
    {
        // Dump metadata.
        std::string metadata = mc::MessageImpl::GetProperty(PROP_METADATA).toString();		
        mc::MessageImpl::Write(stream, metadata);

        // Dump message data.
		if ( data_ && dataLength_ > 0 ) 
		{
			stream.write((char*)&dataLength_, sizeof(size_t));
			stream.write((char*)data_, dataLength_);
		}
		else
		{
			dataLength_ = 0;
			stream.write((char*)&dataLength_, sizeof(size_t));
		}
        
        return mc::_S_OK;
    }

	mc::result DataMessage::ReadBody(std::istream& stream)
    {
        // Read metadata.        
        std::string metadata;		
        mc::MessageImpl::Read(stream, metadata);        
        mc::MessageImpl::PutProperty(PROP_METADATA, metadata);
		
        // Read message data.        
		delete [] data_;
		data_ = NULL;

		stream.read((char*)&dataLength_, sizeof(size_t));
		if ( 0 == dataLength_ )
			return mc::_S_OK;

		data_ = new char [dataLength_];
		stream.read((char*)data_, dataLength_);
        
        return mc::_S_OK;
    }

}

