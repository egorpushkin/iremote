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

import java.util.Vector;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.SeekBar;

import com.scientificsoft.iremote.android.iremote.R;
import com.scientificsoft.iremote.android.iremote.ui.controls.CompoundButton;
import com.scientificsoft.iremote.android.iremote.ui.controls.HotButton;
import com.scientificsoft.iremote.server.DUIHost;
import com.scientificsoft.iremote.server.Features;
import com.scientificsoft.iremote.server.Messenger;
import com.scientificsoft.iremote.server.messages.ScreenshotMessage;
import com.scientificsoft.iremote.server.messages.ShellResponse;
import com.scientificsoft.iremote.server.messages.VolumeMessage;
import com.scientificsoft.iremote.server.tools.KeyCodes;

public class HotsTab extends LinearLayout implements DUIHost {
	
	private ActionsContainer actions_ = null;
	
	private View clearSection_ = null;
	
	private SeekBar volumeControl_ = null; 
	
	/** Host section. */
	
	public void screenshot(ScreenshotMessage message) {
    }
    
    public void shellRespose(ShellResponse message) {    	
    }	
    
    public void volumeLevel(VolumeMessage message) {
    	try {
    		int volume = message.getProperty(VolumeMessage.PROP_VALUE).toInt();
    		volumeControl_.setProgress(volume);
	  	} catch ( Exception e ) {    		
	    }    	    	
    }	
    
    /** Volume handler. */
    
    private class SeekBarHandler implements SeekBar.OnSeekBarChangeListener {
	    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
	    	if ( fromUser ) {
	    		Messenger.sendVolume(progress);
	    	}
	    }
	    public void onStartTrackingTouch(SeekBar seekBar) {	    	
	    }
	    public void onStopTrackingTouch(SeekBar seekBar) {	    	
	    }
    }

	/** Handlers initialization. */
	
	protected void setupKeyHandlers() {
		setupHandler(R.id.hot_f1, KeyCodes.F1);
		setupHandler(R.id.hot_f2, KeyCodes.F2);
		setupHandler(R.id.hot_f3, KeyCodes.F3);
		setupHandler(R.id.hot_f4, KeyCodes.F4);
		setupHandler(R.id.hot_f5, KeyCodes.F5);
		setupHandler(R.id.hot_f6, KeyCodes.F6);
		setupHandler(R.id.hot_f7, KeyCodes.F7);
		setupHandler(R.id.hot_f8, KeyCodes.F8);
		setupHandler(R.id.hot_f9, KeyCodes.F9);
		setupHandler(R.id.hot_f10, KeyCodes.F10);
		setupHandler(R.id.hot_f11, KeyCodes.F11);
		setupHandler(R.id.hot_f12, KeyCodes.F12);		
		setupHandler(R.id.hot_esc, KeyCodes.ESCAPE);
		setupHandler(R.id.hot_tab, KeyCodes.TAB);
		setupHandler(R.id.hot_enter, KeyCodes.RETURN);
		setupHandler(R.id.hot_shift, KeyCodes.SHIFT);
		setupHandler(R.id.hot_ctrl, KeyCodes.CONTROL);		
		setupHandler(R.id.hot_alt, KeyCodes.MENU);
		setupHandler(R.id.hot_win, KeyCodes.LWIN);
		setupHandler(R.id.hot_popup, KeyCodes.APPS);
		setupHandler(R.id.hot_space, KeyCodes.SPACE);
		setupHandler(R.id.hot_ins, KeyCodes.INSERT);
		setupHandler(R.id.hot_del, KeyCodes.DELETE);
		setupHandler(R.id.hot_home, KeyCodes.HOME);
		setupHandler(R.id.hot_end, KeyCodes.END);		
		setupHandler(R.id.hot_pgup, KeyCodes.PRIOR);
		setupHandler(R.id.hot_pgdn, KeyCodes.NEXT);
		setupHandler(R.id.hot_left, KeyCodes.LEFT);
		setupHandler(R.id.hot_right, KeyCodes.RIGHT);
		setupHandler(R.id.hot_up, KeyCodes.UP);
		setupHandler(R.id.hot_down, KeyCodes.DOWN);
		setupHandler(R.id.hot_eject, KeyCodes.APPS);
	}
	
	/** Generic button touch handler. */
	
	protected void setupHandler(int id, byte vk) {
		try {
			View view = findViewById(id);
			if ( null != view ) {
				HotButton button = (HotButton)view;
				button.setOnButtonEventListener(new ButtonListener(vk));
			}
		} catch ( Exception e ) {
		}
	}
	
	private class ButtonListener implements CompoundButton.OnButtonEventListener {
		private byte vk_;
		private boolean longClick_;
		
		public ButtonListener(byte vk) {
			vk_ = vk;
			longClick_ = false;
		}		
		public void onDown(CompoundButton button) {
			longClick_ = false;
		}		
		public void onLong(CompoundButton button) {
			actions_.add(button, vk_);
			longClick_ = true;
		}		
		public void onUp(CompoundButton button) {
			actions_.add(button, vk_);
			if ( longClick_ ) {				
			} else {
				actions_.fire();
			}			
			longClick_ = false;
		}
	}
	
	/** Key combinations support. */
	
	protected void toggleClearSection() {
		boolean show = ( actions_.count() > 0 );
		clearSection_.setVisibility(show ? View.VISIBLE : View.GONE);
	}
	
	private View.OnClickListener clearListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	actions_.cancel();
	    }
    };   
    	
	
	private class ActionsContainer {
		private class Action {
			public CompoundButton button_;
			byte vk_;
		}		
		private Vector<Action> actions_ = new Vector<Action>();
		
		public void add(CompoundButton button, byte vk) {
			// Check for duplicates.
			for ( Action action : actions_ ) {
				if ( vk == action.vk_ )
					return;
			}
			// Add new action.
			Action action = new Action();
			action.button_ = button;
			action.vk_ = vk;
			actions_.addElement(action);
			toggleClearSection();
		}		
		public void fire() {
			for ( Action action : actions_ ) {
				Messenger.sendVirtualKey(action.vk_, true);
			}
			for ( Action action : actions_ ) {
				Messenger.sendVirtualKey(action.vk_, false);
			}
			cancel();
		}		
		public void cancel() {
			for ( Action action : actions_ ) {
				action.button_.setChecked(false);
			}			
			actions_.clear(); 
			toggleClearSection();
		}		
		public int count() {
			return actions_.size();
		}		
	}	
	
    /** Life cycle and UI initialization. */

	public HotsTab(Context context) {
		super(context);
		configureUI(context);
	}
	
	public HotsTab(Context context, AttributeSet attrs) {
		super(context, attrs);
		configureUI(context);
	}
	
	private void configureUI(Context context) {
		// Construct view.
        this.addView(loadContentView(context));
        // Setup key handlers.
        setupKeyHandlers();
        // Configure other controls (volume).        
		volumeControl_ = (SeekBar)findViewById(R.id.hot_volume);
		volumeControl_.setOnSeekBarChangeListener(new SeekBarHandler());
		// Initialize key combinations support.
		actions_ = new ActionsContainer();		
		clearSection_ = findViewById(R.id.hot_clear_section);
		View clearButton = findViewById(R.id.hot_clear);
		clearButton.setOnClickListener(clearListener_);
	}	
	
	private View loadContentView(Context context) {
		LayoutInflater factory = LayoutInflater.from(context);
		if ( Features.isHostMac() )
			return factory.inflate(R.layout.tab_hots_mac, null);
		else
			return factory.inflate(R.layout.tab_hots_win, null);
	}
}
