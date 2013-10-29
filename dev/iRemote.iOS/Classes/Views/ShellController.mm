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

#import "ShellController.h"
#import "../Controls/Controls.h"
#import "../Controls/ShellView.h"

// Help controller
#import "../Doc/HelpController.h"

// Server access
#include "Connector/Server.h"
#include "Versions/Features.h"

@implementation ShellController

#pragma mark Helpers

- (void)printIntro {
	[shellOutput clear];
	[shellOutput add:@"           ***** ***"];
	[shellOutput add:@"  *     ******  * **                                     *"];
	[shellOutput add:@" ***   **   *  *  **                                    **"];
	[shellOutput add:@"  *   *    *  *   **                                    **"];
	[shellOutput add:@"          *  *    *                           ****    ********"];
	[shellOutput add:@"***      ** **   *      *** *** **** ****    * ***  ************"];
	[shellOutput add:@" ***     ** **  *      * *** *** **** ***  **   ****    **  * ***"];
	[shellOutput add:@"  **     ** ****      *   *** **  **** ******    **     ** *   ***"];
	[shellOutput add:@"  **     ** **  ***  **    *****   **   ** **    **     ****    ***"];
	[shellOutput add:@"  **     ** **    ** ******** **   **   ** **    **     **********"];
	[shellOutput add:@"  **     *  **    ** *******  **   **   ** **    **     *********"];
	[shellOutput add:@"  **        *     ** **       **   **   ** **    **     ****"];
	[shellOutput add:@"  **    ****      *******    ***   **   **  ******      ******    *"];
	[shellOutput add:@"  *** **  ****    **  ******* ***  ***  ***  ****        *********"];
	[shellOutput add:@"   ****    **     *    *****   ***  ***  ***                *****"];
	[shellOutput add:@"      *"];
	[shellOutput add:@"       **"];
	[shellOutput add:@" "];
	[shellOutput add:@"      *******     *             ***  ***"];
	[shellOutput add:@"    *       *** **               ***  ***"];
	[shellOutput add:@"   *         ** **                **   **"];
	[shellOutput add:@"   **        *  **                **   **"];
	[shellOutput add:@"    ***         **                **   **"];
	[shellOutput add:@"   ** ***       **  ***     ***   **   **"];
	[shellOutput add:@"    *** ***     ** * ***   * ***  **   **"];
	[shellOutput add:@"      *** ***   ***   *** *   *** **   **"];
	[shellOutput add:@"        *** *** **     ****    *****   **"];
	[shellOutput add:@"          ** *****     ********** **   **"];
	[shellOutput add:@"           ** ****     *********  **   **"];
	[shellOutput add:@"            * * **     ****       **   **"];
	[shellOutput add:@"  ***        *  **     ******    ***   **"];
	[shellOutput add:@" *  *********   **     ** ******* *** **** *"];
	[shellOutput add:@"*     *****      **    **  *****   ***  ***"];
	[shellOutput add:@"*                      *"];
	[shellOutput add:@" **                   *"];
	[shellOutput add:@"                     *"];
	[shellOutput add:@"                    *"];
	[shellOutput add:@" "];	
	[shellOutput add:@" "];	
	[shellOutput add:@"iRemote Suite Shell"];	    
	[shellOutput add:@"Type your commands here..."];		
    [shellOutput add:@" "];	
    [shellOutput add:@" "];	    
}

- (void)printRemotePCCompatibilityWarning {
	[shellOutput clear];
	[shellOutput add:@" "];
	[shellOutput add:@" "];
	[shellOutput add:@"  WARNING: "];
	[shellOutput add:@" "];
	[shellOutput add:@"Your Remote PC Suite desktop software"];
	[shellOutput add:@"is too old. Please, follow "];
	[shellOutput add:@"http://www.scientific-soft.com/mobile/apps/iremote/"];
	[shellOutput add:@"to download latest application package."];	
	[shellOutput add:@" "];
	[shellOutput add:@" "];
	[shellOutput add:@"Shell support was introduced in"];
	[shellOutput add:@"Remote PC Suite 1.3. Ensure that"];	
	[shellOutput add:@"you are running Remote PC Suite 1.3"];	
	[shellOutput add:@"or obove."];
	[shellOutput add:@" "];
	[shellOutput add:@" "];
}

