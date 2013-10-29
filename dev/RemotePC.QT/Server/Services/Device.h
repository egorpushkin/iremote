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

#ifndef DEVICE_H__REMOTEPC__INCLUDED_
#define DEVICE_H__REMOTEPC__INCLUDED_

namespace RemotePC
{
    class Device
        : public mc::CommonImpl< IDevice >
        , public mc::CommonImpl< mc::DProtocol >
    {
    public:
        typedef mc::CommonImpl< IDevice > ClassRoot_;

        Device(mc::IProtocolRef device);

        /**
         * IDevice section
         */

        virtual mc::IProtocolPtr GetDevice() const;

        virtual void RegisterStream(IServiceHandlerRef handler, mc::IProtocolRef stream);

        virtual ServiceDescriptorPtr FindDescriptor(IServiceHandlerRef handler);

        virtual void KillStreams(mc::RefGuid serviceId);

        /**
         * DProtocol section
         */

        virtual mc::result MessageArrived(mc::IProtocolRef stream, mc::IMessageRef message);

        virtual mc::result DataErrorOccured(mc::IProtocolRef stream);

        virtual mc::result Disconnected(mc::IProtocolRef stream);


    private:

        /**
         * Private tools section
         */

        void CacheStream(mc::IProtocolRef stream, IServiceHandlerRef handler);

        IServiceHandlerPtr HandlerFromCache(mc::IProtocolRef stream);

        void ClearCache(mc::IProtocolRef stream);

        void Subscribe(mc::IProtocolRef stream);

        void Unsubscribe(mc::IProtocolRef stream);

        void Close(mc::IProtocolRef stream);

    private:

        IServiceHandlerPtr serviceHandler_;

        mc::IProtocolPtr device_;

        mc::IMutexPtr descriptorsLock_;

        typedef std::map< mc::Guid, ServiceDescriptorPtr > Descriptors_;

        Descriptors_ descriptors_;

        mc::IMutexPtr cacheLock_;

        typedef std::map< mc::IProtocolPtr, IServiceHandlerPtr > Cache_;

        Cache_ cache_;

        mc::IMutexPtr streamsLock_;
    };
}

#endif // !DEVICE_H__REMOTEPC__INCLUDED_
