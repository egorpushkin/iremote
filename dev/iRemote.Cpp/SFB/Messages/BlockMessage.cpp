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

#include "BlockMessage.h"

namespace RemotePC
{
namespace SFB
{

    // {52EB22D9-67AF-4090-B8EF-C2D9832CBCB0}
    const mc::Guid BlockMessage::messageId_ =
        { 0x52EB22D9, 0x67AF, 0x4090, { 0xB8, 0xEF, 0xC2, 0xD9, 0x83, 0x2C, 0xBC, 0xB0 } };

    BlockMessage::BlockMessage()
        : mc::MessageImpl( messageId_ )
        , mc::CommonImpl< IScreenshotMessage >()
        , dataLength_( 0 )
        , data_( NULL )
    {
        mc::MessageImpl::PutProperty(PROP_X, 1.0f);
        mc::MessageImpl::PutProperty(PROP_Y, 1.0f);
        mc::MessageImpl::PutProperty(PROP_WIDTH, 1.0f);
        mc::MessageImpl::PutProperty(PROP_HEIGHT, 1.0f);
    }

    BlockMessage::BlockMessage(float x, float y, float width, float height, size_t dataLength, const char * data)
        : mc::MessageImpl( messageId_ )
        , mc::CommonImpl< IScreenshotMessage >()
        , dataLength_( dataLength )
        , data_( NULL )
    {
        mc::MessageImpl::PutProperty(PROP_X, x);
        mc::MessageImpl::PutProperty(PROP_Y, y);
        mc::MessageImpl::PutProperty(PROP_WIDTH, width);
        mc::MessageImpl::PutProperty(PROP_HEIGHT, height);

        if ( dataLength > 0 && data )
        {
            data_ = new char[ dataLength ];
            memcpy( data_, data, dataLength );
        }
        else
            throw std::exception();
    }

    BlockMessage::~BlockMessage()
    {
        if ( data_ )
            delete [] data_;
    }

    // IScreenshotMessage section
    int BlockMessage::GetDataLength()
    {
        return dataLength_;
    }

    const char * BlockMessage::GetData()
    {
        return data_;
    }

    // IMessage section
    mc::IMessage::Size_ BlockMessage::GetDataSize()
    {
        return
            // Size of rect property
            ( sizeof(float) << 2 ) +
            // Size of data length property
            sizeof( size_t ) +
            // Size of data
            dataLength_;
    }

    // MessageImpl section
    mc::result BlockMessage::WriteBody(std::ostream& stream)
    {
        float x = mc::MessageImpl::GetProperty(PROP_X).toFloat();
		float y = mc::MessageImpl::GetProperty(PROP_Y).toFloat();
		float width = mc::MessageImpl::GetProperty(PROP_WIDTH).toFloat();
		float height = mc::MessageImpl::GetProperty(PROP_HEIGHT).toFloat();
		
        mc::MessageImpl::Write(stream, x);        
        mc::MessageImpl::Write(stream, y);        
        mc::MessageImpl::Write(stream, width);        
        mc::MessageImpl::Write(stream, height);        		

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

    mc::result BlockMessage::ReadBody(std::istream& stream)
    {
        float x = 1.0f;
        float y = 1.0f;
		float width = 1.0f;
		float height = 1.0f;		
        
        mc::MessageImpl::Read(stream, x);
        mc::MessageImpl::Read(stream, y);        
		mc::MessageImpl::Read(stream, width);
		mc::MessageImpl::Read(stream, height);		
		
        mc::MessageImpl::PutProperty(PROP_X, x);        
        mc::MessageImpl::PutProperty(PROP_Y, y);        
        mc::MessageImpl::PutProperty(PROP_WIDTH, width);        
        mc::MessageImpl::PutProperty(PROP_HEIGHT, height);        		

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
}

