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

#ifndef SERVICESMANAGER_H__REMOTEPC__INCLUDED_
#define SERVICESMANAGER_H__REMOTEPC__INCLUDED_

namespace RemotePC
{
    class ServicesManager
        : public mc::CommonImpl< IServicesManager >
    {
    public:

        ServicesManager();

        /**
         * IServicesManager section
         */

        virtual void SetPort(const std::string& port);

        virtual void RegisterService(IServiceHandlerRef serviceHandler);

        virtual void UnregisterService(IServiceHandlerRef serviceHandler);

        virtual void DeviceConnected(mc::IProtocolRef device, mc::IMessageRef authMessage);

        virtual void DeviceDisconnected(mc::IProtocolRef device);

        virtual void StreamConnected(mc::IProtocolRef stream, mc::IMessageRef message);

        virtual void ControlMessage(mc::IProtocolRef device, mc::IMessageRef message);

        virtual void Handle(IDeviceHandlerRef handler);

        virtual mc::IMutexPtr DevicesLock();

        virtual size_t DevicesCount();

        virtual IDevicePtr DeviceAtIndex(size_t index);

    private:

        /**
         * Message handlers
         */

        void HandleServiceRequest(mc::IProtocolRef device, mc::IMessageRef message);

        /**
         * Private tools section
         */

        IServiceHandlerPtr FindHandler(mc::RefGuid serviceId);

        IDevicePtr AddDevice(mc::IProtocolRef device);

        IDevicePtr FindDevice(mc::IProtocolRef device);

        void RemoveDevice(const IDevicePtr& device);

        mc::Guid RegisterRequest(const IDevicePtr& device);

        IDevicePtr FindDevice(mc::RefGuid cookie);

    private:

        std::string port_;

        mc::IMutexPtr handlersLock_;

        typedef std::map< mc::Guid, IServiceHandlerPtr > Handlers_;

        Handlers_ handlers_;

        mc::IMutexPtr devicesLock_;

        typedef std::vector< IDevicePtr > Devices_;

        Devices_ devices_;

        mc::IMutexPtr cookiesLock_;

        typedef std::map< mc::Guid, IDevicePtr > Cookies_;

        Cookies_ cookies_;

    };
}

#endif // !SERVICESMANAGER_H__REMOTEPC__INCLUDED_
