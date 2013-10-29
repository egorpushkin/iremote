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

package com.scientificsoft.iremote.server.hosts;

import com.scientificsoft.iremote.platform.tools.Vector;

import com.scientificsoft.iremote.server.protocol.Guid;

public class Host {
	
    /** Unique host identifier. */
    private Guid hostId_ = null;

    /** User password is stored in md5. */       
    private String password_ = null;
    
    /** List of all host's network interfaces. */   
    private Vector netInterfaces_ = new Vector();
    
    /** Indicates a moment, when last connection was established. */
    private String lastAccessTime_ = null;
    
    public void setHostId(String hostId) {
    	try {
    		hostId_ = new Guid(hostId);
    	} catch ( Exception e ) {    		
    	}
    }
    
    public void setHostId(Guid hostId) {
        hostId_ = hostId;
    }
    
    public boolean hasHostId() {
    	return ( null != hostId_ );
    }
    
    public Guid hostId() {
        return hostId_;
    }
    
    /** Accepts md5 password string. */
    public void setMd5Password(String password) {
        password_ = password;
    }
    
    /** Returns md5 hash of the password. */
    public String password() {
        return password_;
    }
    
    public void resetPassword() {
        password_ = null;
    }
    
    public void addInterface(NetInterface netInterface) {
        netInterfaces_.addElement(netInterface);
    }
    
    public int interfacesCount() {
        return netInterfaces_.size();
    }
    
    public NetInterface interfaceAtIndex(int index) {
        return (NetInterface)netInterfaces_.elementAt(index);
    }
    
    public void removeInterface(int index) {
        netInterfaces_.removeElementAt(index);
    }
    
    public void setLastAccessTime(String time) {
    	lastAccessTime_ = time;
    }

    public String lastAccessTime() {
    	return lastAccessTime_;
    }
}

