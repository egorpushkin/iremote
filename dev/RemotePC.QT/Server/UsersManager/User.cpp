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

#include "User.h"
#include "../Config.h"

#include <hashlibpp.h>

namespace RemotePC
{

	User::User(const std::string& deviceId, const std::string& deviceName, const std::string& ip, bool request)
		: deviceId_(deviceId)
		, deviceName_(deviceName)
		, ip_(ip)
		, requireIp_(true)
		, hasPassword_(false)
		, requestAuth_(request)
		, password_()
		, cookieHolder_( mc::Class< mc::CommonImpl< mc::ICommon > >::Create() )
	{
	}

	void User::SetPassword(const std::string& password)
	{	
		hasPassword_ = true;
		requestAuth_ = true;
		// Calculate md5.
		password_ = CalculateMD5(password);
	}

	void User::SetMd5Password(const std::string& password)
	{
		hasPassword_ = true;
		password_ = password;
	}

	bool User::TestPassword(const std::string& password) const
	{
		return ( password == password_ );
	}

	std::string User::GetDeviceId() const
	{
		return deviceId_;
	}

	void User::SetDeviceName(const std::string& name)
	{
		deviceName_ = name;
	}

	std::string User::GetDeviceName() const
	{
		return deviceName_;
	}

	std::string User::GetPassword() const
	{
		return password_;
	}

	void User::SetIp(const std::string& ip)
	{
		ip_ = ip;
	}

	std::string User::GetIp() const
	{
		return ip_;
	}

	void User::SetIpRequired(bool required)
	{
		requireIp_ = required;
	}

	bool User::IsIpRequired() const
	{
		return requireIp_;
	}

	void User::RequestAuth(bool request)
	{
		requestAuth_ = request;
	}

	bool User::IsAuthRequested() const
	{
		return requestAuth_;
	}

	size_t User::GetCookie() const
	{
		return cookieHolder_->GetCookie();
	}

	// Private 
	std::string User::CalculateMD5(const std::string& source)
	{
		try
		{
			Loki::SmartPtr< hashwrapper > md5Wrapper( new md5wrapper() );
			return md5Wrapper->getHashFromString(source);
		}
		catch ( ... )
		{
			return std::string("");
		}
	}

}
