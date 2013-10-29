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

import com.scientificsoft.iremote.platform.tools.CoreGuid;

public class Guid {
    
    public int data1_ = 0x0;
    
    public short data2_ = 0x0;
    
    public short data3_ = 0x0;
    
    public int data23int_ = 0x0;
    
    public byte[] data4_ = new byte[8];
    
    public int data41int_ = 0x0;
    
    public int data42int_ = 0x0;
    
    public Guid() { 
        data4_[0] = 0x0;
        data4_[1] = 0x0;
        data4_[2] = 0x0;
        data4_[3] = 0x0;
        data4_[4] = 0x0;
        data4_[5] = 0x0;
        data4_[6] = 0x0;
        data4_[7] = 0x0;
    }
    
    public Guid(String str) throws Exception {
        if ( !CoreGuid.fromString(str, this) )
        	throw new Exception();
    }
    
    public Guid(
        int data1, int data2, int data3, 
        int data41, int data42, int data43, int data44, 
        int data45, int data46, int data47, int data48) {
        
        data1_ = data1;
        data2_ = (short)data2;
        data3_ = (short)data3;
        data4_[0] = (byte)data41;
        data4_[1] = (byte)data42;
        data4_[2] = (byte)data43;
        data4_[3] = (byte)data44;
        data4_[4] = (byte)data45;
        data4_[5] = (byte)data46;
        data4_[6] = (byte)data47;
        data4_[7] = (byte)data48;
        
        update();       
    }
    
    public void update() {
        data23int_ 
            = ( shortToInt(data2_) )
            | ( shortToInt(data3_) << 16 );
        data41int_ 
            = ( byteToInt(data4_[0]) ) 
            | ( byteToInt(data4_[1]) << 8 ) 
            | ( byteToInt(data4_[2]) << 16 )
            | ( byteToInt(data4_[3]) << 24 );
        data42int_ 
            = ( byteToInt(data4_[4]) ) 
            | ( byteToInt(data4_[5]) << 8 ) 
            | ( byteToInt(data4_[6]) << 16 )
            | ( byteToInt(data4_[7]) << 24 );      
    }
    
    public boolean equals(Object object) {
        Guid guid1 = this;
        Guid guid2 = (Guid)object; 
        int diff = guid1.data1_ - guid2.data1_;
        if ( 0 != diff )
            return false;
        diff = guid1.data23int_ - guid2.data23int_;
        if ( 0 != diff )
            return false;
        diff = guid1.data41int_ - guid2.data41int_;
        if ( 0 != diff )
            return false;
        diff = guid1.data42int_ - guid2.data42int_;
        return ( 0 == diff );
    }

    /**
     * Due to the fact that UUIDs are unique in the Universe,
     * it is impossible to pack any UUID into integer value. Thus, 
     * UUIDs should be compared through Guid::equals method. This
     * behavior is achieved by providing all UUIDs with the same hash.
     * 
     * Android. As HashMap was chosen to store key-value pairs, 
     * it is required to calculate Guid hash even increasing the 
     * possibility of potential collisions.
     */
    public int hashCode() {
        return ((( data1_ ^ data23int_ ) ^ data41int_ ) ^ data42int_ );
    }
    
    public String toString() {
    	return CoreGuid.toString(this);
    }
    
    public static int byteToInt(byte value) {
    	return ( value < 0 ) ? ( value + 256 ) : value;
    }

    public static int shortToInt(short value) {
    	return ( value < 0 ) ? ( value + 65536 ) : value;
    }
}

