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

#include "AudioDeviceList.h"
#include "AudioDevice.h"

AudioDeviceList::AudioDeviceList(bool inputs) :
	mInputs(inputs)
{
	BuildList();
}

AudioDeviceList::~AudioDeviceList()
{
}

void	AudioDeviceList::BuildList()
{
	mDevices.clear();
	
	UInt32 propsize;
	
	verify_noerr(AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &propsize, NULL));
	int nDevices = propsize / sizeof(AudioDeviceID);	
	AudioDeviceID *devids = new AudioDeviceID[nDevices];
	verify_noerr(AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &propsize, devids));
	
	for (int i = 0; i < nDevices; ++i) {
		AudioDevice dev(devids[i], mInputs);
		//if (dev.CountChannels() > 0) 
		{
			Device d;
			
			d.mID = devids[i];
			dev.GetName(d.mName, sizeof(d.mName));
			mDevices.push_back(d);
		}
	}
	delete[] devids;
}
