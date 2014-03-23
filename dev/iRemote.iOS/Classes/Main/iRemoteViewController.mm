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

#import "iRemoteViewController.h"
#import "iRemoteAppDelegate.h"

// Platform versioning
#include "Common/Common.h"
#include "Versions/Versions.h"

// Controls
#import "../Controls/Controls.h"
#import "../Controls/TiledButton.h"
#import "../Controls/PasswordAlert.h"

// State tool
#import "../State/LocalStorage.h"

// Doc controllers
#import "../Doc/AboutController.h"
#import "../Doc/HelpController.h"
#import "../Doc/TutorialController.h"

// Hosts controller
#import "../Hosts/HostsViewController.h"

// AdMob support
#import "../../3rdParty/AdMob/AdMobDelegate.h"
#import "../../3rdParty/AdMob/AdMobView.h"

@interface iRemoteViewController(PrivateMethods)

- (void)showConnecting;
- (void)showNormal;
- (void)showAuthenticating;

@end

#include "Connector/Server.h"
#include "../State/UIConfig.h"

@implementation iRemoteViewController

- (IBAction)onAbout {
    // Disallow 'About' action while attempting to connect.
    if ( !ipAddress.enabled )
        return;    
    // Display About page.
  	[AboutController showOnTopOf:self];
}

- (IBAction)onHelp {
    // Disallow 'Help' action while attempting to connect.
    if ( !ipAddress.enabled )
         return;    
    // Display help content.
	[HelpController showOnTopOf:self];
}

- (IBAction)onTutorial {
    // Disallow 'Tutorial' action while attempting to connect.
    if ( !ipAddress.enabled )
		return;    
	// Display tutorial content.
	[TutorialController showOnTopOf:self];
}

- (IBAction)onHosts {
    // Disallow viewing hosts while attempting to connect.
    if ( !ipAddress.enabled )
        return;        
    // Present hosts controller.
    HostsViewController *hosts = [[HostsViewController alloc] initWithStyle:UITableViewStylePlain];
    [self pushViewController:hosts animated:YES];
    [hosts release];
}

- (IBAction)onConnect {    
	// Validate input fields.
	NSCharacterSet *whiteCharacters = [NSCharacterSet whitespaceCharacterSet];
	ipAddress.text = [ipAddress.text stringByTrimmingCharactersInSet:whiteCharacters];
	service.text = [service.text stringByTrimmingCharactersInSet:whiteCharacters];
    // Initiate connection to host. 	
    RemotePC::Holder::Instance().Connect([ipAddress.text UTF8String], [service.text UTF8String]);
    // Display connection progress.
    [self showConnecting];
    // Start timer to prevent endless delay.
    if ( connectingTimer )
        [connectingTimer release];
    connectingTimer = [[NSTimer alloc] initWithFireDate:[NSDate dateWithTimeIntervalSinceNow:(RemotePC::UIConfig::connectionTimeout_)] interval:0.0 target:self selector:@selector(connectingExpired:) userInfo:nil repeats:NO];    
    [[NSRunLoop mainRunLoop] addTimer:connectingTimer forMode:NSDefaultRunLoopMode];
}

- (IBAction)onCancel {
    // Invalidate timer.
    [connectingTimer invalidate];    
    // Breat current connection.
    RemotePC::Holder::Instance().Stop();
    // Notify UI.
    [self showNormal];
}

- (void)requestPassword:(NSNumber*)wrongPass {
    if ( passwordAlert ) {
        [passwordAlert hide];
        [passwordAlert release];
    }
    passwordAlert = [[PasswordAlert alloc] initWithPass:wrongPass andDelegate:self];
    [passwordAlert show];
}

/**
 * UIAlertViewDelegate method
 */
- (void)alertView:(UIAlertView *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
	if ( 0 == buttonIndex ) {
        // 'Cancel' handler. 
        // Simulate cancel event.
        [self onCancel];
    } else if ( 1 == buttonIndex ) {
        // 'OK' handler.
        // Chech whether we are still connected to th host.
        if ( !RemotePC::Holder::Instance().IsConnected() ) 
        {
            [self onCancel];
            return;
        }
        
        // Get password.
        NSString *password = [(PasswordAlert *)actionSheet password];
        // Submit password to server.
        RemotePC::Holder::Instance().SubmitPassword([password UTF8String]);
        // Update UI status strings.
        [self showAuthenticating];
        
        // After this method system will destroy PasswordAlert and it's child - password field.
        // This causes keyboard disappearance. To prevent this we should setup new responder.
        [hiddenEdit becomeFirstResponder];        
    }
    
    // Hide and destroy password alert.
    [passwordAlert hide];
    [passwordAlert release];
    passwordAlert = nil;
}

- (BOOL)isConnectng {
    return !connectingIndicator.hidden;
}

- (void)onConnected {
    [connectingTimer invalidate];    
    cancelButton.hidden = NO;
    statusText1.text = @"Approving connection...";
    statusText2.text = @"Press 'Allow' button on PC to proceed.";        
}

- (void)onApproved {   
    // Revert UI to normal state after approval.
    [self showNormal];
}

- (void)onError:(NSString*)message {
    // Invalidate timer.
    [connectingTimer invalidate];    
    // Init controls.
    [self showNormal];
    // Display error message.
    errorMessage.text = message;
}

