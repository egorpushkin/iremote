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

package com.scientificsoft.iremote.server;

public class Runtime {
	
	public static float zoomInOutCoefficient_ = 1.2f;  
	
	private float minZoom_ = 1.0f;
	
	private float maxZoom_ = 1.0f;
	
	private float currentZoom_ = 1.0f;
	
	public void zoomParams(float min, float max, float current) {
		minZoom_ = min;
		maxZoom_ = max;
		currentZoom_ = current;
		adjustZoom();
	}
	
	public float zoomIn() {
		currentZoom_ /= zoomInOutCoefficient_;
		adjustZoom();
		return currentZoom_;
	}
	
	public float zoomOut() {
		currentZoom_ *= zoomInOutCoefficient_;
		adjustZoom();
		return currentZoom_;
	}	
	
	public float zoom(float factor) {
		currentZoom_ *= factor;
		adjustZoom();
		return currentZoom_;
	}		
	
	public float flushZoom() {
		currentZoom_ = 1.0f;
		adjustZoom();
		return currentZoom_;
	}
	
	public void adjustZoom() {
		if ( currentZoom_ < minZoom_ )
			currentZoom_ = minZoom_;
		if ( currentZoom_ > maxZoom_ )
			currentZoom_ = maxZoom_;		
	}
	
	public float minZoom() {
		return minZoom_;
	}
	
	public float maxZoom() {
		return maxZoom_;
	}
	
	public float currentZoom() {
		return currentZoom_;
	}	

}
