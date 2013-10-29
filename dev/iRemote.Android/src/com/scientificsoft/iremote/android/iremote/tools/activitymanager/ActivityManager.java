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

import java.util.Hashtable;
import java.util.Vector;

import android.app.Activity;
import android.os.Handler;

public class ActivityManager {
	
	public static final int EVENT_CREATE = 0x1;
	public static final int EVENT_DESTROY = 0x2;
	public static final int EVENT_RESUME = 0x3;
	public static final int EVENT_PAUSE = 0x4;
	
	public static final int STATUS_FRONT = 0x1;
	public static final int STATUS_BACK = 0x2;
	
	private static final int TIMEOUT_SEND_TO_BACK = 10000;
	
	private static final int TIMEOUT_BRING_TO_FRONT = 300;
	
	private IAppController appController_ = null;
	
	private int status_ = STATUS_FRONT;
	
	private int active_ = 0;
	
	private Hashtable<Integer, Vector<Activity>> groups_ = new Hashtable<Integer, Vector<Activity>>();
	
	private Handler timer_ = new Handler();
	
	private Runnable backCallback_ = null;
	
	private Runnable frontCallback_ = null;

	public ActivityManager() {		
	}
	
	public void setAppController(IAppController controller) {
		appController_ = controller;
	}
	
	public void killGroup(int group) {
		Integer groupKey = new Integer(group);
		if ( groups_.containsKey(groupKey) ) {
			Vector<Activity> activities = groups_.get(groupKey);
			groups_.remove(groupKey);
			for ( Activity activity: activities ) {
				activity.finish();
			}
		}
	}
	
	public void logEvent(int event, int group, Activity activity) {
		switch ( event ) {
		case EVENT_CREATE:
			onCreate(group, activity);
			break;
		case EVENT_DESTROY:
			onDestroy(group, activity);
			break;		
		case EVENT_RESUME:
			onResume(group, activity);
			break;
		case EVENT_PAUSE:
			onPause(group, activity);
			break;			
		}
	}
	
	public void onCreate(int group, Activity activity) {
		Integer groupKey = new Integer(group);
		Vector<Activity> activities = null;
		if ( groups_.containsKey(groupKey) ) {
			activities = groups_.get(groupKey);
		} else {
			activities = new Vector<Activity>();
			groups_.put(groupKey, activities);
		}
		activities.add(activity);
	}
	
	public void onDestroy(int group, Activity activity) {
		Integer groupKey = new Integer(group);
		if ( groups_.containsKey(groupKey) ) {
			Vector<Activity> activities = groups_.get(groupKey);
			activities.remove(activity);
		}
	}
	
	public void onResume(int group, Activity activity) {
		++active_;
		skipBackTimer();
		if ( 1 == active_ ) {
			startFrontTimer();
		}		
	}	
	
	public void onPause(int group, Activity activity) {
		--active_;		
		skipFrontTimer();
		if ( 0 == active_ ) {			
			startBackTimer();
		}		
	}		
	
	protected void sendToBack() {
		if ( STATUS_FRONT == status_ ) {
			status_ = STATUS_BACK;
			if ( null != appController_ ) {
				appController_.sendToBack();
			}
		}
	}
	
	protected void bringToFront() {
		if ( STATUS_BACK == status_ ) {
			status_ = STATUS_FRONT;
			if ( null != appController_ ) {			
				appController_.bringToFront();
			}
		}		
	}	
	
	protected void startFrontTimer() {
		if ( null != frontCallback_ ) {
			timer_.removeCallbacks(frontCallback_);
		}
		frontCallback_ = new BringToFrontTimer();
		timer_.postDelayed(frontCallback_, TIMEOUT_BRING_TO_FRONT);
	}
	
	protected void skipFrontTimer() {
		if ( null != frontCallback_ ) {
			timer_.removeCallbacks(frontCallback_);
			frontCallback_ = null;
		}
	}		
	
	protected void startBackTimer() {
		if ( null != backCallback_ ) {
			timer_.removeCallbacks(backCallback_);
		}
		backCallback_ = new SendToBackTimer();
		timer_.postDelayed(backCallback_, TIMEOUT_SEND_TO_BACK);
	}
	
	protected void skipBackTimer() {
		if ( null != backCallback_ ) {
			timer_.removeCallbacks(backCallback_);
			backCallback_ = null;
		}
	}	
	
	private class BringToFrontTimer implements Runnable {
		public void run() {
			if ( active_ > 0 ) {
				bringToFront();			
			}
			frontCallback_ = null;
		}
	}	

	private class SendToBackTimer implements Runnable {
		public void run() {
			if ( active_ <= 0 ) {
				sendToBack();			
			}
			backCallback_ = null;
		}
	}	
}
