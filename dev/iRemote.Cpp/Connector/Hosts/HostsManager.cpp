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

#include "NetInterface.h"
#include "Host.h"
#include "HostsManager.h"

#include "../Config/Config.h"

namespace RemotePC
{

	HostsManager::HostsManager()
		: lock_( mc::Library::ReadWriteLock() )
		, hosts_()
	{
	}

	HostsManager& HostsManager::Instance()
	{
		return Loki::SingletonHolder< HostsManager >::Instance();
	}

	mc::IReadWriteLockPtr HostsManager::GetLock()
	{
		return lock_;
	}

	size_t HostsManager::GetHostsCount() const
	{
		return hosts_.size();
	}

	Host& HostsManager::GetHost(size_t index)
	{
		return hosts_.at(index);
	}
	
	Host& HostsManager::GetHost(mc::RefGuid hostId)
	{
		Hosts_::iterator iter = hosts_.begin();
		Hosts_::iterator end = hosts_.end();
		for ( ; iter != end ; ++iter )
		{		
			if ( (*iter).GetHostId() == hostId )
				return (*iter);
		}
		throw std::exception();		
	}
	
	bool HostsManager::HasHost(mc::RefGuid hostId)
	{
		Hosts_::iterator iter = hosts_.begin();
		Hosts_::iterator end = hosts_.end();
		for ( ; iter != end ; ++iter )
		{		
			if ( (*iter).GetHostId() == hostId )
				return true;
		}
		return false;
	}		

	void HostsManager::AddHost(const Host& host)
	{
		// This operation locks users database internally to write data.
		mc::IReadWriteLock::Writer_ locker(lock_);
		
		// Do add user.
		hosts_.push_back(host);
        
        // Save config.
        Config::Instance().Save();
	}

	void HostsManager::RemoveInterface(size_t hostIndex, size_t interfaceIndex)
	{
		// This operation locks users database internally to write data.
		mc::IReadWriteLock::Writer_ locker(lock_);
		
		// Check whether specified index exists in the list.
		if ( hosts_.size() <= hostIndex )
			return;

		// Acquire host, which must be removed.
		Host& host = hosts_.at(hostIndex);		
		if ( host.GetInterfacesCount() <= 1 )
		{
			// Remove host with specified index.
			hosts_.erase( hosts_.begin() + hostIndex);
		}
		else
		{
			// Remove host interface with specified index.
			host.RemoveInterface(interfaceIndex);
		}
        
        // Save config.
        Config::Instance().Save();
	}
	
	void HostsManager::SaveHost(const Host& srcHost) 
	{
		// This locks users database internally for writing.
		mc::IReadWriteLock::Writer_ locker(lock_);
		
		// Check whether host record is ready to be saved. It should meet the 
		// Following conditions:
		// - host id must be specified;
		// - at least one interface must be added.
		if ( !srcHost.HasHostId() || 0 == srcHost.GetInterfacesCount() )
			return;
		
		// Remember first network interface of the host to be saved.
		const NetInterface& srcInterface = srcHost.GetInterface(0);
		
		try
		{
			// Find host by id.
			Host& dstHost = HostsManager::Instance().GetHost(srcHost.GetHostId());				
			
			// Find network interface in found host with the same ip and port.
			bool interfaceFound = false;
			size_t dstInterfacesCount = dstHost.GetInterfacesCount();
			for ( size_t i = 0 ; i < dstInterfacesCount ; ++i )
			{
				const NetInterface& dstNetInterface = dstHost.GetInterface(i);
				if ( ( dstNetInterface.GetIp() == srcInterface.GetIp() ) && 
					( dstNetInterface.GetPort() == srcInterface.GetPort() ) )
				{
					interfaceFound = true;
					break;
				}				
			}
			// Check whether interface with current ip and port is found.
			if ( !interfaceFound )
			{
				// Add one more interface to found host.
				NetInterface netInterface;
				netInterface.SetIp(srcInterface.GetIp());
				netInterface.SetPort(srcInterface.GetPort());	
				dstHost.AddInterface(netInterface);
			}
			
			// Update records of found host.
            // Host object, whih is added upon successful authentication (srcHost),
            // contains password, if
            // - it was entered by user (Holder::SubmitPassword);
            // - if was extracted from the db (ServerContext::HandlePasswordRequest);
            // and does not contain password if
            // - device was authorized anonymously (user allowed connection once).
            // Thus, password should be saved, if it exists, and should be 
            // cleared, otherwise.
            if ( srcHost.HasPassword() )
				dstHost.SetMd5Password(srcHost.GetPassword());			
            else
                dstHost.ResetPassword();
		}
		catch ( ... )
		{
			// Host was not found. Add new host here. 
			AddHost(srcHost);
		}
        
        // Save config.
        Config::Instance().Save();
	}
	
}
