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
#include "ProcessTool.h"
#include "CtrlHandler.h"

#if defined(__MACH__)
#include <signal.h>
#include <sys/sysctl.h>
#include "Processes.bsd.h"
#endif // Platform

namespace RemotePC
{

#if defined(WIN32)
	BOOL WINAPI CtrlHandlerRoutine(DWORD /* dwCtrlType */)
	{
		// Nothing specific should be done here, but TRUE must be returned.
		return TRUE;
	}
#elif defined(__MACH__)
	void CtrlHandlerRoutine(int /* signal */)
	{
	}
#endif // Platform

	void CtrlHandler::GenerateCtrl(ProcessTool::ProcessId_ pid)
	{
#if defined(WIN32)
		::AttachConsole(pid);
		::SetConsoleCtrlHandler(CtrlHandlerRoutine, TRUE);
		::GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
		::FreeConsole();
#elif defined(__MACH__)

		// Setup SIGINT handler.
		struct sigaction sa;
		memset(&sa, 0, sizeof(sa));
		sa.sa_handler = &CtrlHandlerRoutine;
		sigaction(SIGINT, &sa, NULL);

		// Here we need to send SIGINT to all children of the process,
		// identified by pid (bash in our case).

		// Acquire the list of processes by means of sysctl.
		kinfo_proc *processes = NULL;
		size_t count = 0;
		int code = Processes::GetBSDProcessList(&processes, &count);
		if ( 0 != code )
			// TODO: Plan 'B' should follow here.
			return;
		// Enumerate processes and send SIGINT to all children.
		for ( size_t i = 0 ; i < count ; ++i )
		{
			kinfo_proc *process = processes + i;
			if ( pid == Processes::UnwrapParentPid(process) )
			{
				// Send 'ctrl-c' signal to each shell's child process.
                kill(Processes::UnwrapPid(process), SIGINT);
			}
		}
		// Free memory, allocated by Processes::GetBSDProcessList.
		Processes::Free(processes);

#endif // Platform
	}

}
