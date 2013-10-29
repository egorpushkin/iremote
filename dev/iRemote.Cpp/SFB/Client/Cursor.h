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

#ifndef CURSOR_H__SFB__REMOTEPC__INCLUDED_
#define CURSOR_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{		
	class Cursor;
	typedef boost::shared_ptr< Cursor > CursorPtr;	
	
	class Cursor
	{
	public:
		
		Cursor();
		
		void set(float x, float y);
		void set(const CursorPtr& cursor);
		bool isSet();		
		void press(bool left, bool right);
		void pressLeft(bool down);
		void pressRight(bool down);
		
		float x() const;
		float y() const;
		bool leftPressed() const;
		bool rightPressed() const;
		
	private:
		
		mc::IMutexPtr lock_;
		
		bool set_;
		
		float x_;		
		float y_;
		
		bool leftPressed_;
		bool rightPressed_;

	};				
}
}

#endif // CURSOR_H__SFB__REMOTEPC__INCLUDED_
