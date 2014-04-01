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

package com.scientificsoft.iremote.android.iremote;

import android.app.Application;
import com.scientificsoft.iremote.android.iremote.tools.ContextHolder;
import com.scientificsoft.iremote.android.iremote.tools.CrashReporter;
import com.scientificsoft.iremote.android.iremote.tools.ForegroundExecutor;
import com.scientificsoft.iremote.server.Holder;
import com.scientificsoft.iremote.server.config.Config;

/**
 * Class to share global application context and to provide entry and exit
 * points for the application instance.
 */
public class iRemoteApp extends Application {
	
	private ForegroundExecutor foregroundExecutor_ = null;

	public iRemoteApp() {
	}
	
	/**
	 * Good place to load configs and setup global exception handler.
	 */
	public void onCreate() {
		super.onCreate();
		
		// Setup exception handler for the entire application.
		CrashReporter.setup(this);
		CrashReporter.initialize(this);
		
		// Remember application context. 
		ContextHolder.instance().setContext(this);

        // Initialize foreground executor. 
        foregroundExecutor_ = new ForegroundExecutor();
        // Configure server component with foreground executor. 
        Holder.instance().configureExecutor(foregroundExecutor_);
        
        // Load config.
        Config.load();
	}
}
