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
#include "IServicesManager.h"
#include "IServiceHandler.h"
#include "ServicesManager.h"
#include "StreamContext.h"

#include "Messages/StreamRequest.h"

namespace MinCOM
{
	// {C22D0BA5-A37B-46B4-BE10-9C603F8583BF}
    template<> const Guid TypeInfo< RemotePC::IServicesManager >::iid_ =
		{ 0xC22D0BA5, 0xA37B, 0x46B4, { 0xBE, 0x10, 0x9C, 0x60, 0x3F, 0x85, 0x83, 0xBF } };
	
	// {B68D4BCE-5C4B-47AD-AE29-31F7132D9E83}
    template<> const Guid TypeInfo< RemotePC::IServiceHandler >::iid_ =
		{ 0xB68D4BCE, 0x5C4B, 0x47AD, { 0xAE, 0x29, 0x31, 0xF7, 0x13, 0x2D, 0x9E, 0x83 } };		
}

namespace RemotePC
{
    ServicesManager::ServicesManager(mc::IServiceRef service)
        : mc::CommonImpl< IServicesManager >()
        , mc::CommonImpl< mc::DProtocol >()
		, service_(service)
		, deviceLock_( mc::Library::Mutex() )
        , device_()
		, ip_()
        , handlersLock_( mc::Library::Mutex() )
		, handlers_()	
        , descriptorsLock_( mc::Library::Mutex() )
        , descriptors_()
        , cacheLock_( mc::Library::Mutex() )
        , cache_()
        , streamsLock_( mc::Library::Mutex() )
	    , requestsLock_( mc::Library::Mutex() )
	    , requests_()
    {
    }

    /**
     * IServicesManager section
     */
	
	void ServicesManager::RegisterService(IServiceHandlerRef serviceHandler)
	{
        mc::MutexScope locker(handlersLock_);
		// Initialize service.
		serviceHandler->SetManager( ClassRoot_::GetSelf() );
		// Save service in local storage.
        handlers_[serviceHandler->ServiceId()] = serviceHandler;
	}
	
	void ServicesManager::UnregisterService(IServiceHandlerRef serviceHandler)
	{
		// Remove service from local cache.
		{
			mc::MutexScope locker(handlersLock_);
			Handlers_::iterator iter = handlers_.find(serviceHandler->ServiceId());
			if ( handlers_.end() == iter )
				return;
			handlers_.erase(iter);
		}
		// Kill service streams.
		KillStreams(serviceHandler->ServiceId());
		// Cleanup service.
		serviceHandler->SetManager(NULL);		
	}	
	
	void ServicesManager::DeviceConnected(mc::IProtocolRef device, const std::string& ip)
	{
		// Remember device.
		{
			mc::MutexScope locker(deviceLock_);				
			device_ = device;
			ip_ = ip;
		}
		
		// Notify handlers.
		{
			mc::MutexScope locker(handlersLock_);	
			Handlers_::iterator iter = handlers_.begin();
			Handlers_::iterator end = handlers_.end();			
			for ( ; iter != end ; ++iter )
			{
				IServiceHandlerPtr handler( (*iter).second );
				handler->DeviceConnected();
			}
		}		
	}
	
	void ServicesManager::DeviceDisconnected(mc::IProtocolRef device)
	{
		// Forget device.
		{
			mc::MutexScope locker(deviceLock_);				
			if ( !device_ ) 
				return;
			device_ = NULL;		
		}
		
		// Shutdown all current streams. 		
		{
			mc::MutexScope locker(descriptorsLock_);				
			Descriptors_::iterator iter = descriptors_.begin();
			Descriptors_::iterator end = descriptors_.end();		
			for ( ; iter != end ; ++iter )
			{
				ServiceDescriptorPtr descriptor( (*iter).second );
				KillStreams(descriptor);
			}
		}
		
		// Cleanup caches. 
		ClearCache();		
		ClearRequests();
		
		// Notify handlers.
		{
			mc::MutexScope locker(handlersLock_);	
			Handlers_::iterator iter = handlers_.begin();
			Handlers_::iterator end = handlers_.end();			
			for ( ; iter != end ; ++iter )
			{
				IServiceHandlerPtr handler( (*iter).second );
				handler->DeviceDisconnected();
			}
		}				
	}
	
	void ServicesManager::ControlMessage(mc::IMessageRef message)
	{
		// Handle message locally. 
		mc::Guid messageId = message->GetCode();
		if ( StreamRequest::messageId_ == messageId )
		{
			HandleStreamRequest(message);
		}		
		
		// Notify handlers.
		{
			mc::MutexScope locker(handlersLock_);	
			Handlers_::iterator iter = handlers_.begin();
			Handlers_::iterator end = handlers_.end();			
			for ( ; iter != end ; ++iter )
			{
				IServiceHandlerPtr handler( (*iter).second );
				handler->ControlMessage(message);
			}
		}
	}

    mc::IProtocolPtr ServicesManager::GetDevice() const
    {
        return device_;
    }
	
	void ServicesManager::RequestStream(IServiceHandlerRef handler)
	{
		if ( !IsDeviceConnected() )
			return;
		
		mc::Guid serviceId(handler->ServiceId());
		if ( IsRequesting(serviceId) )
			// Already requesting stream for this service.
			return;		
		
		// Remember that we are requesting stream for the serivce.
		SetRequesting(serviceId, true);
		// Request service descriptor, where we store session cookie. 
		ServiceDescriptorPtr descriptor( FindDescriptor(handler) );		
		
		// Perform request in background.
		mc::IThreadPtr context( mc::Library::Thread() );
		context->SetContext( mc::Class< StreamContext >::Create(
			ClassRoot_::GetSelf(), handler, service_, ip_, descriptor->StreamRequest(), context) );
		context->Start();		
	}

