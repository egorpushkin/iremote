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

package com.scientificsoft.iremote.server.tools;

import com.scientificsoft.iremote.server.IExecutor;

/** 
 * Timer is designed to invoke tasks on a thread, to which provided
 * executor is attached.
 */
public class Timer {
	
	/** Tool to invoke tasks on a specific thread. */
	private IExecutor executor_ = null;
	
	/** Task to be executed repeatedly. */
	private Runnable task_ = null;
	
	/** Internal tool to manage reoccurrence. */
	private TimerTask timerTask_ = null;
	
	/** Time interval between tasks (includes task invocation time). */
	private long interval_ = -1;
	
	/** Indicates whether task should be executed repeatedly forever,
	 * until stopped. */
	private boolean repeat_ = false;
	
	public Timer(IExecutor executor) {
		executor_ = executor; 
		timerTask_ = new TimerTask();
	}
	
	public synchronized void start(Runnable task, long interval, boolean repeat) {
		if ( null != task_ )
			return;
		task_ = task;
		interval_ = interval;	
		repeat_ = repeat;
		executor_.runDelayed(timerTask_, interval_);
	}
	
	public synchronized void stop() {
		if ( null == task_ ) 
			return;
		executor_.cancel(timerTask_);
		task_ = null;
	}
	
	public synchronized boolean isStarted() {
		return ( null != task_ );
	}
	
	private class TimerTask implements Runnable {
		public void run() {
			synchronized ( Timer.this ) {
				// Check whether task is still specified.
				if ( null == Timer.this.task_ )
					return;
				// Reoccur the task. 
				// It is done here to include task invocation time into 
				// specified interval between tasks.
				if ( Timer.this.repeat_ )
					Timer.this.executor_.runDelayed(this, Timer.this.interval_);
				// Execute task.
				Timer.this.task_.run();
				// Mark timer as completed and ready to handle new jobs.
				if ( !Timer.this.repeat_ ) 
					Timer.this.task_ = null;
			}
		}
	}
}
