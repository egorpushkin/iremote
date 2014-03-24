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

#import "iRemoteAppDelegate.h"
#import "iRemoteViewController.h"
#import "../Doc/LicenseController.h"
#import "../Controls/Controls.h"

// Tab bar controllers notification interface.
#import "../Controls/TabbarController.h"

// It is temporarily used to access shell controller.
#import "../Views/ShellController.h"

// Applocation state
#import "../State/LocalStorage.h"
#import "../State/UISettings.h"

// 3rd-party. Localytics.
#import "../../3rdParty/Localytics/LocalyticsSession.h"
// 3rd-party. Crashlytics.
#import <Crashlytics/Crashlytics.h>

@interface iRemoteAppDelegate (PrivateMethods)

- (void)dismissModalFoyrController:(UIViewController *)controller;
- (void)switchToLoginPageWithMessage:(NSString *)message;
- (void)switchToContentPage;

@end

// Connector to the server.
#include "Connector/Server.h"
#include "Connector/Device.h"

// Application config
#include "../../Server/Config/Config.h"
// Persistent UI config
#include "../State/UIConfig.h"
// Events handlers.
#include "../Annunciators/UIEvents.h"

@implementation iRemoteAppDelegate

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    // Initialize crash reporting.
    [Crashlytics startWithAPIKey:@"1c51e83d4d0dc7bea62a746e20c2ad2c96734a17"];
    
    // Load application state and configuration.
    [[LocalStorage instance] loadData];    
	RemotePC::Config::Instance().Load();
    RemotePC::UIConfig::Instance().Load();    
	[[UISettings instance] loadSettings];
	
	// Initialize tool responsible for events delivery.
	RemotePC::IUIEventsPtr events(mc::Class< RemotePC::UIEvents >::Create());
	RemotePC::Holder::Instance().SetEvents(events);
    
    // Display status bar. This trick helps to avoid status bar on splash screen.
    application.statusBarHidden = NO;
    // Specify initial application orientation.
    previousOrientation = UIInterfaceOrientationPortrait;
    
    // Configure login form delegate.
    viewController.delegate = viewController;    
    // Display login form. 
    [signInWindow addSubview:viewController.view];
    // Check whether EULA has been accepted.
    if ( ![[LocalStorage instance] eulaAccepted] ) {
        // Display license dialog on top of login form.
        [viewController presentViewController:licenseController animated:NO completion:NULL];
    }

    // Present application window.
    signInWindow.rootViewController = viewController;
    [signInWindow makeKeyAndVisible];    
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Save application state and configuration.
    [[LocalStorage instance] saveData];    
	RemotePC::Config::Instance().Save();
    RemotePC::UIConfig::Instance().Save();
	[[UISettings instance] saveSettings];
    
    // Localytics.
    [[LocalyticsSession shared] close];
    [[LocalyticsSession shared] upload];
}

/**
 * Application goes to sleep. Should disconnect here.
 */
- (void)applicationWillResignActive:(UIApplication *)application {
    // Check whether application is currently connecting.
    if ( [viewController isConnectng] ) {
        [viewController onCancel];
        return;
    }
    
    // Check whether application is connected. 
    // Note: This should not be done by analyzing presence of valid cookie 
    // because this lieads to numerous errors as cookie may be not released 
    // correctly. 
    // This should be done by analysing holder's server property.
    if ( !RemotePC::Holder::Instance().GetServer() ) 
        // Nothing should be done if client is not connected to server.
        return;    
    
    // Lock cookie. This is required for automatic reconnection after wakeup.
    RemotePC::Holder::Instance().LockCookie(true);
    // Close connection. 
    RemotePC::Holder::Instance().Stop();
    // Switch to login page.
    [self onQiut];
    
    // Localytics.
    [[LocalyticsSession shared] close];
    [[LocalyticsSession shared] upload];
}

