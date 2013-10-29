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

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Rect;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.MotionEvent;

import com.scientificsoft.iremote.android.iremote.R;
import com.scientificsoft.iremote.android.iremote.tools.ApiFeatures;
import com.scientificsoft.iremote.android.iremote.ui.controls.opengl.Texture;
import com.scientificsoft.iremote.server.Messenger;
import com.scientificsoft.iremote.server.messages.ScreenshotMessage;

public class ControlAreaGL extends GLSurfaceView implements IScreenshotHost {
	
	private static final int SCREENSHOT_TIMER_INTERVAL = 2000;
	private static final int SCREENSHOT_LIFE_TIME = 3000;
	
    public static final float scrollInterval_ = 10.0f;        	
	public static final float factorDelta_ = 0.002f;       	

	private AreaRenderer renderer_ = new AreaRenderer();
	
	private GestureDetector gestureDetector_ = null;
	
	private ScaleGestureDetector scaleDetector_ = null;
	
	private boolean scaleHandled_ = false;
	
	private Handler screenshotTimer_ = new Handler();
	
	private ScreenshotTimerCallback screenshotTimerCallback_ = new ScreenshotTimerCallback();
		
	public ControlAreaGL(Context context) {
		super(context);
		setupRenderer(); 
		setupGestureListeners(context);
	}
	
	public ControlAreaGL(Context context, AttributeSet attrs) {
		super(context, attrs);
		setupRenderer();
		setupGestureListeners(context);
	}
	
