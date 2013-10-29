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

#ifndef FBSERVICE_H__SFB__REMOTEPC__INCLUDED_
#define FBSERVICE_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{
    class FBManager;
    typedef mc::Strong< FBManager > FBManagerPtr;

    class FBService
        : public mc::CommonImpl< IServiceHandler >
    {
    public:

        typedef mc::CommonImpl< IServiceHandler > ClassRoot_;

        static const mc::Guid serviceId_;

        typedef enum tagProperties_
        {
            PROP_SFB_SUPPORT = 0,
            PROP_SFB_REQUIRED,
            PROP_SFB_FULLSCREEN
        }
        Properties_;

    public:

        FBService(IServicesManagerRef servicesManager);

        /**
         * ICommon section
         */

        virtual mc::result PostInit();

        /**
         * IServiceHandler section
         */

        virtual mc::Guid ServiceId();

        virtual bool IsVersionSupported(int major, int minor);

        virtual void ReloadSettings();

        virtual void DeviceConnected(const IDevicePtr& device, mc::IMessageRef authMessage);

        virtual void DeviceDisconnected(const IDevicePtr& device);

        virtual void ServiceRequested(const IDevicePtr& device);

        virtual void StreamConnected(const IDevicePtr& device, mc::IProtocolRef stream);

        virtual void StreamDisconnected(const IDevicePtr& device, mc::IProtocolRef stream);

        virtual void ControlMessage(const IDevicePtr& device, mc::IMessageRef message);

        virtual void StreamMessage(const IDevicePtr& device, mc::IProtocolRef stream, mc::IMessageRef message);

    private:

        /**
         * Manager control
         */

        void ConfigureManager();

        void TriggerManagerStatus();

        /**
         * Messages section
         */

        mc::result HandleDeviceRequest(const IDevicePtr& device, mc::IProtocolRef stream, mc::IMessageRef message);

        mc::result HandleControlMessage(const IDevicePtr& device, mc::IProtocolRef stream, mc::IMessageRef message);

    private:

        IServicesManagerWeak servicesManager_;

        /**
         * SFB engine.
         */
        SFB::FBManagerPtr fbManager_;

        bool enabled_;

    };
}
}

#endif // !FBSERVICE_H__SFB__REMOTEPC__INCLUDED_
