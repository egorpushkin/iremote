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

#include "ServiceDescriptor.h"
#include "IDevice.h"
#include "IServiceHandler.h"
#include "Device.h"

namespace MinCOM
{
    // {D0FBDCF9-C6BD-424C-89D9-809F08CF1BFB}
    template<> const Guid TypeInfo< RemotePC::IDevice >::iid_ =
        { 0xD0FBDCF9, 0xC6BD, 0x424C, { 0x89, 0xD9, 0x80, 0x9F, 0x08, 0xCF, 0x1B, 0xFB } };
}

namespace RemotePC
{
    Device::Device(mc::IProtocolRef device)
        : mc::CommonImpl< IDevice >()
        , mc::CommonImpl< mc::DProtocol >()
        , serviceHandler_()
        , device_(device)
        , descriptorsLock_( mc::Library::Mutex() )
        , descriptors_()
        , cacheLock_( mc::Library::Mutex() )
        , cache_()
        , streamsLock_( mc::Library::Mutex() )
    {
    }

    /**
     * IDevice section
     */

    mc::IProtocolPtr Device::GetDevice() const
    {
        return device_;
    }

    void Device::RegisterStream(IServiceHandlerRef handler, mc::IProtocolRef stream)
    {
        // Add stream to local storage.
        ServiceDescriptorPtr descriptor = FindDescriptor(handler);
        descriptor->AddStream(stream);
        CacheStream(stream, handler);

        // Subscribe on stream notifications.
        Subscribe(stream);

        // Notify corresponding service.
        handler->StreamConnected(ClassRoot_::GetSelf(), stream);
    }

    ServiceDescriptorPtr Device::FindDescriptor(IServiceHandlerRef handler)
    {
        mc::MutexScope locker(descriptorsLock_);
        Descriptors_::iterator iter = descriptors_.find(handler->ServiceId());
        if ( descriptors_.end() == iter )
        {
            ServiceDescriptorPtr descriptor(new ServiceDescriptor(handler));
            descriptors_[handler->ServiceId()] = descriptor;
            return descriptor;
        }
        return (*iter).second;
    }

    void Device::KillStreams(mc::RefGuid serviceId)
    {
        // Find service descriptor.
        ServiceDescriptorPtr descriptor;
        {
            mc::MutexScope locker(descriptorsLock_);
            Descriptors_::iterator iter = descriptors_.find(serviceId);
            if ( descriptors_.end() == iter )
                return;
            descriptor = (*iter).second;
        }

        // Close streams.
        mc::MutexScope locker( descriptor->StreamsLock() );
        size_t streams = descriptor->StreamsCount() > 0;
        for ( size_t j = 0 ; j < streams ; ++j )
        {
            mc::IProtocolPtr stream( descriptor->StreamAtIndex(j) );
            Disconnected( stream );
        }
    }

    /**
     * DProtocol section
     */

    mc::result Device::MessageArrived(mc::IProtocolRef stream, mc::IMessageRef message)
    {
        IServiceHandlerPtr handler = HandlerFromCache(stream);
        if ( !handler )
            return mc::_S_FALSE;

        handler->StreamMessage(ClassRoot_::GetSelf(), stream, message);
        return mc::_S_OK;
    }

    mc::result Device::DataErrorOccured(mc::IProtocolRef stream)
    {
        return Disconnected(stream);
    }

    mc::result Device::Disconnected(mc::IProtocolRef stream)
    {
        // Unsubscribe from steam notifications.
        Unsubscribe(stream);

        // Find corresponding service handler.
        IServiceHandlerPtr handler = HandlerFromCache(stream);
        if ( !handler )
            return mc::_S_FALSE;

        // Remove stream from local storage.
        ServiceDescriptorPtr descriptor = FindDescriptor(handler);
        if ( ServiceDescriptorPtr() != descriptor )
        {
            descriptor->RemoveStream(stream);
            ClearCache(stream);
        }

        // Notify handler on disconnection.
        handler->StreamDisconnected(ClassRoot_::GetSelf(), stream);

        // Close connection.
        Close(stream);

        return mc::_S_OK;
    }

    /**
     * Private tools section
     */

    void Device::CacheStream(mc::IProtocolRef stream, IServiceHandlerRef handler)
    {
        mc::MutexScope locker(cacheLock_);
        cache_[stream] = handler;
    }

    IServiceHandlerPtr Device::HandlerFromCache(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(cacheLock_);
        Cache_::iterator iter = cache_.find(stream);
        return ( cache_.end() == iter ) ? ( NULL ) : ( (*iter).second );
    }

    void Device::ClearCache(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(cacheLock_);
        cache_.erase(stream);
    }

    void Device::Subscribe(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(streamsLock_);
        mc::Events::Advise(stream,
            mc::CommonImpl< mc::DProtocol >::GetSelf(),
            mc::TypeInfo< mc::DProtocol >::GetGuid());
    }

    void Device::Unsubscribe(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(streamsLock_);
        mc::Events::Unadvise(stream,
            mc::CommonImpl< mc::DProtocol >::GetSelf(),
            mc::TypeInfo< mc::DProtocol >::GetGuid());
    }

    void Device::Close(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(streamsLock_);
        stream->GetConnection()->Close();
    }
}

