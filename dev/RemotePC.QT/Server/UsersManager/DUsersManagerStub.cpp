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

#include "DUsersManager.h"
#include "DUsersManagerStub.h"

namespace MinCOM
{

	// {0F288B2E-E6C4-4c20-8B48-F8195E4430C0}
	template<> const Guid TypeInfo< RemotePC::DUsersManager >::iid_ = 
		{ 0xf288b2e, 0xe6c4, 0x4c20, { 0x8b, 0x48, 0xf8, 0x19, 0x5e, 0x44, 0x30, 0xc0 } };

}

namespace RemotePC
{

	// {5D416610-1FF3-4695-B1CB-F3271B8A8485}
	const mc::Guid UserAddedFnIid = 
		{ 0x5d416610, 0x1ff3, 0x4695, { 0xb1, 0xcb, 0xf3, 0x27, 0x1b, 0x8a, 0x84, 0x85 } };


	void DUsersManagerStub::UserAdded(size_t userCookie)
	{
		mc::Call::Make(
			mc::CommonImpl< DUsersManager >::GetSelf(), 
			UserAddedFnIid, 
			userCookie);
	}

	// ICommon section
	mc::result DUsersManagerStub::Invoke(const mc::Call& call)
	{
		DUsersManagerPtr events( mc::CommonImpl< DUsersManager >::GetTarget() );
		if ( !events )
			return mc::_E_NOTINIT;

		if ( UserAddedFnIid == call.GetMethod() )
		{
			events->UserAdded(call.GetArguments().at(0).toLong());
			call.GetResults().push_back(mc::_S_OK);
		}

		return mc::_S_OK;
	}

}
