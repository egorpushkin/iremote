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

#include "Services.h"

namespace RemotePC
{
    ServiceDescriptor::ServiceDescriptor(IServiceHandlerRef serviceHandler)
        : serviceHandler_(serviceHandler)
        , properties_( mc::Class< mc::MessageImpl >::Create( mc::Guid() ) )
        , streamsLock_( mc::Library::Mutex() )
        , streams_()
    {
    }

    IServiceHandlerPtr ServiceDescriptor::Handler() const
    {
        return serviceHandler_;
    }

    mc::IMessagePtr ServiceDescriptor::Properties() const
    {
        return properties_;
    }

    mc::IMutexPtr ServiceDescriptor::StreamsLock() const
    {
        return streamsLock_;
    }

    size_t ServiceDescriptor::StreamsCount() const
    {
        mc::MutexScope locker(streamsLock_);
        return streams_.size();
    }

    mc::IProtocolPtr ServiceDescriptor::StreamAtIndex(size_t index) const
    {
        mc::MutexScope locker(streamsLock_);
        return streams_.at(index);
    }

    void ServiceDescriptor::AddStream(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(streamsLock_);
        streams_.push_back(stream);
    }

    void ServiceDescriptor::RemoveStream(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(streamsLock_);
        Streams_::iterator iter = std::find(streams_.begin(), streams_.end(), stream);
        if ( streams_.end() == iter )
            return;
        streams_.erase(iter);
    }
}
