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

#if defined(__MACH__)
#include <boost/interprocess/sync/file_lock.hpp>
#include <QApplication>
#endif // Platform
#include "InstanceLock.h"

namespace RemotePC
{

	InstanceLockPtr InstanceLock::SharedLock()
	{
		try
		{			
			return InstanceLockPtr( new InstanceLock() );
		}
		catch ( ... )
		{
			return InstanceLockPtr();
		}
	}
	
#if defined(WIN32)
	InstanceLock::InstanceLock()
		: boost::noncopyable()
		, mutex_(mc::Library::Mutex(false, "RemotePCApp{720215BC-9673-4ab5-9889-6EA255809630}"))
	{
		if ( !mutex_ )
			throw std::exception();
	}
#elif defined(__MACH__)
	InstanceLock::InstanceLock()
		: boost::noncopyable()
		, fileLock_()
	{
		QString appLock
			= QApplication::applicationDirPath()
			+ QString("/rpc_sync");
		fileLock_ = FileLockPtr_(new FileLock_(appLock.toUtf8().constData()));
		if ( !fileLock_->try_lock() )
			throw std::exception();
		fileLock_->lock();
	}
#endif // Platform

	InstanceLock::~InstanceLock()
	{
#if defined(__MACH__)
		if ( fileLock_ )
			fileLock_->unlock();
#endif // Platform
	}

}
