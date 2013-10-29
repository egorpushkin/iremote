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

// Core audio services.
#include <CoreAudio/CoreAudio.h>

// Precompiled header
#include "Common/Common.h"

#include "../Provider/IVolumeControl.h"
#include "VolumeControl.h"

namespace RemotePC
{

	VolumeControl::VolumeControl()
		: mc::CommonImpl< IVolumeControl >()
	{
	}

	VolumeControl::~VolumeControl()
	{
	}

	// IVolumeControl section
	mc::result VolumeControl::GetVolume(double &volume) const
	{
		// Acquire audio device.
		AudioDeviceID device;
        UInt32 deviceSize = sizeof(device);
        OSStatus err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &deviceSize, &device);
		if( noErr != err )
			return mc::_E_FAIL;

		// Get master volume (channel 0)
		float localVolume;
        UInt32 volumeSize = sizeof(localVolume);
        err = AudioDeviceGetProperty(device, 0, 0, kAudioDevicePropertyVolumeScalar, &volumeSize, &localVolume);
        if( noErr == err )
        {
            volume = localVolume;
            return mc::_S_OK;
        }

        // Try seperate channels.
        UInt32 channels[2];
        UInt32 channelsSize = sizeof(channels);
        err = AudioDeviceGetProperty(device, 0, 0, kAudioDevicePropertyPreferredChannelsForStereo, &channelsSize, &channels);
        if( noErr != err )
            return mc::_E_FAIL;

        // Obtain output device.
        AudioDeviceID theDevice = 0;
        UInt32 theSize = sizeof(AudioDeviceID);
        AudioObjectPropertyAddress theAddress = { kAudioHardwarePropertyDefaultOutputDevice,
                                                  kAudioObjectPropertyScopeGlobal,
                                                  kAudioObjectPropertyElementMaster };
        OSStatus theError = AudioObjectGetPropertyData(kAudioObjectSystemObject, &theAddress, 0, NULL, &theSize, &theDevice);
        if( noErr != theError )
            return mc::_E_FAIL;

        // Get volume by channel.
        Float32 theVolume[2];
        theSize = sizeof(Float32);
        AudioObjectPropertyScope theScope = kAudioDevicePropertyScopeOutput;
        AudioObjectPropertyAddress theAddress2 = { kAudioDevicePropertyVolumeScalar,
                       theScope,
                       channels[0] };
        theError = AudioObjectGetPropertyData(theDevice, &theAddress2, 0, NULL, &theSize, &theVolume[0]);
        if( noErr != theError )
            return mc::_E_FAIL;

        AudioObjectPropertyAddress theAddress3 = { kAudioDevicePropertyVolumeScalar,
                       theScope,
                       channels[1] };
        theError = AudioObjectGetPropertyData(theDevice, &theAddress3, 0, NULL, &theSize, &theVolume[1]);
        if( noErr != theError )
            return mc::_E_FAIL;

        volume = ( theVolume[0] + theVolume[1] ) / 2.0;
        return mc::_S_OK;

        /*
        // Obsolete API. Fails to work under 10.6.x.
        float localVolumes[2];
        err = AudioDeviceGetProperty(device, channels[0], 0, kAudioDevicePropertyVolumeScalar, &volumeSize, &localVolumes[0]);
        err = AudioDeviceGetProperty(device, channels[1], 0, kAudioDevicePropertyVolumeScalar, &volumeSize, &localVolumes[1]);
        if( err != noErr )
            return mc::_E_FAIL;

        volume = ( localVolumes[0] + localVolumes[1] ) / 2.0;
        return mc::_S_OK; */
	}

	mc::result VolumeControl::SetVolume(double volume) const
	{
        float localVolume = (float)volume;
        size_t volumeSize = sizeof(localVolume);

		// Acquire audio device.
		AudioDeviceID device;
        UInt32 deviceSize = sizeof(device);
		OSStatus err = AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice, &deviceSize, &device);
		if( noErr != err )
			return mc::_E_FAIL;

		// Check whether property is writable.
        Boolean canset = false;
        UInt32 cansetSize = sizeof(canset);
		err = AudioDeviceGetPropertyInfo(device, 0, false, kAudioDevicePropertyVolumeScalar, &cansetSize, &canset);
        if( ( noErr == err ) && ( canset ) )
        {
            // Set master volume.
            err = AudioDeviceSetProperty(device, NULL, 0, false, kAudioDevicePropertyVolumeScalar, volumeSize, &localVolume);
            return ( noErr == err ) ? ( mc::_S_OK ) : ( mc::_E_FAIL );
        }

        // Try seperate channels.
        UInt32 channels[2];
        UInt32 channelsSize = sizeof(channels);
        err = AudioDeviceGetProperty(device, 0, false, kAudioDevicePropertyPreferredChannelsForStereo, &channelsSize, &channels);
        if( err != noErr )
            return mc::_E_FAIL;

        // Set volume.
        err = AudioDeviceSetProperty(device, 0, channels[0], false, kAudioDevicePropertyVolumeScalar, volumeSize, &localVolume);
        err = AudioDeviceSetProperty(device, 0, channels[1], false, kAudioDevicePropertyVolumeScalar, volumeSize, &localVolume);
        return ( noErr == err ) ? ( mc::_S_OK ) : ( mc::_E_FAIL );
	}

	mc::result VolumeControl::GetNormalizedVolume(unsigned long &volume) const
	{
		double localVolume = 0.0;
		mc::result code = GetVolume(localVolume);
		volume = localVolume * 65535;
		return code;
	}

	mc::result VolumeControl::SetNormalizedVolume(unsigned long volume) const
	{
		return SetVolume( ( volume * 1.0 ) / 65535.0 );
	}

	double VolumeControl::GetMinimumVolume() const
	{
		return 0.0;
	}

	double VolumeControl::GetMaximumVolume() const
	{
		return 1.0;
	}

}
