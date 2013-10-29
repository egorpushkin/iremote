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

public class ZoomParamsMessage extends MessageImpl {

    public static final Guid id_ 
        = new Guid(0x9e67de9a, 0xbbec, 0x40a6, 0x8c, 0xa6, 0x22, 0xe9, 0x3b, 0x56, 0xa4, 0xe4);

    public static final int PROP_MIN_ZOOM = 1;
    public static final int PROP_MAX_ZOOM = 2;
    public static final int PROP_CURRENT = 3;

    public ZoomParamsMessage() {
        super(id_);
        registerProperty(PROP_MIN_ZOOM, new Property((float)0.0f));
        registerProperty(PROP_MAX_ZOOM, new Property((float)0.0f));
        registerProperty(PROP_CURRENT, new Property((float)0.0f));
    }
    
    public IMessage clone() {
        return new ZoomParamsMessage();
    }          
        
}
