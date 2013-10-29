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
#include "Texture.h"

#include <OpenGLES/ES1/gl.h>
#include <CoreGraphics/CoreGraphics.h>

namespace RemotePC
{
namespace SFB
{
	Texture::Texture(const std::string& _filename)
		: id_(wrongTexture_) 
		, width_(0)
		, height_(0)
		, bytesPerPixel_(4)
		, sync_(mc::Library::Mutex())
		, buffer_(NULL)
		, length_(0)	
	{
		generate();
		NSBundle *applicationBundle = [NSBundle mainBundle];		
		NSString *filename = [NSString stringWithCString:_filename.c_str() encoding:NSUTF8StringEncoding];
		NSString *filepath = [applicationBundle pathForResource:filename ofType:nil];
		NSData *filedata = [NSData dataWithContentsOfFile:filepath];
		updateFromData((unsigned char *)[filedata bytes], [filedata length]);
	}
}
}
