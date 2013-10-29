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

namespace RemotePC
{

	Host::Host()
        : hasHostId_(false)	
	    , hostId_()	
	    , hasPassword_(false)
	    , password_()
		, netInterfaces_()
	{
	}
	
	void Host::SetHostId(const std::string& hostId)
	{
		hasHostId_ = true;
		hostId_ = mc::Guid::FromString(hostId);
	}	
	
	void Host::SetHostId(mc::RefGuid hostId)
	{
		hasHostId_ = true;		
		hostId_ = hostId;
	}
	
	bool Host::HasHostId() const
	{
		return hasHostId_;
	}
	
	mc::Guid Host::GetHostId() const
	{
		return hostId_;
	}
	
	void Host::SetMd5Password(const std::string& password)
	{
		hasPassword_ = true;
		password_ = password;		
	}
	
	std::string Host::GetPassword() const
	{
		return password_;
	}
	
	bool Host::HasPassword() const
	{
		return hasPassword_;
	}	
    
    void Host::ResetPassword()
    {
        hasPassword_ = false;
        password_ = std::string();
    }
	
	void Host::AddInterface(const NetInterface& netInterface)
	{
		netInterfaces_.push_back(netInterface);
	}
	
	size_t Host::GetInterfacesCount() const
	{
		return netInterfaces_.size();
	}
	
	const NetInterface& Host::GetInterface(size_t index) const
	{
		return netInterfaces_.at(index);
	}
	
	void Host::RemoveInterface(size_t index) 
	{
		// Check whether specified index exists in the list.
		if ( netInterfaces_.size() <= index )
			return;
		// Remove interface with specified index.
		netInterfaces_.erase( netInterfaces_.begin() + index );
	}

}
