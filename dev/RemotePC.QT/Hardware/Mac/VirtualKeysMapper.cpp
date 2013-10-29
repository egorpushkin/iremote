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

#include "VirtualKeysMapper.h"

// Windows virtual key codes.
#include "KeyCodes/VirtualCodes.Win32.h"

namespace RemotePC
{

	VirtualKeysMapper::VirtualKeysMapper()
		: vkMap_()
	{
		// Complete codepage transtation table initialization goes here.
		// First (top) row.
		vkMap_[VK_ESCAPE] = 0x35;
		vkMap_[VK_F1] = 0x7a;
		vkMap_[VK_F2] = 0x78;
		vkMap_[VK_F3] = 0x63;
		vkMap_[VK_F4] = 0x76;
		vkMap_[VK_F5] = 0x60;
		vkMap_[VK_F6] = 0x61;
		vkMap_[VK_F7] = 0x62;
		vkMap_[VK_F8] = 0x64;
		vkMap_[VK_F9] = 0x65;
		vkMap_[VK_F10] = 0x6d;
		vkMap_[VK_F11] = 0x67;
		vkMap_[VK_F12] = 0x6f;
		vkMap_[VK_NUMLOCK] = 0x0;
		vkMap_[VK_SCROLL] = 0x0;
		vkMap_[VK_SNAPSHOT] = 0x0;
		vkMap_[VK_PAUSE] = 0x0;
		// Second (numbers) row.
		vkMap_[VK_OEM_3] = 0x32;
		vkMap_['1'] = 0x12;
		vkMap_['2'] = 0x13;
		vkMap_['3'] = 0x14;
		vkMap_['4'] = 0x15;
		vkMap_['5'] = 0x17;
		vkMap_['6'] = 0x16;
		vkMap_['7'] = 0x1a;
		vkMap_['8'] = 0x1c;
		vkMap_['9'] = 0x19;
		vkMap_['0'] = 0x1d;
		vkMap_[VK_OEM_MINUS] = 0x1b;
		vkMap_[VK_OEM_PLUS] = 0x18;
		vkMap_[VK_BACK] = 0x33;
		vkMap_[VK_HOME] = 0x73;
		// Third (qwerty) row.
		vkMap_[VK_TAB] = 0x30;
		vkMap_['Q'] = 0x0c;
		vkMap_['W'] = 0x0d;
		vkMap_['E'] = 0x0e;
		vkMap_['R'] = 0x0f;
		vkMap_['T'] = 0x11;
		vkMap_['Y'] = 0x10;
		vkMap_['U'] = 0x20;
		vkMap_['I'] = 0x22;
		vkMap_['O'] = 0x1f;
		vkMap_['P'] = 0x23;
		vkMap_[VK_OEM_4] = 0x21;
		vkMap_[VK_OEM_6] = 0x1e;
		vkMap_[VK_OEM_5] = 0x2a;
		vkMap_[VK_PRIOR] = 0x74;
		// Fouth (capslock) row
		vkMap_[VK_CAPITAL] = 0x39;
		vkMap_['A'] = 0x00;
		vkMap_['S'] = 0x01;
		vkMap_['D'] = 0x02;
		vkMap_['F'] = 0x03;
		vkMap_['G'] = 0x05;
		vkMap_['H'] = 0x04;
		vkMap_['J'] = 0x26;
		vkMap_['K'] = 0x28;
		vkMap_['L'] = 0x25;
		vkMap_[VK_OEM_1] = 0x29;
		vkMap_[VK_OEM_7] = 0x27;
		vkMap_[VK_RETURN] = 0x24;
		vkMap_[VK_NEXT] = 0x79;
		// Fifth (shift) row
		vkMap_[VK_LSHIFT] = 0x38;
		vkMap_['Z'] = 0x06;
		vkMap_['X'] = 0x07;
		vkMap_['C'] = 0x08;
		vkMap_['V'] = 0x09;
		vkMap_['B'] = 0x0b;
		vkMap_['N'] = 0x2d;
		vkMap_['M'] = 0x2e;
		vkMap_[VK_OEM_COMMA] = 0x2b;
		vkMap_[VK_OEM_PERIOD] = 0x2f;
		vkMap_[VK_OEM_2] = 0x2c;
		vkMap_[VK_RSHIFT] = 0x38;
		vkMap_[VK_UP] = 0x7e;
		vkMap_[VK_END] = 0x77;
		// Sixth (space) row
		vkMap_[VK_LCONTROL] = 0x3b;
		vkMap_[VK_LWIN] = 0x37;
		vkMap_[VK_LMENU] = 0x3a;
		vkMap_[VK_SPACE] = 0x31;
		vkMap_[VK_RMENU] = 0x3a;
		vkMap_[VK_RWIN] = 0x37;
		vkMap_[VK_APPS] = 0x0;
		vkMap_[VK_INSERT] = 0x72;
		vkMap_[VK_DELETE] = 0x75;
		vkMap_[VK_LEFT] = 0x7b;
		vkMap_[VK_DOWN] = 0x7d;
		vkMap_[VK_RIGHT] = 0x7c;
		// Additional keys
		vkMap_[VK_SHIFT] = 0x38;
		vkMap_[VK_CONTROL] = 0x3b;
		vkMap_[VK_MENU] = 0x3a;
	}

	/**
	 * Current implementation assumes that each key has a mapping and relies on
	 * default std::map behaviour otherwise.
	 */
	unsigned short VirtualKeysMapper::Translate(unsigned char key)
	{
		return vkMap_[key];
	}

}