#pragma mark UI callbacks section

- (IBAction)onBack:(id)sender {
    self.tabBarController.selectedIndex = 0;    
}

- (IBAction)onHelp {
    // Display help content.
	[HelpController showOnTopOf:self];
}

- (IBAction)onSend {    
    RemotePC::Holder::Instance().ExecuteShellCommand([command.text UTF8String]);    
    command.text = @"";
}

- (IBAction)onClear {
	[shellOutput clear];
}

- (IBAction)onCtrlc {
	RemotePC::Holder::Instance().ExecuteShellCommand(std::string("ctrl-c"));    
}

/**
 * This method enables/disables user input on this screen. Clear, Ctrl-C, Command
 * and Send controls are affected.
 */ 
- (void)enableControls:(BOOL)enable {
	clearButton.userInteractionEnabled = enable;
	ctrlcButton.userInteractionEnabled = enable;
	sendButton.userInteractionEnabled = enable;
	command.userInteractionEnabled = enable;	
}

#pragma mark Internal object model callbacks section

- (void)showShellResponse:(NSString*)response {
    [shellOutput add:response];
}

#pragma mark Animations

- (void)movePromptDown {
	[UIView beginAnimations:nil context:nil];
	[UIView setAnimationDuration:0.3f];
	promptHolder.frame = CGRectMake(
		0.0, 325.0, promptHolder.frame.size.width, promptHolder.frame.size.height);
	shellOutput.frame = CGRectMake(0.0, 0.0, 320.0, 325.0);
	[UIView commitAnimations];
}

- (void)movePromptUp {
	[UIView beginAnimations:nil context:nil];
	[UIView setAnimationDuration:0.3f];
	promptHolder.frame = CGRectMake(
		0.0, 156.0, promptHolder.frame.size.width, promptHolder.frame.size.height);
	shellOutput.frame = CGRectMake(0.0, 0.0, 320.0, 156.0);
	[UIView commitAnimations];
}

#pragma mark Text field (prompt) callbacks

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
	if ( textField == command ) {
		// Hide keyboard.
		[command resignFirstResponder];
		// Move prompt to tabbar.
		[self movePromptDown];
		// Allow to hide keyboard.
		return YES;
	}
	return NO;
}

- (BOOL)textFieldShouldBeginEditing:(UITextField *)textField {
	if ( textField == command ) {
		// Move prompt to the middle of the screen.
		[self movePromptUp];
		// Allow to show keyboard.
		return YES;
	}
	return NO;	
}

#pragma mark Tabbar controller section

- (void)connected {
    // Warn on old RemotePC if shell is not supported.
    int major = 0, minor = 0;    
    BOOL shellSupported = 
		( mc::Error::IsSucceeded(RemotePC::Holder::Instance().GetRemotePCVersion(major, minor)) ) &&
		( RemotePC::Features::RemotePCShell(major, minor) );    
    if ( !shellSupported ) {
        // If shell is not supported UI should be disabled to prevent delivering
        // undesirable messages to server (this may actually lead to disconnection).
		
		// 1. Disable user input.
		[self enableControls:NO];
		// 2. Hide keyboard.
		[command resignFirstResponder];  		
		// 3. Move prompt down.
		[self movePromptDown];	
		// 4. Print warning.
		[self printRemotePCCompatibilityWarning];
    } else {        
        // Shell is supported. Everything goes fine.

		// 1. Enable user input.
		[self enableControls:YES];
		// 2. Display keyboard.
		[command becomeFirstResponder];  
		// 3. Move prompt up.
		[self movePromptUp];				
		// 4. Print introduction.
		[self printIntro];
    }		
}

#pragma mark View controller section

- (void)viewDidLoad {
	// Configure back control.
	self.topViewController.navigationItem.leftBarButtonItem = [Controls backButtonItem:self action:@selector(onBack:)];	
}

#pragma mark Life cycle section

- (void)dealloc {
    [super dealloc];
}

@end
