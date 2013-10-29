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

#include "IVolumeControl.h"
#include "IMouseControl.h"
#include "IKeyboardControl.h"
#include "IDisplayControl.h"
#include "IAudioInput.h"

#if defined(WIN32)
#include "../Win32/IVolumeControlInitializer.h"
#endif

namespace MinCOM
{

	// {561C6BCB-E60A-4145-A616-88B78C99900F}
	template<> const Guid TypeInfo< RemotePC::IVolumeControl >::iid_ = 
		{ 0x561c6bcb, 0xe60a, 0x4145, { 0xa6, 0x16, 0x88, 0xb7, 0x8c, 0x99, 0x90, 0xf } };

	// {4BD3770D-FDD7-4f2d-BD72-E91508224B51}
	template<> const Guid TypeInfo< RemotePC::IMouseControl >::iid_ = 
		{ 0x4bd3770d, 0xfdd7, 0x4f2d, { 0xbd, 0x72, 0xe9, 0x15, 0x8, 0x22, 0x4b, 0x51 } };

	// {F584F8E6-C540-4b26-ABB1-740E13C09520}
	template<> const Guid TypeInfo< RemotePC::IKeyboardControl >::iid_ = 
		{ 0xf584f8e6, 0xc540, 0x4b26, { 0xab, 0xb1, 0x74, 0xe, 0x13, 0xc0, 0x95, 0x20 } };

	// {D65D5B11-B2E7-454a-9CE7-696A7EA757FC}
	template<> const Guid TypeInfo< RemotePC::IDisplayControl >::iid_ =
		{ 0xd65d5b11, 0xb2e7, 0x454a, { 0x9c, 0xe7, 0x69, 0x6a, 0x7e, 0xa7, 0x57, 0xfc } };

    // {2E03DB3F-1AC6-4516-97B8-11869D65CB62}
    template<> const Guid TypeInfo< RemotePC::IAudioInput >::iid_ =
        { 0x2E03DB3F, 0x1AC6, 0x4516, { 0x97, 0xB8, 0x11, 0x86, 0x9D, 0x65, 0xCB, 0x62 } };

#if defined(WIN32)
	// {B2165673-46AF-4210-A9A0-DA35FF16D8CC}
	template<> const Guid TypeInfo< RemotePC::IVolumeControlInitializer >::iid_ =
		{ 0xb2165673, 0x46af, 0x4210, { 0xa9, 0xa0, 0xda, 0x35, 0xff, 0x16, 0xd8, 0xcc } };
#endif

}
