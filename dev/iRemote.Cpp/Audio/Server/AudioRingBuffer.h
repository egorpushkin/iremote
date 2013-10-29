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

#ifndef __AudioRingBuffer_h__
#define __AudioRingBuffer_h__

#include <CoreServices/CoreServices.h>

// used for now to cache a couple of seconds (?) of input and access it for audio thruing
class AudioRingBuffer {
public:
	AudioRingBuffer(UInt32 bytesPerFrame, UInt32 capacityFrames);
	~AudioRingBuffer();
	
	void	Allocate(UInt32 bytesPerFrame, UInt32 capacityFrames);
	void	Clear();
	bool	Store(const Byte *data, UInt32 nFrames, SInt64 frameNumber);
	double	Fetch(Byte *data, UInt32 nFrames, SInt64 frameNumber);

protected:
	UInt32		FrameOffset(SInt64 frameNumber) { return (mStartOffset + UInt32(frameNumber - mStartFrame) * mBytesPerFrame) % mCapacityBytes; }

protected:
	UInt32		mBytesPerFrame;
	UInt32		mCapacityFrames;
	UInt32		mCapacityBytes;
	Byte *		mBuffer;
	UInt32		mStartOffset;
	SInt64		mStartFrame;
	SInt64		mEndFrame;
};

#endif // __AudioRingBuffer_h__
