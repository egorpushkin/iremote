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

import java.util.Formatter;
import java.util.UUID;

import com.scientificsoft.iremote.server.protocol.Guid;

/**
 * The following format is used for string representation of guids:
 * {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
 * 01234567890123456789012345678901234567 (character index)
 */
public class CoreGuid {	
	public static boolean fromString(String str, Guid guid) {
		if ( 38 != str.length() )
			return false;
		try {
			guid.data1_ = (int)Long.parseLong(str.substring(1 , 9 ), 16);
			guid.data2_ = (short)Integer.parseInt(str.substring(10, 14), 16);
			guid.data3_ = (short)Integer.parseInt(str.substring(15, 19), 16);
			guid.data4_[0] = (byte)Short.parseShort(str.substring(20, 22), 16);
			guid.data4_[1] = (byte)Short.parseShort(str.substring(22, 24), 16);
			guid.data4_[2] = (byte)Short.parseShort(str.substring(25, 27), 16);
			guid.data4_[3] = (byte)Short.parseShort(str.substring(27, 29), 16);
			guid.data4_[4] = (byte)Short.parseShort(str.substring(29, 31), 16);
			guid.data4_[5] = (byte)Short.parseShort(str.substring(31, 33), 16);
			guid.data4_[6] = (byte)Short.parseShort(str.substring(33, 35), 16);
			guid.data4_[7] = (byte)Short.parseShort(str.substring(35, 37), 16);		
			guid.update();
		} catch ( Exception e ) {
			return false;
		}
		return true;
	}
	
	public static String toString(Guid guid) {
		Formatter formatter = new Formatter();
		formatter = formatter.format("{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", 
			guid.data1_, guid.data2_, guid.data3_, guid.data4_[0], guid.data4_[1],
			guid.data4_[2], guid.data4_[3], guid.data4_[4], guid.data4_[5], guid.data4_[6], guid.data4_[7]);
		String str = formatter.toString();
		formatter.close();
		return str;
	}
	
	public static void generate(Guid guid) {
		UUID uuid = UUID.randomUUID();
		String str = "{" + uuid.toString() + "}";
		fromString(str, guid);
	}
}
