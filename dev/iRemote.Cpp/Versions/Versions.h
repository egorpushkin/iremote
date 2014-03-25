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

#ifndef VERSIONS_H__REMOTEPC__INCLUDED_
#define VERSIONS_H__REMOTEPC__INCLUDED_

// iRemote and RemotePC version constants
//////////////////////////////////////////////////////////////////////////

namespace RemotePC 
{
	typedef enum tagVersions
	{
		iREMOTE_VER_MAJOR = 1,
		iREMOTE_VER_MINOR = 3,
        iREMOTE_VER_FIX = 0,
		iREMOTE_VER_REV = 502,

		iREMOTE_LICENSE = 2,  		
		
		iREMOTE_SFB_VER_MAJOR = 1,
		iREMOTE_SFB_VER_MINOR = 0,

		REMOTEPC_VER_MAJOR = 1,
        REMOTEPC_VER_MINOR = 5,
        REMOTEPC_VER_REV = 500
	} 
    Versions;

    // Status of Remote PC Suite build ("Dev", "Stable", etc.).
    extern const std::string RemotePCBuildStatus;

	// Unique identifier of RemotePC application.
	extern const mc::Guid RemotePCApplicationGuid;

	// Unique identifier of iRemote application.
	extern const mc::Guid iRemoteApplicationGuid;

}

#endif // VERSIONS_H__REMOTEPC__INCLUDED_
