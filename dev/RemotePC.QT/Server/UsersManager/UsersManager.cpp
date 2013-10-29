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
#include "DUsersManager.h"
#include "UsersManager.h"
#include "DUsersManagerStub.h"
#include "../Config.h"

namespace RemotePC
{

	UsersManager::UsersManager()
		: lock_( mc::Library::ReadWriteLock() )
		, users_()
		, eventsProvider_( mc::Class< mc::APImpl >::Create() )
		, managerEvents_()
	{
		// Register local dispatcher of DUsersManager events.
		mc::FactoryHolder::Instance()->Register(
			mc::TypeInfo< DUsersManager >::GetGuid(), mc::Class< DUsersManagerStub >::Create );
		// Create events spreader.
		managerEvents_ = mc::APImpl::AdviseAndThrow(eventsProvider_, mc::TypeInfo< DUsersManager >::GetGuid());
	}

	UsersManager& UsersManager::Instance()
	{
		return Loki::SingletonHolder< UsersManager >::Instance();
	}

	mc::IReadWriteLockPtr UsersManager::GetLock()
	{
		return lock_;
	}

	size_t UsersManager::GetUsersCount() const
	{
		return users_.size();
	}

	User& UsersManager::GetUser(size_t index)
	{
		return users_.at(index);
	}

	User& UsersManager::GetUser(const std::string & deviceId)
	{
		Users_::iterator iter = users_.begin();
		Users_::iterator end = users_.end();
		for ( ; iter != end ; ++iter )
		{		
			if ( (*iter).GetDeviceId() == deviceId )
				return (*iter);
		}
		throw std::exception();
	}

	User& UsersManager::GetUserByCookie(size_t cookie)
	{
		Users_::iterator iter = users_.begin();
		Users_::iterator end = users_.end();
		for ( ; iter != end ; ++iter )
		{		
			if ( (*iter).GetCookie() == cookie )
				return (*iter);
		}
		throw std::exception();
	}

	void UsersManager::AddUser(const User& user)
	{
		// This operation locks users database internally to write data.
		mc::IReadWriteLock::Writer_ locker(lock_);
		
		// Do add user.
		users_.push_back(user);

		// Notify all subscribers on new user.
		// TODO: Generate user cookie.
		managerEvents_->UserAdded(user.GetCookie());
	}

	void UsersManager::RemoveUser(size_t cookie)
	{
		// This operation locks users database internally to write data.
		mc::IReadWriteLock::Writer_ locker(lock_);

		// Find user by the specified cookie.
		Users_::iterator iter = users_.begin();
		Users_::iterator end = users_.end();
		for ( ; iter != end ; ++iter )
		{		
			if ( (*iter).GetCookie() == cookie )
			{
				// Remove found user from the database.
				users_.erase(iter);
				return;
			}
		}		
	}

	bool UsersManager::CheckPassword(const std::string& deviceId, const std::string& password) const
	{
		// This operation locks users database internally to read data.
		mc::IReadWriteLock::Reader_ locker(lock_);

		Users_::const_iterator iter = users_.begin();
		Users_::const_iterator end = users_.end();
		for ( ; iter != end ; ++iter )
		{
			if ( (*iter).GetDeviceId() != deviceId )
				continue;
			// Device is found. Check the password. 
			return (*iter).TestPassword(password);
		}
		// Device with specified id was not found.
		return false;
	}

	bool UsersManager::IsValidUser(const std::string& deviceId) const
	{
		// This operation locks users database internally to read data.
		mc::IReadWriteLock::Reader_ locker(lock_);

		Users_::const_iterator iter = users_.begin();
		Users_::const_iterator end = users_.end();
		for ( ; iter != end ; ++iter )
		{		
			if ( (*iter).GetDeviceId() == deviceId )
				return true;
		}
		// Device with specified id was not found.
		return false;
	}

	mc::result UsersManager::Advise(mc::ICommonRef sink)
	{
		return mc::Events::Advise(eventsProvider_, sink, mc::TypeInfo< DUsersManager >::GetGuid());			
	}

	mc::result UsersManager::Unadvise(mc::ICommonRef sink)
	{
		return mc::Events::Unadvise(eventsProvider_, sink, mc::TypeInfo< DUsersManager >::GetGuid());			
	}

}
