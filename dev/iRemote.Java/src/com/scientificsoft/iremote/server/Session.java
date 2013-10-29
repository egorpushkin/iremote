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

import com.scientificsoft.iremote.server.hosts.Host;
import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.server.protocol.IMessage;

public class Session {
	
	/** Host object, associated with current session. */
	private Host host_ = null;
	
	/** Authentication message, that was sent to server. */
	private IMessage authMessage_ = null;
	
	/** Authentication response, received form server. */
	private IMessage authResponse_ = null;	
	
	/** Key to perform simplified authentication. */
	private Guid cookie_ = null;
	
	/** Flag that prevents cookie to be cleared on disconnection. */
	private boolean cookieLock_ = false;
	
	public Host host() {
		return host_;
	}
	
	public void setAuthMessage(IMessage authMessage) {
		authMessage_ = authMessage;
	}
	
	public IMessage authMessage() {
		return authMessage_;
	}
	
	public void setAuthResponse(IMessage authResponse) {
		authResponse_ = authResponse;
	}	
	
	public IMessage authResponse() {
		return authResponse_;
	}
	
    public void setCookie(Guid cookie) {
        cookie_ = cookie;
        // Cookie should not be locked initially.
        lockCookie(false);
    }
    
    /**
     * Should be called each time, when disconnection occurs.
     */
    public void releaseCookie() {
        // Locking prevents from releasing cookie when going to sleep.
        if ( cookieLock_ ) 
        	return;
        cookie_ = null;  
    }    
    
    public Guid cookie() {
        return cookie_ ;
    }
    
    /**
     * Should be invoked, when disconnection occurs to preserve battery,
     * when going to sleep. 
     */
    public void lockCookie(boolean lock) {
        if ( null == cookie_ )
            return;               
        // Prevents from removing cookie.
        cookieLock_ = lock;
        // If connection with host is alive and cookie is being locked.
        if ( cookieLock_ ) {
            // Notify host on cookie so it may be used for further reconnection.
        	Messenger.sendCookie(cookie_);
        }
    }
	
	protected void flush() {
		host_ = new Host();
		authMessage_ = null;
		authResponse_ = null;
		cookie_ = null;
		cookieLock_ = false;
	}
}
