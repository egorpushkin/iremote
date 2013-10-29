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

#ifndef USERSMANAGER_H__REMOTEPC__INCLUDED_
#define USERSMANAGER_H__REMOTEPC__INCLUDED_

namespace RemotePC 
{

	class UsersManager
	{
	protected:

		/** Allows to protect class constructor. */
		friend struct Loki::CreateUsingNew< UsersManager >;

		UsersManager();

	public:

		static UsersManager& Instance();

		mc::IReadWriteLockPtr GetLock();

		size_t GetUsersCount() const;

		User& GetUser(size_t index);

		User& GetUser(const std::string & deviceId);

		User& GetUserByCookie(size_t cookie);

		void AddUser(const User& user);

		void RemoveUser(size_t cookie);

		/** 
		 * Accepts password in md5.
		 */
		bool CheckPassword(const std::string& deviceId, const std::string& password) const;

		bool IsValidUser(const std::string& deviceId) const;

		mc::result Advise(mc::ICommonRef sink);

		mc::result Unadvise(mc::ICommonRef sink);

	private:

		mc::IReadWriteLockPtr lock_;

		typedef std::vector< User > Users_;

		Users_ users_;

		mc::IAccessProviderPtr eventsProvider_;

		DUsersManagerPtr managerEvents_;

	};
	
}


#endif // !USERSMANAGER_H__REMOTEPC__INCLUDED_
