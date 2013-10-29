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

package com.scientificsoft.iremote.server.handlers;

import com.scientificsoft.iremote.server.DHolder;
import com.scientificsoft.iremote.server.DSession;
import com.scientificsoft.iremote.server.Features;
import com.scientificsoft.iremote.server.ServerContext;
import com.scientificsoft.iremote.server.hosts.Host;
import com.scientificsoft.iremote.server.hosts.HostsManager;
import com.scientificsoft.iremote.server.messages.AuthMessage;
import com.scientificsoft.iremote.server.messages.AuthResponse2;
import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.server.protocol.IMessage;
import com.scientificsoft.iremote.server.protocol.IMessageHandler;

/**
 * This message is only sent from Remote Suite PC 1.2 and newer. 
 * Connection should be handled. 
 */
public class AuthResponse2Handler implements IMessageHandler {
    
    private ServerContext server_ = null;
    
    private DHolder holder_ = null;
    
    public AuthResponse2Handler(ServerContext server, DHolder holder) {
        server_ = server;
        holder_ = holder;
    }   
    
    public void process(IMessage message) {   	
        try {
        	// Check whether server is enough new to support Java client.
        	int major = message.getProperty(AuthResponse2.PROP_VER_MAJOR).toInt();
        	int minor = message.getProperty(AuthResponse2.PROP_VER_MINOR).toInt();
        	if ( !Features.remotePCJavaClients(major, minor) ) {
        		server_.requestShutdown(DSession.ERROR_OLD_REMOTE_PC);
        		return;
        	}
        	
        	// Handle cookie. 
        	handleCookie(message);        	
        	// Save current session (host settings).
        	handleHost(message);        	
        	// Remember authentication response message for future use.
        	holder_.session().setAuthResponse(message);
        	
            // Notify UI on authentication approval. 
            server_.uiSession().approved();
        } catch ( Exception e ) {
        }
    }
    
    /**
     * Cookie should be saved to be able to restore session after disconnection
     * without handshake authentication.
     */
    private void handleCookie(IMessage message) {
    	try {
    		boolean hasCookie = message.getProperty(AuthResponse2.PROP_HAS_CODE).toBool();
	    	if ( hasCookie ) {
	    		// Acquire cookie.
	    		Guid cookie = message.getProperty(AuthResponse2.PROP_CODE).toGuid();		
	    		// Save cookie.
	    		holder_.session().setCookie(cookie);   		    		
	    	}
    	} catch ( Exception e ) {
        }
    }
    
    private void handleHost(IMessage message) {
    	try {
            // Host record should be modified only, if authentication has not been
            // performed with the help of cookie. That is why, the check is made over
    		// authMessage_ property and not over authResponse_. authMessage_ property
    		// contains initial message, that was sent from client to server. Cookie
    		// presence there means that this was not initial authentication, but
    		// session was restored. In this case it is not necessary to save/update 
    	    // host information as it has not been changed.
        	IMessage authMessage = holder_.session().authMessage();
        	boolean hasCookie = authMessage.getProperty(AuthMessage.PROP_HAS_CODE).toBool();
        	if ( !hasCookie ) {
                // Acquire host id from authentication response message and remember it (host id).
                Guid hostId = message.getProperty(AuthResponse2.PROP_HOST_ID).toGuid();
                Host host = holder_.session().host();
                host.setHostId(hostId);
                // Save or update information about current host.
                HostsManager.instance().saveHost( host );
        	}
    	} catch ( Exception e ) {    		
    	}
    }
}
 