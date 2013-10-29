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

import com.scientificsoft.iremote.platform.system.Device;

import com.scientificsoft.iremote.server.Versions;
import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.server.protocol.MessageImpl;
import com.scientificsoft.iremote.server.protocol.Property;

public class AuthMessage extends MessageImpl {
    
    public static final Guid id_ 
        = new Guid(0x1fe3a457, 0x4369, 0x42c7, 0x87, 0x8f, 0x10, 0x75, 0xf7, 0xb, 0x44, 0xcf);
    
    public static final int PROP_DEVICE = 1;
    public static final int PROP_ID = 2;
    public static final int PROP_NAME = 3;
    public static final int PROP_HAS_CODE = 4;
    public static final int PROP_CODE = 5;
    public static final int PROP_APP = 6;
    public static final int PROP_VER_MAJOR = 7;
    public static final int PROP_VER_MINOR = 8;
    public static final int PROP_VER_REV = 9;
    
    public AuthMessage(String id, String name, Guid cookie) {
        super(id_);
        registerProperty(PROP_DEVICE, new Property(Device.type()));
        registerProperty(PROP_ID, new Property(id));
        registerProperty(PROP_NAME, new Property(name));
        registerProperty(PROP_HAS_CODE, new Property(null != cookie));       
        registerProperty(PROP_CODE, new Property(cookie));
        registerProperty(PROP_APP, new Property(Versions.iRemoteApplicationGuid));
        registerProperty(PROP_VER_MAJOR, new Property(Versions.iREMOTE_VER_MAJOR));
        registerProperty(PROP_VER_MINOR, new Property(Versions.iREMOTE_VER_MINOR));
        registerProperty(PROP_VER_REV, new Property(Versions.iREMOTE_VER_REV));
    }

}
