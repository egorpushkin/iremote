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

package com.scientificsoft.iremote.android.iremote.tools;

import android.content.Context;
import android.os.Build;
import android.view.GestureDetector;
import android.view.View;

import com.scientificsoft.iremote.android.iremote.ui.controls.ScaleGestureDetector;
import com.scientificsoft.iremote.android.iremote.ui.controls.SimpleListener3;
import com.scientificsoft.iremote.android.iremote.ui.controls.SimpleListener5;
import com.scientificsoft.iremote.platform.system.Device4;

public class ApiFeatures {
	
	public static final int sdk_ = Build.VERSION.SDK_INT;
	
	/**
	 * Creates ScaleGestureDetector.
	 * ScaleGestureDetector exists since API level 8.
	 * Custom implementation is supported since API level 5.
	 */
	public static ScaleGestureDetector createScaleDetector(Context context, ScaleGestureDetector.OnScaleGestureListener listener) {
		try {			
			if ( sdk_ >= 5 ) {
				return new ScaleGestureDetector(context, listener);
			}
		} catch ( Throwable th ) {			
		}			
		return null;
	}

	/**
	 * Instantiates gesture listener depending on device OS version.
	 * SimpleListener5 depends on some MotionEvent features introduced in API level 5.
	 */
	public static GestureDetector.OnGestureListener createGestureListener(View view) {
		try {
			if ( sdk_ >= 5 ) {
				return new SimpleListener5(view);
			} else {
				return new SimpleListener3(view);
			}
		} catch ( Throwable th ) {			
		}			
		return null;
	}
	
	/**
	 * android.os.Build.MANUFACTURER constant was introduced in API level 4. 
	 */
	public static String deviceManufacturer() {
		try {
			if ( sdk_ >= 4 )
				return new Device4().manufacturer();
		} catch ( Throwable th ) {
		}		
		return "";
	}
}
