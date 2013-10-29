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

import com.scientificsoft.iremote.platform.system.DateTimeUtils;
import com.scientificsoft.iremote.platform.tools.Vector;
import com.scientificsoft.iremote.server.config.Config;
import com.scientificsoft.iremote.server.protocol.Guid;

/**
 * HostsManager is not thread safe. The reason is that platform code 
 * does not require accessing it from multiple threads simultaneously.
 * It is utilized by client code immediately before establishing connection
 * and by platform immediately after connection is established.
 * Client code should not refer to HostsManager between DSession.connecting
 * and DSession.approved events.
 */
public class HostsManager {
    
    /** Holds single reference to class instance. */
    private static HostsManager instance_ = null;    
    
    /** List of known hosts. */
    private Vector hosts_ = new Vector();
    
    /** Last accessed host. */
    private NetInterface lastAccessed_ = new NetInterface("192.168.1.1", "8444");
    
    public static synchronized HostsManager instance() {
        if ( null == instance_ ) {
            instance_ = new HostsManager();
        }
        return instance_;
    }
    
    private HostsManager() {
    }    

    public NetInterface lastAccessed() {
    	return lastAccessed_;
    }
    
    public void setLastAccessed(String ip, String port) {
    	lastAccessed_.ip_ = ip;
    	lastAccessed_.port_ = port;
    	Config.save();
    }
    
    public int hostsCount() {
    	return hosts_.size();
    }
    
    public Host host(int index) {
    	if ( index < 0 || index >= hosts_.size() )
    		return null;
    	return (Host)hosts_.elementAt(index);    	
    }
    
    public Host host(Guid hostId) {
    	int amount = hosts_.size();
    	for ( int i = 0 ; i < amount ; ++i ) {
    		Host host = (Host)hosts_.elementAt(i);
    		if ( host.hostId().equals(hostId) )
    			return host;
    	}
    	return null;
    }
    
    public void addHost(Host host) {
    	try {
    		hosts_.add(host);
    	} catch ( Exception e ) {
    	}
    }
    
    public void removeInterface(int hostIndex, int interfaceIndex) {
    	try {
	    	// Check whether specified index exists in the list.
			if ( hosts_.size() <= hostIndex ) {
				return;
			}
			// Acquire host, which must be removed.
			Host host = (Host)hosts_.elementAt(hostIndex);		
			if ( host.interfacesCount() <= 1 ) {
				// Remove host with specified index.
				hosts_.removeElementAt(hostIndex);
			} else {
				// Remove host interface with specified index.
				host.removeInterface(interfaceIndex);
			}
	    	Config.save();
    	} catch ( Exception e ) {
    	}		
    }
    
	public void saveHost(Host srcHost) {
		// This locks users database internally for writing.
		try {
			// Check whether host record is ready to be saved. It should meet the 
			// Following conditions:
			// - host id must be specified;
			// - at least one interface must be added.
			if ( !srcHost.hasHostId() || 0 == srcHost.interfacesCount() ) {
				return;
			}		
            // Update last access time.
			srcHost.setLastAccessTime(DateTimeUtils.formatCurrentDate());
			// Remember first network interface of the host to be saved.
			NetInterface srcInterface = srcHost.interfaceAtIndex(0);
			// Find host by id.
			Host dstHost = host(srcHost.hostId());
			if ( null == dstHost ) {
	            // Add new host.
				addHost(srcHost);
	            Config.save();
				return;				
			}				
			// Find network interface in found host with the same ip and port.
			boolean interfaceFound = false;
			int dstInterfacesCount = dstHost.interfacesCount();
			for ( int i = 0 ; i < dstInterfacesCount ; ++i ) {
				NetInterface dstNetInterface = dstHost.interfaceAtIndex(i);
				if ( ( dstNetInterface.ip_.equalsIgnoreCase(srcInterface.ip_) ) && 
					( dstNetInterface.port_.equalsIgnoreCase(srcInterface.port_) ) ) {
					interfaceFound = true;
					break;
				}				
			}
			// Check whether interface with current ip and port is found.
			if ( !interfaceFound ) {
				// Add one more interface to found host.
				NetInterface netInterface = new NetInterface(srcInterface.ip_, srcInterface.port_);
				dstHost.addInterface(netInterface);
			}
				
			// Update records of found host.
            // Host object, which is added upon successful authentication (srcHost),
            // contains password, if
            // - it was entered by user (Holder::SubmitPassword);
            // - if was extracted from the db (ServerContext::HandlePasswordRequest);
            // and does not contain password if
            // - device was authorized anonymously (user allowed connection once).
            // Thus, password should be saved, if it exists, and should be 
            // cleared, otherwise.
			String password = srcHost.password();
            if ( null != password )
				dstHost.setMd5Password(password);			
            else
                dstHost.resetPassword();            
            // Update last access time.
            dstHost.setLastAccessTime(srcHost.lastAccessTime());
            Config.save();
		} catch ( Exception e ) {
		}		
	}
}
