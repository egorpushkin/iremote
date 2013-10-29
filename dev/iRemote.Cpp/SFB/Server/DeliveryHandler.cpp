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
#include "SFB.h"

#include "DeliveryHandler.h"
#include "FBService.h"

namespace RemotePC
{
namespace SFB
{
    DeliveryHandler::DeliveryHandler(IServiceHandlerRef serviceHandler, const DeliveryQueuePtr& deliveryQueue, const MatrixPtr& matrix)
        : mc::CommonImpl< IDeviceHandler >()
        , serviceHandler_(serviceHandler)
        , deliveryQueue_(deliveryQueue)
        , matrix_(matrix)
    {
    }

    mc::result DeliveryHandler::Handle(const IDevicePtr& device)
    {
        ServiceDescriptorPtr descriptor = device->FindDescriptor(serviceHandler_);
        bool notify = descriptor->Properties()->GetProperty(FBService::PROP_SFB_REQUIRED).toBool();
        if ( notify )
        {
            mc::MutexScope locker( descriptor->StreamsLock() );
            if ( descriptor->StreamsCount() > 0 )
            {
                mc::IProtocolPtr stream( descriptor->StreamAtIndex(0) );

                bool fullscreen = descriptor->Properties()->GetProperty(FBService::PROP_SFB_FULLSCREEN).toBool();
                descriptor->Properties()->PutProperty(FBService::PROP_SFB_FULLSCREEN, false);

                deliveryQueue_->deliver(stream, matrix_, fullscreen);
            }
        }
        return mc::_S_OK;
    }
}
}
