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

package com.scientificsoft.iremote.server.protocol;

import java.io.IOException;
import java.io.InputStream;
import com.scientificsoft.iremote.server.protocol.MessageBuffer;

public class Parser {
    
    public static MessageHeader readHeader(InputStream stream) throws ProtocolException, EndOfStreamException, IOException {        
        MessageHeader messageHeader = new MessageHeader();
        
        // Extract message header from stream.
        MessageBuffer buffer = new MessageBuffer(4 + 4 + 16);
        buffer.read(stream);
        
        // Read message header.
        // Read begin marker.
        Property messageMarker = new Property((int)0x0);
        buffer.parse(messageMarker);
        messageHeader.beginMarker_ = messageMarker.toInt();
        if ( 0xfefefefe != messageHeader.beginMarker_ ) {
            throw new ProtocolException();
        }       
        
        // Read message size.
        Property messageSize = new Property((int)0x0);
        buffer.parse(messageSize);
        messageHeader.size_ = messageSize.toInt();
        
        // Read message code.
        Property messageCode = new Property(new Guid());
        buffer.parse(messageCode);
        messageHeader.code_ = messageCode.toGuid();
                        
        return messageHeader;
    }
    
    public static void skipMessage(InputStream stream, MessageHeader messageHeader) throws ProtocolException, IOException {
        // Skip message body.
        stream.skip(messageHeader.size_ - (4 + 4 + 16 + 4));
        // Extract terminating marker.
        MessageBuffer footer = new MessageBuffer(4);
        try {
            footer.read(stream);
        } catch ( Exception e ) {
            throw new ProtocolException();
        }        
        Property endMarker = new Property((int)0);
        footer.parse(endMarker);
        // Check terminating marker for correctness.
        if ( 0xcdcdcdcd != endMarker.toInt() ) {
            throw new ProtocolException();
        }
    }
    
    public static void readMessage(InputStream stream, IMessage message) throws EndOfStreamException, ProtocolException, IOException {
        int result = message.read(stream);
        if ( 0 != result ) {
            throw new ProtocolException();
        }
    }

}