/**
 * Application wakes up. Should reconnect if there is a cookie.
 * Note: This method is invoked at startup.
 */
- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Remember wakeup time.
    if ( wakeupTime )
        [wakeupTime release];
    wakeupTime = [[NSDate date] retain];
    
    // Note: Cookie should not be unlocked here because this leads to preventing
    // reconnect attempts. Instead, cookie should be unlocked:
    // - when connection is established;
    // - when connection is finally failed (due to timeout).
    
    // Check whether cookie presents.
    if ( RemotePC::Holder::Instance().HasCookie() ) {
        // Restore connection if any.
        [viewController onConnect];       
    }
    
    // Localytics setup.
    [[LocalyticsSession shared] LocalyticsSession:@"fcf4cef7273eabe14aaa97e-2086a592-4b8a-11e0-c50b-007af5bd88a0"];
    [[LocalyticsSession shared] resume];
    [[LocalyticsSession shared] upload];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Localytics.
    [[LocalyticsSession shared] resume];
    [[LocalyticsSession shared] upload];
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Localytics.
    [[LocalyticsSession shared] close];
    [[LocalyticsSession shared] upload];
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
#if defined(_DEBUG) || defined(DEBUG)
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
#endif
}

+ (iRemoteAppDelegate*)delegate {
    return (iRemoteAppDelegate*)[UIApplication sharedApplication].delegate;
}

- (iRemoteViewController *)loginController {
	return viewController;
}

- (AboutController *)aboutController {
    return aboutController;
}

- (HelpController *)helpController {
    return helpController;
}

- (void)onConnected {
    // Unlock cookie.
    RemotePC::Holder::Instance().LockCookie(false);
    // Destroy wakeup time if any.
    if ( wakeupTime ) {
        [wakeupTime release];
        wakeupTime = nil;
    }   
    // Notify login view.
    [viewController onConnected];
}

- (void)onApproved {
    // Notify login view on successful approval.
    [viewController onApproved];
    // Switch to content page.
    [self switchToContentPage];
    
    // Log event.
    [[LocalyticsSession shared] tagEvent:@"connected"];
}

- (void)onPasswordRequested:(NSNumber*)wrongPass {
    // Check whether login controller is presented and 
    if ( nil == viewController.view.superview && RemotePC::Holder::Instance().IsConnected() ) 
        return;
    // Delegate password requesting to login controller.
    [viewController requestPassword:wrongPass];
}

- (void)onError:(NSString*)error {
    // Note: This method may be invoked even after connection is established. 
    // Of course, recorrect should not occur in this case. This is prevented by releasing 
    // wateupTime as soon as connection gets extablished (inside onConnected routine). 

    // Additional loop of reconnect attempts that should orrur after waking up.
    // 1. Check whether cookie presents.
    if ( RemotePC::Holder::Instance().HasCookie() ) {
        // 2. Check whether wakeup time exists.
        if ( wakeupTime ) {
            // 3. Check whether wakeup is not yet expired.
            NSTimeInterval intervalAfterWakeup = [[NSDate date] timeIntervalSinceDate:wakeupTime];
            if ( intervalAfterWakeup < RemotePC::UIConfig::reconnectAfterWakeup_ ) {            
                // Try to reconnect again.
                [viewController onError:error];                    
                [viewController onConnect];   
                return;
            } else {
                // Destroy wakeup time.
                [wakeupTime release];
                wakeupTime = nil;
            }
        }        
    }    
    
    // Unlock cookie.
    RemotePC::Holder::Instance().LockCookie(false);   
    // Cookie should be released here to prevent automatic reconnection after failure. 
    RemotePC::Holder::Instance().ReleaseCookie();
    
    // Check whether login screen is hidden.
    if ( !viewController.view.superview ) {
        // Switch to login screen.
        [self onDisconnected];
    }    
    // Display error on login screen.    
    [viewController onError:error];    
}

