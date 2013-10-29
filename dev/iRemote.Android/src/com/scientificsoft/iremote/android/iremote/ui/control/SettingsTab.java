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

import com.scientificsoft.iremote.android.iremote.R;
import com.scientificsoft.iremote.server.config.Config;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.CheckBox;
import android.widget.LinearLayout;

public class SettingsTab extends LinearLayout {
	
	private ISettingsListener listener_ = null;
	
	private CheckBox inversePanning_ = null;
	private CheckBox shellWrap_ = null;
	private CheckBox shellScroll_ = null;

	private View.OnClickListener inversePanningListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	Config.inversePanning_ = inversePanning_.isChecked();
	    	listener_.settingsChanged();
	    }
    };
    
	private View.OnClickListener shellWrapListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	Config.shellWrap_ = shellWrap_.isChecked();
	    	listener_.settingsChanged();
	    }
    };
    
	private View.OnClickListener shellScrollListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	Config.shellScroll_ = shellScroll_.isChecked();
	    	listener_.settingsChanged();
	    }
    };    
	
	public void loadSettings() {		
		inversePanning_.setChecked(Config.inversePanning_);
		shellWrap_.setChecked(Config.shellWrap_);
		shellScroll_.setChecked(Config.shellScroll_);
	}	
	
    /** Life cycle and UI initialization. */

	public SettingsTab(Context context) {
		super(context);
		configureUI(context);
		configureUI();
	}
	
	public SettingsTab(Context context, AttributeSet attrs) {
		super(context, attrs);
		configureUI(context);
		configureUI();
	}
	
	public void setListener(ISettingsListener listener) { 
		listener_ = listener;	
	}
	
	private void configureUI(Context context) {
		LayoutInflater factory = LayoutInflater.from(context);
        final View tabView = factory.inflate(R.layout.tab_settings, null);
        this.addView(tabView);
		// This actually allows to embed view from another activity. 
		// ViewGroup settingsTab = (ViewGroup)findViewById(R.id.tab_settings);
		// settingsTab.addView(getLocalActivityManager().startActivity("SettingsActivity",
        //    new Intent(this, SettingsActivity.class).addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP)).getDecorView());        
	}	
	
	private void configureUI() {
		inversePanning_ = (CheckBox)findViewById(R.id.settings_inverse_panning);
		inversePanning_.setOnClickListener(inversePanningListener_);		
		shellWrap_ = (CheckBox)findViewById(R.id.settings_shell_wrap);
		shellWrap_.setOnClickListener(shellWrapListener_);		
		shellScroll_ = (CheckBox)findViewById(R.id.settings_shell_scroll);
		shellScroll_.setOnClickListener(shellScrollListener_);
		loadSettings();
	}
}
