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

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;

public class CompoundButton extends Button {
	
	public OnButtonEventListener listener_ = null;
	
	private boolean checked_ = false;
	
	public CompoundButton(Context context) {
		super(context);
        setOnTouchListener(touchListener_);         
        setOnLongClickListener(longClickListener_);        
	}
	
	public CompoundButton(Context context, AttributeSet attrs) {
		super(context, attrs);
        setOnTouchListener(touchListener_);         
        setOnLongClickListener(longClickListener_);        		
	}	
	
	public boolean isChecked() {
		return checked_;
	}
	
	public void setChecked(boolean checked) {
		checked_ = checked;
	}
	
	public void setOnButtonEventListener(OnButtonEventListener listener) {
		listener_ = listener;
	}	
	
	public OnButtonEventListener getOnButtonEventListener() {
		return listener_;
	}
	
	private View.OnTouchListener touchListener_ = new View.OnTouchListener() {
	    public boolean onTouch(View v, MotionEvent event) {
	    	if ( MotionEvent.ACTION_DOWN == event.getAction() ) {
	    		if ( null != listener_ ) {
	    			listener_.onDown(CompoundButton.this);
	    		}
    			if ( isChecked() ) {
    				setChecked(false);
    			}
	    	} else if ( MotionEvent.ACTION_UP == event.getAction() ) {
	    		if ( null != listener_ ) {
	    			listener_.onUp(CompoundButton.this);
	    		}	    			    		
	    	}
	    	return false;
	    }
    };
    
	private View.OnLongClickListener longClickListener_ = new View.OnLongClickListener() {
		public boolean onLongClick (View v) {
			setChecked(true);
    		if ( null != listener_ ) {
    			listener_.onLong(CompoundButton.this);
    		}
			return false;
		}
	};	
	
	public interface OnButtonEventListener {
		abstract public void onDown(CompoundButton button);
		abstract public void onLong(CompoundButton button);
		abstract public void onUp(CompoundButton button);
	}

}
