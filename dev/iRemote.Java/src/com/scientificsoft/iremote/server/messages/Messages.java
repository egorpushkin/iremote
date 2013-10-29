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

package com.scientificsoft.iremote.server.messages;

import com.scientificsoft.iremote.server.protocol.Guid;

public class Messages {

	// {07D33EAE-49E4-4f01-B418-CD98C866FC48}
	public static Guid leftButtonDown_ = new Guid
		( 0x7d33eae, 0x49e4, 0x4f01, 0xb4, 0x18, 0xcd, 0x98, 0xc8, 0x66, 0xfc, 0x48 );

	// {51DE4225-704B-4add-9438-9925956C6C59}
	public static Guid leftButtonUp_ = new Guid
		( 0x51de4225, 0x704b, 0x4add, 0x94, 0x38, 0x99, 0x25, 0x95, 0x6c, 0x6c, 0x59 );

	// {58C60134-42EA-4f5f-B274-ED18748A43CF}
	public static Guid middleButtonDown_ = new Guid
		( 0x58c60134, 0x42ea, 0x4f5f, 0xb2, 0x74, 0xed, 0x18, 0x74, 0x8a, 0x43, 0xcf );

	// {DDB209E0-B1F9-4066-B6FE-3B0EEC393BB2}
	public static Guid middleButtonUp_ = new Guid
		( 0xddb209e0, 0xb1f9, 0x4066, 0xb6, 0xfe, 0x3b, 0xe, 0xec, 0x39, 0x3b, 0xb2 );

	// {34169C8F-2BE4-4a8c-81B5-60C5C734FBDD}
	public static Guid rightButtonDown_ = new Guid
		( 0x34169c8f, 0x2be4, 0x4a8c, 0x81, 0xb5, 0x60, 0xc5, 0xc7, 0x34, 0xfb, 0xdd );

	// {51FE97C0-A719-40db-B531-D3407E85B12B}
	public static Guid rightButtonUp_ = new Guid
		( 0x51fe97c0, 0xa719, 0x40db, 0xb5, 0x31, 0xd3, 0x40, 0x7e, 0x85, 0xb1, 0x2b );
	
}
