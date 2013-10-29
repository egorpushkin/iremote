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

#ifndef FEATURES_H__REMOTEPC__INCLUDED_
#define FEATURES_H__REMOTEPC__INCLUDED_

// iRemote and RemotePC constants
//////////////////////////////////////////////////////////////////////////
namespace RemotePC 
{
	
	class Features
	{
	public:

		static bool RemotePCPasswordProtection(int major, int minor, int revision = 0);
		
		static bool RemotePCZooming(int major, int minor, int revision = 0);
        
        static bool RemotePCShell(int major, int minor, int revision = 0);

		static bool RemotePCServices(int major, int minor, int revision = 0);
		
		static bool iRemotePasswordProtection(int major, int minor, int revision = 0);

		static bool iRemoteExtendedAuthentication(int major, int minor, int revision = 0);
		
		static bool iRemoteZooming(int major, int minor, int revision = 0);

        static bool iRemoteMac(int major, int minor, int revision = 0);

		static bool iRemoteServices(int major, int minor, int revision = 0);
        		
	private:
		
		static bool Satisfies(int major, int minor, int majorReq, int minorReq);

	};

}

#endif // FEATURES_H__REMOTEPC__INCLUDED_
