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

package com.scientificsoft.iremote.server;

public interface DSession {
        
    public static final int ERROR_UNKNOWN = -0x1;
    public static final int ERROR_OK = 0x0; 
    public static final int ERROR_FAILED_TO_CONNECT = 0x1;
    public static final int ERROR_TIMEOUT = 0x2;
    public static final int ERROR_FAILED_TO_CLOSE = 0x3;    
    public static final int ERROR_PROTOCOL = 0x4;   
    public static final int ERROR_TERMINATED_BY_HOST = 0x5;
    public static final int ERROR_TERMINATED_BY_USER = 0x6;
    public static final int ERROR_WRONG_DEVICE_IP = 0x7;
    public static final int ERROR_LIMIT_REACHED = 0x8;
    public static final int ERROR_OLD_REMOTE_PC = 0x9;
    
    abstract public void connecting();
                
    abstract public void connected();
    
    abstract public void passwordRequested(boolean wrong);
    
    abstract public void approved();
    
    abstract public void disconnected(int reason);
    
}
