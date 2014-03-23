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

#import "ControlViewControlleriPad.h"
#import "../Main/iRemoteAppDelegate.h"
#import "../Controls-iPad/FBSurfaceHolder.h"
#import "../Controls-iPad/FBControlArea.h"
#import "../Controls-iPad/FBPreviewArea.h"
#import "../Controls-iPad/WindowsPanel.h"
#import "../Controls-iPad/FBModePickerController.h"
#import "SettingsControlleriPad.h"
#import "NetworkStatsController.h"
#import "../State/UISettings.h"
#import "../Doc/TutorialController.h"
#import "../Doc/HelpController.h"

#include "Connector/Server.h"
#include "KeyCodes/VirtualCodes.Win32.h"

@implementation ControlViewControlleriPad

#pragma mark Connection state handlers

- (void)connected {
	// NOTE: Do not repaint OpenGL views here. It causes drawing code
	// to freeze inside presentRenderbuffer during subsequent calls.
}

#pragma mark Popover helpers

- (void)presentPopover:(UIViewController *)viewController sender:(id)sender {	
	popoverController_ = 
		[[UIPopoverController alloc] initWithContentViewController:viewController];
	popoverController_.delegate = self;
	[popoverController_ presentPopoverFromBarButtonItem:sender permittedArrowDirections:UIPopoverArrowDirectionUp animated:YES];	
}

- (ControlPopover)dismissPopover {
	ControlPopover controlPopover = controlPopover_;
	if ( nil != popoverController_ ) {
		[popoverController_ dismissPopoverAnimated:NO];
		[popoverController_ release];
		popoverController_ = nil;		
		controlPopover_ = ControlPopoverNone;
	}
	return controlPopover;
}

#pragma mark User event handlers

- (IBAction)onQuit:(id)sender {
    // Close current connection.
    RemotePC::Holder::Instance().Stop();
    // Notify UI.
    [[iRemoteAppDelegate delegate] onQiut];         
}

- (IBAction)onWindows:(id)sender {
	// Dismiss popover, if presented.
	[self dismissPopover];		
	// Toggle windows panel.
	[UIView beginAnimations:nil context:nil];
	[UIView setAnimationDuration:0.5f];
	windowsPanel_.alpha = ( 0.0f == windowsPanel_.alpha ) ? 1.0f : 0.0f;
	[UIView commitAnimations];
}

- (IBAction)onMode:(id)sender {
	ControlPopover controlPopover = [self dismissPopover];
	if ( ControlPopoverMode == controlPopover ) 
		return;	
	FBModePickerController *viewController = [[FBModePickerController alloc] initWithControlArea:controlArea_];        
	[self presentPopover:viewController sender:sender];
	[viewController release];
	controlPopover_ = ControlPopoverMode;	
}

- (IBAction)onSettings:(id)sender {
	ControlPopover controlPopover = [self dismissPopover];
	if ( ControlPopoverSettings == controlPopover ) 
		return;	
	SettingsControlleriPad *viewController = [[SettingsControlleriPad alloc] initController];        
	[self presentPopover:viewController sender:sender];
	[viewController release];
	controlPopover_ = ControlPopoverSettings;	
}

- (IBAction)onStats:(id)sender {
	ControlPopover controlPopover = [self dismissPopover];
	if ( ControlPopoverStats == controlPopover ) 
		return;	
	NetworkStatsController *viewController = [[NetworkStatsController alloc] initController];        
	[self presentPopover:viewController sender:sender];
	[viewController release];	
	controlPopover_ = ControlPopoverStats;		
}

- (IBAction)onTutorial:(id)sender {	
	[TutorialController showOnTopOf:self];
}

- (IBAction)onHelp:(id)sender {
	[HelpController showOnTopOf:self];
}

#pragma mark Windows panel handlers

- (BOOL)isDefaultKeyboardPresented {
    return [defaultEdit_ isFirstResponder];
}

- (IBAction)toggleDefaultKeybopard:(id)sender {
    if ( [self isDefaultKeyboardPresented] )
        [defaultEdit_ resignFirstResponder];
    else
        [defaultEdit_ becomeFirstResponder];
}

- (BOOL)isCustomKeyboardPresented {
    return [customEdit_ isFirstResponder];
}

