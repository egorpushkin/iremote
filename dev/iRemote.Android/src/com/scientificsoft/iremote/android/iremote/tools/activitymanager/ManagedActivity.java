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

package com.scientificsoft.iremote.android.iremote.tools.activitymanager;

import com.scientificsoft.iremote.android.iremote.tools.ContextHolder;

import android.app.Activity;
import android.os.Bundle;

abstract public class ManagedActivity extends Activity {	
	
	abstract public int getActivityGroupId();
	
	@Override    
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		ContextHolder.instance().activityManager().onCreate(getActivityGroupId(), this);
	}
	
	@Override    
	protected void onDestroy() {
		super.onDestroy();
		ContextHolder.instance().activityManager().onDestroy(getActivityGroupId(), this);
	}

    @Override    
    public void onPause() {
        super.onPause();
        ContextHolder.instance().activityManager().onPause(getActivityGroupId(), this);
    }       
    
    @Override    
    public void onResume() {
        super.onResume();	
        ContextHolder.instance().activityManager().onResume(getActivityGroupId(), this);
    }
}
