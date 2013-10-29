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

import com.scientificsoft.iremote.platform.encryption.MD5;

import com.scientificsoft.iremote.server.handlers.AuthResponse2Handler;
import com.scientificsoft.iremote.server.handlers.AuthResponseHandler;
import com.scientificsoft.iremote.server.handlers.ErrorHandler;
import com.scientificsoft.iremote.server.handlers.PasswordRequestHandler;
import com.scientificsoft.iremote.server.handlers.ScreenshotHandler;
import com.scientificsoft.iremote.server.handlers.ShellResponseHandler;
import com.scientificsoft.iremote.server.handlers.VolumeMessageHandler;
import com.scientificsoft.iremote.server.handlers.WrongPasswordHandler;
import com.scientificsoft.iremote.server.handlers.ZoomParamsHandler;
import com.scientificsoft.iremote.server.messages.AuthResponse;
import com.scientificsoft.iremote.server.messages.AuthResponse2;
import com.scientificsoft.iremote.server.messages.CookieMessage;
import com.scientificsoft.iremote.server.messages.KeyboardMessage;
import com.scientificsoft.iremote.server.messages.LimitOfDevicesReached;
import com.scientificsoft.iremote.server.messages.MoveMessage;
import com.scientificsoft.iremote.server.messages.PasswordMessage;
import com.scientificsoft.iremote.server.messages.PasswordRequest;
import com.scientificsoft.iremote.server.messages.ScreenshotMessage;
import com.scientificsoft.iremote.server.messages.ScrollMessage;
import com.scientificsoft.iremote.server.messages.ShellRequest;
import com.scientificsoft.iremote.server.messages.ShellResponse;
import com.scientificsoft.iremote.server.messages.VolumeMessage;
import com.scientificsoft.iremote.server.messages.WrongDeviceIp;
import com.scientificsoft.iremote.server.messages.WrongPassword;
import com.scientificsoft.iremote.server.messages.ZoomLevelMessage;
import com.scientificsoft.iremote.server.messages.ZoomParamsMessage;
import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.server.protocol.MessageImpl;

public class Messenger {
	
	/**
	 * Instantiates and sends basic message with the specified code. 
	 */
	public static void sendMessage(Guid id) {
		Holder.instance().send(new MessageImpl(id));
	}
    
    /**
     * Accepts raw password string and delivers it to server in md5.
     */
    public static void sendPassword(String password) {
        // Calculate message hash of the password. 
        password = MD5.calculate(password);       
        // Submit password to server.
        sendMd5Password(password);
    }
    
    /**
     * Accepts md5 password string and delivers it to server.
     */
    public static void sendMd5Password(String password) {   
        // Submit password to server.
        Holder.instance().send(new PasswordMessage(password));
        // Save password for the current session.
		Holder.instance().session().host().setMd5Password(password);        
    }    
    
    /**
     * Send shell request. 
     */
    public static void sendShellRequest(String request) {
        Holder.instance().send(new ShellRequest(request));
    }
    
    /**
     * Zoom screen in. 
     */
    public static void zoomIn() {
    	float zoomLevel = Holder.instance().runtime().zoomIn();
    	Holder.instance().send(new ZoomLevelMessage(zoomLevel));
    }
    
    /**
     * Zoom screen out. 
     */
    public static void zoomOut() {
    	float zoomLevel = Holder.instance().runtime().zoomOut();
    	Holder.instance().send(new ZoomLevelMessage(zoomLevel));
    }
    
    /**
     * Apply scale factor. 
     */
    public static void zoom(float factor) {
    	float zoomLevel = Holder.instance().runtime().zoom(factor);
    	Holder.instance().send(new ZoomLevelMessage(zoomLevel));
    }
    
    /**
     * Scroll. 
     */
    public static void scroll(boolean vertical, int value) {
    	Holder.instance().send(new ScrollMessage(vertical, value));
    }        
    
