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

#include "BatchHeader.h"

namespace RemotePC
{
namespace SFB
{

    // {571F2836-B4A8-451D-A24E-68D7AF802666}
    const mc::Guid BatchHeader::messageId_ =
        { 0x571F2836, 0xB4A8, 0x451D, { 0xA2, 0x4E, 0x68, 0xD7, 0xAF, 0x80, 0x26, 0x66 } };

    BatchHeader::BatchHeader()
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICEID, mc::TypeInfo< mc::ICommon >::GetGuid());
        mc::MessageImpl::PutProperty(PROP_COUNT, (unsigned int)1);
    }

    BatchHeader::BatchHeader(const mc::Guid& id, size_t count)
        : mc::MessageImpl( messageId_ )
    {
        mc::MessageImpl::PutProperty(PROP_DEVICEID, id);
        mc::MessageImpl::PutProperty(PROP_COUNT, count);
    }

    // IMessage section
    mc::IMessage::Size_ BatchHeader::GetDataSize()
    {
        return
            // Size of device id property
            sizeof( mc::Guid )
            // Size of count property
            + sizeof( size_t );
    }

    // MessageImpl section
    mc::result BatchHeader::WriteBody(std::ostream& stream)
    {
        mc::Guid id = mc::MessageImpl::GetProperty(PROP_DEVICEID).toGuid();
        size_t count = mc::MessageImpl::GetProperty(PROP_COUNT).toUInt();

        mc::MessageImpl::Write(stream, id);
        mc::MessageImpl::Write(stream, count);

        return mc::_S_OK;
    }

    mc::result BatchHeader::ReadBody(std::istream& stream)
    {
        mc::Guid id;
        size_t count = 0;

        mc::MessageImpl::Read(stream, id);
        mc::MessageImpl::Read(stream, count);

        mc::MessageImpl::PutProperty(PROP_DEVICEID, id);
        mc::MessageImpl::PutProperty(PROP_COUNT, count);

        return mc::_S_OK;
    }

}
}

