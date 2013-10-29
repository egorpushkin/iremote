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
#include "ServicesManager.h"

#include "Device.h"
#include "Messages/Messages.h"
#include "Messages/ServiceRequest.h"
#include "Messages/StreamRequest.h"

namespace MinCOM
{
    // {FFA9AD48-FA68-4220-A50A-42DCE8372261}
    template<> const Guid TypeInfo< RemotePC::IServicesManager >::iid_ =
        { 0xFFA9AD48, 0xFA68, 0x4220, { 0xA5, 0x0A, 0x42, 0xDC, 0xE8, 0x37, 0x22, 0x61 } };

    // {B2775695-F8F0-4298-A1A4-A00FE32CF7FA}
    template<> const Guid TypeInfo< RemotePC::IServiceHandler >::iid_ =
        { 0xB2775695, 0xF8F0, 0x4298, { 0xA1, 0xA4, 0xA0, 0x0F, 0xE3, 0x2C, 0xF7, 0xFA } };

    // {0B418E7C-D1AA-4741-8AAA-BBADE9EB5713}
    template<> const Guid TypeInfo< RemotePC::IDeviceHandler >::iid_ =
        { 0x0B418E7C, 0xD1AA, 0x4741, { 0x8A, 0xAA, 0xBB, 0xAD, 0xE9, 0xEB, 0x57, 0x13 } };
}

namespace RemotePC
{
    ServicesManager::ServicesManager()
        : mc::CommonImpl< IServicesManager >()
        , port_()
        , handlersLock_( mc::Library::Mutex() )
        , handlers_()
        , devicesLock_( mc::Library::Mutex() )
        , devices_()
        , cookiesLock_( mc::Library::Mutex() )
        , cookies_()
    {
    }

    /**
     * IServicesManager section
     */

    void ServicesManager::SetPort(const std::string& port)
    {
        port_ = port;
    }

    void ServicesManager::RegisterService(IServiceHandlerRef serviceHandler)
    {
        mc::MutexScope locker(handlersLock_);
        handlers_[serviceHandler->ServiceId()] = serviceHandler;
    }

    void ServicesManager::UnregisterService(IServiceHandlerRef serviceHandler)
    {
        mc::MutexScope locker(handlersLock_);
        Handlers_::iterator iter = handlers_.find(serviceHandler->ServiceId());
        if ( handlers_.end() == iter )
            return;
        handlers_.erase(iter);
    }

    void ServicesManager::DeviceConnected(mc::IProtocolRef device, mc::IMessageRef authMessage)
    {
        // Add device.
        IDevicePtr deviceDescr( AddDevice(device) );

        // Notify handlers.
        {
            mc::MutexScope locker(handlersLock_);
            Handlers_::iterator iter = handlers_.begin();
            Handlers_::iterator end = handlers_.end();
            for ( ; iter != end ; ++iter )
            {
                (*iter).second->DeviceConnected(deviceDescr, authMessage);
            }
        }
    }

    void ServicesManager::DeviceDisconnected(mc::IProtocolRef device)
    {
        // Find the device.
        IDevicePtr deviceDescr = FindDevice(device);
        if ( !deviceDescr )
            return;

        // Remove device.
        RemoveDevice(deviceDescr);

        // Notify handlers.
        {
            mc::MutexScope locker(handlersLock_);
            Handlers_::iterator iter = handlers_.begin();
            Handlers_::iterator end = handlers_.end();
            for ( ; iter != end ; ++iter )
            {
                IServiceHandlerPtr serviceHandler((*iter).second);
                // Notify handler.
                serviceHandler->DeviceDisconnected(deviceDescr);
                // Close all streams for the device.
                deviceDescr->KillStreams(serviceHandler->ServiceId());
            }
        }
    }

    void ServicesManager::StreamConnected(mc::IProtocolRef stream, mc::IMessageRef message)
    {
        // Find owner device via cookie.
        mc::Guid cookie = message->GetProperty(StreamRequest::PROP_COOKIE).toGuid();
        IDevicePtr deviceDescr = FindDevice(cookie);

        // Refuse connection, if cookie is wrong.
        if ( !deviceDescr )
        {
            // Notify stream on failure.
            stream->Send( mc::Class< mc::MessageImpl >::Create( Messages::streamFailure_ ) );
            // Forcibly terminate associated connection.
            stream->GetConnection()->Close();
            return;
        }

        // Handle stream.
        mc::Guid serviceId = message->GetProperty(StreamRequest::PROP_ID).toGuid();
        IServiceHandlerPtr handler( FindHandler(serviceId) );
        if ( !handler )
        {
            // Notify stream on failure.
            stream->Send( mc::Class< mc::MessageImpl >::Create( Messages::streamFailure_ ) );
            // Forcibly terminate associated connection.
            stream->GetConnection()->Close();
            return;
        }

        // Notify stream on success.
        stream->Send( mc::Class< mc::MessageImpl >::Create( Messages::streamSuccess_ ) );

        // Register stream locally.
        deviceDescr->RegisterStream(handler, stream);
    }

    void ServicesManager::ControlMessage(mc::IProtocolRef device, mc::IMessageRef message)
    {
        // Find the device.
        IDevicePtr deviceDescr = FindDevice(device);
        if ( !deviceDescr )
            return;

        // Handle message locally.
        mc::Guid messageId = message->GetCode();
        if ( ServiceRequest::messageId_ == messageId )
        {
            HandleServiceRequest(device, message);
        }

        // Notify handlers.
        {
            mc::MutexScope locker(handlersLock_);
            Handlers_::iterator iter = handlers_.begin();
            Handlers_::iterator end = handlers_.end();
            for ( ; iter != end ; ++iter )
            {
                (*iter).second->ControlMessage(deviceDescr, message);
            }
        }
    }

