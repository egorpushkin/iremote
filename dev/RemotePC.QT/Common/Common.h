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

#ifndef COMMON_H__REMOTEPC__INCLUDED_
#define COMMON_H__REMOTEPC__INCLUDED_

#if defined(REMOTEPC_OS_WINXP)
#define REMOTEPC_OS_VER 0x0501
#elif defined(REMOTEPC_OS_WINVISTA)
#define REMOTEPC_OS_VER 0x0600
#endif

#if defined(WIN32) && !defined(WINVER)
#define WINVER REMOTEPC_OS_VER		
#endif

#if defined(WIN32) && !defined(_WIN32_WINNT)
#define _WIN32_WINNT REMOTEPC_OS_VER
#endif 

// Disable warning (L1 unreferenced formal parameter).
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4100)
#endif

// This should be included before MinCOM. Because it (MinCOM) includes 
// WIN32_LEAN_AND_MEAN Windows itself.
#include <boost/asio.hpp>

#ifdef _MSC_VER
#pragma warning( pop ) 
#endif

#if defined(WIN32)
// Windows multimedia
#include <mmsystem.h>
#endif

// Minimalist COM
#include "MinCOM/MinCOM.h"

#endif // COMMON_H__REMOTEPC__INCLUDED_
