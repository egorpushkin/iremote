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

package com.scientificsoft.iremote.platform.net;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.SocketTimeoutException;

import com.scientificsoft.iremote.server.tools.TimeoutException;

public class Socket {        

    private java.net.Socket _socket = null;         
    
    public Socket(String host, String port, long timeout) throws IOException, TimeoutException {    
        try {
            _socket = new java.net.Socket();
            _socket.connect(new InetSocketAddress(host, new Integer(port)), (int)timeout);
        } catch ( SocketTimeoutException e ) {
        	throw new TimeoutException();
        } catch ( Exception e ) {
            throw new IOException("Failed to establish connection.");
        }       
    }
    
    public InputStream getInputStream() throws IOException {
        return _socket.getInputStream();
    }
        
    public OutputStream getOutputStream() throws IOException {
        return _socket.getOutputStream();
    }
    
    public void close() throws IOException {
    	try {
    		_socket.shutdownInput();
    	} catch ( Exception e ) {
      	}
    	try {
    		_socket.shutdownOutput();
    	} catch ( Exception e ) {
      	}
    	_socket.close();
    }    
    
} 

