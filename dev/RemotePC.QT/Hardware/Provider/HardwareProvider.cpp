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

#include "../Hardware.h"

#if defined(WIN32)
#include "../Win32/IVolumeControlInitializer.h"
#include "../Win32/HardwareControl.h"
#include "../Win32/DisplayControl.h"
#include "../Vista/EndpointVolumeControl.h"
#elif defined(__MACH__)
#include "../Mac/VolumeControl.h"
#include "../Mac/VirtualKeysMapper.h"
#include "../Mac/HardwareControl.h"
#include "../Mac/DisplayControl.h"
#endif // Platform

#include "Messages/ZoomParamsMessage.h"

namespace RemotePC
{

	HardwareProvider::HardwareProvider()
		: volumeControl_()
		, hardwareControl_()
		, displayControl_()
        , audioInput_()
	{
#if defined(WIN32)
        volumeControl_ = mc::Class< EndpointVolumeControl >::Create();
		hardwareControl_ = mc::Class< HardwareControl >::Create();
		displayControl_ = mc::Class< DisplayControl >::Create();
#elif defined(__MACH__)
		volumeControl_ = mc::Class< VolumeControl >::Create();
		hardwareControl_ = mc::Class< HardwareControl >::Create();
		displayControl_ = mc::Class< DisplayControl >::Create();
#endif // Platform
	}

	HardwareProvider& HardwareProvider::Instance()
	{
		return Loki::SingletonHolder< HardwareProvider >::Instance();
	}

	IVolumeControlPtr HardwareProvider::GetVolumeControl()
	{
		return volumeControl_;
	}

	IMouseControlPtr HardwareProvider::GetMouseControl()
	{
		return hardwareControl_;
	}

	IKeyboardControlPtr HardwareProvider::GetKeyboardControl()
	{
		return hardwareControl_;
	}

	IDisplayControlPtr HardwareProvider::GetDisplayControl()
	{
		return displayControl_;
	}

    IAudioInputPtr HardwareProvider::GetAudioInput()
    {
        return audioInput_;
    }

	mc::IMessagePtr HardwareProvider::PrepareZoomParams() const
	{
		// Minimal zoom is hardcoded for now.
		float minZoom = 0.2f;
		// Calculate maximum zoom.
		size_t displayWidth = displayControl_->Width();
		size_t displayHeight = displayControl_->Height();
		size_t screenDimension = ( displayWidth > displayHeight ) ? displayWidth : displayHeight ;
		size_t maxDimension = screenDimension + 300;
		float maxZoom = ( maxDimension * 1.0f ) / 256.0f;
		// Current zoom is also hardcoded for now. In fuure it can be
		// saved for each device for user's convenience.
		float current = 1.0;

		return mc::Class< ZoomParamsMessage >::Create( minZoom, maxZoom, current );
	}

}
