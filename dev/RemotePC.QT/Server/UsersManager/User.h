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

#ifndef USER_H__REMOTEPC__INCLUDED_
#define USER_H__REMOTEPC__INCLUDED_

namespace RemotePC 
{

	/**
	 * Contains information about single user account. 
	 * This class operates with user passwords and stores it in md5 locally on 
	 * the host side. Each routine of the class defines whether it receives
	 * (returns) strings in raw or md5 form. 
	 */
	class User
	{
	public:

		explicit User(const std::string& deviceId, const std::string& deviceName, const std::string& ip, bool request);

		/** 
		 * Accepts raw password string.
		 */
		void SetPassword(const std::string& password);

		/** 
		 * Accepts md5 password string.
		 */
		void SetMd5Password(const std::string& password);

		/**
		 * Accepts password in md5 and returns true if it is correct.
		 */
		bool TestPassword(const std::string& password) const;

		void SetIp(const std::string& ip);

		std::string GetIp() const;

		void SetIpRequired(bool required);

		bool IsIpRequired() const;

		std::string GetDeviceId() const;

		void SetDeviceName(const std::string& name);

		std::string GetDeviceName() const;

		/** 
		 * Returns md5 form of a password.
		 */
		std::string GetPassword() const;	

		void RequestAuth(bool request);

		bool IsAuthRequested() const;

		size_t GetCookie() const;

	protected:

		std::string CalculateMD5(const std::string& source);

	private:

		std::string deviceId_;

		std::string deviceName_;

		std::string ip_;

		bool requireIp_;

		bool hasPassword_;

		bool requestAuth_;

		/** User password is stored in md5. */		 
		std::string password_;

		/** 
		 * This field is required because &user always gives different result,
		 * because users are stored by value in std::vector.
		 */
		mc::ICommonPtr cookieHolder_;

	};
	
}


#endif // !USER_H__REMOTEPC__INCLUDED_