- (void)showConnecting {
    [hiddenEdit becomeFirstResponder];       
    ipAddress.enabled = NO;
    service.enabled = NO;
    connectButton.hidden = YES;
    connectingIndicator.hidden = NO;    
    [connectingIndicator startAnimating]; 
    cancelButton.hidden = YES;
    errorMessage.text = @"";
    errorMessage.hidden = YES;
    statusText1.hidden = NO;
    statusText2.hidden = NO;    
    statusText1.text = @"Connecting to host...";
    statusText2.text = @"Establishing TCP connection."; 
    hostsButton.enabled = NO;
}

- (void)showNormal {
    if ( passwordAlert ) {
        [passwordAlert hide];
        [passwordAlert release];
        passwordAlert = nil;
    }    
    ipAddress.enabled = YES;
    service.enabled = YES;
    [ipAddress becomeFirstResponder];       
    connectButton.hidden = NO;
    [connectingIndicator stopAnimating]; 
    connectingIndicator.hidden = YES;    
    cancelButton.hidden = YES;
    errorMessage.text = @"";
    errorMessage.hidden = NO;        
    statusText1.hidden = YES;
    statusText2.hidden = YES;     
    hostsButton.enabled = YES;
}

- (void)showAuthenticating {
    // TOCHECK: showConnecting should be probably called here for consistency.
    statusText1.text = @"Approving connection...";
    statusText2.text = @"Verifying your password.";       
}

- (void)connectingExpired:(NSTimer*)theTimer {
    // Breat current connection.
    RemotePC::Holder::Instance().Stop();
    // Notify UI.
    [self onError:@"Connection is expired."];
}

- (void)showIp:(NSString *)ip andPort:(NSString *)port {
	// Disable multiple simultaneous connection attempts.
    if ( !ipAddress.enabled )
		return;
    
    // Set ip and port.
    ipAddress.text = ip;
    service.text = port;       

    // On the phone we attempt to establish connection, when
    // login page is presented again (hosts controller is popped).
    connectWhenPresented = YES;
}

- (void)restoreHostSettingsFromConfig {
    ipAddress.text = [LocalStorage instance].ip;
    service.text = [LocalStorage instance].port;    
}

- (void)saveHostSettingsToConfig {
    [LocalStorage instance].ip = ipAddress.text;
    [LocalStorage instance].port = service.text;    
}

- (void)showKeyboard {
    if ( ipAddress.enabled )
        [ipAddress becomeFirstResponder];    
    else
        [hiddenEdit becomeFirstResponder];    
}

#pragma mark AdMob tools

- (void)initAdMob {
    adMobDelegate = [[AdMobDelegate alloc] initWithViewController:self];
    adMobView = [[AdMobView requestAdWithDelegate:adMobDelegate] retain];
    adMobView.frame = CGRectMake(0.0f, 0.0f, 320.0f, 48.0f);
    [adPlace addSubview:adMobView];    
}

- (void)refreshAd {
    [adMobView requestFreshAd];
}

- (void)startRefreshingAds {
    if ( adRefreshTimer )
        [adRefreshTimer release];
    adRefreshTimer = [[NSTimer alloc] initWithFireDate:
        [NSDate dateWithTimeIntervalSinceNow:(RemotePC::UIConfig::adRefreshInterval_)] 
        interval:RemotePC::UIConfig::adRefreshInterval_ 
        target:self selector:@selector(refreshAd) userInfo:nil repeats:YES];    
    [[NSRunLoop mainRunLoop] addTimer:adRefreshTimer forMode:NSDefaultRunLoopMode];        
}

- (void)stopRefreshingAds {
    [adRefreshTimer invalidate]; 
    [adRefreshTimer release]; 
    adRefreshTimer = nil;
}

#pragma mark View controller methods

- (void)viewDidLoad {
	[super viewDidLoad];
    [self initAdMob];
	// Tablet specific initialization.
	hostsController.tableView = hostsTable;
	// Format version.
	version.text = [NSString stringWithFormat:@"iRemote %d.%d%@", 
		RemotePC::iREMOTE_VER_MAJOR, RemotePC::iREMOTE_VER_MINOR, 
		( 0 == RemotePC::iREMOTE_VER_FIX ) ? @"" : [NSString stringWithFormat:@".%d", RemotePC::iREMOTE_VER_FIX] ];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];    
    [self showKeyboard];
    [self restoreHostSettingsFromConfig];
    [self startRefreshingAds];
	[hostsController updateHosts];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [connectingTimer invalidate];        
    [self saveHostSettingsToConfig]; 
    [self stopRefreshingAds];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	if ( [Controls isTablet] )
		return YES;
	else 	
		return ( UIInterfaceOrientationPortrait == interfaceOrientation );
}

- (NSUInteger)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskPortrait;
}

#pragma mark Navigation controller methods

- (void)navigationController:(UINavigationController *)navigationController didShowViewController:(UIViewController *)viewController animated:(BOOL)animated {
    if ( [self.viewControllers objectAtIndex:0] == viewController ) {        
        if ( connectWhenPresented ) {
            connectWhenPresented = NO;
            [self onConnect];            
        }
    }
}

#pragma mark Text fiels methods

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    if ( textField != ipAddress && textField != service )
        return NO;
    // Perform connect on return from ip or port fields.
    [self onConnect];
    return YES;    
}

#pragma mark Life cycle tools

- (id)init {
    if ((self = [super init])) {
        // Initialize flags and fields.
        connectWhenPresented = NO;       
        adRefreshTimer = nil;
    }
    return self;
}

- (void)dealloc {
    [adMobView release];
    [adMobDelegate release];
    [adRefreshTimer release];
    if ( passwordAlert )
        [passwordAlert release];
    [connectingTimer release];
    [super dealloc];
}

@end
