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

#import "../../Tools/OpenGL/GLContext.h"
#import "../Controls-iPad/FBSurfaceHolder.h"

#include "FBEvents.h"

namespace MinCOM
{
	// {F9D5FE6F-46ED-4052-873A-54D9C3A12FAF}
	template<> const Guid TypeInfo< RemotePC::SFB::IFBEvents >::iid_ = 
		{ 0xF9D5FE6F, 0x46ED, 0x4052, { 0x87, 0x3A, 0x54, 0xD9, 0xC3, 0xA1, 0x2F, 0xAF } };
}

namespace RemotePC
{
namespace SFB
{	
	FBEvents::FBEvents()
		: mc::CommonImpl< IFBEvents >()
	{
	}
	
	/**
	 * IFBEvents section
	 */
	
	void FBEvents::attachGLContext()
	{
		[[GLContext instance] setCurrentContext];		
	}
	
	void FBEvents::init(const SurfacePtr& surface)
	{
		[[FBSurfaceHolder instance] initControls:surface];
	}
	
	void FBEvents::update()
	{
		[[FBSurfaceHolder instance] updateControls];
	}
	
	void FBEvents::cursor()
	{
		[[FBSurfaceHolder instance] updateCursor:YES];
	}
}
}
