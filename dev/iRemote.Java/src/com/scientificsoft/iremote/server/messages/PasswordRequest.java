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

package com.scientificsoft.iremote.server.messages;

import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.server.protocol.IMessage;
import com.scientificsoft.iremote.server.protocol.MessageImpl;
import com.scientificsoft.iremote.server.protocol.Property;

public class PasswordRequest extends MessageImpl {
    
    public static final Guid id_ 
        = new Guid(0xf6c157c4, 0x5ed4, 0x4350, 0x8f, 0xdc, 0xa, 0xa7, 0x8a, 0xe1, 0xe4, 0x30);

    public static final int PROP_HOST_ID = 1;
    public static final int PROP_REQUEST_AUTH = 2;
    
    public PasswordRequest() {
        super(id_);
        registerProperty(PROP_HOST_ID, new Property(new Guid()));
        registerProperty(PROP_REQUEST_AUTH, new Property(false));
    }
    
    public void setSize(int size) {
    	// Compatibility issue. Remote PC versions below 1.4 had a bug sending 
    	// wrong message size of this particular message (44 instead of 45).
    	// At the same time 45 bytes of data were sent.   
    	if ( 44 == size )
    		size = 45;
    	super.setSize(size);    		
    }
    
    public IMessage clone() {
        return new PasswordRequest();
    }    
}
