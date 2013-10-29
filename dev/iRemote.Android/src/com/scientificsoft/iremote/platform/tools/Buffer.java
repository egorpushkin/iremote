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

package com.scientificsoft.iremote.platform.tools;

public class Buffer {
    
	private java.nio.ByteBuffer buffer_ = null;
	
    public Buffer(int size) {    
    	buffer_ = java.nio.ByteBuffer.allocate(size);
    	buffer_.order(java.nio.ByteOrder.LITTLE_ENDIAN);	
    }
    
    public byte[] array() {
        return buffer_.array();
    }
    
    public void put(byte b) {
    	buffer_.put(b);
    }
    
    public void put(byte [] b) {
    	buffer_.put(b);
    }
    
    public void putShort(short s) {
    	buffer_.putShort(s);
    }
    
    public void putInt(int i) {
    	buffer_.putInt(i);
    }    
    
    public void putFloat(float f) {
    	buffer_.putFloat(f);
    }      
    
    public void putDouble(double d) {
    	buffer_.putDouble(d);
    }       
    
    public byte get()
    {
        return buffer_.get();
    }   
    
    public void get(byte [] b) 
    {
    	buffer_.get(b);
    }       
    
    public short getShort()
    {
        return buffer_.getShort();
    }       
    
    public int getInt()
    {
        return buffer_.getInt();
    }
    
    public float getFloat()
    {
        return buffer_.getFloat();
    }           

    public double getDouble()
    {
        return buffer_.getDouble();
    }      
    
} 
