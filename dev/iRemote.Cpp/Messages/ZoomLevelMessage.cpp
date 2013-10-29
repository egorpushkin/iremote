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

#include "ZoomLevelMessage.h"

namespace RemotePC
{

	// {E2E30A08-3A9F-4ac8-BA2D-D8AC0627F211}
	const mc::Guid ZoomLevelMessage::messageId_ =
		{ 0xe2e30a08, 0x3a9f, 0x4ac8, { 0xba, 0x2d, 0xd8, 0xac, 0x6, 0x27, 0xf2, 0x11 } };

	ZoomLevelMessage::ZoomLevelMessage()
		: mc::MessageImpl( messageId_ )
	{
		mc::MessageImpl::PutProperty(PROP_ZOOM_LEVEL, 1.0f);
	}

	ZoomLevelMessage::ZoomLevelMessage( float level )
		: mc::MessageImpl( messageId_ )
	{
		mc::MessageImpl::PutProperty(PROP_ZOOM_LEVEL, level);
	}

	// IMessage section
	mc::IMessage::Size_ ZoomLevelMessage::GetDataSize()
	{
		return
			// Size of zoom level property
			sizeof( float );
	}

	// MessageImpl section
	mc::result ZoomLevelMessage::WriteBody(std::ostream& stream)
	{
		float level = mc::MessageImpl::GetProperty(PROP_ZOOM_LEVEL).toFloat();

		mc::MessageImpl::Write(stream, level);

		return mc::_S_OK;
	}

	mc::result ZoomLevelMessage::ReadBody(std::istream& stream)
	{
		float level = 1.0f;

		mc::MessageImpl::Read(stream, level);

		mc::MessageImpl::PutProperty(PROP_ZOOM_LEVEL, level);

		return mc::_S_OK;
	}

}
