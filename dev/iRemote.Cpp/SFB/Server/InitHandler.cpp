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

#include "../Server/Services/Services.h"
#include "../Server/Config.h"

#include "InitHandler.h"
#include "FBService.h"

// SFB messaging
#include "../Messages/InitMessage.h"

namespace RemotePC
{
namespace SFB
{
    InitHandler::InitHandler(IServiceHandlerRef serviceHandler)
        : mc::CommonImpl< IDeviceHandler >()
        , serviceHandler_(serviceHandler)
    {
    }

    mc::result InitHandler::Handle(const IDevicePtr& device)
    {
        ServiceDescriptorPtr descriptor = device->FindDescriptor(serviceHandler_);
        bool notify = descriptor->Properties()->GetProperty(FBService::PROP_SFB_REQUIRED).toBool();
        if ( notify )
        {
            mc::MutexScope locker( descriptor->StreamsLock() );
            if ( descriptor->StreamsCount() > 0 )
            {
                mc::IProtocolPtr stream( descriptor->StreamAtIndex(0) );
                stream->Send( InitMessage() );
            }
        }
        return mc::_S_OK;
    }

    mc::IMessagePtr InitHandler::InitMessage()
    {
        size_t cellSize = Config::Instance().GetSFBCellSize();
        return mc::IMessagePtr( mc::Class< SFB::InitMessage >::Create(cellSize, "") );
    }
}
}
