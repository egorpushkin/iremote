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

#ifndef HARDWAREPROVIDER_H__REMOTEPC__INCLUDED_
#define HARDWAREPROVIDER_H__REMOTEPC__INCLUDED_

namespace RemotePC
{

	/** 
	 * Implements OS abstraction layer.
	 */ 
	class HardwareProvider
	{
	protected:

		/** Allows to protect class constructor. */
		friend struct Loki::CreateUsingNew< HardwareProvider >;

		HardwareProvider();

	public:

		static HardwareProvider& Instance();

	public:

		/**
		 * Provides an application with platform specific volume control.
		 */ 
		IVolumeControlPtr GetVolumeControl();

		/**
		 * Provides an application with platform specific mouse control.
		 */
		IMouseControlPtr GetMouseControl();

		/**
		 * Provides an application with platform specific keyboard control.
		 */
		IKeyboardControlPtr GetKeyboardControl();

		/** 
		 * Provides an application with platform specific display control.
		 */
		IDisplayControlPtr GetDisplayControl();

        /**
         * Provides an application with audio input service.
         */
        IAudioInputPtr GetAudioInput();

	public:

		mc::IMessagePtr PrepareZoomParams() const;

	private:

		/** Instance of volume control, which is shared in bounds of the 
		 * application. */
		IVolumeControlPtr volumeControl_;

		/** Instance of hardware control, which is shared in bounds of the 
		 * application. */
		mc::ICommonPtr hardwareControl_;

		/** Instance of display control. */
		IDisplayControlPtr displayControl_;

        /** Instance of audio input service. */
        IAudioInputPtr audioInput_;

	};

}

#endif // !HARDWAREPROVIDER_H__REMOTEPC__INCLUDED_