- (IBAction)toggleCustomKeybopard:(id)sender {
    if ( [self isCustomKeyboardPresented] )
        [customEdit_ resignFirstResponder];
    else
        [customEdit_ becomeFirstResponder];
}

- (IBAction)togglePreview:(id)sender {
	previewArea_.hidden = !previewArea_.hidden;
}

#pragma mark UIPopoverControllerDelegate section

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController {
	[self dismissPopover];
}

- (BOOL)popoverControllerShouldDismissPopover:(UIPopoverController *)popoverController {
	return YES;
}

#pragma mark UITextFieldDelegate section

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {    
    // Test on backspace button.
    if ( [string length] == 0 ) {
        // Backspace button has been pressed. Notify host on event.
        RemotePC::Holder::Instance().OnVirtualKey(VK_BACK, true);
        RemotePC::Holder::Instance().OnVirtualKey(VK_BACK, false);
    } else {
        // Acquire key code.
        unichar character = [string characterAtIndex:0]; 
        // Notify host on event.
        RemotePC::Holder::Instance().OnUnicodeKey( character );        
    }    
    // Clear hidden field.
    defaultEdit_.text = @" ";       
    return NO;    
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    // Notify host on 'Enter' key pressed.
    RemotePC::Holder::Instance().OnVirtualKey(VK_RETURN, true);
    RemotePC::Holder::Instance().OnVirtualKey(VK_RETURN, false);    
    // Clear hidden field.
    defaultEdit_.text = @" ";       
    return NO;       
}

#pragma mark IFBControl section

- (void)initControl {
	// Hide progress, when surface is ready. 
	if ( [[FBSurfaceHolder instance] surface] ) {
		surfaceProgress_.hidden = YES;
		// No need to listen to surface events any more.
		[[FBSurfaceHolder instance] unsubscribeSafely:self];	
	}
}

- (void)updateControl {
	// Not implemented by design.
}

- (void)updateCursor:(BOOL)server {
	// Not implemented by design.	
}

#pragma mark View controller section

- (void)viewDidLoad {
    [super viewDidLoad];
	// Configure custom keyboard input.
	customEdit_.inputView = customWinPortrait_;	
}

- (void)subscribeLater { 
    [[FBSurfaceHolder instance] subscribe:self];	
    [[FBSurfaceHolder instance] subscribe:controlArea_];
    [[FBSurfaceHolder instance] subscribe:previewArea_];	
	[[UISettings instance] subscribe:controlArea_];
	[[UISettings instance] subscribe:previewArea_];			
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    // Add single space to edit field to distinguish backspace button.
    defaultEdit_.text = @" ";
	
	// Show surface progress.
	surfaceProgress_.hidden = NO;
	
	// This is workaround for hanging on login screen,
	// when swithing to content screen. FBControlArea synchronous drawing
	// (which is caused by subscription mechanisms) hangs inside
	// - [EAGLView drawViewFinal]
	// -- [EAGLContext presentRenderbuffer:GL_RENDERBUFFER_OES];	
	[self performSelector:@selector(subscribeLater) withObject:nil afterDelay:0.5];	
	
	// Repaint areas to clear old screen image, which is still rendered.
	[controlArea_ drawView];
	[previewArea_ drawView];		
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated {
	[super viewWillDisappear:animated];

    [[FBSurfaceHolder instance] unsubscribe:self];	
    [[FBSurfaceHolder instance] unsubscribe:controlArea_];
    [[FBSurfaceHolder instance] unsubscribe:previewArea_];		
	[[UISettings instance] unsubscribe:controlArea_];
	[[UISettings instance] unsubscribe:previewArea_];		
	
	// Check whether popover is presented and dismiss it.
	[self dismissPopover];
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration {
	[super willRotateToInterfaceOrientation:toInterfaceOrientation duration:duration];
	if ( ( UIInterfaceOrientationPortrait == toInterfaceOrientation )
		|| ( UIInterfaceOrientationPortraitUpsideDown == toInterfaceOrientation ) ) {
		customEdit_.inputView = customWinPortrait_;				
	} else {
		customEdit_.inputView = customWinLandscape_;			
	}
	if ( [self isCustomKeyboardPresented] ) {
        [customEdit_ resignFirstResponder];
        [customEdit_ becomeFirstResponder];		
	}
}

#pragma mark Life cycle section

- (void)dealloc {
    [super dealloc];
}

@end
