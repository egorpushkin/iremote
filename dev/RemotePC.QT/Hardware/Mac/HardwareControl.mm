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

// Mac specific keyboard and mouse apis (including Quartz Event Services).
#include "Foundation/Foundation.h"
#include "ApplicationServices/ApplicationServices.h"
#include "AppKit/NSEvent.h"
#include "Carbon/Carbon.h"

// Precompiled header
#include "Common/Common.h"

// Virtual key codes mapper.
#include "VirtualKeysMapper.h"

// Public interface and dependencies.
#include "../Provider/IMouseControl.h"
#include "../Provider/IKeyboardControl.h"
#include "HardwareControl.h"

// Keyboard events triggerign tool.
#include "KeyboardEvents.h"

namespace RemotePC
{

	HardwareControl::HardwareControl()
        : mc::CommonImpl< IMouseControl >()
		, mc::CommonImpl< IKeyboardControl >()
		, leftPressed_(false)
		, keysMapper_()
	{
	}

	// IMouseControl section
	void HardwareControl::LeftButtonDown() const
	{
		MouseButton(kCGEventLeftMouseDown);
		leftPressed_ = true;
	}

	void HardwareControl::LeftButtonUp() const
	{
		MouseButton(kCGEventLeftMouseUp);
		leftPressed_ = false;
	}

	void HardwareControl::MiddleButtonDown() const
	{
		MouseButton(kCGEventOtherMouseDown);
	}

	void HardwareControl::MiddleButtonUp() const
	{
		MouseButton(kCGEventOtherMouseUp);
	}

	void HardwareControl::RightButtonDown() const
	{
		MouseButton(kCGEventRightMouseDown);
	}

	void HardwareControl::RightButtonUp() const
	{
		MouseButton(kCGEventRightMouseUp);
	}

    void HardwareControl::MoveCursorBy(float dx, float dy) const
	{
        MouseMoveBy(dx, dy);
	}

    void HardwareControl::MoveCursorTo(float x, float y) const
    {
        MouseMoveTo(x, y);
    }

    void HardwareControl::MouseClick(float x, float y) const
    {
        MouseButton(kCGEventLeftMouseDown, x, y);
        MouseButton(kCGEventLeftMouseUp, x, y);
    }

	void HardwareControl::Scroll(bool vertical, int value) const
	{
		if ( vertical )
		{
            CGPostScrollWheelEvent(1, ( value > 0 ) ? ( 10 ) : ( -10 ));
		}
        else
        {
            CGPostScrollWheelEvent(2, 0, ( value > 0 ) ? ( 10 ) : ( -10 ));
        }
	}

    void HardwareControl::Scroll2(bool vertical, int value) const
    {
        // Adjust value to the interval of [-10; 10].
        if ( value > 10 )
            value = 10;
        else if ( value < -10 )
            value = -10;
        // Simulate scroll event.
        CGEventSourceRef eventSource = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
        CGEventRef event;
        event = CGEventCreateScrollWheelEvent(
            eventSource, kCGScrollEventUnitPixel,
            vertical ? 1 : 2, vertical ? value : 0, vertical ? 0 : value);
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
        CFRelease(eventSource);
    }

	ScreenPoint HardwareControl::GetPosition() const
	{
		// HIGetMousePosition appeared in 10.5.
		// HIPoint currentLocation = { 0.0, 0.0 };
		// HIGetMousePosition(kHICoordSpaceScreenPixel, NULL, &currentLocation);

		// Get screen height.
		CGDirectDisplayID mainDisplay = CGMainDisplayID();
		size_t displayHeight = CGDisplayPixelsHigh(mainDisplay);
		// Get current cursor position.
		NSPoint currentLocation = [NSEvent mouseLocation];
		ScreenPoint point;
		point.x_ = currentLocation.x;
		point.y_ = displayHeight - currentLocation.y;
		return point;
	}

	// IKeyboardControl section
	void HardwareControl::KeyboardEventVirtual(unsigned char vk, bool keydown) const
	{
		// Process specific keyboard events (Eject, CapsLock, etc.).
		if ( KeyboardEvents::HandleVirtualKey(vk, keydown) )
			return;
		// Handle the rest of keys.
		KeyboardEvents::PostRawEvent(keysMapper_.Translate(vk), keydown);
	}

	void HardwareControl::KeyboardEventUnicode(unsigned short code) const
	{
		// Create event source to simulate keyboard event.
		CGEventSourceRef eventSource = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);

		// Keyboard receives unicode input by means of Quartz Event Services.
		// CGEventKeyboardSetUnicodeString associates unicode string with
		// keyboard event.

		// Create and post key down event.
		CGEventRef kbevent = CGEventCreateKeyboardEvent(eventSource, (CGKeyCode)0, true);
		CGEventKeyboardSetUnicodeString(kbevent, 1, &code);
		CGEventPost(kCGHIDEventTap, kbevent);
		CFRelease(kbevent);
		// Create and post key up event.
		kbevent = CGEventCreateKeyboardEvent(eventSource, (CGKeyCode)0, false);
		CGEventKeyboardSetUnicodeString(kbevent, 1, &code);
		CGEventPost(kCGHIDEventTap, kbevent);
		CFRelease(kbevent);

		// Release event source.
		CFRelease(eventSource);
	}

	// Private helpers
    void HardwareControl::MouseMoveBy(float dx, float dy) const
	{
        // Calculate new mouse position.
        ScreenPoint currentPosition = GetPosition();
        MouseMoveTo(currentPosition.x_ + dx, currentPosition.y_ + dy);
	}

    void HardwareControl::MouseMoveTo(float x, float y) const
    {
        // Create event source to simulate keyboard event.
        CGEventSourceRef eventSource = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);

        // Calculate new mouse position.
        CGPoint newPosition = CGPointMake(x, y);

        // Simulate mouse move event.
        CGEventRef mouseEvent;
        if ( leftPressed_ )
            mouseEvent = CGEventCreateMouseEvent(eventSource, kCGEventLeftMouseDragged, newPosition, 0);
        else
            mouseEvent = CGEventCreateMouseEvent(eventSource, kCGEventMouseMoved, newPosition, 0);
        CGEventPost(kCGHIDEventTap, mouseEvent);
        CFRelease(mouseEvent);

        // Release event source.
        CFRelease(eventSource);
    }

	void HardwareControl::MouseButton(int button) const
	{
		ScreenPoint currentPosition = GetPosition();
        MouseButton(button, currentPosition.x_, currentPosition.y_);
	}

    void HardwareControl::MouseButton(int button, float x, float y) const
    {
        // Create event source to simulate keyboard event.
        CGEventSourceRef eventSource = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);

        // Simulate mouse move event.
        CGEventRef mouseEvent = CGEventCreateMouseEvent(
            eventSource,
            button,
            CGPointMake(x, y),
            2);
        CGEventPost(kCGHIDEventTap, mouseEvent);
        CFRelease(mouseEvent);

        // Release event source.
        CFRelease(eventSource);
    }

}
