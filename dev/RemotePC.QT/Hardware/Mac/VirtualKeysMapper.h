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

#ifndef VIRTUALKEYSMAPPER_H__REMOTEPC__INCLUDED_
#define VIRTUALKEYSMAPPER_H__REMOTEPC__INCLUDED_

namespace RemotePC
{

	/**
	 * iRemote uses Windows single-byte virtual keys for internal data exchange.
	 * Hosts and clients on other platforms platforms (like RemotePC on Mac)
	 * are forced to utilize some kind of translators between encodings.
	 *
	 * This class translates Windows virtual keys to Mac virtual keys
	 * (CGKeyCode).
	 */
	class VirtualKeysMapper
	{
	public:

		VirtualKeysMapper();

		unsigned short Translate(unsigned char key);

	private:

		std::map< unsigned char, unsigned short > vkMap_;

	};


}

#endif // VIRTUALKEYSMAPPER_H__REMOTEPC__INCLUDED_
