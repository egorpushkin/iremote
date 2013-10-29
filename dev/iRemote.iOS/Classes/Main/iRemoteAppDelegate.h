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

@class AboutController;
@class HelpController;
@class LicenseController;
@class iRemoteViewController;

@interface iRemoteAppDelegate : NSObject < UIApplicationDelegate > {
    IBOutlet UIWindow *signInWindow;
    IBOutlet AboutController *aboutController;    
    IBOutlet HelpController *helpController;               
    IBOutlet LicenseController *licenseController;    
    IBOutlet iRemoteViewController *viewController;
    IBOutlet UIViewController *mainController;
    
    NSDate *wakeupTime;
    
    /** Stores interface orientation, specified before going to sleep. */ 
    UIInterfaceOrientation previousOrientation;
}

+ (iRemoteAppDelegate*)delegate;

- (iRemoteViewController *)loginController;
- (AboutController *)aboutController;
- (HelpController *)helpController;

- (void)onConnected;
- (void)onApproved;
- (void)onPasswordRequested:(NSNumber*)wrongPass;
- (void)onError:(NSString*)error;
- (void)onDisconnected;
- (void)onQiut;

- (void)warnOnOldRemotePC;
- (void)showShellResponse:(NSString*)response;

- (BOOL)isContentOnTop;

@end

