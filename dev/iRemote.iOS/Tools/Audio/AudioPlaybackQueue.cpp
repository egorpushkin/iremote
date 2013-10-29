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

#include <AudioToolbox/AudioToolbox.h>

#include "AudioPlaybackQueue.h"

#define PRINTERROR(LABEL) printf("%s err %4.4s %d\n", LABEL, &err, err)

namespace RemotePC
{
    
    AudioPlaybackQueue::AudioPlaybackQueue()
	    : audioFileStream_(NULL)
     	, queue_(NULL)
        , isRunning_(false)	
		, isFailed_(false)
	    , mutex_()
	    , cond_()
		, done_()
	    , buffers_()
		, bufferInUse_()
		, packetDescs_()
	    , fillBufferIndex_(0)
    	, bytesFilled_(0)
		, packetsFilled_(0)
    {
    }
    
    AudioPlaybackQueue::~AudioPlaybackQueue()
    {
        Shutdown();
    }
	
	bool AudioPlaybackQueue::Initialize()
	{
		Shutdown();
		
		// Initialize audio session.
        AudioSessionInitialize(NULL, NULL, AudioPlaybackQueue::interruptionListenerCallback, this);
		
		// Initialize a mutex and condition so that we can block on buffers in use.
		pthread_mutex_init(&mutex_, NULL);
		pthread_cond_init(&cond_, NULL);
		pthread_cond_init(&done_, NULL);
		
		// Create an audio file stream parser.
		OSStatus err = AudioFileStreamOpen(this, AudioPlaybackQueue::FileStreamPropertiesListener, AudioPlaybackQueue::FileStreamPacketsListener, kAudioFileWAVEType, &audioFileStream_);
		if (err) 
		{ 
			PRINTERROR("AudioFileStreamOpen"); 
			return false;
		}
		
		//////////////
		AudioStreamBasicDescription format;
		format.mSampleRate = 4000;
		format.mFormatID = kAudioFormatLinearPCM;
		format.mFormatFlags 
			= kAudioFormatFlagIsSignedInteger
			| kAudioFormatFlagIsPacked;
		format.mBytesPerPacket = 2;
		format.mFramesPerPacket = 1;
		format.mBytesPerFrame = 2;
		format.mChannelsPerFrame = 1;
		format.mBitsPerChannel = 16;
		format.mReserved = 0;
		
		// err = AudioFileStreamSetProperty(audioFileStream_, kAudioFileStreamProperty_DataFormat, sizeof(format), &format);		
		
		// Create the audio queue.
		err = AudioQueueNewOutput(&format, AudioPlaybackQueue::AudioQueueOutputListener, this, NULL, NULL, 0, &queue_);
		if (err) 
		{ 
			PRINTERROR("AudioQueueNewOutput"); 
			isFailed_ = true;
			return false; 
		}
		
		// Allocate audio queue buffers
		for (unsigned int i = 0; i < buffersCount_; ++i) 
		{
			err = AudioQueueAllocateBuffer(queue_, bufferSize_, &buffers_[i]);
			if (err) 
			{ 
				PRINTERROR("AudioQueueAllocateBuffer"); 
				isFailed_ = true;
				return false; 
			}
		}
		//////////////

		return true;
	}
	
	void AudioPlaybackQueue::Shutdown()
	{
		// TODO:
		
		
		
	}
	
	void AudioPlaybackQueue::Handle(const char * data, size_t length)
	{
		FileStreamPacketsListener(this, length, 0, data, NULL);

/*		
		// Parse the data. this will call FileStreamPropertiesListener and FileStreamPacketsListener.
		OSStatus err = AudioFileStreamParseBytes(audioFileStream_, length, data, 0);
		if (err) 
		{ 
		 	PRINTERROR("AudioFileStreamParseBytes"); 
		}		
*/
	}
	
	////////////////////////////////////////////////////////////////////////////////
	// File stream callbacks
	////////////////////////////////////////////////////////////////////////////////
	
