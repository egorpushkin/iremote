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

#include "Features.h"

namespace RemotePC
{

	/**
	 * This feature was added in Remote PC Suite 1.2.
	 */
	bool Features::RemotePCPasswordProtection(int major, int minor, int /* revision */)
	{
		return Satisfies(major, minor, 1, 2);
	}
	
	/**
	 * This feature was added in Remote PC Suite 1.3.
	 */	
	bool Features::RemotePCZooming(int major, int minor, int /* revision */)
	{
		return Satisfies(major, minor, 1, 3);
	}
    
	/**
	 * This feature was added in Remote PC Suite 1.3.
	 */	    
    bool Features::RemotePCShell(int major, int minor, int /* revision */) 
    {
        return Satisfies(major, minor, 1, 3);
    }

	/**
	 * This feature was added in Remote PC Suite 1.5.
	 */	    	
	bool Features::RemotePCServices(int major, int minor, int /* revision *//* = 0 */)
	{
        return Satisfies(major, minor, 1, 5);
	}

	/**
	 * This feature was added in iRemote Suite 1.1.
	 */
	bool Features::iRemotePasswordProtection(int major, int minor, int /* revision */)
	{
		return Satisfies(major, minor, 1, 1);
	}

	/**
	 * This feature was added in iRemote Suite 1.1.
	 */
	bool Features::iRemoteExtendedAuthentication(int major, int minor, int /* revision */)
	{
		return Satisfies(major, minor, 1, 1);
	}
	
	/**
	 * This feature was added in iRemote Suite 1.2.
	 */	
	bool Features::iRemoteZooming(int major, int minor, int /* revision */)
	{
		return Satisfies(major, minor, 1, 2);
	}

	/**
	 * This feature was added in iRemote Suite 1.2.
	 */	    
	bool Features::iRemoteMac(int major, int minor, int /* revision */)
    {
		return Satisfies(major, minor, 1, 2);        
    }

	/**
	 * This feature was added in iRemote Suite 1.3.
	 */	    
	bool Features::iRemoteServices(int major, int minor, int /* revision *//* = 0 */)
	{
		return Satisfies(major, minor, 1, 3);    
	}
	
	/**
	 * Return true, when ( major.minor >= majorReq.minorReq ) is true.
	 */
	bool Features::Satisfies(int major, int minor, int majorReq, int minorReq)
	{
		return ( 
			( major > majorReq ) ||
			( ( major == majorReq ) && ( minor >= minorReq ) )
		);				
	}

}
