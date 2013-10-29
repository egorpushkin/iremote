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

package com.scientificsoft.iremote.server;

import com.scientificsoft.iremote.server.config.Config;
import com.scientificsoft.iremote.server.messages.AuthResponse2;
import com.scientificsoft.iremote.server.protocol.IMessage;

public class Features {
	
	/**
	 * @return Returns true, if appropriate version of EULA has already been accepted.
	 */
	public static boolean eulaAccepted() {
		return ( Versions.iREMOTE_LICENSE <= Config.acceptedEulaVersion_ );
	}

	/**
	 * Remembers that user accepted EULA, which was supplied along with this build.
	 */
	public static void acceptEula() {
		Config.acceptedEulaVersion_ = Versions.iREMOTE_LICENSE;
		Config.save();
	}
	
	/**
	 * This feature was added in Remote PC Suite 1.4.
	 */		
	public static boolean remotePCJavaClients(int major, int minor) {
		return satisfies(major, minor, 1, 4);
	}
	
	/**
	 * Return true, when ( major.minor >= majorReq.minorReq ) is true.
	 */
	public static boolean satisfies(int major, int minor, int majorReq, int minorReq)
	{
		return ( 
			( major > majorReq ) ||
			( ( major == majorReq ) && ( minor >= minorReq ) )
		);				
	}	
	
	/**
	 * Determines whether application is connected to host running MacOS.
	 */
	public static boolean isHostMac() {
		return testHostOS(AuthResponse2.OS_MAC);
	}
	
	/**
	 * Determine whether application is connected to the host running
	 * specified OS. 
	 * @return Returns false, if not connected or any other error occurs.
	 */
	public static boolean testHostOS(int os) {
		try {
			IMessage authResponse = Holder.instance().session().authResponse();
			int hostOs = authResponse.getProperty(AuthResponse2.PROP_HOSTOS).toInt();
			return ( os == hostOs );
		} catch ( Exception e ) {
			return false;
		}
	}

}
