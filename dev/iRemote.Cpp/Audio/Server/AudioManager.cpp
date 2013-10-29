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

#include "AudioManager.h"

#include "AudioDeviceList.h"
#include "AudioThruEngine.h"

namespace RemotePC
{
namespace Audio
{
    AudioManager& AudioManager::Instance()
    {
        return Loki::SingletonHolder< AudioManager >::Instance();
    }

    AudioManager::AudioManager()
    {
        engine = new AudioThruEngine();

        AudioDeviceList mOutputDeviceList(false);
        AudioDeviceList::DeviceList &thelist = mOutputDeviceList.GetList();
        int index = 0;
        for (AudioDeviceList::DeviceList::iterator i = thelist.begin(); i != thelist.end(); ++i, ++index)
        {
            if (0 == strcmp("Soundflower (2ch)", (*i).mName))
            {
                // mSoundflower2Device = (*i).mID;
                engine->SetInputDevice((*i).mID);
                break;
            }
        }
    }

    void AudioManager::SetClient(mc::IProtocolRef client)
    {
        client_ = client;
    }

    mc::IProtocolPtr AudioManager::GetClient()
    {
        return client_;
    }
}
}
