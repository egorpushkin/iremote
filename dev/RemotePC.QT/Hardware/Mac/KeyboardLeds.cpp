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

#include "KeyboardLeds.h"

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <sysexits.h>
#include <mach/mach_error.h>
#include <sys/cdefs.h>

// Core Foundation framework include file.
// This code was added here to have a chance to comment unused parts.
////////////////////////////////////////////////////////////////////////////////
__BEGIN_DECLS

#include <CoreFoundation/CFPlugIn.h>
#if COREFOUNDATION_CFPLUGINCOM_SEPARATE
#include <CoreFoundation/CFPlugInCOM.h>
#endif

#include <IOKit/IOKitLib.h>

/* C244E858-109C-11D4-91D4-0050E4C6426F */
#define kIOCFPlugInInterfaceID CFUUIDGetConstantUUIDWithBytes(NULL,	\
	0xC2, 0x44, 0xE8, 0x58, 0x10, 0x9C, 0x11, 0xD4,			\
	0x91, 0xD4, 0x00, 0x50, 0xE4, 0xC6, 0x42, 0x6F)

#define IOCFPLUGINBASE							\
	UInt16	version;						\
	UInt16	revision;						\
	IOReturn (*Probe)(void *thisPointer, CFDictionaryRef propertyTable,	\
					io_service_t service, SInt32 * order);		\
	IOReturn (*Start)(void *thisPointer, CFDictionaryRef propertyTable,	\
					  io_service_t service);				\
	IOReturn (*Stop)(void *thisPointer)

typedef struct IOCFPlugInInterfaceStruct {
	IUNKNOWN_C_GUTS;
	IOCFPLUGINBASE;
} IOCFPlugInInterface;


kern_return_t
IOCreatePlugInInterfaceForService(io_service_t service,
				CFUUIDRef pluginType, CFUUIDRef interfaceType,
				IOCFPlugInInterface *** theInterface, SInt32 * theScore);

// RemotePC Note:
// This fragment causes unclear error in original file.
// kern_return_t
// IODestroyPlugInInterface(IOCFPlugInInterface ** interface);

__END_DECLS
////////////////////////////////////////////////////////////////////////////////

#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDUsageTables.h>

namespace RemotePC
{

	namespace MacLeds
	{
		int  manipulate_led(UInt32 whichLED, UInt32 value);
	}

	void KeyboardLeds::TriggerCapsLock(bool enable)
	{
		MacLeds::manipulate_led(kHIDUsage_LED_CapsLock, enable);
	}

	namespace MacLeds
	{

		static IOHIDElementCookie capslock_cookie = (IOHIDElementCookie)0;
		static IOHIDElementCookie numlock_cookie  = (IOHIDElementCookie)0;

		void         usage(void);
		inline void  print_errmsg_if_io_err(int expr, char* msg);
		inline void  print_errmsg_if_err(int expr, char* msg);

		io_service_t find_a_keyboard(void);
		void         find_led_cookies(IOHIDDeviceInterface122** handle);
		void         create_hid_interface(io_object_t hidDevice, IOHIDDeviceInterface*** hdi);

		inline void
		print_errmsg_if_io_err(int expr, char* /* msg */)
		{
			IOReturn err = (expr);
			if (err != kIOReturnSuccess) {
				// fprintf(stderr, "*** %s - %s(%x, %d).\n", msg, mach_error_string(err), err, err & 0xffffff);
				// fflush(stderr);
				// exit(EX_OSERR);
			}
		}

		inline void
		print_errmsg_if_err(int expr, char* /* msg */)
		{
			if (expr) {
				// fprintf(stderr, "*** %s.\n", msg);
				// fflush(stderr);
				// exit(EX_OSERR);
			}
		}

		io_service_t
		find_a_keyboard(void)
		{
			io_service_t result = (io_service_t)0;

			CFNumberRef usagePageRef = (CFNumberRef)0;
			CFNumberRef usageRef = (CFNumberRef)0;
			CFMutableDictionaryRef matchingDictRef = (CFMutableDictionaryRef)0;

			if (!(matchingDictRef = IOServiceMatching(kIOHIDDeviceKey))) {
				return result;
			}

			UInt32 usagePage = kHIDPage_GenericDesktop;
			UInt32 usage = kHIDUsage_GD_Keyboard;

			if (!(usagePageRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType,
												&usagePage))) {
				goto out;
			}

			if (!(usageRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType,
											&usage))) {
				goto out;
			}

			CFDictionarySetValue(matchingDictRef, CFSTR(kIOHIDPrimaryUsagePageKey),
								 usagePageRef);
			CFDictionarySetValue(matchingDictRef, CFSTR(kIOHIDPrimaryUsageKey),
								 usageRef);

			result = IOServiceGetMatchingService(kIOMasterPortDefault, matchingDictRef);

		out:
			if (usageRef) {
				CFRelease(usageRef);
			}
			if (usagePageRef) {
				CFRelease(usagePageRef);
			}

