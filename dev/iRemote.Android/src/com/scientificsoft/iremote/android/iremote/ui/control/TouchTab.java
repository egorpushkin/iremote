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

import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.LinearLayout;

import com.scientificsoft.iremote.android.iremote.R;
import com.scientificsoft.iremote.android.iremote.ui.controls.CompoundButton;
import com.scientificsoft.iremote.android.iremote.ui.controls.MouseButton;
import com.scientificsoft.iremote.server.DUIHost;
import com.scientificsoft.iremote.server.Messenger;
import com.scientificsoft.iremote.server.messages.Messages;
import com.scientificsoft.iremote.server.messages.ScreenshotMessage;
import com.scientificsoft.iremote.server.messages.ShellResponse;
import com.scientificsoft.iremote.server.messages.VolumeMessage;
import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.server.tools.KeyCodes;

public class TouchTab extends LinearLayout implements DUIHost {
	
	private MouseButton mouseLeft_;
	private MouseButton mouseMiddle_;
	private MouseButton mouseRight_;
	
	/** UI Callbacks. */
	
	private View.OnClickListener zoomInListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	Messenger.zoomIn();
	    }
    };
	
	private View.OnClickListener zoomOutListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	Messenger.zoomOut();
	    }
    };
	
	private View.OnClickListener flushZoomListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	Messenger.flushZoom();
	    }
    };   
    
    protected void skipCheckedState() {
    	if ( mouseLeft_.isChecked() ) {
    		mouseLeft_.setChecked(false);    		
    		CompoundButton.OnButtonEventListener listener = mouseLeft_.getOnButtonEventListener();
    		if ( null != listener )
    			listener.onUp(mouseLeft_);
    	}
    	if ( mouseMiddle_.isChecked() ) {
    		mouseMiddle_.setChecked(false);    		
    		CompoundButton.OnButtonEventListener listener = mouseMiddle_.getOnButtonEventListener();
    		if ( null != listener )
    			listener.onUp(mouseMiddle_);
    	}
    	if ( mouseRight_.isChecked() ) {
    		mouseRight_.setChecked(false);    		
    		CompoundButton.OnButtonEventListener listener = mouseRight_.getOnButtonEventListener();
    		if ( null != listener )
    			listener.onUp(mouseRight_);
    	}
    }
    
    private class MouseButtonListener implements CompoundButton.OnButtonEventListener {
    	private Guid down_;
    	private Guid up_;
    	
    	public MouseButtonListener(Guid down, Guid up) {    	
    		down_ = down;
    		up_ = up;
    	}    	
		public void onDown(CompoundButton button) {
			if ( button.isChecked() ) {
			} else {
				Messenger.sendMessage(down_);
			}
			skipCheckedState();
		}		
		public void onLong(CompoundButton button) {
		}		
		public void onUp(CompoundButton button) {
			if ( button.isChecked() ) {				
			} else {
				Messenger.sendMessage(up_);	  
			}
		}
    };
    
	private View.OnClickListener controlClickListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
			Messenger.sendMessage(Messages.leftButtonDown_);
			Messenger.sendMessage(Messages.leftButtonUp_);   
			skipCheckedState();
	    }
	};
    
	private View.OnClickListener toggleKeyboardListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	// Toggle keyboard.
	    	InputMethodManager inputMgr =     		
	    		(InputMethodManager)getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
	    	inputMgr.toggleSoftInput(0, 0);
	    }
	};	
	
	/** Event handlers. */
	
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
    	// Handle delete key separately from all other keyboard events.
    	if ( KeyEvent.KEYCODE_DEL == keyCode ) {
    		Messenger.sendVirtualKey(KeyCodes.BACKSPACE, true);
    		Messenger.sendVirtualKey(KeyCodes.BACKSPACE, false);
    	} else {
    		// Send only events which produce glyph, space or new line.  
    		if ( event.isPrintingKey() 
    			|| ( KeyEvent.KEYCODE_SPACE == keyCode ) 
    			|| ( KeyEvent.KEYCODE_ENTER == keyCode ) ) {
		    	Messenger.sendUnicodeKey((short)event.getUnicodeChar());
    		}
    	}
        return super.onKeyUp(keyCode, event);
    }    	
	
	/** DHost section. */

	public void screenshot(ScreenshotMessage message) {
	}
    
    public void shellRespose(ShellResponse message) {    	
    }	
    
    public void volumeLevel(VolumeMessage message) {    	
    }
	
    /** Life cycle and UI initialization. */

	public TouchTab(Context context) {
		super(context);
		configureUI(context);
	}
	
	public TouchTab(Context context, AttributeSet attrs) {
		super(context, attrs);
		configureUI(context);
	}
	
	private void configureUI(Context context) {
		// Load shell tab view.
		LayoutInflater factory = LayoutInflater.from(context);
        final View tabView = factory.inflate(R.layout.tab_touchpad, null);
        this.addView(tabView);	
        
        // Setup callbacks and handlers.
        Button zoomIn = (Button)this.findViewById(R.id.zoom_in);
        zoomIn.setOnClickListener(zoomInListener_);     
        Button zoomOut = (Button)this.findViewById(R.id.zoom_out);
        zoomOut.setOnClickListener(zoomOutListener_); 
        Button flushZoom = (Button)this.findViewById(R.id.flush_zoom);
        flushZoom.setOnClickListener(flushZoomListener_);
        Button toggleKeyboard = (Button)this.findViewById(R.id.toggle_keyboard);
        toggleKeyboard.setOnClickListener(toggleKeyboardListener_);     
        View controlView = this.findViewById(R.id.screenshot_view);
        controlView.setOnClickListener(controlClickListener_);
        
        // Setup and remember mouse buttons. 
        mouseLeft_ = (MouseButton)this.findViewById(R.id.mouse_left);
        mouseLeft_.setOnButtonEventListener(new MouseButtonListener(
        	Messages.leftButtonDown_, Messages.leftButtonUp_));
        mouseLeft_.setChecked(false);               
        mouseMiddle_ = (MouseButton)this.findViewById(R.id.mouse_middle);
        mouseMiddle_.setOnButtonEventListener(new MouseButtonListener(
            Messages.middleButtonDown_, Messages.middleButtonUp_));        
        mouseMiddle_.setChecked(false);   
        mouseRight_ = (MouseButton)this.findViewById(R.id.mouse_right);
        mouseRight_.setOnButtonEventListener(new MouseButtonListener(
            Messages.rightButtonDown_, Messages.rightButtonUp_));        
        mouseRight_.setChecked(false);   
        
        // Specific alpha for buttons over touch pad.
        zoomIn.getBackground().setAlpha(100);
        zoomOut.getBackground().setAlpha(100);
        flushZoom.getBackground().setAlpha(100);
        toggleKeyboard.getBackground().setAlpha(100);        
	}	
}
