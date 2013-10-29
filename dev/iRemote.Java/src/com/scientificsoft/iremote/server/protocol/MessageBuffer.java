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
import java.io.UnsupportedEncodingException;
import com.scientificsoft.iremote.platform.tools.Buffer;

public class MessageBuffer {
    
    private int size_ = 0x0;

    private Buffer buffer_ = null;
    
    public MessageBuffer(int size) {
        size_ = size;
        buffer_ = new Buffer(size_);
    }
    
    public void flush(OutputStream stream) throws IOException {
        stream.write(buffer_.array());
    }   
    
    /**
     * This method must extract size_ bytes from stream and block
     * until it is done. 
     * 
     * This raises EndOfStreamException, when reading signals on end 
     * of stream reached (InputStream::read returns -1). 
     */
    public void read(InputStream stream) throws EndOfStreamException, IOException {
        int totalRead = 0;
        while ( totalRead < size_ ) {
            // Read remaining portion of data from stream. 
            int bytesRead = stream.read(buffer_.array(), totalRead, size_ - totalRead);
            // Ensure that reading was successful.
            if ( -1 == bytesRead )
                throw new EndOfStreamException();           
            // Increment a number of read bytes.
            totalRead += bytesRead; 
        }
    }       
    
    public void dump(Property value) {
        switch ( value.type() ) {
        case Property.TYPE_BOOL:
            buffer_.put( value.toBool() ? ( (byte)1 ) : ( (byte)0 ) );
            break;
        case Property.TYPE_BYTE:
        	buffer_.put(value.toByte());
        	break;
        case Property.TYPE_SHORT:
            buffer_.putShort(value.toShort());
            break;
        case Property.TYPE_INT:
            buffer_.putInt(value.toInt());
            break;
        case Property.TYPE_FLOAT:
            buffer_.putFloat(value.toFloat());
            break;          
        case Property.TYPE_DOUBLE:
            buffer_.putDouble(value.toDouble());
            break;
        case Property.TYPE_STRING:
            String valueStr = value.toString();
            buffer_.putInt(valueStr.length() + 1);
            buffer_.put(valueStr.getBytes());
            buffer_.put((byte)0);   
            break;          
        case Property.TYPE_GUID:
            Guid valueGuid = value.toGuid();
            if ( null == valueGuid )
            	valueGuid = new Guid();
            buffer_.putInt(valueGuid.data1_);
            buffer_.putShort(valueGuid.data2_);
            buffer_.putShort(valueGuid.data3_);
            buffer_.put(valueGuid.data4_[0]);
            buffer_.put(valueGuid.data4_[1]);
            buffer_.put(valueGuid.data4_[2]);
            buffer_.put(valueGuid.data4_[3]);
            buffer_.put(valueGuid.data4_[4]);
            buffer_.put(valueGuid.data4_[5]);
            buffer_.put(valueGuid.data4_[6]);
            buffer_.put(valueGuid.data4_[7]);
            break;          
        default:
            // Dump nothing, if property has unknown type.
            break;
        }
    }
    
    public void parse(Property value) throws IOException {
        switch ( value.type() ) {
        case Property.TYPE_BOOL:
            value.setBool( 1 == buffer_.get() ? ( true ) : ( false ) );
            break;      
        case Property.TYPE_BYTE:
        	value.setByte(buffer_.get());
        	break;
        case Property.TYPE_SHORT:
            value.setShort(buffer_.getShort());
            break;
        case Property.TYPE_INT:
            value.setInt(buffer_.getInt());
            break;
        case Property.TYPE_FLOAT:
            value.setFloat(buffer_.getFloat());
            break;          
        case Property.TYPE_DOUBLE:
            value.setDouble(buffer_.getDouble());
            break;
        case Property.TYPE_STRING:
            int length = buffer_.getInt();
            byte []characters = new byte[length];
            buffer_.get(characters);
            try {
                value.setString(new String(characters, 0, length, "utf-8"));
            } catch ( UnsupportedEncodingException e ) {
                value.setString("");
            }
            break;          
        case Property.TYPE_GUID:
            Guid guid = new Guid(); 
            guid.data1_ = buffer_.getInt();
            guid.data2_ = buffer_.getShort();
            guid.data3_ = buffer_.getShort();
            guid.data4_[0] = buffer_.get();
            guid.data4_[1] = buffer_.get();
            guid.data4_[2] = buffer_.get();
            guid.data4_[3] = buffer_.get();
            guid.data4_[4] = buffer_.get();
            guid.data4_[5] = buffer_.get();
            guid.data4_[6] = buffer_.get();
            guid.data4_[7] = buffer_.get();
            guid.update();
            value.setGuid(guid);            
            break;          
        default:
            // Dump nothing, if property has unknown type.
            break;
        }
    }   
    
    public byte[] extract(int size) {
        byte[] data = new byte[size];
        buffer_.get(data);
        return data;
    }
    
    public static int size(Property value) {
        switch ( value.type() ) {
        case Property.TYPE_BOOL:
            return 1;       
        case Property.TYPE_BYTE:
        	return 1;
        case Property.TYPE_SHORT:
            return 2;
        case Property.TYPE_INT:
            return 4;
        case Property.TYPE_FLOAT:
            return 4;
        case Property.TYPE_DOUBLE:
            return 8;
        case Property.TYPE_STRING:
            return 4 + value.toString().length() + 1;
        case Property.TYPE_GUID:
            return 16;
        default:
            // Dump nothing, if property has unknown type.
            return 0;
        }
    }
    
}
