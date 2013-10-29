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

import com.scientificsoft.iremote.server.hosts.NetInterface;
import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.server.protocol.IMessage;

public class Holder extends DHolder {
        
    public static final int STATUS_NOT_CONNECTED = 0x1;     
    public static final int STATUS_CONNECTED = 0x2;
    public static final int STATUS_DISCONNECTED = 0x3;
    
    /** Holds single reference to class instance. */
    private static Holder instance_ = null;
    
    /** Utility, which is responsible for translating events to main thread. */
    private IExecutor executor_ = null;     
    
    /** Reference to control interface dedicated for session events. */
    private SessionStub uiSession_ = null;
    
    /** Reference to control interface dedicated for UI events. */
    private HostStub uiHost_ = null;
    
    /** Reference to control interface dedicated for local events. */
    private LocalStub local_ = null;
    
    /** An instance of server interaction tool . */
    private ServerContext server_ = null; 
    
    /** Encapsulates properties related to current connection session. */
    private Session session_ = null;
    
    /** Encapsulates server run time properties. */
    private Runtime runtime_ = null;
    
    /** Controls concurrent access to server instance. */
    private Object serverLock_ = null;
    
    /** Holds service status. */
    private int status_ = STATUS_NOT_CONNECTED;
    
    /** Holds last disconnection reason. */
    private int reason_ = DSession.ERROR_UNKNOWN;
    
    public static synchronized Holder instance() {
        if ( null == instance_ ) {
            instance_ = new Holder();
        }
        return instance_;
    }
    
    private Holder() {
    	runtime_ = new Runtime(); 
    	session_ = new Session();
    	serverLock_ = new Object();
    }
    
    /**
     * This method must be called, when application is launched.
     * Thread Safety:
     * - May be invoked from UI thread(s) only. 
     */
    public void configureExecutor(IExecutor executor) {
        executor_ = executor;
        uiSession_ = new SessionStub(executor_);                
        uiHost_ = new HostStub(executor_);
        local_ = new LocalStub(executor_);
        local_.configure(new Local());
    }
    
    /**
     * This method must be called, when session host is changed.
     * Thread Safety:
     * - May be invoked from UI thread(s) only. 
     */
    public void configureSession(DSession session) {
        uiSession_.configure(session);
    }
    
    /**
     * This method is called when UI host is changed.
     * Thread Safety:
     * - May be invoked from UI thread(s) only. 
     */
    public void configureHost(DUIHost host) {
            uiHost_.configure(host);
    }       
    
    /**
     * Initiates connection to server. 
     * Thread Safety:
     * - May be invoked from UI thread(s) only.
     */
    public void connect(String host, String port) {
        synchronized ( serverLock_ ) {
            if ( null != server_ )
                return;     
    		// Remember cookie from previous session, if any.
            Guid cookie = session_.cookie();
            // Start new session.
            session_.flush();
    		// Add interface with current connection settings to the host, which is
    		// stored in current session. This host will be saved upon successful
    		// connection attempt.
    		session_.host().addInterface( new NetInterface(host, port) );    
    		
            // Start thread, responsible for network interaction with server.
            server_ = new ServerContext(host, port, cookie, this, uiSession_);
            Messenger.configureContext(server_, uiSession_, uiHost_, local_, this);
            server_.start();            
            
            // Synchronously notify UI.
            uiSession_.connecting();
        }
    } 
    
    /**
     * Terminates connection with server.
     * Thread Safety:
     * - May be invoked from UI thread(s) only. 
     */
    public void disconnect() {
    	disconnect(DSession.ERROR_TERMINATED_BY_USER);
    }
    
    /**
     * Terminates connection with server.
     * Thread Safety:
     * - May be invoked from UI thread(s) only. 
     */
    public void disconnect(int reason) {
        synchronized ( serverLock_ ) {
            if ( null == server_ )
                return;         
            server_.requestShutdown(reason);
        }
    }    
    
    /**
     * Intended to deliver message to server.
     * Thread Safety:
     * - May be invoked from any application thread. 
     */     
    public void send(IMessage message) {
        synchronized ( serverLock_ ) {
            try {
                if ( null == server_ )
                    return;
                server_.send(message);
            } catch ( Exception e ) {                       
            }
        }
    }
    
    public void setStatus(int status, int reason) {
        status_ = status;
        reason_ = reason;
    }
    
    public int status() {
        return status_;
    }
    
    public int reason() {
        return reason_;
    }
    
    public Runtime runtime() {
    	return runtime_;
    }
    
    /** DHolder section. */
    
    /** 
     * Provides access to a tool performing actions on main thread. 
     */
    public IExecutor executor() {
    	return executor_;
    }       
    
    /**
     * Notifies Holder on connection.
     * Thread Safety:
     * - May be invoked from any thread belonging to Server component.  
     */
    protected void connected() {
    }
    
    /**
     * Notifies Holder on disconnection.
     * Thread Safety:
     * - May be invoked from any thread belonging to Server component.  
     */
    protected void disconnected() {
        synchronized ( serverLock_ ) {
            try {
                session_.releaseCookie();            	
                server_ = null;
            } catch ( Exception e ) {                       
            }
        }
    }    
    
    public Session session() {
    	return session_;
    }
    
}
