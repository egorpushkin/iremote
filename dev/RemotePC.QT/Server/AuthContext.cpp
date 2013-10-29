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

#include "AuthContext.h"

namespace RemotePC
{

	AuthContext::AuthContext(size_t cookie, IUIEventsRef events)
		: mc::CommonImpl< mc::IRunnable >()
		, cookie_(cookie)
		, events_(events)
	{
	}

	// IRunnable section
	mc::result AuthContext::Run()
	{	
		MC_LOG_ROUTINE;
		if ( !events_ )
			return mc::_S_FALSE;

		// Wait until the only allowed Dialog::ProveAuthentication quits.
		events_->GetAuthSem()->Wait();

		// Display confirmation dialog.
		events_->ProveAuthentication(cookie_);
	
		// Authentication window is now shown asynchronously.
		// UI should take care about client handling by means of
		// IServerContext::HandleClient().

		return mc::_S_OK;
	}

}

