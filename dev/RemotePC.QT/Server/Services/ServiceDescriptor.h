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

#ifndef SERVICEDESCRIPTOR_H__REMOTEPC__INCLUDED_
#define SERVICEDESCRIPTOR_H__REMOTEPC__INCLUDED_

namespace RemotePC
{
    struct IServiceHandler;
    typedef mc::Strong< IServiceHandler > IServiceHandlerPtr;
    typedef mc::Weak< IServiceHandler > IServiceHandlerWeak;
    typedef const IServiceHandlerPtr& IServiceHandlerRef;

    class ServiceDescriptor
    {
    public:

        ServiceDescriptor(IServiceHandlerRef serviceHandler);

        IServiceHandlerPtr Handler() const;

        mc::IMessagePtr Properties() const;

        mc::IMutexPtr StreamsLock() const;

        size_t StreamsCount() const;

        mc::IProtocolPtr StreamAtIndex(size_t index) const;

        void AddStream(mc::IProtocolRef stream);

        void RemoveStream(mc::IProtocolRef stream);

    private:

        IServiceHandlerPtr serviceHandler_;

        mc::IMessagePtr properties_;

        mc::IMutexPtr streamsLock_;

        typedef std::vector< mc::IProtocolPtr > Streams_;

        Streams_ streams_;

    };
}

#endif // !SERVICEDESCRIPTOR_H__REMOTEPC__INCLUDED_
