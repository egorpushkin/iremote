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

#ifndef __AudioThruEngine_h__
#define __AudioThruEngine_h__

#include "AudioDevice.h"

class AudioRingBuffer;

class AudioThruEngine {
public:
	AudioThruEngine();
	~AudioThruEngine();
	
	void	SetDevices(AudioDeviceID input, AudioDeviceID output);
	void	SetInputDevice(AudioDeviceID input);
	void	SetOutputDevice(AudioDeviceID output);
	
	void	Start();
	bool	Stop();
	void	Mute(bool mute = true) { mMuting = mute; }
	bool	IsRunning() { return mRunning; }
	
	void	EnableThru(bool enable) { mThruing = enable; }
	void	SetBufferSize(UInt32 size);
	void	SetInputLoad(double load) { mInputLoad = load; }
	void	SetOutputLoad(double load) { mOutputLoad = load; }
	
	void	SetExtraLatency(SInt32 frames);
	double	GetThruTime() { return mThruTime; }
	SInt32	GetThruLatency() { return mActualThruLatency; }
	
	UInt32	GetOutputNchnls();
	AudioDeviceID	GetOutputDevice() { return mOutputDevice.mID; }
	AudioDeviceID	GetInputDevice() { return mInputDevice.mID; }
	
	OSStatus	MatchSampleRate(bool useInputDevice);
	
	// valid values are 0 to nchnls-1;  -1 = off
	void	SetChannelMap(int ch, int val) { mChannelMap[ch] = val; }
	int		GetChannelMap(int ch) { return mChannelMap[ch]; }
	
//	char *	GetErrorMessage() { return mErrorMessage; }

	Byte			*mWorkBuf;
	
	
protected:
	enum IOProcState {
		kOff,
		kStarting,
		kRunning,
		kStopRequested
	};
	
//	void	ApplyLoad(double load);

	static OSStatus InputIOProc (	AudioDeviceID			inDevice,
									const AudioTimeStamp*	inNow,
									const AudioBufferList*	inInputData,
									const AudioTimeStamp*	inInputTime,
									AudioBufferList*		outOutputData,
									const AudioTimeStamp*	inOutputTime,
									void*					inClientData);

	static OSStatus OutputIOProc (	AudioDeviceID			inDevice,
									const AudioTimeStamp*	inNow,
									const AudioBufferList*	inInputData,
									const AudioTimeStamp*	inInputTime,
									AudioBufferList*		outOutputData,
									const AudioTimeStamp*	inOutputTime,
									void*					inClientData);

	static OSStatus OutputIOProc16 (	AudioDeviceID			inDevice,
									const AudioTimeStamp*	inNow,
									const AudioBufferList*	inInputData,
									const AudioTimeStamp*	inInputTime,
									AudioBufferList*		outOutputData,
									const AudioTimeStamp*	inOutputTime,
									void*					inClientData);
									
	void	ComputeThruOffset();

	AudioDevice		mInputDevice, mOutputDevice;
	bool			mRunning;
	bool			mMuting;
	bool			mThruing;
	IOProcState		mInputProcState, mOutputProcState;
	Float64			mLastInputSampleCount, mIODeltaSampleCount;
	UInt32			mBufferSize;
	SInt32			mExtraLatencyFrames;
	SInt32			mActualThruLatency;
	Float64			mSampleRate;
	Float64			mInToOutSampleOffset;		// subtract from the output time to obtain input time
	AudioRingBuffer *mInputBuffer;
	double			mInputLoad, mOutputLoad;
	double			mThruTime;
	
	int				mChannelMap[16];
	AudioDeviceIOProc mOutputIOProc;
	//char			mErrorMessage[128];
};


#endif // __AudioThruEngine_h__
