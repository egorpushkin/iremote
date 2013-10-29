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

#ifndef __AudioDevice_h__
#define __AudioDevice_h__

#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>

class AudioDevice {
public:
	AudioDevice() : mID(kAudioDeviceUnknown) { }
	AudioDevice(AudioDeviceID devid, bool isInput) { Init(devid, isInput); }

	void	Init(AudioDeviceID devid, bool isInput);
	
    bool	Valid() { return mID != kAudioDeviceUnknown; }
	
	void	SetBufferSize(UInt32 size);
	OSStatus SetSampleRate(Float64 sr);
	
	void	UpdateFormat();
	
	int		CountChannels();
	char *	GetName(char *buf, UInt32 maxlen);

public:
	AudioDeviceID					mID;
	bool							mIsInput;
	UInt32							mSafetyOffset;
	UInt32							mBufferSizeFrames;
	AudioStreamBasicDescription		mFormat;	
};


#endif // __AudioDevice_h__
