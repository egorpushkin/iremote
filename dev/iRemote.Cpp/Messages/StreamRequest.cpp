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

#include "StreamRequest.h"

namespace RemotePC
{
    // {6D967CD5-537E-4045-895C-53D0E79EAB7E}
    const mc::Guid StreamRequest::messageId_ =
        { 0x6D967CD5, 0x537E, 0x4045, { 0x89, 0x5C, 0x53, 0xD0, 0xE7, 0x9E, 0xAB, 0x7E } };

    StreamRequest::StreamRequest()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_ID, mc::TypeInfo< mc::ICommon >::GetGuid());
        mc::MessageImpl::PutProperty(PROP_PORT, "");
        mc::MessageImpl::PutProperty(PROP_COOKIE, mc::TypeInfo< mc::ICommon >::GetGuid());
    }

    StreamRequest::StreamRequest( const mc::Guid& id, const std::string& port, const mc::Guid& cookie )
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_ID, id);
        mc::MessageImpl::PutProperty(PROP_PORT, port);
        mc::MessageImpl::PutProperty(PROP_COOKIE, cookie);
    }

    // IMessage section
    mc::IMessage::Size_ StreamRequest::GetDataSize()
    {
        return
            // Size of id property
            sizeof( mc::Guid ) +
            // Size of port property
            sizeof( size_t ) + mc::MessageImpl::SizeOf( mc::MessageImpl::GetProperty(PROP_PORT).toString() ) +
            // Size of cookie id
            sizeof( mc::Guid );
    }

    // MessageImpl section
    mc::result StreamRequest::WriteBody(std::ostream& stream)
    {
        mc::Guid id = mc::MessageImpl::GetProperty(PROP_ID).toGuid();
        std::string port = mc::MessageImpl::GetProperty(PROP_PORT).toString();
        mc::Guid cookie = mc::MessageImpl::GetProperty(PROP_COOKIE).toGuid();

        mc::MessageImpl::Write(stream, id);
        mc::MessageImpl::Write(stream, port);
        mc::MessageImpl::Write(stream, cookie);

        return mc::_S_OK;
    }

    mc::result StreamRequest::ReadBody(std::istream& stream)
    {
        mc::Guid id;
        std::string port;
        mc::Guid cookie;

        mc::MessageImpl::Read(stream, id);
        mc::MessageImpl::Read(stream, port);
        mc::MessageImpl::Read(stream, cookie);

        mc::MessageImpl::PutProperty(PROP_ID, id);
        mc::MessageImpl::PutProperty(PROP_PORT, port);
        mc::MessageImpl::PutProperty(PROP_COOKIE, cookie);

        return mc::_S_OK;
    }
}
