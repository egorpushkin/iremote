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
@class FBControlArea;
@class FBPreviewArea;
@class WindowsPanel;
#import "../Controls-iPad/IFBControl.h"

enum {
	ControlPopoverNone              = 0,
	ControlPopoverWindows           = 1 << 0,	
	ControlPopoverMode              = 1 << 1,	
	ControlPopoverSettings          = 1 << 2,		
	ControlPopoverStats             = 1 << 3		
};
typedef NSUInteger ControlPopover;	

@interface ControlViewControlleriPad : UIViewController< UIPopoverControllerDelegate, UITextFieldDelegate, IFBControl > {

	IBOutlet FBControlArea *controlArea_;
	IBOutlet FBPreviewArea *previewArea_;
	IBOutlet WindowsPanel *windowsPanel_;
	IBOutlet UITextField *defaultEdit_;	
	IBOutlet UITextField *customEdit_;		
	IBOutlet UIView *surfaceProgress_;
	
	IBOutlet UIView *customWinPortrait_;
	IBOutlet UIView *customWinLandscape_;	
	
	ControlPopover controlPopover_;
	UIPopoverController *popoverController_;	
	
}

- (IBAction)onQuit:(id)sender;
- (IBAction)onWindows:(id)sender;
- (IBAction)onMode:(id)sender;
- (IBAction)onSettings:(id)sender;
- (IBAction)onStats:(id)sender;
- (IBAction)onTutorial:(id)sender;
- (IBAction)onHelp:(id)sender;

- (IBAction)toggleDefaultKeybopard:(id)sender;
- (IBAction)toggleCustomKeybopard:(id)sender;
- (IBAction)togglePreview:(id)sender;

@end
