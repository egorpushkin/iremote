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

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.scientificsoft.iremote.platform.net.Socket;
import com.scientificsoft.iremote.platform.system.Device;

import com.scientificsoft.iremote.server.config.Config;
import com.scientificsoft.iremote.server.messages.AuthMessage;
import com.scientificsoft.iremote.server.protocol.EndOfStreamException;
import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.server.protocol.IMessage;
import com.scientificsoft.iremote.server.protocol.IMessageHandler;
import com.scientificsoft.iremote.server.protocol.MessageHandlers;
import com.scientificsoft.iremote.server.protocol.MessageHeader;
import com.scientificsoft.iremote.server.protocol.MessagesFactory;
import com.scientificsoft.iremote.server.protocol.Parser;
import com.scientificsoft.iremote.server.protocol.ProtocolException;
import com.scientificsoft.iremote.server.tools.TimeoutException;

/**
 * The ServerContext class is intended to establish connection 
 * with Remote PC Suite server application and receive data.
 *
 * The ServerContext notifies application on all kind of related 
 * events through DSession and DUIHost interfaces.
 * 
 * Application UI is notified on disconnection from single place,
 * within server context. This occurs when read operation fails.
 */
public class ServerContext extends Thread {
    
    private String host_;
    
    private String port_;
    
    private Guid cookie_;
    
    private DHolder holder_ = null;
    
    private DSession uiSession_ = null;
    
    private Socket socket_ = null;
    
    private InputStream istream_ = null;
    
    private OutputStream ostream_ = null;   
    
    private MessagesFactory factory_ = new MessagesFactory();
    
    private MessageHandlers handlers_ = new MessageHandlers();
    
    private long threadId_ = 0;
    
    private boolean shutdownRequested_ = false;
    
    private boolean cleanupPerformed_ = false;
    
    private boolean holderNotified_ = false;
    
    private boolean uiNotified_ = false;
    
    private int shutdownReason_ = DSession.ERROR_OK;
    
    public ServerContext(String host, String port, Guid cookie, DHolder holder, DSession uiSession) {
        host_ = host;
        port_ = port;
        cookie_ = cookie;
        holder_ = holder;
        uiSession_ = uiSession;
    }
    
    public MessagesFactory factory() {
        return factory_;
    }
    
    public MessageHandlers handlers() {
        return handlers_;
    }
    
    public DSession uiSession() {
        return uiSession_;
    }
    
    public void requestShutdown(int reason) {
        shutdownReason_ = reason;
        shutdownRequested_ = true;
        if ( Thread.currentThread().hashCode() != threadId_ ) {        	
            // It is required to close socket, when terminating
            // the service from other thread to break data exchange loop.
            try {
                if ( null != socket_ ) {
                    socket_.close();
                    // socket_ = null; 
                }
            } catch ( Exception e ) {                             
            }      	
        }
    }
    
    public void send(IMessage message) {
            if ( null == ostream_ )
                return;
            try {
                message.write(ostream_);
            } catch ( IOException e ) {
            } 
    }
    
    public void run() {
        // Remember self id.
        threadId_ = Thread.currentThread().hashCode();
        try {
            // Establish connection.
            if ( establishConnection() ) {
                // Start data exchange.
                readData();
            }
            // Stop interaction with server.
            terminateConnection(shutdownReason_);
        } catch ( Exception e ) {
            // This should not happen. But..  
            terminateConnection(DSession.ERROR_UNKNOWN);
        } catch ( Error e ) {
            // This should not happen. But..  
            terminateConnection(DSession.ERROR_UNKNOWN);
        }               
    }
    
    private boolean establishConnection() {         
        try {
            // Create streaming socket.
            socket_ = new Socket(host_, port_, Config.connectionTimeout_);                
            istream_ = socket_.getInputStream();            
            ostream_ = socket_.getOutputStream();
        } catch ( TimeoutException e ) {
	        // Notify application on failure.
	        shutdownReason_ = DSession.ERROR_TIMEOUT;
	        return false;                   
        } catch ( Exception e ) {
            // Notify application on failure.
            shutdownReason_ = DSession.ERROR_FAILED_TO_CONNECT;
            return false;                   
        }
        // Initiate authentication. 
        sendAuthRequest();
        // Notify Holder on success.
        holder_.connected();
        // Notify application on success.
        uiSession_.connected();
        return true;
    }
    
    private boolean readData() {
        // Start receiving data.
        try {
            while ( !shutdownRequested_ ) {
                // Read message id from input stream.
                MessageHeader messageHeader = Parser.readHeader(istream_);
                // Try to construct message by id. 
                IMessage message = factory_.messageByCode(messageHeader.code_); 
                if ( null == message ) {
                    // Read all message data from stream and ensure
                    // that message terminator is correct (session must be
                    // broken otherwise).
                    Parser.skipMessage(istream_, messageHeader);
                    continue;
                }
                // Specify message size to support buffered input.
                message.setSize(messageHeader.size_);
                // Message was constructed successfully. It must be extracted 
                // from input stream now.                               
                Parser.readMessage(istream_, message);
                // Message was successfully extracted, if we are still here.
                // Message must be handled now.
                IMessageHandler handler = handlers_.handlerByCode(messageHeader.code_);
                if ( null != handler )
                    handler.process(message);
                // It is not critical problem, if handler is not found. This
                // situation leads to just skipping message for now.
            }
        } catch ( ProtocolException e ) {
            shutdownReason_ = DSession.ERROR_PROTOCOL;
            return false;
        } catch ( EndOfStreamException e ) {
            if ( !shutdownRequested_ )
                shutdownReason_ = DSession.ERROR_TERMINATED_BY_HOST;
            return false;
        } catch ( IOException e ) {
            if ( !shutdownRequested_ )
                shutdownReason_ = DSession.ERROR_TERMINATED_BY_HOST;
            return false;
        }
        return true;
    }
    
    /**
     * Exception safety:
     * - This method must not throw exceptions.
     */
    private void terminateConnection(int code) {            
        try {   
            if ( null != socket_ ) {
                socket_.close();
            }
        } catch ( Exception e ) {
            code = DSession.ERROR_FAILED_TO_CLOSE;
        }
        // Perform cleanup.
        cleanup();
        // Notify holder on disconnection.
        if ( !holderNotified_ ) {                       
            holder_.disconnected();
            holderNotified_ = true;
        }
        // Notify application UI on disconnection.
        if ( !uiNotified_ ) {
            try {
                uiSession_.disconnected(code);
            } catch ( Exception e ) {
            }
            uiNotified_ = true;
        }
    }       
            
    private void cleanup() {
        if ( cleanupPerformed_ )
            return;
        istream_ = null;
        ostream_ = null;
        socket_ = null;
        cleanupPerformed_ = true;
    }
    
    protected void sendAuthRequest() {
        AuthMessage authMessage = new AuthMessage(Device.deviceId(), Device.deviceName(), cookie_);
        send(authMessage);        
        holder_.session().setAuthMessage(authMessage);        
    }    
}
