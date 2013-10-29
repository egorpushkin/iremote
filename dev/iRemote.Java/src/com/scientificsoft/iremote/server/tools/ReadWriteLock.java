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

package com.scientificsoft.iremote.server.tools;

/** 
 * Implements the following rule:
 *  - Multiple readers can read the data simultaneously.
 *  - Only one writer can write the data at any time.
 *  - A reader and a writer cannot be in critical section together.
 * 
 * This is powered by simple and very efficient schema based on 
 * locking resources but not operations. Two mutexes are required to 
 * power this schema: one for counters and another one for target
 * resource.
 */
public class ReadWriteLock {
	
	/** Prevents access to the resource from multiple threads. */
	private Object resourceMutex_ = new Object();

	/** Prevents access to counters from multiple threads. */
	private Object countersMutex_ = new Object();

	/** Number of actual writers. This property was made signed intentionally
	 * for debug purposes. This allows to alert if negative value is met here.*/
	private long writersCount_ = 0;

	/** Number of actual readers. */
	private long readersCount_ = 0;
	
	public void waitToWrite() {
		try {
			countersMutex_.wait();
			++writersCount_;
			countersMutex_.notify();
			resourceMutex_.wait();	
		} catch ( Exception e ) {}
	}

	public void doneWriting() {
		try {
			countersMutex_.wait();
			--writersCount_;
			countersMutex_.notify();
			resourceMutex_.notify();	
		} catch ( Exception e ) {}		
	}

	public void waitToRead() {
		try {		
			countersMutex_.wait();
			if ( ( writersCount_ > 0 ) || ( readersCount_ == 0 ) )
			{
				countersMutex_.notify();
				resourceMutex_.wait();
				countersMutex_.wait();
			}
			++readersCount_;
			countersMutex_.notify();		
		} catch ( Exception e ) {}		
	}

	public void doneReading() {
		try {
			countersMutex_.wait();
			--readersCount_;
			if ( 0 == readersCount_ )
			{
				resourceMutex_.notify();
			}
			countersMutex_.notify();		
		} catch ( Exception e ) {}			
	}	
}
