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
import com.scientificsoft.iremote.server.ServerContext;
import com.scientificsoft.iremote.server.protocol.IMessage;
import com.scientificsoft.iremote.server.protocol.IMessageHandler;

/**
 * This message is only sent from the server, which is older than
 * Remote Suite PC 1.2. Connection should be aborted. 
 */
public class AuthResponseHandler implements IMessageHandler {
    
    private ServerContext server_ = null;
    
    public AuthResponseHandler(ServerContext server) {
        server_ = server;
    }   
    
    public void process(IMessage message) {
        try {
            server_.requestShutdown(DSession.ERROR_OLD_REMOTE_PC);
        } catch ( Exception e ) {           
        }
    }
    
}
