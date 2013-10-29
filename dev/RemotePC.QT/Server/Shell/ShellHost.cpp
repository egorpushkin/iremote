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

#include "ShellSupport.h"

namespace RemotePC
{

	ShellHost::ShellHost()
		: shelllock_( mc::Library::Mutex() )
		, shellThread_()
		, shellControl_()
	{		
	}

	ShellHost::~ShellHost()
	{
		Exit();	
	}

	ShellHost& ShellHost::Instance()
	{
		return Loki::SingletonHolder< ShellHost >::Instance();
	}

	mc::result ShellHost::Launch()
	{
		mc::MutexScope locker(shelllock_);

		if ( shellControl_ )
			return mc::_E_ALREADYINIT;

		// Start shell service.
		shellThread_ = mc::Library::Thread();
		shellControl_ = mc::Class< ShellContext >::Create();
		shellThread_->SetContext(shellControl_);
		shellThread_->Start();

		return mc::_S_OK;
	}

	mc::result ShellHost::Exit()
	{
		mc::MutexScope locker(shelllock_);

		if ( !shellControl_ )
			return mc::_E_NOTINIT;

		shellControl_->Exit();
		shellThread_->Join();
		shellControl_ = NULL;
		shellThread_ = NULL;

		return mc::_S_OK;
	}

	mc::result ShellHost::Repair()
	{
		return mc::_E_NOTIMPL;
	}

	IShellControlPtr ShellHost::GetShellControl()
	{
		mc::MutexScope locker(shelllock_);
		return shellControl_;
	}
	
}
