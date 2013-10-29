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

import com.scientificsoft.iremote.server.DSession;
import com.scientificsoft.iremote.server.Messenger;
import com.scientificsoft.iremote.server.config.Config;
import com.scientificsoft.iremote.server.hosts.Host;
import com.scientificsoft.iremote.server.hosts.HostsManager;
import com.scientificsoft.iremote.server.messages.PasswordRequest;
import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.server.protocol.IMessage;
import com.scientificsoft.iremote.server.protocol.IMessageHandler;

public class PasswordRequestHandler implements IMessageHandler {

    private DSession session_ = null;
    
    public PasswordRequestHandler(DSession session) {
        session_ = session;
    }   
    
    public void process(IMessage message) {
        try {
        	handlePasswordRequest(message);            
        } catch ( Exception e ) {      
        }
    }
    
	private void handlePasswordRequest(IMessage message) {
		boolean forcedAuth = true;
		Guid hostId = null;
		try {
			forcedAuth = message.getProperty( PasswordRequest.PROP_REQUEST_AUTH ).toBool();
	        hostId = message.getProperty( PasswordRequest.PROP_HOST_ID ).toGuid();
		} catch ( Exception e ) {			
		}
		if ( forcedAuth ) {      
            // Password may be requested here, if
            // - it was changed on the host side;
            // - it is the first authentication attempt.
            // If the host is saved in local db, password record should be cleared.
			resetPassword(hostId);            
            // Present password popup.
            session_.passwordRequested(false);
		} else {			
			// Check whether host record contains password.
			String password = passwordForHost(hostId);
			if ( null != password ) {
				// Construct password message with password extracted from user record.
				// Host record stores and returns password in MD5. Password message does not modify 
				// input string. Thus, it may be constructed with value extracted from host record.
				Messenger.sendMd5Password(password);
			} else {
				// Host was not found. Proceed to regular authentication (ask user for the password).
				session_.passwordRequested(false);
			}
		}		
	}        
	
	private void resetPassword(Guid hostId) {
		try {
			// Find host by id.
			Host host = HostsManager.instance().host(hostId);
            if ( null != host ) {
	            // Reset password, if host record is found.
	            host.resetPassword();
	            Config.save();
            }
        } catch ( Exception e ) {
        }		
	}
	
	private String passwordForHost(Guid hostId) {
		String password = null;
		try {
			// Find host by id.
			Host host = HostsManager.instance().host(hostId);
            if ( null != host ) {
	            password = host.password();
            }
        } catch ( Exception e ) {
        }		
        return password;
	}
	
}