    void ServicesManager::Handle(IDeviceHandlerRef handler)
    {
        mc::MutexScope locker(devicesLock_);
        Devices_::iterator iter = devices_.begin();
        Devices_::iterator end = devices_.end();
        for ( ; iter != end ; ++iter )
        {
            IDevicePtr deviceDescr(*iter);
            handler->Handle( deviceDescr );
        }
    }

    mc::IMutexPtr ServicesManager::DevicesLock()
    {
        return devicesLock_;
    }

    size_t ServicesManager::DevicesCount()
    {
        return devices_.size();
    }

    IDevicePtr ServicesManager::DeviceAtIndex(size_t index)
    {
        return devices_.at(index);
    }

    /**
     * Message handlers
     */

    void ServicesManager::HandleServiceRequest(mc::IProtocolRef device, mc::IMessageRef message)
    {
        mc::Guid serviceId = message->GetProperty(ServiceRequest::PROP_ID).toGuid();
        int verMajor = message->GetProperty(ServiceRequest::PROP_VER_MAJOR).toInt();
        int verMinor = message->GetProperty(ServiceRequest::PROP_VER_MINOR).toInt();

        // Find service handler.
        IServiceHandlerPtr serviceHandler = FindHandler(serviceId);
        if ( !serviceHandler )
        {
            device->Send( mc::Class< mc::MessageImpl >::Create( Messages::serviceFailure_ ) );
            return;
        }

        // Check whether service version is supported.
        if ( !serviceHandler->IsVersionSupported(verMajor, verMinor) )
        {
            device->Send( mc::Class< mc::MessageImpl >::Create( Messages::serviceFailure_ ) );
            return;
        }

        // Find a device.
        IDevicePtr deviceDescr = FindDevice(device);
        if ( !deviceDescr )
        {
            device->Send( mc::Class< mc::MessageImpl >::Create( Messages::serviceFailure_ ) );
            return;
        }

        // Associate cookie with a device.
        mc::Guid cookie = RegisterRequest(deviceDescr);

        // Send confirmation to the device.
        mc::IMessagePtr serviceResponse( mc::Class< StreamRequest >::Create(
            serviceId, port_, cookie) );
        device->Send(serviceResponse);

        // Notify service on request.
        serviceHandler->ServiceRequested(deviceDescr);
    }

    /**
     * Private tools section
     */

    IServiceHandlerPtr ServicesManager::FindHandler(mc::RefGuid serviceId)
    {
        mc::MutexScope locker(handlersLock_);
        Handlers_::iterator iter = handlers_.find(serviceId);
        return ( handlers_.end() == iter ) ? ( NULL ) : ( (*iter).second );
    }

    IDevicePtr ServicesManager::AddDevice(mc::IProtocolRef device)
    {
        mc::MutexScope locker(devicesLock_);
        IDevicePtr deviceDescr( mc::Class< Device >::Create( device ) );
        devices_.push_back(deviceDescr);
        return deviceDescr;
    }

    IDevicePtr ServicesManager::FindDevice(mc::IProtocolRef device)
    {
        mc::MutexScope locker(devicesLock_);
        Devices_::iterator iter = devices_.begin();
        Devices_::iterator end = devices_.end();
        for ( ; iter != end ; ++iter )
        {
            IDevicePtr deviceDescr(*iter);
            if ( deviceDescr->GetDevice() == device )
                return deviceDescr;
        }
        return NULL;
    }

    void ServicesManager::RemoveDevice(const IDevicePtr& device)
    {
        {
            // Remove from the list of devices.
            mc::MutexScope locker(devicesLock_);
            Devices_::iterator iter = devices_.begin();
            Devices_::iterator end = devices_.end();
            for ( ; iter != end ; ++iter )
            {
                IDevicePtr deviceDescr(*iter);
                if ( deviceDescr == device )
                {
                    devices_.erase(iter);
                    break;
                }
            }
        }
        {
            // Check for associated records in cookies cache.
            mc::MutexScope locker(cookiesLock_);
            Cookies_::iterator iter = cookies_.begin();
            Cookies_::iterator end = cookies_.end();
            for ( ; iter != end ; )
            {
                if ( (*iter).second == device )
                {
                    // As cookie is associated with a device, it is enought
                    // to find and erathe the first entrance.
                    cookies_.erase(iter);
                    break;
                }
            }
        }
    }

    /**
     * Cookie is unique per device.
     */
    mc::Guid ServicesManager::RegisterRequest(const IDevicePtr& device)
    {
        mc::MutexScope locker(cookiesLock_);
        // Try to find existing cookie.
        Cookies_::iterator iter = cookies_.begin();
        Cookies_::iterator end = cookies_.end();
        for ( ; iter != end ; )
        {
            IDevicePtr currDevice( (*iter).second );
            if ( (*iter).second == device )
            {
                return (*iter).first;
            }
        }
        // Allocate new cookie.
        mc::Guid cookie = mc::Guid::Generate();
        cookies_[cookie] = device;
        return cookie;
    }

    IDevicePtr ServicesManager::FindDevice(mc::RefGuid cookie)
    {
        mc::MutexScope locker(cookiesLock_);
        Cookies_::iterator iter = cookies_.find(cookie);
        if ( cookies_.end() == iter )
            return NULL;
        IDevicePtr deviceDescr = (*iter).second;
        cookies_.erase(iter);
        return deviceDescr;
    }
}