	/**
	 * this is called by audio file stream when it finds property values.
	 */
	void AudioPlaybackQueue::FileStreamPropertiesListener(void *inClientData, AudioFileStreamID inAudioFileStream, AudioFileStreamPropertyID inPropertyID, UInt32 *ioFlags)
	{			
		AudioPlaybackQueue* playbackQueue = (AudioPlaybackQueue*)inClientData;
		OSStatus err = noErr;
		
		// Dump property id to the debug log.
		// printf("found property '%c%c%c%c'\n", (inPropertyID>>24)&255, (inPropertyID>>16)&255, (inPropertyID>>8)&255, inPropertyID&255);
		
		switch (inPropertyID) 
		{
			case kAudioFileStreamProperty_ReadyToProducePackets:
			{
				// The file stream parser is now ready to produce audio packets.
				// Get the stream format.
				AudioStreamBasicDescription asbd;
				UInt32 asbdSize = sizeof(asbd);
				err = AudioFileStreamGetProperty(inAudioFileStream, kAudioFileStreamProperty_DataFormat, &asbdSize, &asbd);
				if ( err ) 
				{ 
					PRINTERROR("get kAudioFileStreamProperty_DataFormat"); 
					playbackQueue->isFailed_ = true; 
					break; 
				}
				
				// Create the audio queue.
				err = AudioQueueNewOutput(&asbd, AudioPlaybackQueue::AudioQueueOutputListener, playbackQueue, NULL, NULL, 0, &playbackQueue->queue_);
				if (err) 
				{ 
					PRINTERROR("AudioQueueNewOutput"); 
					playbackQueue->isFailed_ = true;
					break; 
				}
				
				// Allocate audio queue buffers
				for (unsigned int i = 0; i < buffersCount_; ++i) 
				{
					err = AudioQueueAllocateBuffer(playbackQueue->queue_, bufferSize_, &playbackQueue->buffers_[i]);
					if (err) 
					{ 
						PRINTERROR("AudioQueueAllocateBuffer"); 
						playbackQueue->isFailed_ = true;
						break; 
					}
				}
				
				// Get the cookie size
				UInt32 cookieSize;
				Boolean writable;
				err = AudioFileStreamGetPropertyInfo(inAudioFileStream, kAudioFileStreamProperty_MagicCookieData, &cookieSize, &writable);
				if (err) 
				{ 
					PRINTERROR("info kAudioFileStreamProperty_MagicCookieData"); 
					break; 
				}
				
				// Dump cookie size.
				// printf("cookieSize %d\n", cookieSize);
				
				// Get the cookie data.
				void* cookieData = calloc(1, cookieSize);
				err = AudioFileStreamGetProperty(inAudioFileStream, kAudioFileStreamProperty_MagicCookieData, &cookieSize, cookieData);
				if (err) 
				{ 
					PRINTERROR("get kAudioFileStreamProperty_MagicCookieData"); 
					free(cookieData); 
					break; 
				}
				
				// Set the cookie on the queue.
				err = AudioQueueSetProperty(playbackQueue->queue_, kAudioQueueProperty_MagicCookie, cookieData, cookieSize);
				free(cookieData);
				if (err) 
				{ 
					PRINTERROR("set kAudioQueueProperty_MagicCookie"); 
					break; 
				}
				
				// listen for kAudioQueueProperty_IsRunning
				err = AudioQueueAddPropertyListener(playbackQueue->queue_, kAudioQueueProperty_IsRunning, AudioPlaybackQueue::AudioQueueIsRunningListener, playbackQueue);
				if (err) 
				{ 
					PRINTERROR("AudioQueueAddPropertyListener"); 
					playbackQueue->isFailed_ = true; 
					break; 
				}
				
				break;
			}
		}
	}
	
