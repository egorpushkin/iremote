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

#include "Server/IUIEvents.h"
#include "Events.h"

#include "dialog.h"

#undef max

namespace MinCOM
{

	// {E671318E-B04F-4387-B7FC-0AAF60CE7138}
	template<> const Guid TypeInfo< RemotePC::IUIEvents >::iid_ = 
        { 0xe671318e, 0xb04f, 0x4387, { 0xb7, 0xfc, 0xa, 0xaf, 0x60, 0xce, 0x71, 0x38 } };

}

namespace RemotePC
{

	Events::Events(Dialog * window)
		: mc::CommonImpl< IUIEvents >()
		, window_(window)
		// Single window is allowed by default.
		, authSem_( mc::Library::Semaphore( 1, std::numeric_limits< long >::max() ) )
	{
	}

	// IUIEvents section
	void Events::ServiceStarted()
	{
		MC_LOG_ROUTINE;
		if ( !window_ )
			return;
		window_->ServiceStarted();
	}

	void Events::FailedToStartService()
	{
		MC_LOG_ROUTINE;
		if ( !window_ )
			return;
		window_->FailedToStartService();
	}

	void Events::ProveAuthentication(size_t cookie)
	{
		MC_LOG_ROUTINE;
		if ( !window_ )
			return;
		window_->ProveAuthentication(cookie);
	}

	mc::ISemaphorePtr Events::GetAuthSem() const
	{
		return authSem_;
	}

	void Events::OnNewClient(size_t cookie)
	{
		MC_LOG_ROUTINE;
		if ( !window_ )
			return;
		window_->ShowDevice(cookie);
	}

	void Events::CheckAuth()
	{
		MC_LOG_ROUTINE;
		if ( !window_ )
			return;
		window_->CheckAuth();
	}
		
	void Events::OnDisconnection(size_t cookie)
	{
		MC_LOG_ROUTINE;
		if ( !window_ )
			return;
		window_->HideDevice(cookie);
	}

	void Events::OnOldiRemote(size_t cookie)
	{
		MC_LOG_ROUTINE;
		if ( !window_ )
			return;
		window_->ShowOldiRemoteWarning(cookie);
	}

}
