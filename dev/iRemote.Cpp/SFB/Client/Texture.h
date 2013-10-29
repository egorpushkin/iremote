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

#ifndef TEXTURE_H__SFB__REMOTEPC__INCLUDED_
#define TEXTURE_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{
		
	class Texture
	{
	public:
		
		static const unsigned int wrongTexture_ = 0;
		
	public:
		
		Texture(size_t width, size_t height);	
		
		Texture(unsigned char *data, size_t length);
		
		Texture(const std::string& filename);		
		
		virtual ~Texture();
		
		size_t width() const;

		size_t height() const;		
		
		void draw(float x, float y, float width, float height);
		
		void updateFromRawData(unsigned char *data, size_t length);
		
		void updateFromData(unsigned char *data, size_t length);
		
	private:
		
		void generate();
		
		void createEmpty();
		
	private:
		
		unsigned int id_;
		
		size_t width_;			
		size_t height_;			
		
		size_t bytesPerPixel_;
		
		mc::IMutexPtr sync_;
		
		bool initialized_;
		
		unsigned char *buffer_;
		size_t length_;
		
	};
	
	typedef boost::shared_ptr< Texture > TexturePtr;			
		
}
}

#endif // TEXTURE_H__SFB__REMOTEPC__INCLUDED_

