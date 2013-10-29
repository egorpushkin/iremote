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

#include "Messages.h"

namespace RemotePC
{

	// Password protection messages
	//////////////////////////////////////////////////////////////////////////
	// {BC97B91B-9C1E-4366-A69B-DD355B524B4E}
	const mc::Guid Messages::wrongPassword_ =
		{ 0xbc97b91b, 0x9c1e, 0x4366, { 0xa6, 0x9b, 0xdd, 0x35, 0x5b, 0x52, 0x4b, 0x4e } };

	// {375C6C2F-990B-4e43-BEB0-A51D58D35586}
	const mc::Guid Messages::wrongDeviceIp_ =
		{ 0x375c6c2f, 0x990b, 0x4e43, { 0xbe, 0xb0, 0xa5, 0x1d, 0x58, 0xd3, 0x55, 0x86 } };

	// Control messages
	//////////////////////////////////////////////////////////////////////////
	// {B73497E8-F0B9-44f6-AD37-33DDAE3D37BA}
	const mc::Guid Messages::limitReached_ =
		{ 0xb73497e8, 0xf0b9, 0x44f6, { 0xad, 0x37, 0x33, 0xdd, 0xae, 0x3d, 0x37, 0xba } };

    // Services messages
    //////////////////////////////////////////////////////////////////////////
	// {F7122507-26A4-4199-A38B-6949A6F64C61}
    const mc::Guid Messages::serviceFailure_ =
		{ 0xF7122507, 0x26A4, 0x4199, { 0xA3, 0x8B, 0x69, 0x49, 0xA6, 0xF6, 0x4C, 0x61 } };
	
    // {7335F949-BF86-4E5E-ABE0-1D36BC9C6313}
    const mc::Guid Messages::streamSuccess_ =
        { 0x7335F949, 0xBF86, 0x4E5E, { 0xAB, 0xE0, 0x1D, 0x36, 0xBC, 0x9C, 0x63, 0x13 } };

    // {BE0DFDFE-F785-4396-A3C3-9DD6FCB198FD}
    const mc::Guid Messages::streamFailure_ =
        { 0xBE0DFDFE, 0xF785, 0x4396, { 0xA3, 0xC3, 0x9D, 0xD6, 0xFC, 0xB1, 0x98, 0xFD } };

	// Mouse messages
	//////////////////////////////////////////////////////////////////////////

	// {07D33EAE-49E4-4f01-B418-CD98C866FC48}
	const mc::Guid Messages::leftButtonDown_ =
        { 0x7d33eae, 0x49e4, 0x4f01, { 0xb4, 0x18, 0xcd, 0x98, 0xc8, 0x66, 0xfc, 0x48 } };

	// {51DE4225-704B-4add-9438-9925956C6C59}
	const mc::Guid Messages::leftButtonUp_ =
        { 0x51de4225, 0x704b, 0x4add, { 0x94, 0x38, 0x99, 0x25, 0x95, 0x6c, 0x6c, 0x59 } };

	// {58C60134-42EA-4f5f-B274-ED18748A43CF}
	const mc::Guid Messages::middleButtonDown_ =
        { 0x58c60134, 0x42ea, 0x4f5f, { 0xb2, 0x74, 0xed, 0x18, 0x74, 0x8a, 0x43, 0xcf } };

	// {DDB209E0-B1F9-4066-B6FE-3B0EEC393BB2}
	const mc::Guid Messages::middleButtonUp_ =
        { 0xddb209e0, 0xb1f9, 0x4066, { 0xb6, 0xfe, 0x3b, 0xe, 0xec, 0x39, 0x3b, 0xb2 } };

	// {34169C8F-2BE4-4a8c-81B5-60C5C734FBDD}
	const mc::Guid Messages::rightButtonDown_ =
        { 0x34169c8f, 0x2be4, 0x4a8c, { 0x81, 0xb5, 0x60, 0xc5, 0xc7, 0x34, 0xfb, 0xdd } };

	// {51FE97C0-A719-40db-B531-D3407E85B12B}
	const mc::Guid Messages::rightButtonUp_ =
        { 0x51fe97c0, 0xa719, 0x40db, { 0xb5, 0x31, 0xd3, 0x40, 0x7e, 0x85, 0xb1, 0x2b } };

}