			return result;
		}

		void
		find_led_cookies(IOHIDDeviceInterface122** handle)
		{
			IOHIDElementCookie cookie;
			CFTypeRef          object;
			long               number;
			long               usage;
			long               usagePage;
			CFArrayRef         elements;
			CFDictionaryRef    element;
			IOReturn           result;

			if (!handle || !(*handle)) {
				return;
			}

			result = (*handle)->copyMatchingElements(handle, NULL, &elements);

			if (result != kIOReturnSuccess) {
				// fprintf(stderr, "Failed to copy cookies.\n");
				// exit(1);
				return;
			}

			CFIndex i;

			for (i = 0; i < CFArrayGetCount(elements); i++) {
				element = (CFDictionaryRef)CFArrayGetValueAtIndex(elements, i);
				object = (CFDictionaryGetValue(element, CFSTR(kIOHIDElementCookieKey)));
				if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID()) {
					continue;
				}
				if (!CFNumberGetValue((CFNumberRef) object, kCFNumberLongType,
									  &number)) {
					continue;
				}
				cookie = (IOHIDElementCookie)number;

				object = CFDictionaryGetValue(element, CFSTR(kIOHIDElementUsageKey));
				if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID()) {
					continue;
				}
				if (!CFNumberGetValue((CFNumberRef)object, kCFNumberLongType,
									  &number)) {
					continue;
				}
				usage = number;

				object = CFDictionaryGetValue(element,CFSTR(kIOHIDElementUsagePageKey));
				if (object == 0 || CFGetTypeID(object) != CFNumberGetTypeID()) {
					continue;
				}
				if (!CFNumberGetValue((CFNumberRef)object, kCFNumberLongType,
									  &number)) {
					continue;
				}
				usagePage = number;

				if (usagePage == kHIDPage_LEDs) {
					switch (usage) {

					case kHIDUsage_LED_NumLock:
						numlock_cookie = cookie;
						break;

					case kHIDUsage_LED_CapsLock:
						capslock_cookie = cookie;
						break;

					default:
						break;
					}
				}
			}

			return;
		}

		void
		create_hid_interface(io_object_t hidDevice, IOHIDDeviceInterface*** hdi)
		{
			IOCFPlugInInterface** plugInInterface = NULL;

			io_name_t className;
			HRESULT   plugInResult = S_OK;
			SInt32    score = 0;
			IOReturn  ioReturnValue = kIOReturnSuccess;

			ioReturnValue = IOObjectGetClass(hidDevice, className);

			print_errmsg_if_io_err(ioReturnValue, "Failed to get class name.");

			ioReturnValue = IOCreatePlugInInterfaceForService(
								hidDevice, kIOHIDDeviceUserClientTypeID,
								kIOCFPlugInInterfaceID, &plugInInterface, &score);
			if (ioReturnValue != kIOReturnSuccess) {
				return;
			}

			plugInResult = (*plugInInterface)->QueryInterface(plugInInterface,
							 CFUUIDGetUUIDBytes(kIOHIDDeviceInterfaceID), /*(LPVOID)*/(void**)hdi);
			print_errmsg_if_err(plugInResult != S_OK,
								"Failed to create device interface.\n");

			(*plugInInterface)->Release(plugInInterface);
		}

		int
		manipulate_led(UInt32 whichLED, UInt32 value)
		{
			io_service_t           hidService = (io_service_t)0;
			io_object_t            hidDevice = (io_object_t)0;
			IOHIDDeviceInterface **hidDeviceInterface = NULL;
			IOReturn               ioReturnValue = kIOReturnError;
			IOHIDElementCookie     theCookie = (IOHIDElementCookie)0;
			IOHIDEventStruct       theEvent;

			if (!(hidService = find_a_keyboard())) {
				// fprintf(stderr, "No keyboard found.\n");
				return ioReturnValue;
			}

			hidDevice = (io_object_t)hidService;

			create_hid_interface(hidDevice, &hidDeviceInterface);

			find_led_cookies((IOHIDDeviceInterface122 **)hidDeviceInterface);

			ioReturnValue = IOObjectRelease(hidDevice);
			if (ioReturnValue != kIOReturnSuccess) {
				goto out;
			}

			ioReturnValue = kIOReturnError;

			if (hidDeviceInterface == NULL) {
				// fprintf(stderr, "Failed to create HID device interface.\n");
				return ioReturnValue;
			}

			if (whichLED == kHIDUsage_LED_NumLock) {
				theCookie = numlock_cookie;
			} else if (whichLED == kHIDUsage_LED_CapsLock) {
				theCookie = capslock_cookie;
			}

			if (theCookie == 0) {
				// fprintf(stderr, "Bad or missing LED cookie.\n");
				goto out;
			}

			ioReturnValue = (*hidDeviceInterface)->open(hidDeviceInterface, 0);
			if (ioReturnValue != kIOReturnSuccess) {
				// fprintf(stderr, "Failed to open HID device interface.\n");
				goto out;
			}

			ioReturnValue = (*hidDeviceInterface)->getElementValue(hidDeviceInterface,
													   theCookie, &theEvent);
			if (ioReturnValue != kIOReturnSuccess) {
				(void)(*hidDeviceInterface)->close(hidDeviceInterface);
				goto out;
			}

			// fprintf(stdout, "%s\n", (theEvent.value) ? "on" : "off");
			if ((int)value != -1) {
				if (theEvent.value != (int)value) {
					theEvent.value = value;
					ioReturnValue = (*hidDeviceInterface)->setElementValue(
										hidDeviceInterface, theCookie,
										&theEvent, 0, 0, 0, 0);
					if (ioReturnValue == kIOReturnSuccess) {
						// fprintf(stdout, "%s\n", (theEvent.value) ? "on" : "off");
					}
				}
			}

			ioReturnValue = (*hidDeviceInterface)->close(hidDeviceInterface);

		out:
			(void)(*hidDeviceInterface)->Release(hidDeviceInterface);

			return ioReturnValue;
		}

	} // namespace MacLeds

} // namespace RemotePC
