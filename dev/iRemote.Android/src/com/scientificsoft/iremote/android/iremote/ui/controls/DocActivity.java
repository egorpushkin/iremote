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

package com.scientificsoft.iremote.android.iremote.ui.controls;

import android.content.res.Configuration;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.webkit.WebView;
import android.widget.TextView;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;

import com.scientificsoft.iremote.android.iremote.R;
import com.scientificsoft.iremote.android.iremote.tools.activitymanager.ManagedActivity;
import com.scientificsoft.iremote.android.iremote.ui.home.HomeActivity;

public class DocActivity extends ManagedActivity {
	
	protected TextView title_;	
	protected WebView content_;
	
	private AdView ad_;
	
	public int getActivityGroupId() {
		return HomeActivity.GROUP_CONTROL;
	}
    
	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);        
        // Display page UI.
        setContentView(R.layout.doc_activity);
        // Find controls.
        title_ = (TextView)findViewById(R.id.navbar_title);
        content_ = (WebView)findViewById(R.id.doc_web_view);        
        content_.setScrollBarStyle(WebView.SCROLLBARS_OUTSIDE_OVERLAY);        
        ad_ = (AdView)findViewById(R.id.ad);
        // Start showing ads.
        AdRequest adRequest = new AdRequest.Builder().build();
        ad_.loadAd(adRequest);     
    }
    
    public void onDestroy() {
    	if ( null != ad_ ) {
    		ad_.destroy();
    	}
        super.onDestroy();
    }    
    
    /** Called when the activity is going into the background. */    
    @Override    
    public void onPause() {
        if ( null != ad_ ) {
        	ad_.pause();
        }    	
        super.onPause();
    }       
    
    /** Called when the activity is restored on the screen. */    
    @Override    
    public void onResume() {
        super.onResume();
        if ( null != ad_ ) {
        	ad_.resume();
        }        
    }    
    
    /** Called once(!), the first time the options menu is displayed. */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	getMenuInflater().inflate(R.menu.doc, menu);
    	return true;
    }    
    
    /** Called when options item is selected. */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	int itemId = item.getItemId();
    	switch ( itemId ) {
    	case R.id.menu_back:
    		this.finish();
    		break;
    	}    	
    	return true;
    } 
    
    /** Called when orientation is changed. */
    @Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		boolean showAds = true;
		if ( Configuration.ORIENTATION_LANDSCAPE == newConfig.orientation )
			showAds = false;
		View ad = findViewById(R.id.ad);
		ad.setVisibility(showAds ? View.VISIBLE : View.GONE);
	}    
}
