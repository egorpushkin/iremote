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

package com.scientificsoft.iremote.android.iremote.ui.control;

import com.scientificsoft.iremote.server.DSession;
import com.scientificsoft.iremote.server.DUIHost;
import com.scientificsoft.iremote.server.Holder;
import com.scientificsoft.iremote.server.messages.ScreenshotMessage;
import com.scientificsoft.iremote.server.messages.ShellResponse;
import com.scientificsoft.iremote.server.messages.VolumeMessage;
import com.scientificsoft.iremote.android.iremote.tools.ContextHolder;
import com.scientificsoft.iremote.android.iremote.tools.activitymanager.ManagedTabActivity;
import com.scientificsoft.iremote.android.iremote.ui.controls.IScreenshotHost;
import com.scientificsoft.iremote.android.iremote.ui.home.HelpActivity;
import com.scientificsoft.iremote.android.iremote.ui.home.HomeActivity;
import com.scientificsoft.iremote.android.iremote.R;

import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TabHost;
import android.widget.TabHost.TabSpec;
import android.widget.TextView;

public class ControlActivity extends ManagedTabActivity implements DUIHost, DSession, ISettingsListener {
	
	/** Navigation bar title. */
	private TextView navBarTitle_ = null;
	
	/** Control tab bar. */
	private View tabbar_ = null;
	
	/** TouchPad tab controller. */
	private TouchTab touchTab_ = null;
	
	/** Shell tab controller. */
	private ShellTab shellTab_ = null;
	
	/** Hot keys tab controller. */
	private HotsTab hotsTab_ = null;
	
	/** Settings tab controller. */
	private SettingsTab settingsTab_ = null;	
	
	/** Device, which is currently configured to display screenshots. */
	private IScreenshotHost screenshotHost_ = null;
	
	/** Field, saved for optimization considerations. */
	private IScreenshotHost screenshotHostCached_ = null;	
	
	public static final int TAB_TOUCHPAD = 0;
	public static final int TAB_SHELL = 1;
	public static final int TAB_HOTS = 2;
	public static final int TAB_SETTINGS = 3;
	
    /** Activity section. */
	
