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

public interface IMessage {
    
    abstract public IMessage clone();
        
    /**
     * Specifies size of message to be extracted from stream.
     * Message size is required for buffered input.  
     */
    abstract public void setSize(int size); 
    
    abstract public void setCode(Guid code);
    
    abstract public Guid code();

    abstract public int size();

    /**
     * Message implementation must write entire message content
     * to the buffer (header, body and footer).  
     */
    abstract public int write(OutputStream stream) throws IOException;

    /**
     * Message implementation must read from buffer only body and 
     * footer! Header is extracted by message factory. 
     */
    abstract public int read(InputStream stream) throws EndOfStreamException, IOException;
    
    abstract public void registerProperty(int id);
    
    abstract public void registerProperty(int id, Property value);
    
    abstract public void putProperty(int id, Property value);
    
    abstract public Property getProperty(int id) throws Exception;  

}
