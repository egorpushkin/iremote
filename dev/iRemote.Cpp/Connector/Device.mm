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

#import <UIKit/UIKit.h>

#include "Common/Common.h"
#include <string>

#include "Device.h"

namespace MinCOM
{
namespace iPhone
{

    std::string Device::GetId()
    {
        static NSString* deviceIdKey = @"com.scientific-soft.iremote.ios.device.id";
        
        NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
        [defaults synchronize];
        NSString* deviceId = [defaults stringForKey:deviceIdKey];
        if ( !deviceId )
        {
            deviceId = [NSString stringWithUTF8String:Guid::Generate().ToString().c_str()];
            [defaults setValue:deviceId forKey:deviceIdKey];
            [defaults synchronize];
        }
        return std::string( [deviceId UTF8String] );
    }

    std::string Device::GetName()
    {
        return std::string( [[UIDevice currentDevice].name UTF8String] );        
    }
	
	std::string Device::GetDocuments()
	{
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];		
        NSArray * paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString * docpath = [paths objectAtIndex:0];		
        std::string pathString([docpath UTF8String]);
        [pool release];	
		
		return pathString;		
	}
    
    void Device::Remove(const std::string& filepath)
    {
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];		
        NSString *path = [NSString stringWithCString:filepath.c_str() encoding:NSUTF8StringEncoding];
        [[NSFileManager defaultManager] removeItemAtPath:path error:nil];        
        [pool release];	       
    }
	
	bool Device::IsTablet()
	{
        // Tablets are not currently supported.
		return NO;
        
        // Correct check looks like that:
        // return
		//     ( UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad );
	}
}
}