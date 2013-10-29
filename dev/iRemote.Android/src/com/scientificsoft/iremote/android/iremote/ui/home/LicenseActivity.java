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

package com.scientificsoft.iremote.android.iremote.ui.home;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.webkit.WebView;

import com.scientificsoft.iremote.android.iremote.R;

public class LicenseActivity extends Activity {
	
	protected void acceptLicense() {
		this.setResult(RESULT_OK);
		this.finish();
	}

	protected void declineLicense() {		
		this.finish();
	}
	
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);        
    	// Hide window title.
    	requestWindowFeature(Window.FEATURE_NO_TITLE);
        // Display page UI.
        setContentView(R.layout.license_activity);      
        WebView content = (WebView)findViewById(R.id.doc_web_view);
        content.setScrollBarStyle(WebView.SCROLLBARS_OUTSIDE_OVERLAY);        
        content.loadUrl("file:///android_asset/License.htm");
    	// Setup button handlers.
    	findViewById(R.id.accept).setOnClickListener(acceptListener_);
    	findViewById(R.id.decline).setOnClickListener(declineListener_);        
    }
    
    /** Called once(!), the first time the options menu is displayed. */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	getMenuInflater().inflate(R.menu.license, menu);
    	return true;
    }    
    
    /** Called when options item is selected. */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	int itemId = item.getItemId();
    	switch ( itemId ) {
    	case R.id.menu_accept:
    		acceptLicense();
    		break;
    	case R.id.menu_decline:
    		declineLicense();
    		break;
    	}    	
    	return true;
    }          
    
	private View.OnClickListener acceptListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	acceptLicense();
	    }
    };
    
	private View.OnClickListener declineListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	declineLicense();
	    }
    };
}