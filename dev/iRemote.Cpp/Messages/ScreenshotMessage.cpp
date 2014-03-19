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

#include "ScreenshotMessage.h"

#ifdef false // _DEBUG
#include <fstream>
#endif // _DEBUG

namespace MinCOM
{
	// {6E8F09B1-4181-4876-9FAD-7661F9E806D5}
	template<> const Guid TypeInfo< RemotePC::IScreenshotMessage >::iid_ =
        { 0x6e8f09b1, 0x4181, 0x4876, { 0x9f, 0xad, 0x76, 0x61, 0xf9, 0xe8, 0x6, 0xd5 } };


}

namespace RemotePC
{
    // {8FD4CD0D-A763-43AF-B57C-0994E1FA433B}
	const mc::Guid ScreenshotMessage::serviceId_ =
		{ 0x8fd4cd0d, 0xa763, 0x43af, { 0xb5, 0x7c, 0x9, 0x94, 0xe1, 0xfa, 0x43, 0x3b } };

	// {211140BB-ED67-4952-AFD6-7360AF70D785}
	const mc::Guid ScreenshotMessage::messageId_ =
        { 0x211140bb, 0xed67, 0x4952, { 0xaf, 0xd6, 0x73, 0x60, 0xaf, 0x70, 0xd7, 0x85 } };
        
	ScreenshotMessage::ScreenshotMessage()
        : mc::MessageImpl( messageId_ )
		, mc::CommonImpl< IScreenshotMessage >()
		, dataLength_( 0 )
		, data_( NULL )
    {
    }
    
    ScreenshotMessage::ScreenshotMessage(int dataLength, const char * data)
		: mc::MessageImpl( messageId_ )
		, mc::CommonImpl< IScreenshotMessage >()
		, dataLength_( dataLength )
		, data_( NULL )
	{
		if ( dataLength > 0 && data )
		{
			data_ = new char[ dataLength ];
			memcpy( data_, data, dataLength );
		}
		else
			throw std::exception();
	}

	ScreenshotMessage::~ScreenshotMessage()
	{
		if ( data_ )
			delete [] data_;
	}

	// IScreenshotMessage section
    int ScreenshotMessage::GetDataLength()
	{
		return dataLength_;
	}

	const char * ScreenshotMessage::GetData()
	{
		return data_;
	}
    
    // IMessage section
	mc::IMessage::Size_ ScreenshotMessage::GetDataSize()
    {
        return 
			// Size of data length property
            sizeof( int ) +
			// Size of data
			dataLength_;
    }

    // MessageImpl section
	mc::result ScreenshotMessage::WriteBody(std::ostream& stream)
    {
#ifdef false // _DEBUG
		// Dump screenshot to file.
		std::ofstream dbgFile("screenshot.jpg", std::ios::binary);
		if ( !dbgFile.bad() )
		{
			dbgFile.write(data_, dataLength_);
			dbgFile.close();
		}		
#endif

		if ( data_ && dataLength_ > 0 ) 
		{
            stream.write((char*)&dataLength_, sizeof(int));
			stream.write((char*)data_, dataLength_);
		}
		else
		{
			dataLength_ = 0;
            stream.write((char*)&dataLength_, sizeof(int));
		}
        
        return mc::_S_OK;
    }

	mc::result ScreenshotMessage::ReadBody(std::istream& stream)
    {
		delete [] data_;
		data_ = NULL;

        stream.read((char*)&dataLength_, sizeof(int));
		if ( 0 == dataLength_ )
			return mc::_S_OK;

		data_ = new char [dataLength_];
		stream.read((char*)data_, dataLength_);
        
        return mc::_S_OK;
    }

}