    void ServicesManager::RegisterStream(IServiceHandlerRef handler, mc::IProtocolRef stream)
    {
		// Skip requesting flag for the service.
		mc::Guid serviceId(handler->ServiceId());
		SetRequesting(serviceId, false);
		
        // Add stream to local storage.
        ServiceDescriptorPtr descriptor = FindDescriptor(handler);
        descriptor->AddStream(stream);
        CacheStream(stream, handler);

        // Subscribe on stream notifications.
        Subscribe(stream);

        // Notify corresponding service.
        handler->StreamConnected(stream);
    }
	
    void ServicesManager::RequestFailed(IServiceHandlerRef handler)
	{
		// Skip requesting flag for the service.
		mc::Guid serviceId(handler->ServiceId());
		SetRequesting(serviceId, false);		
	}

    ServiceDescriptorPtr ServicesManager::FindDescriptor(IServiceHandlerRef handler)
    {
        mc::MutexScope locker(descriptorsLock_);
		mc::Guid serviceId = handler->ServiceId();
        Descriptors_::iterator iter = descriptors_.find(serviceId);
        if ( descriptors_.end() == iter )
        {
			ServiceDescriptorPtr descriptor(new ServiceDescriptor(handler));
			descriptors_[serviceId] = descriptor;
            return descriptor;
        }
        return (*iter).second;
    }

    /**
     * DProtocol section
     */

    mc::result ServicesManager::MessageArrived(mc::IProtocolRef stream, mc::IMessageRef message)
    {
        IServiceHandlerPtr handler = HandlerFromCache(stream);
        if ( !handler )
            return mc::_S_FALSE;

        handler->StreamMessage(stream, message);
        return mc::_S_OK;
    }

    mc::result ServicesManager::DataErrorOccured(mc::IProtocolRef stream)
    {
        return Disconnected(stream);
    }

    mc::result ServicesManager::Disconnected(mc::IProtocolRef stream)
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
        handler->StreamDisconnected(stream);

        // Close connection.
        Close(stream);

        return mc::_S_OK;
    }
	
	/**
	 * Control message handlers
	 */
	
	void ServicesManager::HandleStreamRequest(mc::IMessageRef message)
	{
		// Attempt to acquire corresponding service handler.
		IServiceHandlerPtr handler;
		{			
			mc::MutexScope locker(handlersLock_);			
			mc::Guid serviceId = message->GetProperty(StreamRequest::PROP_ID).toGuid();
			handler = handlers_[serviceId];
		}
		if ( !handler )
			return;
		ServiceDescriptorPtr descriptor( FindDescriptor(handler) );
		
		// Services manager should only remember cookie and port from StreamRequest
		// message for future reference. Services decide to establish streams or not
		// on their own.
		descriptor->SetStreamRequest(message);
	}

    /**
     * Private tools section
     */
	
	bool ServicesManager::IsDeviceConnected()
	{
		mc::MutexScope locker(deviceLock_);
		return ( NULL != device_ );
	}
	
    void ServicesManager::KillStreams(mc::RefGuid serviceId)
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
		
        // Close streams of specific service.
		KillStreams(descriptor);
    }
	
	void ServicesManager::KillStreams(const ServiceDescriptorPtr& descriptor)
	{
		while ( descriptor->StreamsCount() > 0 )
		{
			mc::IProtocolPtr stream( descriptor->StreamAtIndex(0) );
			if ( !stream )
				continue;
			Disconnected( stream );
		}					
	}

    void ServicesManager::CacheStream(mc::IProtocolRef stream, IServiceHandlerRef handler)
    {
        mc::MutexScope locker(cacheLock_);
        cache_[stream] = handler;
    }

    IServiceHandlerPtr ServicesManager::HandlerFromCache(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(cacheLock_);
        Cache_::iterator iter = cache_.find(stream);
        return ( cache_.end() == iter ) ? ( NULL ) : ( (*iter).second );
    }
	
	void ServicesManager::ClearCache()	
	{
		mc::MutexScope locker(cacheLock_);
		cache_.clear();
	}

    void ServicesManager::ClearCache(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(cacheLock_);
        cache_.erase(stream);
    }

    void ServicesManager::Subscribe(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(streamsLock_);
        mc::Events::Advise(stream,
            mc::CommonImpl< mc::DProtocol >::GetSelf(),
            mc::TypeInfo< mc::DProtocol >::GetGuid());
    }

    void ServicesManager::Unsubscribe(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(streamsLock_);
        mc::Events::Unadvise(stream,
            mc::CommonImpl< mc::DProtocol >::GetSelf(),
            mc::TypeInfo< mc::DProtocol >::GetGuid());
    }

    void ServicesManager::Close(mc::IProtocolRef stream)
    {
        mc::MutexScope locker(streamsLock_);
        stream->GetConnection()->Close();
    }
	
	bool ServicesManager::IsRequesting(mc::RefGuid serviceId)
	{
		mc::MutexScope locker(requestsLock_);
		return ( requests_.end() != requests_.find(serviceId) );
	}
	
	void ServicesManager::SetRequesting(mc::RefGuid serviceId, bool requesting)
	{
		mc::MutexScope locker(requestsLock_);
		if ( requesting )
		{
			requests_.insert(serviceId);
		}
		else
		{
			requests_.erase(serviceId);
		}
	}
	
	void ServicesManager::ClearRequests()
	{
		requests_.clear();
	}
}

