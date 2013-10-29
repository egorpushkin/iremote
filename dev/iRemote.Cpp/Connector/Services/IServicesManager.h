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

#ifndef ISERVICESMANAGER_H__REMOTEPC__INCLUDED_
#define ISERVICESMANAGER_H__REMOTEPC__INCLUDED_

namespace RemotePC
{
    class ServiceDescriptor;
    typedef boost::shared_ptr< ServiceDescriptor > ServiceDescriptorPtr;

    struct IServicesManager : public mc::ICommon
    {
		virtual void RegisterService(IServiceHandlerRef serviceHandler) = 0;
		
        virtual void UnregisterService(IServiceHandlerRef serviceHandler) = 0;	
		
		virtual void DeviceConnected(mc::IProtocolRef device, const std::string& ip) = 0;
		
		virtual void DeviceDisconnected(mc::IProtocolRef device) = 0;		
		
        virtual void ControlMessage(mc::IMessageRef message) = 0;		
		
        virtual mc::IProtocolPtr GetDevice() const = 0;
		
		virtual void RequestStream(IServiceHandlerRef handler) = 0;

        virtual void RegisterStream(IServiceHandlerRef handler, mc::IProtocolRef stream) = 0;
		
        virtual void RequestFailed(IServiceHandlerRef handler) = 0;		

        virtual ServiceDescriptorPtr FindDescriptor(IServiceHandlerRef handler) = 0;
    };

    typedef mc::Strong< IServicesManager > IServicesManagerPtr;
    typedef mc::Weak< IServicesManager > IServicesManagerWeak;	
    typedef const IServicesManagerPtr& IServicesManagerRef;
}

#endif // !ISERVICESMANAGER_H__REMOTEPC__INCLUDED_
