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

#include "../Provider/IMouseControl.h"
#include "../Provider/IKeyboardControl.h"
#include "HardwareControl.h"

#ifndef MOUSEEVENTF_HWHEEL 
/** 
 * Explicit declaration of the constant which was probably omitted 
 * by Vista Platform SDK. 
 */
#define MOUSEEVENTF_HWHEEL 0x1000
#endif // !MOUSEEVENTF_HWHEEL

namespace RemotePC
{

	void HardwareControl::LeftButtonDown() const
	{
		MouseEvent(MOUSEEVENTF_LEFTDOWN);
	}

	void HardwareControl::LeftButtonUp() const
	{
		MouseEvent(MOUSEEVENTF_LEFTUP);
	}

	void HardwareControl::MiddleButtonDown() const
	{
		MouseEvent(MOUSEEVENTF_MIDDLEDOWN);
	}

	void HardwareControl::MiddleButtonUp() const
	{
		MouseEvent(MOUSEEVENTF_MIDDLEUP);
	}

	void HardwareControl::RightButtonDown() const
	{
		MouseEvent(MOUSEEVENTF_RIGHTDOWN);
	}

	void HardwareControl::RightButtonUp() const
	{
		MouseEvent(MOUSEEVENTF_RIGHTUP);
	}

	void HardwareControl::MoveCursorBy(float dx, float dy) const
	{
		MouseEvent(MOUSEEVENTF_MOVE, dx, dy);
	}

	void HardwareControl::MoveCursorTo(float x, float y) const
	{
		MouseEvent(MOUSEEVENTF_MOVE, x, y, true);
	}

	void HardwareControl::MouseClick(float x, float y) const
    {
		MouseEvent(MOUSEEVENTF_MOVE, x, y, true);
		MouseEvent(MOUSEEVENTF_LEFTDOWN);
		MouseEvent(MOUSEEVENTF_LEFTUP);
	}

	void HardwareControl::Scroll(bool vertical, int value) const
	{
		INPUT buffer;
		buffer.type = INPUT_MOUSE;
		buffer.mi.mouseData = value;
		buffer.mi.dwFlags = ( vertical ) ? ( MOUSEEVENTF_WHEEL ) : ( MOUSEEVENTF_HWHEEL );
		buffer.mi.time = 0;
		buffer.mi.dwExtraInfo = 0;
		::SendInput(1, &buffer, sizeof(INPUT));		
	}

    void HardwareControl::Scroll2(bool vertical, int value) const
	{
		Scroll(vertical, value);
	}

	ScreenPoint HardwareControl::GetPosition() const
	{
		POINT position = { 0, 0 };
		::GetCursorPos(&position);
		ScreenPoint point;
		point.x_ = (float)position.x;
		point.y_ = (float)position.y;
		return point;
	}

	// IKeyboardControl section
	void HardwareControl::KeyboardEventVirtual(unsigned char vk, bool keydown) const
	{
		INPUT buffer;
		buffer.type = INPUT_KEYBOARD;
		buffer.ki.wVk = vk;
		buffer.ki.dwFlags = ( keydown ) ? ( 0 ) : ( KEYEVENTF_KEYUP );
		::SendInput(1, &buffer, sizeof(INPUT));			
	}

	void HardwareControl::KeyboardEventUnicode(unsigned short code) const
	{
		INPUT buffer[2];
		buffer[0].type = INPUT_KEYBOARD;
		buffer[0].ki.wVk = 0;
		buffer[0].ki.wScan = code;
		buffer[0].ki.dwFlags = KEYEVENTF_UNICODE;
		buffer[1].type = INPUT_KEYBOARD;
		buffer[1].ki.wVk = 0;
		buffer[1].ki.wScan = code;
		buffer[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
		::SendInput(2, buffer, sizeof(INPUT));	
	}

	// Private helpers
	void HardwareControl::MouseEvent(unsigned long flags, float dx, float dy, bool absolute) const
	{
		INPUT buffer;
		buffer.type = INPUT_MOUSE;
		if ( absolute )
		{
			flags |= MOUSEEVENTF_ABSOLUTE;
			// This code might be useful when new position is specified in absolute coordinates. 
			int ix = GetSystemMetrics(SM_CXSCREEN);
			int iy = GetSystemMetrics(SM_CYSCREEN);
			buffer.mi.dx = ((LONG)dx) * 65535 / ix;
			buffer.mi.dy = ((LONG)dy) * 65535 / iy;
		}
		else
		{
			buffer.mi.dx = (LONG)dx;
			buffer.mi.dy = (LONG)dy;
		}
		buffer.mi.mouseData = 0;
		buffer.mi.dwFlags = flags;
		buffer.mi.time = 0;
		buffer.mi.dwExtraInfo = 0;
		::SendInput(1, &buffer, sizeof(INPUT));
	}
}
