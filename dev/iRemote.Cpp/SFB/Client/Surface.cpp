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
#include "Cursor.h"
#include "Surface.h"

#include "../Server/Math.h"

namespace RemotePC
{
namespace SFB
{
		
	Surface::Surface(size_t screenWidth, size_t screenHeight, size_t textureSize)
		: screenWidth_(screenWidth)
		, screenHeight_(screenHeight)
		, textureSize_(textureSize)	
		, blockWidth_(Math<size_t>::ceil(screenWidth_, textureSize_))
		, blockHeight_(Math<size_t>::ceil(screenHeight_, textureSize_))	
		, textures_(NULL)
		, cursor_(new Cursor())	
		, internalCursor_(new Cursor())		
	{
		init();
	}
	
	Surface::~Surface()
	{
		destroy();
	}
			
	void Surface::init()
	{
		// Allocate cells.
		textures_ = new Texture**[blockHeight_];
		for ( size_t i = 0 ; i < blockHeight_ ; ++i )
		{
			textures_[i] = new Texture*[blockWidth_];
			for ( size_t j = 0 ; j < blockWidth_ ; ++j )
			{
				textures_[i][j] = new Texture(textureSize_, textureSize_);
			}
		}
	}
			
	void Surface::destroy()
	{
		for ( size_t i = 0 ; i < blockHeight_ ; ++i )
		{
			for ( size_t j = 0 ; j < blockWidth_ ; ++j )
			{
				delete textures_[i][j];
			}
			delete [] textures_[i];
		}
		delete [] textures_;		
	}
		
}
}
