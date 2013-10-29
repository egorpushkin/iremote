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

#ifndef ISERVICEHANDLER_H__REMOTEPC__INCLUDED_
#define ISERVICEHANDLER_H__REMOTEPC__INCLUDED_

namespace RemotePC
{
    struct IServiceHandler : public mc::ICommon
    {
        virtual mc::Guid ServiceId() = 0;
		
        virtual void SetManager(IServicesManagerRef manager) = 0;		
		
        virtual void DeviceConnected() = 0;
		
        virtual void DeviceDisconnected() = 0;		

        virtual void StreamConnected(mc::IProtocolRef stream) = 0;

        virtual void StreamDisconnected(mc::IProtocolRef stream) = 0;

        virtual void ControlMessage(mc::IMessageRef message) = 0;

        virtual void StreamMessage(mc::IProtocolRef stream, mc::IMessageRef message) = 0;
    };
}

#endif // !ISERVICEHANDLER_H__REMOTEPC__INCLUDED_
