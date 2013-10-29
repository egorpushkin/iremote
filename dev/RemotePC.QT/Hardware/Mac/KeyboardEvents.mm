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

// CD/DVD drives enumeration interface.
#import <DiscRecording/DRDevice.h>

// Precompiled header
#include "Common/Common.h"

// Public interface.
#include "KeyboardEvents.h"

// Windows virtual key codes.
#include "KeyCodes/VirtualCodes.Win32.h"

// Keyboard led indicators interface.
#include "KeyboardLeds.h"

namespace RemotePC
{

	static const unsigned char VK_MAC_CAPITAL = 57;

	void KeyboardEvents::PostRawEvent(unsigned char vk, bool keydown)
	{
		CGEventSourceRef eventSource = CGEventSourceCreate(kCGEventSourceStateCombinedSessionState);
		CGEventRef kbevent = CGEventCreateKeyboardEvent(eventSource, (CGKeyCode)vk, keydown);
		// TODO: This should be fixed, because it has no effect in practice.
		// CGEventSetIntegerValueField(kbevent, kCGKeyboardEventAutorepeat, 0x1);
		CGEventPost(kCGHIDEventTap, kbevent);
		CFRelease(kbevent);
		CFRelease(eventSource);
	}

	bool KeyboardEvents::HandleVirtualKey(unsigned char vk, bool keydown)
	{
		// Check for eject key up event.
		if ( VK_APPS == vk )
		{
			if ( !keydown )
			{
				// This actually ejects all connected cd/dvd drives.
				NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
				NSEnumerator *deviceEnum = [[DRDevice devices] objectEnumerator];
				DRDevice *device = nil;
				while ((device = [deviceEnum nextObject]) != nil)
				{
					[device ejectMedia];
				}
				[pool release];
			}
			return true;
		}
		// Check for CapsLock event.
		else if ( VK_CAPITAL == vk )
		{
			if ( !keydown )
			{
				// Invert capslock state here.
				CGEventFlags flags = CGEventSourceFlagsState(kCGEventSourceStateCombinedSessionState);
				if ( flags & kCGEventFlagMaskAlphaShift )
				{
					CGPostKeyboardEvent(0x0, VK_MAC_CAPITAL, false);
					KeyboardLeds::TriggerCapsLock(0);
				}
				else
				{
					CGPostKeyboardEvent(0x0, VK_MAC_CAPITAL, true);
					KeyboardLeds::TriggerCapsLock(1);
				}
			}
			return true;
		}
		// In the rest of cases the routine should return false.
		return false;
	}

}
