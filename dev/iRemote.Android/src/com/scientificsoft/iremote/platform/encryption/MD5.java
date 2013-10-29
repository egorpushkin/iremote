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

package com.scientificsoft.iremote.platform.encryption;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class MD5 {

    /**
     * Calculates md5 hash over given string and produces hex output
     * (zeros are assumed).
     */
    public static String calculate(String str) {
		try {  
			// Create MD5 Hash.  
			MessageDigest digest = java.security.MessageDigest.getInstance("MD5");  
			digest.update(str.getBytes());  
			byte messageDigest[] = digest.digest();  
			   
			// Create Hex String  
			StringBuffer hexString = new StringBuffer();  
			for (int i=0; i<messageDigest.length; i++) {
				String ch = Integer.toHexString(0xFF & messageDigest[i]);
				while (ch.length()<2) ch = "0" + ch;
				hexString.append(ch);
			}
			return hexString.toString();			       
		} catch (NoSuchAlgorithmException e) {
		}  
		return "";  
    }    
    
} 
