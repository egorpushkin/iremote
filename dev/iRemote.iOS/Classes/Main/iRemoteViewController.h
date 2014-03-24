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

@class PasswordAlert;
@class TiledButton;
@class HostsViewController;
@class GADBannerView;

@interface iRemoteViewController : UINavigationController< UINavigationControllerDelegate, UIAlertViewDelegate, UITextFieldDelegate > {

	IBOutlet UILabel *version;    
    IBOutlet UILabel *errorMessage;    
    IBOutlet UITextField *ipAddress;    
    IBOutlet UITextField *service;   
    IBOutlet UILabel *statusText1;
    IBOutlet UILabel *statusText2;    
    IBOutlet UIButton *connectButton;
    IBOutlet UIActivityIndicatorView *connectingIndicator;
    IBOutlet UIButton *cancelButton;
    IBOutlet UITextField *hiddenEdit;
    IBOutlet TiledButton *hostsButton;    

    IBOutlet UIView *adPlace;    
    GADBannerView *adMobView;
    NSTimer *adRefreshTimer;
    
    NSTimer *connectingTimer;
    
    PasswordAlert *passwordAlert;
    
	IBOutlet HostsViewController *hostsController;
	IBOutlet UITableView *hostsTable;
    BOOL connectWhenPresented;

}

- (IBAction)onAbout;
- (IBAction)onHelp;
- (IBAction)onTutorial;
- (IBAction)onHosts;

- (IBAction)onConnect;
- (IBAction)onCancel;

- (void)requestPassword:(NSNumber*)wrongPass;

- (BOOL)isConnectng;

- (void)onConnected;
- (void)onApproved;
- (void)onError:(NSString*)message;

- (void)showIp:(NSString *)ip andPort:(NSString *)port;

@end

