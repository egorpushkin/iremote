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

#ifndef HARWARECONTROL_H__REMOTEPC__INCLUDED_
#define HARWARECONTROL_H__REMOTEPC__INCLUDED_

namespace RemotePC
{

	class HardwareControl
		: public mc::CommonImpl< IMouseControl >
		, public mc::CommonImpl< IKeyboardControl >
	{
	public:

		typedef mc::CommonImpl< IMouseControl > ClassRoot_;

		// IMouseControl section
		virtual void LeftButtonDown() const;

		virtual void LeftButtonUp() const;

		virtual void MiddleButtonDown() const;

		virtual void MiddleButtonUp() const;

		virtual void RightButtonDown() const;

		virtual void RightButtonUp() const;

		virtual void MoveCursorBy(float dx, float dy) const;

		virtual void MoveCursorTo(float x, float y) const;

		virtual void MouseClick(float x, float y) const;

		virtual void Scroll(bool vertical, int value) const;

        virtual void Scroll2(bool vertical, int value) const;

		virtual ScreenPoint GetPosition() const;

		// IKeyboardControl section
		virtual void KeyboardEventVirtual(unsigned char vk, bool keydown) const;

		virtual void KeyboardEventUnicode(unsigned short code) const;

	private:

		void MouseEvent(unsigned long flags, float dx = 0, float dy = 0, bool absolute = false) const;

	};

}

#endif // !HARWARECONTROL_H__REMOTEPC__INCLUDED_
