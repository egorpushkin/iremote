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

import com.localytics.android.LocalyticsSession;
import com.scientificsoft.iremote.android.iremote.tools.activitymanager.ActivityManager;

import android.content.Context;

/**
 * The Class ContextHolder.
 */
public final class ContextHolder {

	/** The instance. */
	private static ContextHolder instance_;

	/** The context. */
	private Context context_;
	
	/** Activity manager. */
	private ActivityManager activityManager_ = new ActivityManager();
	
	/** Analytics engine. */
	private LocalyticsSession localyticsSession_ = null;

	/**
	 * Instantiates a new context holder.
	 */
	private ContextHolder() {
	}

	/**
	 * Gets the single instance of ContextHolder.
	 */
	public static synchronized ContextHolder instance() {
		if (instance_ == null) {
			instance_ = new ContextHolder();
		}
		return instance_;
	}

	@Override
	protected Object clone() throws CloneNotSupportedException {
		throw new CloneNotSupportedException("Singleton");
	}

	/**
	 * Gets the context.
	 */
	public Context context() {
		return context_;
	}

	/**
	 * Sets the context.
	 */
	public void setContext(final Context pContext) {
		this.context_ = pContext.getApplicationContext();
	}	
	
	/**
	 * Gets the activity manager.
	 */
	public ActivityManager activityManager() {
		return activityManager_;
	}
	
	/** Localytics helpers. */
	
    private final static String EVENT_CONNECTED = "connected";	
	
	/**
	 * Gets the localytics section.
	 */
	public LocalyticsSession localyticsSession() {
		return localyticsSession_;
	}		
	
	public void localyticsStart() {
		try {
	        localyticsSession_ = new LocalyticsSession(context_);
	        localyticsSession_.open();
	        localyticsSession_.upload();		
		} catch ( Exception e ) {			
		}
	}
	
	public void localyticsPause() {
		try {
			localyticsSession_.close();
			localyticsSession_.upload();
		} catch ( Exception e ) {			
		}		
	}
	
	public void localyticsResume() {
		try {
			localyticsSession_.open();
		} catch ( Exception e ) {			
		}		
	}	
	
	public void localyticsTagConnected() {
		try {
			localyticsSession_.tagEvent(EVENT_CONNECTED);
		} catch ( Exception e ) {			
		}		
	}
}
