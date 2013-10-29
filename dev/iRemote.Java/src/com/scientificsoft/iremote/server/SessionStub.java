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

/**
 * Invokes all DSession callbacks on main thread.
 * TODO: Investigate, why synchronization is required inside handlers.
 * Probably it is caused by a chance for session to be configured from any thread.
 * But in practive configure method is only invoked from UI thread,
 * so this synchronization should be reduced.
 */
public class SessionStub implements DSession {
    
    /** Utility, which is responsible for translating events to main thread. */
    private IExecutor executor_ = null;     

    /** Reference to control interface dedicated for session events. */
    private DSession uiSession_ = null;
    
    public SessionStub(IExecutor executor) {
        executor_ = executor;
    }
    
    public void configure(DSession session) {
        synchronized ( this ) {
            uiSession_ = session;
        }
    }
    
    public void connecting() {
    	if ( null != SessionStub.this.uiSession_ ) 
            SessionStub.this.uiSession_.connecting();
    }
    
    public void connected() {
        executor_.run(
            new Runnable() {
                public void run() {
                    synchronized ( SessionStub.this ) {
                        Holder.instance().setStatus(Holder.STATUS_CONNECTED, DSession.ERROR_OK);
                        if ( null != SessionStub.this.uiSession_ ) 
                            SessionStub.this.uiSession_.connected();
                    }
                }
            }               
        ); 
    }
    
    public void passwordRequested(boolean wrong) {
        final boolean _wrong = wrong;
        executor_.run( new Runnable() {
            private boolean wrong_ = _wrong;
            public void run() {
                synchronized ( SessionStub.this ) {
                    if ( null != SessionStub.this.uiSession_ ) 
                        SessionStub.this.uiSession_.passwordRequested(wrong_);
                }
            }
        } );
    }
    
    public void approved() {
        executor_.run(
            new Runnable() {
                public void run() {
                    synchronized ( SessionStub.this ) {
                        if ( null != SessionStub.this.uiSession_ ) 
                            SessionStub.this.uiSession_.approved();
                    }
                }
            }               
        ); 
    }
    
    public void disconnected(int reason) {
        final int _reason = reason;
        executor_.run( new Runnable() {
            private int reason_ = _reason;
            public void run() {
                // synchronized ( SessionStub.this ) {
                    Holder.instance().setStatus(Holder.STATUS_DISCONNECTED, reason_);
                    if ( null != SessionStub.this.uiSession_ ) 
                        SessionStub.this.uiSession_.disconnected(reason_);
                // }
            }
        } );
    }
    
}

