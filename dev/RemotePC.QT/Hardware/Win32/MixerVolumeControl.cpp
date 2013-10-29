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

#include "../Provider/IVolumeControl.h"
#include "IVolumeControlInitializer.h"

#include "MixerVolumeControl.h"

namespace RemotePC
{

	MixerVolumeControl::MixerVolumeControl()
		: mc::CommonImpl< IVolumeControl >()
		, mc::CommonImpl< IVolumeControlInitializer >()
		, mixer_(NULL)
		, mixerCaps_()
		, volumeControl_(0)
		, minimumVolume_(0)
		, maximumVolume_(0)
	{
	}

	MixerVolumeControl::~MixerVolumeControl()
	{
		if ( mixer_ )
			::mixerClose(mixer_);
	}

	// IVolumeControlInitializer section
	mc::result MixerVolumeControl::Initialize(HWND hwnd)
	{
		if ( mc::Error::IsError(OpenMixer(hwnd)) )	
			return mc::_E_FAIL;
		if ( mc::Error::IsError(GetMasterControl()) )	
		{
			::mixerClose(mixer_);
			mixer_ = NULL;
			return mc::_E_FAIL;		
		}
		return mc::_S_OK;
	}

	// IVolumeControl section
	mc::result MixerVolumeControl::GetVolume(double &volume) const
	{
		if ( !mixer_ )
			return mc::_E_NOTINIT;
		return GetVolumeInternal(volume);
	}

	mc::result MixerVolumeControl::SetVolume(double volume) const
	{
		if ( !mixer_ )
			return mc::_E_NOTINIT;
		return SetVolumeInternal(volume);
	}

	mc::result MixerVolumeControl::GetNormalizedVolume(unsigned long &volume) const
	{
		double localVolume = 0.0;
		mc::result code = GetVolume(localVolume);
		volume = (unsigned long)localVolume;
		return code;
	}

	mc::result MixerVolumeControl::SetNormalizedVolume(unsigned long volume) const
	{				
		return SetVolume((double)volume);
	}

	double MixerVolumeControl::GetMinimumVolume() const
	{
		return (double)minimumVolume_;
	}

	double MixerVolumeControl::GetMaximumVolume() const
	{
		return (double)maximumVolume_;
	}

	// Internal private tools
	mc::result MixerVolumeControl::OpenMixer(HWND hwnd)
	{
		// Check whether control is already initialized.
		if ( mixer_ )
			return mc::_E_ALREADYINIT;

		// Get the number of mixer devices present in the system.
		UINT mixersNumber = ::mixerGetNumDevs();
		if ( 0 == mixersNumber )
			// There must present at least one device.
			return mc::_E_FAIL;

		// Init control members.
		mixer_ = NULL;
		::ZeroMemory(&mixerCaps_, sizeof(MIXERCAPS));
		volumeControl_ = 0;
		minimumVolume_ = 0;
		maximumVolume_ = 0;

		// Open the first mixer.
		if ( ::mixerOpen(
			&mixer_, 
			0, 
			reinterpret_cast<DWORD>(hwnd), 
			NULL, 
			MIXER_OBJECTF_MIXER | CALLBACK_WINDOW) != MMSYSERR_NOERROR )
		{
			return mc::_E_FAIL;
		}

		// Determine mixer capabilities.
		if ( ::mixerGetDevCaps(
			reinterpret_cast<UINT>(mixer_),
			&mixerCaps_, 
			sizeof(MIXERCAPS)) != MMSYSERR_NOERROR )
		{
			::mixerClose(mixer_);
			return mc::_E_FAIL;
		}

		// Mixer is opened and is ready to be used.
		return mc::_S_OK;
	}

	mc::result MixerVolumeControl::GetMasterControl()
	{
		if ( !mixer_ )
			return mc::_E_NOTINIT;

		// Retrieve information about adjustable audio line intended to drive speakers.
		MIXERLINE speakersLine;
		speakersLine.cbStruct = sizeof(MIXERLINE);
		speakersLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
		if ( ::mixerGetLineInfo(
			reinterpret_cast<HMIXEROBJ>(mixer_),
			&speakersLine,
			MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR )
		{
			return mc::_E_FAIL;
		}

		// Retrieve information about the first control of a specific class for the
		// audio line.
		MIXERCONTROL speakersVolumeControl;
		MIXERLINECONTROLS speakersLineControls;
		speakersLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
		speakersLineControls.dwLineID = speakersLine.dwLineID;
		speakersLineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		speakersLineControls.cControls = 1;
		speakersLineControls.cbmxctrl = sizeof(MIXERCONTROL);
		speakersLineControls.pamxctrl = &speakersVolumeControl;
		if ( ::mixerGetLineControls(
			reinterpret_cast<HMIXEROBJ>(mixer_),
			&speakersLineControls,
			MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR )
		{
			return mc::_E_FAIL;
		}

		// Save control id.
		volumeControl_ = speakersVolumeControl.dwControlID;
		minimumVolume_ = speakersVolumeControl.Bounds.dwMinimum;
		maximumVolume_ = speakersVolumeControl.Bounds.dwMaximum;		

		return mc::_S_OK;
	}

	mc::result MixerVolumeControl::GetVolumeInternal(double &volume) const
	{
		MIXERCONTROLDETAILS_UNSIGNED mxcdVolume;
		MIXERCONTROLDETAILS mxcd;
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID = volumeControl_;
		mxcd.cChannels = 1;
		mxcd.cMultipleItems = 0;
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
		mxcd.paDetails = &mxcdVolume;
		if ( ::mixerGetControlDetails(
			reinterpret_cast<HMIXEROBJ>(mixer_),
			&mxcd,
			MIXER_OBJECTF_HMIXER |
			MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR )
		{
			return mc::_E_FAIL;
		}
		volume = (double)mxcdVolume.dwValue;
		return mc::_S_OK;
	}

	mc::result MixerVolumeControl::SetVolumeInternal(double volume) const
	{
		MIXERCONTROLDETAILS_UNSIGNED mxcdVolume = { (DWORD)volume };
		MIXERCONTROLDETAILS mxcd;
		mxcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mxcd.dwControlID = volumeControl_;
		mxcd.cChannels = 1;
		mxcd.cMultipleItems = 0;
		mxcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
		mxcd.paDetails = &mxcdVolume;
		if ( ::mixerSetControlDetails(
			reinterpret_cast<HMIXEROBJ>(mixer_),
			&mxcd,
			MIXER_OBJECTF_HMIXER |
			MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR )
		{
			return mc::_E_FAIL;;
		}
		return mc::_S_OK;
	}

}
