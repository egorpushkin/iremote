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

public class LocalStub implements DLocal {
	
    /** Utility, which is responsible for translating events to main thread. */
    private IExecutor executor_ = null; 
    
    /** Reference to control interface dedicated for local events. */
    private DLocal local_ = null;
    
    public LocalStub(IExecutor executor) {
        executor_ = executor;
    }
    
    public void configure(DLocal local) {
        synchronized ( this ) {
        	local_ = local;
        }
    }    
    
    public void zoomParams(float min, float max, float current) {
        final float _min = min;
        final float _max = max;
        final float _current = current;
        executor_.run(
            new Runnable() {
                private float min_ = _min;
                private float max_ = _max;
                private float current_ = _current;
                public void run() {
                    synchronized ( LocalStub.this ) {
                        if ( null != LocalStub.this.local_ )
                        	LocalStub.this.local_.zoomParams(min_, max_, current_);
                    }
                }
            }       
        );      
    }      

}
