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

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.scientificsoft.iremote.server.protocol.EndOfStreamException;
import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.server.protocol.IMessage;
import com.scientificsoft.iremote.server.protocol.MessageBuffer;
import com.scientificsoft.iremote.server.protocol.MessageImpl;
import com.scientificsoft.iremote.server.protocol.Property;

public class ScreenshotMessage extends MessageImpl {
    
    public static final Guid id_ 
        = new Guid(0x211140bb, 0xed67, 0x4952, 0xaf, 0xd6, 0x73, 0x60, 0xaf, 0x70, 0xd7, 0x85);
    
    private int dataSize_ = 0;
    
    private byte[] data_ = null;

    public ScreenshotMessage() {
        super(id_);
    }
    
    public IMessage clone() {
        return new ScreenshotMessage();
    }
    
    public int dataSize() {
        return dataSize_;
    }   
    
    public byte[] data() {
        return data_;
    }
    
    public int size() {
        // Header.
        int size = 4 + 4 + 16;
        // Body.
        size += 4 + dataSize_;
        // Footer.
        size += 4;      
        return size;
    }
    
    public int write(OutputStream stream) throws IOException {
        return -1;
    }   
    
    public int read(InputStream stream) throws EndOfStreamException, IOException {
        // Extract image data length.
        MessageBuffer buffer = new MessageBuffer(4);
        buffer.read(stream);
        Property imageSize = new Property((int)0x0);
        buffer.parse(imageSize);
        dataSize_ = imageSize.toInt();

        // Extract image data.
        buffer = new MessageBuffer(dataSize_);
        buffer.read(stream);
        data_ = buffer.extract(dataSize_);
        
        // Parse message footer.
        buffer = new MessageBuffer(4);
        buffer.read(stream);
        Property footer = new Property((int)0x0);
        buffer.parse(footer);
        if ( 0xcdcdcdcd != footer.toInt() ) {
            return -1;
        }
        
        return 0;
    }
    
}
