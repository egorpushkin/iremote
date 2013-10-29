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

#ifndef AUDIOPLAYBACKQUEUE_H__REMOTEPC__INCLUDED_
#define AUDIOPLAYBACKQUEUE_H__REMOTEPC__INCLUDED_

namespace RemotePC
{
    
    class AudioPlaybackQueue 
    {
    public:
            
        AudioPlaybackQueue();
        virtual ~AudioPlaybackQueue();
        
        bool Initialize();
        
        void Shutdown();
        
		void Handle(const char * data, size_t length);
        
    protected:       
        
        /** The number of audio queue buffers to use. */
        static const size_t buffersCount_ = 10;      
		
		/** Number of bytes in each audio queue buffer. */
		static const size_t bufferSize_ = 16 * 1024;		

		/** Number of packet descriptions in our array. */
		static const size_t maxPacketDescriptors_ = 512;
		
		// 
		static void FileStreamPropertiesListener(void *inClientData, AudioFileStreamID inAudioFileStream, AudioFileStreamPropertyID inPropertyID, UInt32 *ioFlags);
		
		static void FileStreamPacketsListener(void *inClientData, UInt32 inNumberBytes, UInt32 inNumberPackets, const void *inInputData, AudioStreamPacketDescription *inPacketDescriptions);

		// 
		int FindQueueBuffer(AudioQueueBufferRef inBuffer);
		
		OSStatus EnqueueBuffer();

		void WaitForFreeBuffer();
		
		OSStatus StartQueueIfNeeded();
		
		// 
		static void interruptionListenerCallback(void *inClientData, UInt32 inInterruptionState);

		static void AudioQueueOutputListener(void *inClientData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer);
		
		static void AudioQueueIsRunningListener(void* inClientData, AudioQueueRef inAQ, AudioQueuePropertyID inID);
		
    private:
		
		/** the audio file stream parser. */
		AudioFileStreamID audioFileStream_;	
		
        /** The playback audio queue created by the application. */        
        AudioQueueRef queue_;		
		
        /** A Boolean value indicating whether or not the audio queue is running. */
        bool isRunning_;		
		
        /** A Boolean value indicating that loading has failed. */
        bool isFailed_;			
		
		/** A mutex to protect the inuse flags. */
		pthread_mutex_t mutex_;			
		
		/** A condition varable for handling the inuse flags. */
		pthread_cond_t cond_;
		
		/** A condition varable for handling the inuse flags. */
		pthread_cond_t done_;			
		
        /** An array holding pointers to the audio queue buffers managed by the audio queue. */
        AudioQueueBufferRef buffers_[buffersCount_];
		
		/** Flags to indicate that a buffer is still in use. */
		bool bufferInUse_[buffersCount_];	
		
		/** Packet descriptions for enqueuing audio. */
		AudioStreamPacketDescription packetDescs_[maxPacketDescriptors_];	
		
		/** The index of the audioQueueBuffer that is being filled. */
		unsigned int fillBufferIndex_;
		
		/** How many bytes have been filled. */
		size_t bytesFilled_;				
		
		/** How many packets have been filled. */
		size_t packetsFilled_;			
        
    };
    
}

#endif // AUDIOPLAYBACKQUEUE_H__REMOTEPC__INCLUDED_
