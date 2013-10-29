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

#ifndef SURFACE_H__SFB__REMOTEPC__INCLUDED_
#define SURFACE_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{
		
	class Surface
	{
	public:
		
		Surface(size_t screenWidth, size_t screenHeight, size_t textureSize);
		~Surface();
		
		inline size_t screenWidth() const
		{
			return screenWidth_;
		}
		
		inline size_t screenHeight() const
		{
			return screenHeight_;
		}		
		
		inline size_t textureSize() const
		{
			return textureSize_;
		}
		
		inline size_t blockWidth() const
		{
			return blockWidth_;
		}
		
		inline size_t blockHeight() const
		{
			return blockHeight_;
		}				
		
		inline Texture * texture(size_t textureX, size_t textureY) const
		{
			return textures_[textureY][textureX];
		}		
		
		inline CursorPtr cursor() const
		{
			return cursor_;
		}	
		
		inline CursorPtr internalCursor() const
		{
			return internalCursor_;
		}			
		
	private:
		
		void init();
		
		void destroy();
		
	private:
		
		size_t screenWidth_;
		
		size_t screenHeight_;
		
		size_t textureSize_;		
		
		size_t blockWidth_;
		
		size_t blockHeight_;		
		
		Texture *** textures_;	
				
		CursorPtr cursor_;		
		
		CursorPtr internalCursor_;				
				
	};
	
	typedef boost::shared_ptr< Surface > SurfacePtr;		
	typedef const SurfacePtr& SurfaceRef;			
		
}
}

#endif // SURFACE_H__SFB__REMOTEPC__INCLUDED_
