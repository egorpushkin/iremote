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

#include "Versions.h"

namespace RemotePC
{
    // Status of Remote PC Suite build ("Dev", "Stable", etc.).
    const std::string RemotePCBuildStatus = "Final";

	// Unique identifier of RemotePC application.
	// {720215BC-9673-4ab5-9889-6EA255809630}
	const mc::Guid RemotePCApplicationGuid =
		{ 0x720215bc, 0x9673, 0x4ab5, { 0x98, 0x89, 0x6e, 0xa2, 0x55, 0x80, 0x96, 0x30 } };

	// Unique identifier of iRemote application.
	// {BA3D977A-1CE8-4d2f-B6F9-1B7065401BC9}
	const mc::Guid iRemoteApplicationGuid = 
		{ 0xba3d977a, 0x1ce8, 0x4d2f, { 0xb6, 0xf9, 0x1b, 0x70, 0x65, 0x40, 0x1b, 0xc9 } };
}

