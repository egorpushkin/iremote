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
import com.scientificsoft.iremote.server.protocol.MessageImpl;
import com.scientificsoft.iremote.server.protocol.Property;

public class KeyboardMessage extends MessageImpl {
	
    public static final Guid id_ 
    	= new Guid(0x274fd65e, 0xed7, 0x4a86, 0x9c, 0x59, 0x22, 0xa9, 0x20, 0x43, 0x7b, 0xb6);
    
    public static final int TYPE_VIRTUAL = 1;
    public static final int TYPE_UNICODE = 2;
    public static final int TYPE_SCAN = 3;	

	public static final int PROP_TYPE = 1;
	public static final int PROP_VK = 2;
	public static final int PROP_SCAN = 3;
	public static final int PROP_EVENT = 4;
	
	public KeyboardMessage(int type, byte vk, boolean keydown) {
	    super(id_);
	    registerProperty(PROP_TYPE, new Property(type));
	    registerProperty(PROP_VK, new Property(vk));
	    registerProperty(PROP_SCAN, new Property((short)0));
	    registerProperty(PROP_EVENT, new Property(keydown));       
	}	
	
	public KeyboardMessage(int type, short scan) {
	    super(id_);
	    registerProperty(PROP_TYPE, new Property(type));
	    registerProperty(PROP_VK, new Property((byte)0));
	    registerProperty(PROP_SCAN, new Property(scan));
	    registerProperty(PROP_EVENT, new Property((boolean)false));       
	}		
}
