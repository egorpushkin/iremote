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

#include "Client.h"
#include "Messages/AuthMessage.h"
#include "Versions/Versions.h"
#include "Versions/Features.h"

namespace RemotePC
{

	Client::Client()
		: ip_()
		, cookie_()
		, authenticated_( false )
		, authMessage_()
        , frameBuffer_( false )
        , screenCaptureEnabled_( true )
		, zoomLevel_(1.0f)
	{
	}

    Client::Client(const std::string& ip, size_t cookie)
		: ip_(ip)
		, cookie_(cookie)
		, authenticated_( false )
		, authMessage_()
        , frameBuffer_( false )
        , screenCaptureEnabled_( true )
		, zoomLevel_(1.0f)
	{
	}

	void Client::AttachMessage(mc::IMessageRef authMessage)
	{
		authMessage_ = authMessage;
	}

	bool Client::HasAuthMessage()
	{
		return ( NULL != authMessage_ );
	}

    mc::IMessagePtr Client::GetAuthMessage() const
    {
        return authMessage_;
    }

	void Client::SetAuthenticated(bool auth)
	{
		authenticated_ = auth;
	}

	bool Client::IsAuthenticated() const
	{
		return authenticated_;
	}

	size_t Client::GetCookie() const
	{
		return cookie_;
	}

	std::string Client::GetIp() const
	{
		return ip_;
	}

	std::string Client::GetId() const
	{
		if ( !authMessage_ )
			throw std::exception();
		return authMessage_->GetProperty(AuthMessage::PROP_ID).toString();
	}

	int Client::GetDevice() const
	{
		if ( !authMessage_ )
			throw std::exception();
		return authMessage_->GetProperty(AuthMessage::PROP_DEVICE).toInt();
	}

	std::string Client::GetName() const
	{
		if ( !authMessage_ )
			throw std::exception();
		return authMessage_->GetProperty(AuthMessage::PROP_NAME).toString();
	}

	int Client::GetMajorVersion() const
	{
		if ( !authMessage_ )
			throw std::exception();
		return authMessage_->GetProperty(AuthMessage::PROP_VER_MAJOR).toInt();
	}

	int Client::GetMinorVersion() const
	{
		if ( !authMessage_ )
			throw std::exception();
		return authMessage_->GetProperty(AuthMessage::PROP_VER_MINOR).toInt();
	}

	bool Client::IsFree() const
	{
		if ( !authMessage_ )
			throw std::exception();
		mc::Guid app = authMessage_->GetProperty(AuthMessage::PROP_APP).toGuid();
		if ( iRemoteApplicationGuid == app )
			return true;
		return false;
	}

	bool Client::SupportPasswordProtection() const
	{
		return Features::iRemotePasswordProtection(GetMajorVersion(), GetMinorVersion());
	}

    void Client::SetFrameBuffer(bool mode)
    {
        frameBuffer_ = mode;
    }

    bool Client::IsFrameBuffer() const
    {
        return frameBuffer_;
    }

    void Client::EnableScreenCapture(bool enable)
    {
        screenCaptureEnabled_ = enable;
    }

    bool Client::IsScreenCaptureEnabled() const
    {
        return screenCaptureEnabled_;
    }

	void Client::SetZoomLevel(float level)
	{
		zoomLevel_ = level;
	}

	float Client::GetZoomLevel() const
	{
		return zoomLevel_;
	}

	bool Client::IsZoomingSupported() const
	{
		return Features::iRemoteZooming(GetMajorVersion(), GetMinorVersion());
	}

	bool Client::AreServicesSupported() const
	{
		return Features::iRemoteServices(GetMajorVersion(), GetMinorVersion());
	}
}
