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

import com.scientificsoft.iremote.server.protocol.Guid;

public class Versions {
	
    public static final boolean DEBUG = false;
    public static final boolean RELEASE = !DEBUG;    	

	public static final int iREMOTE_VER_MAJOR = 1;
	public static final int iREMOTE_VER_MINOR = 2;
	public static final int iREMOTE_VER_FIX = 0;
	public static final int iREMOTE_VER_REV = 371;

	public static final int iREMOTE_LICENSE = 3;
	
    public static Guid iRemoteApplicationGuid 
    	= new Guid( 0xba3d977a, 0x1ce8, 0x4d2f, 0xb6, 0xf9, 0x1b, 0x70, 0x65, 0x40, 0x1b, 0xc9 );	
	
}
