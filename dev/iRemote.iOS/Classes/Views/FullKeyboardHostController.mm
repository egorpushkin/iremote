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

#import "FullKeyboardHostController.h"
#import "FullKeyboardController.h"

#import "../Main/iRemoteAppDelegate.h"

// Platform runtime detection tool. 
#include "Connector/Platform/HostPlatform.h"

@implementation FullKeyboardHostController

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];    
    
    // Hide status bar.
    [UIApplication sharedApplication].statusBarHidden = YES;
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];    
    
    // Choose an appropriate keyboard controller.
    NSString *kbController = @"FullKeyboardWin";
    if ( RemotePC::HostPlatform::IsMac() )
        kbController = @"FullKeyboardMac";
    
    // Present full keyboard controller.
    FullKeyboardController *fullKeyboardController = [[FullKeyboardController alloc] initWithNibName:kbController bundle:[NSBundle mainBundle]];
    [self pushViewController:fullKeyboardController animated:NO];
    [fullKeyboardController release];
}

- (void)viewWillDisappear:(BOOL)animated {
    // Pop full size keyboard from the screen.
    [self popViewControllerAnimated:NO];    
    
    // Show status bar.
    [UIApplication sharedApplication].statusBarHidden = NO;
}

@end
