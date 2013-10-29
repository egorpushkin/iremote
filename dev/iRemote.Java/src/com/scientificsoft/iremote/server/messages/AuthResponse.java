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

public class AuthResponse extends MessageImpl {
    
    public static final Guid id_ 
        = new Guid(0x7680e9aa, 0x868f, 0x4f84, 0x8b, 0x4c, 0x3d, 0x41, 0x21, 0xb4, 0x87, 0x87);

    public static final int PROP_HOSTOS = 1;
    public static final int PROP_HAS_CODE = 2;
    public static final int PROP_CODE = 3;
    
    public AuthResponse() {
        super(id_);
        registerProperty(PROP_HOSTOS, new Property((int)0));
        registerProperty(PROP_HAS_CODE, new Property(false));
        registerProperty(PROP_CODE, new Property(new Guid()));
    }
    
    public IMessage clone() {
        return new AuthResponse();
    }          
    
}
