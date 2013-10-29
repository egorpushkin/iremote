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

/**
 * Getters do not throw exceptions, when invoked for property type 
 * does not correspond to the getter called. 
 */
public class Property {

    public static final int TYPE_UNKNOWN    = 0;
    public static final int TYPE_BOOL = 1;
    public static final int TYPE_BYTE = 2;
    public static final int TYPE_SHORT = 3;  
    public static final int TYPE_INT = 4;
    public static final int TYPE_FLOAT = 5;
    public static final int TYPE_DOUBLE = 6;
    public static final int TYPE_STRING = 7;
    public static final int TYPE_GUID = 8;
    
    private int type_ = TYPE_UNKNOWN;  
    
    private boolean boolVal_ = false;
    
    private byte byteVal_ = 0;
    
    private short shortVal_ = 0;
    
    private int intVal_ = 0;
    
    private float floatVal_ = 0.0f;
    
    private double doubleVal_ = 0.0;

    private String stringVal_ = null;
    
    private Guid guidVal_ = null;

    public int type() {
        return type_;
    }
    
    public Property() {
    }
    
    /**
     * This method in intended to clone just property structure.
     * Aggregated data is not cloned.
     */
    public Property clone() {
        Property property = new Property();
        property.type_ = type_;
        return property;
    }    
    
    public Property(boolean val) {
        type_ = TYPE_BOOL;
        boolVal_ = val;
    }
    
    public void setBool(boolean val)
    {
        type_ = TYPE_BOOL;
        boolVal_ = val;
        stringVal_ = null;
        guidVal_ = null;
    }
    
    public boolean toBool()
    {       
        return boolVal_;
    }       
    
    public Property(byte val) {
        type_ = TYPE_BYTE;
        byteVal_ = val;
    }    
    
    public void setByte(byte val)
    {
        type_ = TYPE_BYTE;
        byteVal_ = val;
        stringVal_ = null;
        guidVal_ = null;
    }
    
    public byte toByte()
    {       
        return byteVal_;
    }         
    
    public Property(short val) {
        type_ = TYPE_SHORT;
        shortVal_ = val;
    }        
    
    public void setShort(short val)
    {
        type_ = TYPE_SHORT;
        shortVal_ = val;
        stringVal_ = null;
        guidVal_ = null;
    }
    
    public short toShort()
    {       
        return shortVal_;
    }
    
    public Property(int val)
    {
        type_ = TYPE_INT;
        intVal_ = val;
    }
    
    public void setInt(int val)
    {
        type_ = TYPE_INT;
        intVal_ = val;
        stringVal_ = null;
        guidVal_ = null;
    }   
    
    public int toInt()
    {       
        return intVal_;
    }

    public Property(float val)
    {
        type_ = TYPE_FLOAT;
        floatVal_ = val;
    }
    
    public void setFloat(float val)
    {
        type_ = TYPE_FLOAT;
        floatVal_ = val;
        stringVal_ = null;
        guidVal_ = null;
    }       
    
    public float toFloat()
    {       
        return floatVal_;
    }

    public Property(double val)
    {
        type_ = TYPE_DOUBLE;
        doubleVal_ = val;
    }
    
    public void setDouble(double val)
    {
        type_ = TYPE_DOUBLE;
        doubleVal_ = val;
        stringVal_ = null;
        guidVal_ = null;
    }       
    
    public double toDouble()
    {       
        return doubleVal_;
    }

    public Property(String val)
    {
        type_ = TYPE_STRING;
        stringVal_ = val;
    }
    
    public void setString(String val)
    {
        type_ = TYPE_STRING;
        stringVal_ = val;
        guidVal_ = null;
    }   
    
    public String toString()
    {       
        return stringVal_;
    }
    
    public Property(Guid val)
    {
        type_ = TYPE_GUID;
        guidVal_ = val;
    }
    
    public void setGuid(Guid val)
    {
        type_ = TYPE_GUID;
        stringVal_ = null;
        guidVal_ = val;
    }       
    
    public Guid toGuid()
    {       
        return guidVal_;
    }   
    
}
