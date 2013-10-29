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

public class VolumeMessage extends MessageImpl {

    public static final Guid id_ 
        = new Guid(0x4b204c3f, 0x6ab8, 0x4191, 0xba, 0x21, 0x76, 0x27, 0x61, 0x8e, 0x15, 0xa4);

    public static final int PROP_DEVICE = 1;
    public static final int PROP_VALUE = 2;
    
	public static final int DEVICE_MASTER = 1;

    public VolumeMessage() {
        super(id_);
        registerProperty(PROP_DEVICE, new Property((int)0x0));
        registerProperty(PROP_VALUE, new Property((int)0x0));           
    }
    
    public VolumeMessage(int device, int value) {
    	super(id_);
        registerProperty(PROP_DEVICE, new Property(device));
        registerProperty(PROP_VALUE, new Property(value));    	
    }
    
    public IMessage clone() {
        return new VolumeMessage();
    }       
        
}

