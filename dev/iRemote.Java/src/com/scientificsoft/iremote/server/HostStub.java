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

import com.scientificsoft.iremote.server.messages.ScreenshotMessage;
import com.scientificsoft.iremote.server.messages.ShellResponse;
import com.scientificsoft.iremote.server.messages.VolumeMessage;

public class HostStub implements DUIHost {

    /** Utility, which is responsible for translating events to main thread. */
    private IExecutor executor_ = null; 
    
    /** Reference to control interface dedicated for UI events. */
    private DUIHost uiHost_ = null;
    
    public HostStub(IExecutor executor) {
        executor_ = executor;
    }
    
    public void configure(DUIHost host) {
        synchronized ( this ) {
            uiHost_ = host;
        }
    }

    /**
     * Screenshot events are propagated as is to allow background processing and
     * to minimize amount of context switches in the flow of each image.
     */
    public void screenshot(ScreenshotMessage message) {
    	synchronized ( HostStub.this ) {
            if ( null != HostStub.this.uiHost_ )
                HostStub.this.uiHost_.screenshot(message);
        }
    }   
    
    public void shellRespose(ShellResponse message) {
        final ShellResponse _message = message;
        executor_.run(
            new Runnable() {
                private ShellResponse message_ = _message;
                public void run() {
                    synchronized ( HostStub.this ) {
                        if ( null != HostStub.this.uiHost_ )
                            HostStub.this.uiHost_.shellRespose(message_);
                    }
                }
            }       
        );          
    }

    public void volumeLevel(VolumeMessage message) {
        final VolumeMessage _message = message;
        executor_.run(
            new Runnable() {
                private VolumeMessage message_ = _message;
                public void run() {
                    synchronized ( HostStub.this ) {
                        if ( null != HostStub.this.uiHost_ )
                            HostStub.this.uiHost_.volumeLevel(message_);
                    }
                }
            }       
        );          
    }
}
