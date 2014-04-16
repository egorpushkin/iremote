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

package com.scientificsoft.iremote.android.iremote.ui.controls.opengl;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.opengles.GL10;

import android.graphics.Bitmap;
import android.graphics.Rect;
import android.opengl.GLUtils;

public class Texture {
	
	private int id_;
	
	private int width_;
	
	private int height_;
	
	public Texture(GL10 gl, Bitmap image) {
        int[] tmp_tex = new int[1];

        gl.glGenTextures(1, tmp_tex, 0);
        id_ = tmp_tex[0];
        loadTexture(GL10.GL_TEXTURE_2D, image, gl);
	}
	
	public void release(GL10 gl) {
		int[] tmp_tex = new int[1];
		tmp_tex[0] = id_;
		gl.glDeleteTextures(1, tmp_tex, 0);
	}
	
	public void loadTexture(int type, Bitmap bmp, GL10 gl) {
		width_ = bmp.getWidth();
		height_ = bmp.getHeight();
        loadTexture(type, width_, height_, bmp, gl);
	}
	
	public void loadTexture(int type, int width, int height, Bitmap bmp, GL10 gl) {
		gl.glEnable(type);
        gl.glBindTexture(type, id_);	        
        GLUtils.texImage2D(type, 0, bmp, 0);
		gl.glTexEnvf(GL10.GL_TEXTURE_ENV, GL10.GL_TEXTURE_ENV_MODE, GL10.GL_MODULATE);			
        gl.glTexParameterf(type, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_LINEAR);
        gl.glTexParameterf(type, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);
	}    	    
	
	public int width() {
		return width_;
	}
	
	public int height() {
		return height_;
	}
	
    protected FloatBuffer makeFloatBuffer(float[] arr) {
        ByteBuffer bb = ByteBuffer.allocateDirect(arr.length*4);
        bb.order(ByteOrder.nativeOrder());
        FloatBuffer fb = bb.asFloatBuffer();
        fb.put(arr);
        fb.position(0);
        return fb;
    }        
    
    public void draw(GL10 gl, Rect dest) {
        float coordinates[] = new float[] { 
                0.0f,	0.0f,
                1.0f,   0.0f, 
                0.0f,	1.0f,
                1.0f,	1.0f };
        
        float vertices[] = new float[] {	
        	dest.left,	dest.top,						
            dest.right,	dest.top,						
            dest.left,	dest.bottom,	
            dest.right,	dest.bottom };
        
        gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
    	gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
    	gl.glEnable(GL10.GL_TEXTURE_2D);                              
        gl.glActiveTexture(GL10.GL_TEXTURE0);
        gl.glColor4f(1.0f, 1.0f, 1.0f, 1.0f);        
        
        gl.glBindTexture(GL10.GL_TEXTURE_2D, id_);
        gl.glVertexPointer(2, GL10.GL_FLOAT, 0, makeFloatBuffer(vertices));
        gl.glTexCoordPointer(2, GL10.GL_FLOAT, 0, makeFloatBuffer(coordinates));
        gl.glDrawArrays(GL10.GL_TRIANGLE_STRIP, 0, 4);
    }
}