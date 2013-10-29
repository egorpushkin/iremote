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

package com.scientificsoft.iremote.platform.system;

import com.scientificsoft.iremote.android.iremote.tools.ApiFeatures;
import com.scientificsoft.iremote.android.iremote.tools.ContextHolder;
import com.scientificsoft.iremote.server.tools.Strings;

import android.content.Context;
import android.os.Vibrator;
import android.provider.Settings.Secure;

public class Device {
            
    public static final int DEVICE_ANDROID = 2;
    
    public static String deviceId() {
    	String id = Secure.getString(ContextHolder.instance().context().getContentResolver(), Secure.ANDROID_ID);
        return ( null != id ) ? ( id ) : ( "ANDROID-DEVICE-EMULATOR-0000" );
    }

    public static String deviceName() {
		String company = ApiFeatures.deviceManufacturer();
		if ( 0 != company.length() )
			company = company + " ";
    	String product = Strings.isEmpty(android.os.Build.MODEL) ? android.os.Build.DEVICE : android.os.Build.MODEL;
        return company + product;
    }
    
    public static int type() {
        return DEVICE_ANDROID;        
    }
    
    public static void vibrate() {
    	try {
	    	Vibrator v = (Vibrator)ContextHolder.instance().context().getSystemService(Context.VIBRATOR_SERVICE);
	    	v.vibrate(100);
    	} catch ( Throwable th ) {    		
    	}
    }

} 
