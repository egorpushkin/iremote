/**
 * Copyright (c) 2014 Egor Pushkin. All rights reserved.
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

#include "AppNapSuspender.h"
#include <Foundation/NSProcessInfo.h>

namespace RemotePC
{

    typedef NS_OPTIONS(uint64_t, NSActivityOptions)
    {
        NSActivityIdleSystemSleepDisabled = (1ULL << 20),
        NSActivityUserInitiated = (0x00FFFFFFULL | NSActivityIdleSystemSleepDisabled),
    };

    class AppNapSuspenderPimpl
    {
    public:
        id<NSObject> _activityId;
    };

    AppNapSuspender::AppNapSuspender()
        : _pimpl(new AppNapSuspenderPimpl())
    {
        suspend();
    }

    AppNapSuspender::~AppNapSuspender()
    {
        resume();
        delete _pimpl;
    }

    void AppNapSuspender::suspend()
    {
        if ( _pimpl->_activityId || ![NSProcessInfo instancesRespondToSelector:@selector(beginActivityWithOptions:reason:)] )
        {
            return;
        }
        _pimpl->_activityId = [[NSProcessInfo processInfo] beginActivityWithOptions:NSActivityUserInitiated reason:@"rpc_bg_activity"];
    }

    void AppNapSuspender::resume()
    {
        if ( !_pimpl->_activityId )
        {
            return;
        }
        [[NSProcessInfo processInfo] endActivity:_pimpl->_activityId];
    }

}
