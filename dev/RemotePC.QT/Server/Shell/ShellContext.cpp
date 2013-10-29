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

namespace MinCOM
{

	// {A31032AC-0033-4e87-945D-DC342A6828A2}
	template<> const Guid TypeInfo< RemotePC::IShellControl >::iid_ = 
		{ 0xa31032ac, 0x33, 0x4e87, { 0x94, 0x5d, 0xdc, 0x34, 0x2a, 0x68, 0x28, 0xa2 } };

}

namespace RemotePC
{

	const int ShellContext::shellFreeze_ = 5;
	
	const int ShellContext::shellWaitOutput_ = 0;

	const int ShellContext::responseDelay_ = 80;

	const size_t ShellContext::shellMaxResponseSize_ = 256;

	ShellContext::ShellContext()
		: mc::CommonImpl< IShellControl >()
		, mc::CommonImpl< mc::IRunnable >()
		, serverLock_( mc::Library::Mutex() )
		, server_()
		, lifetimeLock_( mc::Library::Mutex() )
		, termitationRequested_(false)
		, shell_(NULL)
	{
	}

	ShellContext::~ShellContext()
	{
		Exit();
	}

	// IShellControl section
	void ShellContext::AttachServer(IServerControlRef server)
	{
		mc::MutexScope locker(serverLock_);
		server_ = server;
	}

	mc::result ShellContext::Write(const std::string& command, bool user)
	{
		MC_LOG_ROUTINE;		

		if ( !shell_ )
			return mc::_E_NOTIMPL;

		// Check for control commands.
		if ( "ctrl-c" == command )
		{
			// Send 'ctrl-c' signal.
			CtrlHandler::GenerateCtrl(ProcessTool::UnwrapQPID(shell_->pid()));
		}
		// Execute command as is.
		else
		{
			std::string formattedCommand = command;
			// Only user level commands should be preprocessed.
			if ( user )
			{
				// Remove heading spaces.
				while ( ( formattedCommand.size() > 0 ) && ( ' ' == formattedCommand.at(0) ) )
					formattedCommand = formattedCommand.substr(1);
				// Check for "exit" command and skip it.
				if ( 0 == formattedCommand.find("exit") )
					return mc::_S_FALSE;
			}
			// Add '\n' to the end of the command to inform shell on enter.
			std::string finalCommand 
				= formattedCommand
				+ std::string("\n");
			// Write command directly to shell input stream.
			shell_->write(finalCommand.c_str());
		}

		return mc::_S_OK;
	}

	mc::result ShellContext::Exit()
	{
		MC_LOG_ROUTINE;

		if ( !shell_ )
			return mc::_E_NOTIMPL;

		// Kill all shell's children.
		ProcessTool::KillChildren(ProcessTool::UnwrapQPID(shell_->pid()));

		// Stop shell by sending "exit" command. 
		{
			mc::MutexScope locker(lifetimeLock_);			
			// Notify shell process on termination. 
			Write("exit", false);
			// Notify context on termination.
			termitationRequested_ = true;
		}
		
		return mc::_S_OK;
	}

	// IRunnable section
	mc::result ShellContext::Run()
	{	
		MC_LOG_ROUTINE;
		
		bool infinity = true;
		while ( infinity )
		{
			// Attempt to launch child shell process.
            while ( mc::_S_OK != StartProcess() ) {}

			// Read data from shell until the service is stopped.
			while ( QProcess::Running == shell_->state() ) 
			{
				// Try read standard output.
				while ( shell_->waitForReadyRead(shellWaitOutput_) )
				{
					QByteArray dataRead = shell_->readAllStandardOutput();
					std::string response(dataRead.data()); 			

					// Acquire server control.
					{
						mc::MutexScope locker(serverLock_);
						IServerControlPtr server = IServerControlPtr(server_);
						if ( !server )
						{
							mc::IThread::Sleep(shellFreeze_);
							break;
						}
						SpreadResponse(server, response);
					}
				}

				// Wait before next attempt.
				mc::IThread::Sleep(shellFreeze_);
			}

			// Destroy process wrapper correctly.
			StopProcess();

			// Check whether shell should exists.
			{
				mc::MutexScope locker(lifetimeLock_);				
				if ( termitationRequested_ )
					infinity = false;
			}
		}

		return mc::_S_OK;
	}

	// Private tools
	mc::result ShellContext::StartProcess()
	{
		shell_ = new QProcess();
		shell_->setProcessChannelMode(QProcess::MergedChannels);
#if defined(WIN32)
		shell_->start("cmd.exe");		
#elif defined(__MACH__)
		// Bash is launched with -i option, which makes shell 'interactive'.
		// This is required to redirect prompt and user input to output stream
		// automatically.
		shell_->start("/bin/bash", QStringList() << "-i");
#endif // Platform
		if ( !shell_->waitForStarted() )
		{
			delete shell_;
			shell_ = NULL;
			// Notify caller on error. 
			// Caller takes care about restarting attempts
			return mc::_E_FAIL;
		}
		shell_->setReadChannel(QProcess::StandardOutput);
		return mc::_S_OK;
	}

	mc::result ShellContext::StopProcess()
	{
		shell_->waitForFinished();
		delete shell_;
		shell_ = NULL;
		return mc::_S_OK;
	}

	/**
	 * This method delivers response to clients, breaking into small pieces
	 * (each of shellMaxResponseSize_ or less) to prevent client hung.
	 */
	void ShellContext::SpreadResponse(IServerControlRef server, const std::string response)
	{
		std::string localResponse = response;
		// Check whether we are dealing with long message.
		if ( localResponse.size() > shellMaxResponseSize_ )
		{
			while ( localResponse.size() > shellMaxResponseSize_ )
			{
				size_t endlinePos = localResponse.rfind('\n', shellMaxResponseSize_ - 1);
				std::string responsePortion;
				if ( std::string::npos == endlinePos )
				{
					// No endline character has been found inside the first portion.
					responsePortion = localResponse.substr(0, shellMaxResponseSize_);
					localResponse = localResponse.substr(shellMaxResponseSize_, localResponse.size() - shellMaxResponseSize_);
				}
				else
				{
					// Endline has been found.
					responsePortion = localResponse.substr(0, endlinePos);
					localResponse = localResponse.substr(endlinePos + 1, localResponse.size() - endlinePos - 1);
				}
				// Send portion of data to clients and delay before sending next
				// fragment.
				SpreadResponseCore(server, responsePortion, true);
			}
		}
		// Send message tail if it was too long,
		// or the entire message if it was short enough.
		if ( localResponse.size() > 0 )
			SpreadResponseCore(server, localResponse, false);
	}

	/**
	 * This method send response to clients as is. This additionally
	 */
	void ShellContext::SpreadResponseCore(IServerControlRef server, const std::string response, bool delay)
	{
		server->SpreadMessage( mc::Class< ShellResponse >::Create(response, mc::_S_OK ) );
		if ( delay )
			mc::IThread::Sleep(responseDelay_);
	}

}
