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
 
#include "Common/Common.h"
#include "Cursor.h"

namespace RemotePC
{
namespace SFB
{
	Cursor::Cursor()
		: lock_( mc::Library::Mutex() )
		, set_( false )
		, x_()
		, y_()
	    , leftPressed_( false )
		, rightPressed_( false )
	{
	}
	
	void Cursor::set(float x, float y)
	{
		mc::MutexScope locker(lock_);
		set_ = true;
		x_ = x;
		y_ = y;
	}
	
	void Cursor::set(const CursorPtr& cursor)
	{
		mc::MutexScope locker1(lock_);
		mc::MutexScope locker2(cursor->lock_);		
		set_ = cursor->set_;
		x_ = cursor->x_;
		y_ = cursor->y_;		
	}
	
	bool Cursor::isSet()
	{
		mc::MutexScope locker(lock_);
		return set_;
	}
	
	void Cursor::press(bool left, bool right)
	{
		mc::MutexScope locker(lock_);
		leftPressed_ = left;
		rightPressed_ = right;
	}
	
	void Cursor::pressLeft(bool down)
	{
		mc::MutexScope locker(lock_);
		leftPressed_ = down;		
	}
	
	void Cursor::pressRight(bool down)
	{
		mc::MutexScope locker(lock_);
		rightPressed_ = down;		
	}
	
	float Cursor::x() const
	{
		return x_;
	}
	
	float Cursor::y() const
	{
		return y_;
	}				
	
	bool Cursor::leftPressed() const 
	{
		return leftPressed_;
	}	
	
	bool Cursor::rightPressed() const
	{
		return rightPressed_;		
	}	
}
}
		