- (void)onDisconnected {
    [self switchToLoginPageWithMessage:@"Connection is terminated by host."];
}

- (void)onQiut {
    [self switchToLoginPageWithMessage:@""];
}

- (void)warnOnOldRemotePC {
	[Controls showOldRemotePCAlert];
}

- (void)showShellResponse:(NSString*)response {
    if ( ![self isContentOnTop] )
        return;
	if ( [mainController isKindOfClass:[UITabBarController class]] ) {
		UITabBarController *mainTabController = (UITabBarController *)mainController;		
		// TODO: Refactor this. Shell controller should not be located in such a way.
		ShellController *shellController = [mainTabController.viewControllers objectAtIndex:1];
		[shellController showShellResponse:response];
	}
}

- (void)dismissModalForController:(UIViewController *)controller {
	// Obsolete checks. 
    // Check whether controller is presented.
    // if ( nil != controller.view.superview ) {
	// Check whether modal view controller is presented.
    // if ( controller.modalViewController ) {
	@try {
		// Dismiss modal view.
		[controller.presentedViewController dismissViewControllerAnimated:NO completion:NULL];
	} @catch (NSException * e) {
	}
}

- (void)dismissModal {
    [self dismissModalForController:viewController];
    [self dismissModalForController:mainController];    
}

- (void)notifyControllersOn:(SEL)aSelector {
	if ( [mainController isKindOfClass:[UITabBarController class]] ) {
		UITabBarController *mainTabController = (UITabBarController *)mainController;
		// Enumerate all tabbar controllers.
		for ( id<NSObject> controller in mainTabController.viewControllers ) {
			// Check whether each controller conforms to 'TabbarController' protocol
			// and responds to 'selector' method.
			if ( [controller conformsToProtocol:@protocol(TabbarController)] &&
				[controller respondsToSelector:aSelector] ) {
				// Invoke target method.
				[controller performSelector:aSelector];
			}
		}		
	} else if ( [mainController respondsToSelector:aSelector] ) {
		[mainController performSelector:aSelector];
	}
}

- (void)switchToLoginPageWithMessage:(NSString *)message {
    // Dismiss modal view before switch.
    [self dismissModal];
    
    // Check whether we are not on login page.
    if ( nil == viewController.view.superview ) {
        // Remember interface orientation we had before going to sleep or exiting.
        previousOrientation = [UIApplication sharedApplication].statusBarOrientation;
        
        // Perform switch:
        // 1. Remove current view.
        [mainController.view removeFromSuperview];
        // 2. Display login view.
        [signInWindow addSubview:viewController.view];      
		// 3. Notify controllers on disconnection.
		[self notifyControllersOn:@selector(disconnected)];
        // 4. Update window's root controller.
        signInWindow.rootViewController = viewController;
    }    
    
    // Update login page error message.
    [viewController onError:message];             
}

- (void)switchToContentPage {
    // Dismiss modal view before switch.
    [self dismissModal];
    
    // Check whether we are not on content page.
    if ( nil != mainController.view.superview ) 
        return;

    // Restore previous interface orientation.
    // This must be done before switching views.
    [[UIApplication sharedApplication] setStatusBarOrientation:previousOrientation animated:YES];
    
    // Perform switch:
    // 1. Remove login view.
    [viewController.view removeFromSuperview];
    // 2. Display content view.
    [signInWindow addSubview:mainController.view];       
	// 3. Notify controllers on connection.
	[self notifyControllersOn:@selector(connected)];
    // 4. Update window's root controller.
    signInWindow.rootViewController = mainController;
    
    // Notify server on successful UI initialization.
    RemotePC::Holder::Instance().GetServer()->SetUIReady();
}

- (BOOL)isContentOnTop {
    return ( nil != mainController.view.superview );
}

#pragma mark Life cycle tools

- (void)dealloc {
    if ( wakeupTime )
        [wakeupTime release];
    [super dealloc];
}

@end
