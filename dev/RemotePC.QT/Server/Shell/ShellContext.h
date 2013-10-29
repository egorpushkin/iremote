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

#ifndef SHELLCONTEXT_H__REMOTEPC__INCLUDED_
#define SHELLCONTEXT_H__REMOTEPC__INCLUDED_

namespace RemotePC
{

	class ShellContext
		: public mc::CommonImpl< IShellControl >
		, public mc::CommonImpl< mc::IRunnable >
	{
	public:

		/** Delay after server acquisition failure, ms. */
		static const int shellFreeze_;
		
		static const int shellWaitOutput_;

		static const size_t shellMaxResponseSize_;

		/** Delay between sending portions of long response, ms. */
		static const int responseDelay_;

	public:

		typedef mc::CommonImpl< IShellControl > ClassRoot_;

		ShellContext();
		virtual ~ShellContext();

		// IShellControl section
		virtual void AttachServer(IServerControlRef server);

		virtual mc::result Write(const std::string& command, bool user);

		/**
		 * Caller must join wrapping thread after referencing to this routine.
		 */
		virtual mc::result Exit();

		// IRunnable section
		virtual mc::result Run();

	private:

		// Private tools
		mc::result StartProcess();

		mc::result StopProcess();

		void SpreadResponse(IServerControlRef server, const std::string response);

		void SpreadResponseCore(IServerControlRef server, const std::string response, bool delay);

	private:

		/** Used to lock server reference. */
		mc::IMutexPtr serverLock_;

		/** Contains weak reference to server control. */
		IServerControlWeak server_;

		/** Used to lock lifetime flag. */
		mc::IMutexPtr lifetimeLock_;

		/** Indicates whether termination is requested by the application. */
		bool termitationRequested_;

		/** Wraps and manager shell process. */
		QProcess * shell_;		

	};

}

#endif // !SHELLCONTEXT_H__REMOTEPC__INCLUDED_