    /**
     * Flush zoom level to initial setting. 
     */
    public static void flushZoom() {
    	float zoomLevel = Holder.instance().runtime().flushZoom();
    	Holder.instance().send(new ZoomLevelMessage(zoomLevel));
    }
    
    /**
     * Send cookie to server. 
     */
    public static void sendCookie(Guid cookie) {
    	Holder.instance().send(new CookieMessage(cookie));
    }        
    
    /**
     * Move mouse cursor on dx/dy.
     */
    public static void moveCursor(float dx, float dy) {
    	Holder.instance().send(new MoveMessage(dx, dy));  
    }
    
    /**
     * Send unicode character.
     */
    public static void sendUnicodeKey(short key) {
    	Holder.instance().send(new KeyboardMessage(KeyboardMessage.TYPE_UNICODE, key));
    }
    
    /**
     * Send virtual key code.
     */
    public static void sendVirtualKey(byte key, boolean keydown) {
    	Holder.instance().send(new KeyboardMessage(KeyboardMessage.TYPE_VIRTUAL, key, keydown));
    }

    /**
     * Send new volume level.
     */
    public static void sendVolume(int value) {
    	Holder.instance().send(new VolumeMessage(VolumeMessage.DEVICE_MASTER, value));
    }    
    
    protected static void configureContext(ServerContext context, DSession uiSession, DUIHost uiHost, DLocal local, DHolder holder) {
        // Auth response v1.
        context.factory().registerMessage(AuthResponse.id_, new AuthResponse());
        context.handlers().registerHandler(AuthResponse.id_, new AuthResponseHandler(context));
    	// Auth response v2.
        context.factory().registerMessage(AuthResponse2.id_, new AuthResponse2());
        context.handlers().registerHandler(AuthResponse2.id_, new AuthResponse2Handler(context, holder));
        // Screenshot message.
        context.factory().registerMessage(ScreenshotMessage.id_, new ScreenshotMessage());
        context.handlers().registerHandler(ScreenshotMessage.id_, new ScreenshotHandler(uiHost));
        // Password request.
        context.factory().registerMessage(PasswordRequest.id_, new PasswordRequest());
        context.handlers().registerHandler(PasswordRequest.id_, new PasswordRequestHandler(uiSession));
        // Password request.
        context.factory().registerMessage(WrongPassword.id_, new WrongPassword());
        context.handlers().registerHandler(WrongPassword.id_, new WrongPasswordHandler(uiSession));
        // Wrong device IP.
        context.factory().registerMessage(WrongDeviceIp.id_, new WrongDeviceIp());
        context.handlers().registerHandler(WrongDeviceIp.id_, new ErrorHandler(DSession.ERROR_WRONG_DEVICE_IP));
        // Limit of connections is reached.
        context.factory().registerMessage(LimitOfDevicesReached.id_, new LimitOfDevicesReached());
        context.handlers().registerHandler(LimitOfDevicesReached.id_, new ErrorHandler(DSession.ERROR_LIMIT_REACHED));
        // Old Remote PC.
        context.factory().registerMessage(AuthResponse.id_, new AuthResponse());
        context.handlers().registerHandler(AuthResponse.id_, new ErrorHandler(DSession.ERROR_OLD_REMOTE_PC));
        // Shell response.
        context.factory().registerMessage(ShellResponse.id_, new ShellResponse());
        context.handlers().registerHandler(ShellResponse.id_, new ShellResponseHandler(uiHost));
        // Volume message.
        context.factory().registerMessage(VolumeMessage.id_, new VolumeMessage());
        context.handlers().registerHandler(VolumeMessage.id_, new VolumeMessageHandler(uiHost));
        // Zoom params message.
        context.factory().registerMessage(ZoomParamsMessage.id_, new ZoomParamsMessage());
        context.handlers().registerHandler(ZoomParamsMessage.id_, new ZoomParamsHandler(local));
    }   
}
