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

#include "MemoryCache.h"

namespace RemotePC
{
namespace SFB
{
    MemoryCache::MemoryCache()
		: lock_(mc::Library::Mutex())
        , cache_()
    {
    }

    MemoryCache::~MemoryCache()
    {
        Cache_::iterator iter = cache_.begin();
        Cache_::iterator end = cache_.end();
        for ( ; iter != end ; ++iter )
        {
            CacheQueue_& queue = (*iter).second;
            while ( queue.size() > 0 )
            {
                unsigned char *buffer = queue.front();
                queue.pop();
                delete [] buffer;
            }
        }
    }

    void MemoryCache::reuse(unsigned char *buffer, size_t length)
	{
        mc::MutexScope locker( lock_ );
        cache_[length].push(buffer);
	}

    unsigned char* MemoryCache::request(size_t length)
	{
        mc::MutexScope locker( lock_ );
        CacheQueue_& queue = cache_[length];
        if ( 0 != queue.size() )
		{
            unsigned char *buffer = queue.front();
            queue.pop();
			return buffer;
		}
        unsigned char *buffer = new unsigned char[length];
		memset(buffer, 0, length);
        return buffer;
	}
}
}
