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

#ifndef __AudioDeviceList_h__
#define __AudioDeviceList_h__

#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>
#include <vector>

class AudioDeviceList {
public:
	struct Device {
		char			mName[64];
		AudioDeviceID	mID;
	};
	typedef std::vector<Device> DeviceList;

	AudioDeviceList(bool inputs);
	~AudioDeviceList();

	DeviceList &GetList() { return mDevices; }

protected:
	void		BuildList();
	void		EraseList();

	bool				mInputs;
	DeviceList			mDevices;
	
};

#endif // __AudioDeviceList_h__
