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

import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;

import com.scientificsoft.iremote.server.Messenger;
import com.scientificsoft.iremote.server.config.Config;

public class SimpleListener5 extends GestureDetector.SimpleOnGestureListener {
	private View owner_;
	
	private float vscrollLength_ = 0.0f;
    private float hscrollLength_ = 0.0f;       	   
	
	public SimpleListener5(View owner) {
		owner_ = owner;
	}
	
	public boolean onDoubleTap(MotionEvent e) {		
		return super.onDoubleTap(e);
	}
	public boolean onDoubleTapEvent(MotionEvent e) {
		return super.onDoubleTapEvent(e);
	}
	public boolean onDown(MotionEvent e) {
		return true;
	}
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
		return super.onFling(e1, e2, velocityX, velocityY);
	}
	public void onLongPress(MotionEvent e) {
		super.onLongPress(e);
	}
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
		int e2Pointers = e2.getPointerCount();
		if ( e2Pointers > 1 ) {
		    vscrollLength_ -= distanceY;
		    if ( Math.abs(vscrollLength_) > ControlAreaGL.scrollInterval_ ) {
		    	Messenger.scroll( true, ( vscrollLength_ < 0 ) ? ( 120 ) : ( -120 ) );
		    	vscrollLength_ -= ( vscrollLength_ / Math.abs(vscrollLength_) ) * ControlAreaGL.scrollInterval_;
		    }    		    
		    hscrollLength_ += distanceX;
		    if ( Math.abs(hscrollLength_) > ControlAreaGL.scrollInterval_ ) {
		    	Messenger.scroll( false, ( hscrollLength_ < 0 ) ? ( -120 ) : ( 120 ) );
		    	hscrollLength_ -= ( hscrollLength_ / Math.abs(hscrollLength_) ) * ControlAreaGL.scrollInterval_;
		    }
			/*
			// In perfect world actual distance should be sent to server for smother scrolling. 
			if ( 0.0f != distanceX ) {
				Messenger.scroll(false, (int)distanceX);
			}
			if ( 0.0f != distanceY ) {
				Messenger.scroll(true, (int)distanceY);
			} */  			
		} else {
    		// Simulate mouse move.
    		Messenger.moveCursor(
    			distanceX * ( Config.inversePanning_ ? 1.0f : -1.0f ), 
    			distanceY * ( Config.inversePanning_ ? 1.0f : -1.0f ));
		}
		return super.onScroll(e1, e2, distanceX, distanceY);
	}
	public void onShowPress(MotionEvent e) {
		super.onShowPress(e);
	}
	public boolean onSingleTapConfirmed(MotionEvent e) {
		return super.onSingleTapConfirmed(e);
	}
	public boolean onSingleTapUp(MotionEvent e) {
		// Simulate click.
		owner_.performClick();
		return super.onSingleTapUp(e);
	}
}
