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
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.widget.RadioButton;

public class TabBarButton extends RadioButton {
	
	private int drawableId_ = -1;
	
	private String text_ = null;
	
	private final int fontSize_ = 10;  
	private final int padding_ = 5;
	
	private float deviceFontSize_;  
	private int devicePadding_;	

	public TabBarButton(Context context) {
		super(context);
		calculateDimensions();
	}
	
	public TabBarButton(Context context, AttributeSet attrs) {
		super(context, attrs);
		calculateDimensions();
	}	
	
	private void calculateDimensions() {
		final float density = getContext().getResources().getDisplayMetrics().density;
		deviceFontSize_ = ((float)fontSize_) * density;
		devicePadding_ = (int)(((float)padding_) * density);		
	}
	
	public void setButtonDrawable(int resid) {
		super.setButtonDrawable(resid);
		drawableId_ = resid;
		Bitmap icon = BitmapFactory.decodeResource(getResources(), drawableId_);
		setHeight(icon.getHeight() + (int)deviceFontSize_ + devicePadding_);
		text_ = getText().toString();
		setText(null);
	}
	
	/** 
	 * Called when view content should be drawn. 
	 */
    @Override
    protected void onDraw(Canvas canvas) {
    	if ( -1 == drawableId_ ) 
    		return;

    	Bitmap icon = BitmapFactory.decodeResource(getResources(), drawableId_);
    	int i = ( this.getWidth() - icon.getWidth() ) >> 1;
    	Rect dst = new Rect(i, devicePadding_, i + icon.getWidth(), icon.getHeight());
    	canvas.drawBitmap(icon, null, dst, null);
    	
    	Paint textPainter = new Paint();
    	textPainter.setColor(Color.WHITE);
    	textPainter.setTextSize(deviceFontSize_);
    	
    	float [] textWidth = new float[1];
    	textPainter.breakText(text_, true, 1024.0f, textWidth);
    	
		canvas.drawText(text_, ( this.getWidth() - textWidth[0] ) / 2, icon.getHeight() + ( devicePadding_ * 2 ), textPainter);
    }
	
}



