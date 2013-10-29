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

#include "Messages/IDataMessage.h"
#include "AudioStream.h"

#include <AudioToolbox/AudioToolbox.h>
#include "AudioPlaybackQueue.h"

namespace RemotePC 
{

    AudioStream& AudioStream::Instance()
    {
        return Loki::SingletonHolder< AudioStream >::Instance();        
    }   

	AudioStream::AudioStream()
  	    : audioQueue_(NULL)
	{
	}
	
	void AudioStream::Initialize()
	{
		if ( audioQueue_ ) 
			return;
		audioQueue_ = new AudioPlaybackQueue();
		audioQueue_->Initialize();		
	}
	
	void AudioStream::Shutdown()
	{
		if ( !audioQueue_ )
			return;
		audioQueue_->Shutdown();
		audioQueue_ = NULL;
	}

	void AudioStream::Handle(IDataMessageRef data)
	{
		audioQueue_->Handle(data->GetData(), data->GetDataLength());
	}

}