	public int getActivityGroupId() {
		return HomeActivity.GROUP_CONTROL;
	}
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);        
        try {        
	        // Display activity content.
        	setContentView(R.layout.control_activity);      	
        	// Cache and initialize controls.
    		configureUI();
        	// Add and configure tab bar.
        	setupTabs();
	        
	        // Subscribe on session events.
	        Holder.instance().configureSession(this);
	        // Subscribe on host events.
	        Holder.instance().configureHost(this);
        } catch ( Exception e ) {
        	// Failed to initialize activity.
        	// Return to main screen and make sure that connection to server is terminated.
    		// Disconnect from server. 
        	onDisconnect();
        }
    }	
    
    /** Called when the activity is going into the background. */    
    @Override    
    public void onPause() {
        super.onPause();
        // Stop rendering.
        if ( null != screenshotHost_ ) {
        	screenshotHost_.stop();
        	screenshotHost_ = null;        	
        }
    }       
    
    /** Called when the activity is restored on the screen. */    
    @Override    
    public void onResume() {
        super.onResume();
        // Subscribe on session events.
        Holder.instance().configureSession(this);
        // Start rendering.
		if ( null == screenshotHost_ ) {
    		screenshotHost_ = screenshotHostCached_;
    		screenshotHost_.start();
		}       
    }   
		
    /** Called when device orientation changes. */
    @Override        
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		updateOrientation();
	}    
    
    /** Called once(!), the first time the options menu is displayed. */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	MenuInflater inflater = getMenuInflater();
    	inflater.inflate(R.menu.control, menu);
    	return true;
    }
    
    /** Called when options item is selected. */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	int itemId = item.getItemId();
    	switch ( itemId ) {
    	case R.id.menu_help:
        	showHelp();
        	break;    	
    	case R.id.menu_disconnect:
    		// Disconnect from server. 
    		onDisconnect();
    		break;
    	}
    	return true;
    }
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK && event.getRepeatCount() == 0) {
        	// Disconnect from server. 
        	onDisconnect();
        }
        return super.onKeyDown(keyCode, event);
    }    
    
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
    	int tab = getTabHost().getCurrentTab();
    	if ( TAB_TOUCHPAD == tab ) {
    		touchTab_.onKeyUp(keyCode, event);
    	} if ( TAB_SHELL == tab ) {
    		shellTab_.onKeyUp(keyCode, event);
    	}
        return super.onKeyUp(keyCode, event);
    }    
    
    /**
     * Called when another tab is selected. 
     * The goal of this tool is to configure hosts for screenshots, etc. 
     */
    public void tabChanged(int tabIndex) {
    	if ( TAB_TOUCHPAD == tabIndex ) {
    		if ( null == screenshotHost_ ) {
	    		screenshotHost_ = screenshotHostCached_;
	    		screenshotHost_.start();
    		}
    	} else {
    		if ( null != screenshotHost_ ) {
    			screenshotHost_.stop();
    			screenshotHost_ = null;
    		}
    	}
    }
    
    public void showHelp() {
    	startActivity(new Intent(this, HelpActivity.class));
    }
    
    public void onDisconnect() {
    	// Disconnect from server. 
		Holder.instance().disconnect();    	
		// Kill the entire control group.
		killActivitiesHierarchy();
    }
    
    public void killActivitiesHierarchy() {
    	ContextHolder.instance().activityManager().killGroup(HomeActivity.GROUP_CONTROL);
    }
    
    /** Host section. */
    
    /** Called when next screenshot arrives from server. */
	public void screenshot(ScreenshotMessage message) {
    	try {
    		if ( null == screenshotHost_ )
    			return;
    		screenshotHost_.setScreenshot(message);
    	} catch ( Exception e ) {    		
    	}
    }
    
    public void shellRespose(ShellResponse message) {    	
		shellTab_.shellRespose(message);
    }
    
    public void volumeLevel(VolumeMessage message) {
		hotsTab_.volumeLevel(message);
    }
    
    /** DSession section. */

	public void connecting() {
		// Should not occur on authorized environment.
	}    
    
	public void connected() {
		// Should not occur on authorized environment.
	}
	
	public void requestPassword() {
		// Should not occur on authorized environment.
	}
	
	public void approved() {
		// Should not occur on authorized environment.
	}
	
	public void disconnected(int reason) {
		// Kill the entire control group.
		killActivitiesHierarchy();
	}
	
	public void passwordRequested(boolean wrong) {
		// Should not occur on authorized environment.
	}
	
	/** ISettingsListener section. */ 
	
	public void settingsChanged() {
		shellTab_.settingsChanged();
	}
	
	/** UI tools section. */
	
	private void configureUI() {
		screenshotHostCached_ = (IScreenshotHost)this.findViewById(R.id.screenshot_view);
	}
	    
    private void setupTabs() {
    	// Locate global resources.
    	navBarTitle_ = (TextView)findViewById(R.id.navbar_title);
		tabbar_ = findViewById(R.id.tabbar);
        // Find tabs control.
    	TabHost tabs = getTabHost();      	
    	// Add tabs.
    	addTab(tabs, "tab_touchpad", R.string.touch_pad, R.id.tab_touch_pad);
    	addTab(tabs, "tab_shell", R.string.shell, R.id.tab_shell);
    	addTab(tabs, "tab_hots", R.string.hots, R.id.tab_hots);
    	addTab(tabs, "tab_settings", R.string.settings, R.id.tab_settings);
    	// Configure tabbar.
        tabs.setCurrentTab(0);
        // Find tabbar controllers.
        touchTab_ = (TouchTab)findViewById(R.id.tab_touchpad);
        shellTab_ = (ShellTab)findViewById(R.id.tab_shell);
        hotsTab_ = (HotsTab)findViewById(R.id.tab_hots);
        settingsTab_ = (SettingsTab)findViewById(R.id.tab_settings);
        settingsTab_.setListener(this);
		// Find buttons.
		RadioButton rbFirst = (RadioButton) findViewById(R.id.tabitem_first);
		RadioButton rbSecond = (RadioButton) findViewById(R.id.tabitem_second);
		RadioButton rbThird = (RadioButton) findViewById(R.id.tabitem_third);
		RadioButton rbFourth = (RadioButton) findViewById(R.id.tabitem_fourth);
		// Apply test and icons.
		rbFirst.setText(getResources().getString(R.string.touch_pad));
		rbFirst.setButtonDrawable(R.drawable.tab_touchpad);
		rbSecond.setText(getResources().getString(R.string.shell));
		rbSecond.setButtonDrawable(R.drawable.tab_shell);
		rbThird.setText(getResources().getString(R.string.hots));
		rbThird.setButtonDrawable(R.drawable.tab_hots);
		rbFourth.setText(getResources().getString(R.string.settings));
		rbFourth.setButtonDrawable(R.drawable.tab_settings);
		// Setup change handler.
		RadioGroup tabBar = (RadioGroup)findViewById(R.id.tabbar);
		tabBar.setOnCheckedChangeListener(new TabChangeListener(tabBar));
		// Initialize tabbar visibility.
		updateOrientation();
    }
    
    private void updateOrientation() {
    	boolean showTabbar = true;
		if ( Configuration.ORIENTATION_LANDSCAPE == getResources().getConfiguration().orientation )
			showTabbar = false;
		tabbar_.setVisibility(showTabbar ? View.VISIBLE : View.GONE);    	
    }
    
    private void addTab(TabHost tabs, String tabId, int name, final int view) {
        TabSpec tabSpec = tabs.newTabSpec(tabId);
        tabSpec.setIndicator(getResources().getString(name));
        tabSpec.setContent(view);
        tabs.addTab(tabSpec);
    }
    
	private class TabChangeListener implements RadioGroup.OnCheckedChangeListener {
		public TabChangeListener(RadioGroup group) {
			onCheckedChanged(group, R.id.tabitem_first);
		}
		private void applyTabSelection(int tabIndex, int titleId, String tabId) {
			getTabHost().setCurrentTab(tabIndex);
			navBarTitle_.setText(getResources().getString(titleId));
			tabChanged(tabIndex);
	    }		
		public void onCheckedChanged(RadioGroup group, final int checkedId) {
			switch (checkedId) {
			case R.id.tabitem_first:
				applyTabSelection(TAB_TOUCHPAD, R.string.touch_pad, "tab_touchpad");
				break;
			case R.id.tabitem_second:
				applyTabSelection(TAB_SHELL, R.string.shell, "tab_shell");
				break;
			case R.id.tabitem_third:
				applyTabSelection(TAB_HOTS, R.string.hots, "tab_hots");
				break;
			case R.id.tabitem_fourth:
				applyTabSelection(TAB_SETTINGS, R.string.settings, "tab_settings");
				break;					
			}
		}			
	}    
}