	/**
	 * This is called by audio file stream when it finds packets of audio.
	 */ 
	void AudioPlaybackQueue::FileStreamPacketsListener(void *inClientData, UInt32 inNumberBytes, UInt32 inNumberPackets, const void *inInputData, AudioStreamPacketDescription *inPacketDescriptions)
	{
		AudioPlaybackQueue *playbackQueue = (AudioPlaybackQueue*)inClientData;
		
		// Dumps a number of packets to the log.
		// printf("got data.  bytes: %d  packets: %d\n", inNumberBytes, inNumberPackets);
		
		std::cout << "Data arrived: " << inNumberBytes << " bytes" << std::endl; 
		
		if ( NULL == inPacketDescriptions )
		{
			// If the space remaining in the buffer is not enough for this packet, then enqueue the buffer.
			size_t bufSpaceRemaining = bufferSize_ - playbackQueue->bytesFilled_;
			if ( bufSpaceRemaining < inNumberBytes ) 
			{
				playbackQueue->EnqueueBuffer();
				playbackQueue->WaitForFreeBuffer();
			}
			
			// Copy data to the audio queue buffer.
			AudioQueueBufferRef fillBuf = playbackQueue->buffers_[playbackQueue->fillBufferIndex_];
			memcpy((char*)fillBuf->mAudioData + playbackQueue->bytesFilled_, (const char*)inInputData, inNumberBytes);
			// Keep track of bytes filled and packets filled.
			playbackQueue->bytesFilled_ += inNumberBytes;
			
			std::cout << "Data put into buffer " << playbackQueue->fillBufferIndex_ << std::endl; 
		}
		else
		{
			// The following code assumes we're streaming VBR data. for CBR data, you'd need another code branch here.		
			for (int i = 0; i < inNumberPackets; ++i) 
			{
				SInt64 packetOffset = inPacketDescriptions[i].mStartOffset;
				SInt64 packetSize   = inPacketDescriptions[i].mDataByteSize;
				
				// If the space remaining in the buffer is not enough for this packet, then enqueue the buffer.
				size_t bufSpaceRemaining = bufferSize_ - playbackQueue->bytesFilled_;
				if ( bufSpaceRemaining < packetSize ) 
				{
					playbackQueue->EnqueueBuffer();
					playbackQueue->WaitForFreeBuffer();
				}
				
				// Copy data to the audio queue buffer.
				AudioQueueBufferRef fillBuf = playbackQueue->buffers_[playbackQueue->fillBufferIndex_];
				memcpy((char*)fillBuf->mAudioData + playbackQueue->bytesFilled_, (const char*)inInputData + packetOffset, packetSize);
				// Fill out packet description.
				playbackQueue->packetDescs_[playbackQueue->packetsFilled_] = inPacketDescriptions[i];
				playbackQueue->packetDescs_[playbackQueue->packetsFilled_].mStartOffset = playbackQueue->bytesFilled_;
				// Keep track of bytes filled and packets filled.
				playbackQueue->bytesFilled_ += packetSize;
				playbackQueue->packetsFilled_ += 1;
				
				// If that was the last free packet description, then enqueue the buffer.
				size_t packetsDescsRemaining = maxPacketDescriptors_ - playbackQueue->packetsFilled_;
				if ( 0 == packetsDescsRemaining ) 
				{
					playbackQueue->EnqueueBuffer();
					playbackQueue->WaitForFreeBuffer();
				}
			}	
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////
	// Audio queue tools
	////////////////////////////////////////////////////////////////////////////////	
	
	int AudioPlaybackQueue::FindQueueBuffer(AudioQueueBufferRef inBuffer)
	{
		for (unsigned int i = 0; i < buffersCount_; ++i ) 
		{
			if ( inBuffer == buffers_[i] ) 
				return i;
		}
		return -1;
	}
	
	OSStatus AudioPlaybackQueue::StartQueueIfNeeded()
	{
		OSStatus err = noErr;
		if ( !isRunning_ ) 
		{		
			// Start the queue if it has not been started already.
			err = AudioQueueStart(queue_, NULL);
			if (err) 
			{ 
				PRINTERROR("AudioQueueStart"); 
				isFailed_ = true; 
				return err; 
			}		
			isRunning_ = true;
		}
		return err;
	}	
	
	OSStatus AudioPlaybackQueue::EnqueueBuffer()
	{
		OSStatus err = noErr;
		
		// Set in use flag.	
		bufferInUse_[fillBufferIndex_] = true;		
		
		// Enqueue buffer.
		AudioQueueBufferRef fillBuf = buffers_[fillBufferIndex_];
		fillBuf->mAudioDataByteSize = bytesFilled_;	
		
		std::cout << "Buffer " << fillBufferIndex_ << " is enqued with " << bytesFilled_ << " bytes" << std::endl;
		
		if ( packetsFilled_ )
			err = AudioQueueEnqueueBuffer(queue_, fillBuf, packetsFilled_, packetDescs_);
		else
			err = AudioQueueEnqueueBuffer(queue_, fillBuf, 0, NULL);
		
		if (err) 
		{ 
			PRINTERROR("AudioQueueEnqueueBuffer"); 
			isFailed_ = true; 
			return err; 
		}		
		
		StartQueueIfNeeded();
		
		return err;
	}
	
	
	void AudioPlaybackQueue::WaitForFreeBuffer()
	{
		// Go to next buffer.
		if ( ++fillBufferIndex_ >= buffersCount_ ) 
			fillBufferIndex_ = 0;
		// Reset bytes filled.
		bytesFilled_ = 0;		
		// Reset packets filled.
		packetsFilled_ = 0;		
		
		// Wait until next buffer is not in use.
		pthread_mutex_lock(&mutex_); 
		while ( bufferInUse_[fillBufferIndex_] ) 
		{
			pthread_cond_wait(&cond_, &mutex_);
		}
		pthread_mutex_unlock(&mutex_);
		
		std::cout << "Buffer " << fillBufferIndex_ << " is ready to be filled." << std::endl;
	}
	
	////////////////////////////////////////////////////////////////////////////////
	// Audio queue callbacks
	////////////////////////////////////////////////////////////////////////////////
	
	void AudioPlaybackQueue::interruptionListenerCallback(void *inClientData, UInt32 inInterruptionState)
    {
    }
	
	/**
	 * This is called by the audio queue when it has finished decoding our data. 
	 */
	void AudioPlaybackQueue::AudioQueueOutputListener(void *inClientData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer)
	{
		AudioPlaybackQueue* playbackQueue = (AudioPlaybackQueue*)inClientData;
		
		// The buffer is now free to be reused.
		// Find buffer index.
		unsigned int bufIndex = playbackQueue->FindQueueBuffer(inBuffer);
		
		std::cout << "Buffer " << bufIndex << " is returned by the queue." << std::endl;		
		
		// signal waiting thread that the buffer is free.
		pthread_mutex_lock(&playbackQueue->mutex_);
		playbackQueue->bufferInUse_[bufIndex] = false;
		pthread_cond_signal(&playbackQueue->cond_);
		pthread_mutex_unlock(&playbackQueue->mutex_);
	}
	
	void AudioPlaybackQueue::AudioQueueIsRunningListener(void* inClientData, AudioQueueRef inAQ, AudioQueuePropertyID inID)
	{
		AudioPlaybackQueue* playbackQueue = (AudioPlaybackQueue*)inClientData;
		
		UInt32 running;
		UInt32 size;
		OSStatus err = AudioQueueGetProperty(inAQ, kAudioQueueProperty_IsRunning, &running, &size);
		if (err) 
		{ 
			PRINTERROR("get kAudioQueueProperty_IsRunning"); 
			return; 
		}
		if ( !running ) 
		{
			pthread_mutex_lock(&playbackQueue->mutex_);
			pthread_cond_signal(&playbackQueue->done_);
			pthread_mutex_unlock(&playbackQueue->mutex_);
		}
	}
	
}