	protected void setupRenderer() {
		setEGLConfigChooser(false);
		setRenderer(renderer_);
		setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY); 		
	}
	
	protected void setupGestureListeners(Context context) {
		gestureDetector_ = new GestureDetector(context, ApiFeatures.createGestureListener(this));
		scaleDetector_ = ApiFeatures.createScaleDetector(context, new ScaleListener());
	}
	
	public void start() {
		onResume();		
		screenshotTimerCallback_.run();
	}
	
	public void stop() {
		onPause();
		screenshotTimer_.removeCallbacks(screenshotTimerCallback_);
	}	
	
    public void onResume() { 
        super.onResume(); 
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY); 
        requestRender(); 
    } 
    
    public void onPause() { 
        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY); 
        android.os.SystemClock.sleep(60); 
        super.onPause(); 
    } 	
	
	public void setScreenshot(final ScreenshotMessage scrMessage) {
		// Notify renderer directly. QueueEvent does not work along with
		// RENDERMODE_WHEN_DIRTY mode.
    	renderer_.setScreenshot(scrMessage);
		requestRender();            	
	}		
	
	/**
	 * Called, when user touches the area. 
	 */
	public boolean onTouchEvent(MotionEvent event) {
		if ( null != scaleDetector_ ) {
			scaleHandled_ = false;
			scaleDetector_.onTouchEvent(event);
			if ( scaleHandled_ )
				return true;
		}
		if ( null != gestureDetector_ ) {
			return gestureDetector_.onTouchEvent(event);
		}
		return super.onTouchEvent(event);
	}
	
	private class ScreenshotTimerCallback implements Runnable {
		public void run() {
			renderer_.testScreenshot();
			screenshotTimer_.postDelayed(screenshotTimerCallback_, SCREENSHOT_TIMER_INTERVAL);
		}
	}
	
    private class AreaRenderer implements GLSurfaceView.Renderer {
    	
    	private Object screenshotMutex_ = new Object();
    	
    	/** This object is shared between server and renderer. */
    	private Bitmap rawScreenshot_ = null;
    	
    	private Texture screenshot_ = null;
    	
    	private Texture background_ = null;
    	
    	private Texture mouseCursor_ = null;
    	
    	private long screenshotTS_ = -1;
    	
    	private boolean recycleScreenshot_ = false;
    	
    	public void setScreenshot(ScreenshotMessage scrMessage) {
    		Bitmap rawScreenshot = BitmapFactory.decodeByteArray(scrMessage.data(), 0, scrMessage.dataSize());
    		synchronized ( screenshotMutex_ ) {
    			rawScreenshot_ = rawScreenshot;
    		}
    	}
    	
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        	BitmapFactory.Options loadOptions = new BitmapFactory.Options();
        	loadOptions.inScaled = false;
        	Bitmap backgroud = BitmapFactory.decodeResource(ControlAreaGL.this.getResources(), R.drawable.touchpad_background, loadOptions);
        	background_ = new Texture(gl, backgroud);
        	Bitmap cursor = BitmapFactory.decodeResource(ControlAreaGL.this.getResources(), R.drawable.mouse_cursor, loadOptions);
        	mouseCursor_ = new Texture(gl, cursor);        	
        }

        public void onSurfaceChanged(GL10 gl, int w, int h) {
            gl.glEnable(GL10.GL_BLEND);
            gl.glBlendFunc(GL10.GL_SRC_ALPHA, GL10.GL_ONE_MINUS_SRC_ALPHA);
        	
        	gl.glViewport(0, 0, w, h);
            
            gl.glMatrixMode(GL10.GL_PROJECTION);
            gl.glLoadIdentity();
            gl.glOrthof(0.0f, w, h, 0.0f, -1.0f, 1.0f);
            gl.glMatrixMode(GL10.GL_MODELVIEW);
            gl.glLoadIdentity();                
        }      
        
        public void testScreenshot() {
        	long currentTS = System.currentTimeMillis();
        	if ( ( currentTS - screenshotTS_ ) > SCREENSHOT_LIFE_TIME ) {
        		recycleScreenshot_ = true;
        		requestRender();
        	}
        }

        public void onDrawFrame(GL10 gl) {
        	// Update screenshot, if necessary.
        	Bitmap rawScreenshot = null;
        	synchronized ( screenshotMutex_ ) {
        		rawScreenshot = rawScreenshot_;
        		rawScreenshot_ = null;
        	}
        	if ( null != rawScreenshot ) {
	    		if ( null != screenshot_ ) {
	    			screenshot_.release(gl);
	    		}
	    		screenshot_ = new Texture(gl, rawScreenshot);
	    		screenshotTS_ = System.currentTimeMillis();
	    		rawScreenshot.recycle();
        	}      
        	if ( recycleScreenshot_ ) {
        		recycleScreenshot_ = false;
        		if ( null != screenshot_ ) {
	        		screenshot_.release(gl);
	        		screenshot_ = null;
        		}
        	}
        	
        	// Configure scene. This might be omitted in current implementation,
        	// as rendering itself is primitive.
            // gl.glMatrixMode(GL10.GL_MODELVIEW);
            // gl.glLoadIdentity();                            

        	// Draw screenshot or regular background.
        	int canvasWidth = ControlAreaGL.this.getWidth();
        	int canvasHeight = ControlAreaGL.this.getHeight();        	
            if ( null != screenshot_ ) {
                Rect dest;
            	if ( canvasWidth > canvasHeight ) {
            		// Rectangle for landscape view.
            		int hoffset = ( canvasHeight - canvasWidth ) >> 1;    
            		dest = new Rect(0, hoffset, canvasWidth, canvasHeight - hoffset);
            	} else {
            		// Rectangle for portrait view.
            		int woffset = ( canvasWidth - canvasHeight ) >> 1;    
            		dest = new Rect(woffset, 0, canvasWidth - woffset, canvasHeight);
            	}            
            	screenshot_.draw(gl, dest);
            	// Show mouse cursor.
            	int cursorWidth = mouseCursor_.width();
            	int cursorHeight = mouseCursor_.height();
            	int cursorX = ( canvasWidth - cursorWidth ) >> 1;
            	int cursorY = ( canvasHeight - cursorHeight ) >> 1;
            	dest = new Rect(cursorX, cursorY, cursorX + cursorWidth, cursorY + cursorHeight);
            	mouseCursor_.draw(gl, dest);
            } else {
            	Rect dest = new Rect(0, 0, canvasWidth, canvasHeight);
            	background_.draw(gl, dest);
            }
        }
    }           
    
    private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {      
        public boolean onScale(ScaleGestureDetector detector) {
        	float factor = detector.getScaleFactor();
        	if ( Math.abs( 1.0f - factor ) > factorDelta_ ) {
        		Messenger.zoom(1.0f / factor);
        		scaleHandled_ = true;        		
        		return true;
        	}
            return false;
        }
    }
}
