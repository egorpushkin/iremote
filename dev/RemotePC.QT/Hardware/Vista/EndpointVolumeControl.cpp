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
#include "../Win32/IVolumeControlInitializer.h"

#include "EndpointVolumeControl.h"

#pragma warning(push)
#pragma warning(disable: 4005)
#include <stdio.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h> 
#pragma warning( pop ) 

namespace RemotePC
{

	struct EndpointVolumeControl::StoragePimpl_
	{
		StoragePimpl_()
			: endpointVolume_(NULL)
		{
		}
		IAudioEndpointVolume *endpointVolume_;
	};

	EndpointVolumeControl::EndpointVolumeControl()
		: mc::CommonImpl< IVolumeControl >()
		, storage_( new StoragePimpl_() )
	{
		::CoInitialize(NULL);
	
		IMMDeviceEnumerator *deviceEnumerator = NULL;
		HRESULT hr = ::CoCreateInstance(
			__uuidof(MMDeviceEnumerator), 
			NULL, 
			CLSCTX_INPROC_SERVER, 
			__uuidof(IMMDeviceEnumerator), 
			(LPVOID *)&deviceEnumerator);
		if ( FAILED(hr) || !deviceEnumerator )
			return;

		IMMDevice *defaultDevice = NULL;
		hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
		deviceEnumerator->Release();
		deviceEnumerator = NULL;
		if ( FAILED(hr) || !defaultDevice )
			return;
		
		hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&storage_->endpointVolume_);
		defaultDevice->Release();
		defaultDevice = NULL; 
		if ( FAILED(hr) )
			storage_->endpointVolume_ = NULL;
	}

	EndpointVolumeControl::~EndpointVolumeControl()
	{
		if ( storage_->endpointVolume_ )
			storage_->endpointVolume_->Release();
		CoUninitialize();
		delete storage_;
	}

	// IVolumeControl section
	mc::result EndpointVolumeControl::GetVolume(double &volume) const
	{
		if ( !storage_->endpointVolume_ )
			return mc::_E_NOTINIT;
		float localVolume = 0;
		storage_->endpointVolume_->GetMasterVolumeLevelScalar(&localVolume);
		volume = localVolume;
		return mc::_S_OK;
	}

	mc::result EndpointVolumeControl::SetVolume(double volume) const
	{
		if ( !storage_->endpointVolume_ )
			return mc::_E_NOTINIT;
		storage_->endpointVolume_->SetMasterVolumeLevelScalar((float)volume, &GUID_NULL);
		return mc::_S_OK;
	}

	mc::result EndpointVolumeControl::GetNormalizedVolume(unsigned long &volume) const
	{
		if ( !storage_->endpointVolume_ )
			return mc::_E_NOTINIT;
		double localVolume = 0;
		GetVolume(localVolume);
		volume = (unsigned long)(localVolume * 65535);
		return mc::_S_OK;
	}

	mc::result EndpointVolumeControl::SetNormalizedVolume(unsigned long volume) const
	{				
		if ( !storage_->endpointVolume_ )
			return mc::_E_NOTINIT;
		return SetVolume( ((double)volume)/65535.0 );
	}

	double EndpointVolumeControl::GetMinimumVolume() const
	{
		if ( !storage_->endpointVolume_ )
			return mc::_E_NOTINIT;
		return 0.0;
	}

	double EndpointVolumeControl::GetMaximumVolume() const
	{
		return 1.0;
	}

}
