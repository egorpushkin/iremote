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

#ifndef IMOUSECONTROL_H__REMOTEPC__INCLUDED_
#define IMOUSECONTROL_H__REMOTEPC__INCLUDED_

namespace RemotePC
{

	typedef struct tagScreenPoint
	{
		float x_;
		float y_;
	}
	ScreenPoint;

    struct IMouseControl : public mc::ICommon
	{

		virtual void LeftButtonDown() const = 0;

		virtual void LeftButtonUp() const = 0;

		virtual void MiddleButtonDown() const = 0;

		virtual void MiddleButtonUp() const = 0;

		virtual void RightButtonDown() const = 0;

		virtual void RightButtonUp() const = 0;

        virtual void MoveCursorBy(float dx, float dy) const = 0;

        virtual void MoveCursorTo(float x, float y) const = 0;

        virtual void MouseClick(float x, float y) const = 0;

		virtual void Scroll(bool vertical, int value) const = 0;

        virtual void Scroll2(bool vertical, int value) const = 0;

		virtual ScreenPoint GetPosition() const = 0;

	};

	typedef mc::Strong< IMouseControl > IMouseControlPtr;
	typedef const IMouseControlPtr& IMouseControlRef;

}

#endif // !IMOUSECONTROL_H__REMOTEPC__INCLUDED_
