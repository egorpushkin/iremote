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

#if defined(WIN32)
#include <Tlhelp32.h>
#elif defined(__MACH__)
#include <signal.h>
#include <sys/sysctl.h>
#include "Processes.bsd.h"
#endif // Platform

namespace RemotePC
{

#if defined(WIN32)
	DWORD ProcessTool::UnwrapQPID(Q_PID pid)
	{
		return pid->dwProcessId;
	}

	void ProcessTool::KillChildren(DWORD pid)
	{
		HANDLE processSnap;
		PROCESSENTRY32 pe32;

		// Take a snapshot of all processes in the system.
		processSnap = ::CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
		if ( INVALID_HANDLE_VALUE == processSnap )
			return;
		// Set the size of the structure before using it.
		pe32.dwSize = sizeof( PROCESSENTRY32 );
		// Retrieve information about the first process, and exit if unsuccessful.
		if( !::Process32First( processSnap, &pe32 ) )
		{
			::CloseHandle( processSnap );
			return;
		}
		
		// Now walk the snapshot of processes, and find children of the 
		// specified one.
		do
		{
			// Check whether project with pid is parent of current process.
			if ( pe32.th32ParentProcessID == pid ) 
			{
				// Terminate all children of current process.
				KillChildren(pe32.th32ProcessID);

				// Terminate current process with zero return code
				HANDLE process = ::OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
				::TerminateProcess(process, 0);
				::CloseHandle(process);
			}
		} 
		while( Process32Next( processSnap, &pe32 ) );
		CloseHandle( processSnap );
	}
#elif defined(__MACH__)
	pid_t ProcessTool::UnwrapQPID(Q_PID pid)
	{
		return pid;
	}

	void KillDirectChildren(pid_t pid, kinfo_proc *processes, size_t count)
	{
		// Enumerate all children and kill it.
		for ( size_t i = 0 ; i < count ; ++i )
		{
			kinfo_proc *process = processes + i;
			if ( pid != Processes::UnwrapParentPid(process) )
				continue;
			// process points to direct child of pid.
			// Kill all potential children of this child recursively.
			KillDirectChildren(Processes::UnwrapPid(process), processes, count);
			// Kill this child.
			kill(Processes::UnwrapPid(process), SIGKILL);
		}
	}

	void ProcessTool::KillChildren(pid_t pid)
	{
		kinfo_proc *processes = NULL;
		size_t count = 0;
		// Acquire the list of processes by means of sysctl.
		int code = Processes::GetBSDProcessList(&processes, &count);
		if ( 0 != code )
			// TODO: Plan 'B' should follow here.
			return;
		// Kill all direct children of the process identified by pid.
		KillDirectChildren(pid, processes, count);
		// Free memory, allocated by Processes::GetBSDProcessList.
		Processes::Free(processes);
	}

	/**
	 * This ugly routine was created by me. It is no longer used, but it is
	 * saved as an example.
	 */
/*	void ProcessTool::KillChildren(pid_t pid)
	{
		// Format process identifier to a string.
		std::stringstream pidstream;
		pidstream << pid;
		std::string pidstring;
		pidstream >> pidstring;

		// Format command to achieve list of shild processes ids.
		std::string cmd
			= std::string("ps -ef | awk '$3=='")
			+ pidstring
			+ std::string("'{ print $2 }'");

		// Execute command.
		FILE *fp = popen(cmd.c_str(), "r");
		if ( fp )
		{
			// Read output.
			while ( !feof(fp) )
			{
				// Extrach each id from 'ps' output.
				pid_t childid;
				fscanf(fp, "%d", &childid);

				// Terminate all children of current process.
				KillChildren(childid);

				// Terminate current process with zero return code
				kill(childid, SIGKILL);
			}
			pclose(fp);
		}
	} */

#endif // Platform

}
