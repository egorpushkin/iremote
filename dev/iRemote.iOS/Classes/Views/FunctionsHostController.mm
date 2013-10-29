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

#import "FunctionsHostController.h"

// Hotkeys controller
#import "FunctionsController.h"

// Help controller
#import "../Doc/HelpController.h"

// Platform runtime detection tool. 
#include "Connector/Platform/HostPlatform.h"

@implementation FunctionsHostController

- (IBAction)onHelp {
    // Display help content.
	[HelpController showOnTopOf:self];
}

#pragma mark Tabbar controller section

- (void)connected {
    // Determite target platform.
    if ( RemotePC::HostPlatform::IsMac() )
        self.topViewController.view = macFunctions;
    else
        self.topViewController.view = winFunctions;
}

#pragma mark Helpers

- (UIView*)loadView:(NSString*)nibName {
    UIViewController *controller = [[FunctionsController alloc] initWithNibName:nibName bundle:[NSBundle mainBundle]];
    UIView *view = [controller.view retain];
    controller.view = nil;
    [controller release];
    return [view autorelease];
}

#pragma mark Life cycle tools

- (id)initWithCoder:(NSCoder*)coder {
    if ((self = [super initWithCoder:coder])) {     
        // Construct view controllers.
        winFunctions = [[self loadView:@"FunctionsWin"] retain];
        macFunctions = [[self loadView:@"FunctionsMac"] retain];
    }
    return self;
}

- (void)dealloc {
    [winFunctions release];
    [macFunctions release];
    [super dealloc];
}

@end
