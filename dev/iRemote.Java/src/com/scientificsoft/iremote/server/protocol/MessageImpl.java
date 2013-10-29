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
import java.io.OutputStream;
import com.scientificsoft.iremote.platform.tools.Map;
import com.scientificsoft.iremote.platform.tools.Enumeration;

public class MessageImpl implements IMessage {
    
    /** 
     * Contains the list of all message properties. 
     * Properties are stored in the message binary representation 
     * in order of identifiers grow.  
     */
    private Map properties_ = new Map();

    /** Contains message size, for messages to be extracted from stream. */
    private int size_ = -1; 
    
    /** Contains unique message code. */
    private Guid code_ = null;
    
    /** Constructors. */
    
    public MessageImpl() {
    }
    
    public MessageImpl(Guid code) {
        code_ = code;
    }
    
    /** IMessage section */
    
    public IMessage clone() {
        IMessage message = new MessageImpl(code_);
        for ( Enumeration keysEnum = properties_.mapKeys() ; keysEnum.hasNext() ; ) 
        {
            Integer key = (Integer)keysEnum.next();
            Property value = (Property)properties_.get(key);
            message.registerProperty(key.intValue(), value.clone());
        }
        return message;
    }
    
    public void setSize(int size) {
        size_ = size;
    }
    
    public void setCode(Guid code) {
        code_ = code;
    }
    
    public Guid code() {
        return code_;
    }

    public int size() {
        // Return predefined value, if it is specified.
        if ( -1 != size_ )
            return size_;
        
        // Calculate message size, based on its contents.
        // Header.
        int size = 4 + 4 + 16;
        // Body.
        for ( Enumeration valuesEnum = properties_.mapValues() ; valuesEnum.hasNext() ; ) 
        {            
            Property property = (Property)valuesEnum.next();
            size += MessageBuffer.size(property);
        }        
        // Footer.
        size += 4;      
        return size;
    }

    public int write(OutputStream stream) throws IOException {
        int messageSize = size(); 
        MessageBuffer buffer = new MessageBuffer(messageSize); 
        
        // Dump message header.
        buffer.dump(new Property((int)0xfefefefe));
        buffer.dump(new Property((int)messageSize));
        buffer.dump(new Property(code_));       
        // Dump message body.
        for ( Enumeration valuesEnum = properties_.mapValues() ; valuesEnum.hasNext() ; ) 
        {            
            Property property = (Property)valuesEnum.next();
            buffer.dump(property);
        }         
        // Dump message footer.
        buffer.dump(new Property((int)0xcdcdcdcd));
        
        // Send the message.
        buffer.flush(stream);
        
        return 0;
    }

    /**
     * @return 
     * 0 on success.
     * -1 in case of protocol-related error.
     */
    public int read(InputStream stream) throws EndOfStreamException, IOException {
        if ( -1 == size_ ) {
            // Only buffered input is supported by current implementation.
            // Size property must have already been set by this time.  
            return -1;
        }
        
        int messageSize = size_ - ( 4 + 4 + 16 ); 
        MessageBuffer buffer = new MessageBuffer(messageSize);      
        
        // Read message body and footer.
        buffer.read(stream);
        
        // Parse message body.
        for ( Enumeration valuesEnum = properties_.mapValues() ; valuesEnum.hasNext() ; ) 
        {            
            Property property = (Property)valuesEnum.next();
            buffer.parse(property);
        }          
        // Parse message footer.
        Property footer = new Property((int)0);
        buffer.parse(footer);
        if ( 0xcdcdcdcd != footer.toInt() ) {
            return -1;
        }
        
        return 0;
    }

    public void registerProperty(int id) {
        properties_.put(new Integer(id), new Property());
    }
    
    public void registerProperty(int id, Property value) {
        properties_.put(new Integer(id), value);
    }
    
    public void putProperty(int id, Property value) {
        properties_.put(new Integer(id), value);
    }
    
    public Property getProperty(int id) throws Exception {
        Property _value = (Property)properties_.get(new Integer(id));
        if ( null == _value )
            throw new Exception("Property with soecified id was not found.");
        return _value;
    }
        
}
