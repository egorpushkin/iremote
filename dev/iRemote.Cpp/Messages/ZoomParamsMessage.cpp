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

#include "ZoomParamsMessage.h"

namespace RemotePC
{

	// {9E67DE9A-BBEC-40a6-8CA6-22E93B56A4E4}
	const mc::Guid ZoomParamsMessage::messageId_ =   
		{ 0x9e67de9a, 0xbbec, 0x40a6, { 0x8c, 0xa6, 0x22, 0xe9, 0x3b, 0x56, 0xa4, 0xe4 } };

    ZoomParamsMessage::ZoomParamsMessage()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_MIN_ZOOM, 1.0f);
        mc::MessageImpl::PutProperty(PROP_MAX_ZOOM, 1.0f);
		mc::MessageImpl::PutProperty(PROP_CURRENT, 1.0f);
	}
    
	ZoomParamsMessage::ZoomParamsMessage( float minZoom, float maxZoom, float current )
        : mc::MessageImpl( messageId_ )
    {
		mc::MessageImpl::PutProperty(PROP_MIN_ZOOM, minZoom);
		mc::MessageImpl::PutProperty(PROP_MAX_ZOOM, maxZoom);
		mc::MessageImpl::PutProperty(PROP_CURRENT, current);
    }
    
    // IMessage section
    mc::IMessage::Size_ ZoomParamsMessage::GetDataSize()
    {
        return 
			// Size of min zoom property
			sizeof( float ) + 
			// Size of max zoom property
			sizeof( float ) +
			// Size of current property
			sizeof( float );
    }

    // MessageImpl section
    mc::result ZoomParamsMessage::WriteBody(std::ostream& stream)
    {
        float minZoom = mc::MessageImpl::GetProperty(PROP_MIN_ZOOM).toFloat();
        float maxZoom = mc::MessageImpl::GetProperty(PROP_MAX_ZOOM).toFloat();
		float current = mc::MessageImpl::GetProperty(PROP_CURRENT).toFloat();

        mc::MessageImpl::Write(stream, minZoom);
        mc::MessageImpl::Write(stream, maxZoom);
		mc::MessageImpl::Write(stream, current);

        return mc::_S_OK;
    }

    mc::result ZoomParamsMessage::ReadBody(std::istream& stream)
    {
        float minZoom = 1.0f;
        float maxZoom = 1.0f;
		float current = 1.0f;
        
        mc::MessageImpl::Read(stream, minZoom);
        mc::MessageImpl::Read(stream, maxZoom);        
		mc::MessageImpl::Read(stream, current);

        mc::MessageImpl::PutProperty(PROP_MIN_ZOOM, minZoom);
        mc::MessageImpl::PutProperty(PROP_MAX_ZOOM, maxZoom);
		mc::MessageImpl::PutProperty(PROP_CURRENT, current);
        
        return mc::_S_OK;
    }

}
