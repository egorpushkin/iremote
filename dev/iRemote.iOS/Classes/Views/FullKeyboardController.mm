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

#import "FullKeyboardController.h"
#import "../Controls/ScreenshotProvider.h"
#import "../Main/iRemoteAppDelegate.h"
#import "../Controls/ControlArea.h"
#import "../Controls/ActiveButton.h"

#include "Connector/Server.h"
#include "KeyCodes/VirtualCodes.Win32.h"

@implementation FullKeyboardController

// Global handlers
//////////////////////////////////////////////////////////////////////////

- (IBAction)onBack {
    self.tabBarController.selectedIndex = 0;
}

// Screen rotations and subscription on screenshots
//////////////////////////////////////////////////////////////////////////

#define DEGREES_TO_RADIANS(__ANGLE__) ((__ANGLE__) / 180.0 * M_PI)

-(void)setViewToLandscape:(UIView*)viewObject {
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    [viewObject setCenter:CGPointMake(160, 240)];
    viewObject.transform = CGAffineTransformMakeRotation(DEGREES_TO_RADIANS(90));
    viewObject.bounds = CGRectMake(- ( screenRect.size.height - 480 ) / 2, 0, 480, 320);
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];    
    self.view.userInteractionEnabled = NO;
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];    
    
    if ( !rotated ) {
        // Set landscape mode manually (with animation).
        [UIView beginAnimations:@"rotation" context:nil];
        [UIView setAnimationDuration:0.3f];
        [self setViewToLandscape:self.view];  
        [UIView setAnimationDelegate:self];
        [UIView setAnimationDidStopSelector:@selector(keyboardRotated)];
        [UIView commitAnimations];
        rotated = YES;
    }    

    // Subscribe on screenshots.
    [[ScreenshotProvider instance] setScreenshotHost:controlArea];
}

- (void)viewWillDisappear:(BOOL)animated {
    // Unsibscribe from screenshots.
    [[ScreenshotProvider instance] setScreenshotHost:nil];
}

- (void)keyboardRotated {
    self.view.userInteractionEnabled = YES;    
}

// Screen initialization
//////////////////////////////////////////////////////////////////////////

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        self.hidesBottomBarWhenPushed = YES;
        rotated = NO;
    }
    return self;
}

// Button handlers
//////////////////////////////////////////////////////////////////////////

- (IBAction)onButtonDown:(id)sender {
    TiledButton *button = sender;
    RemotePC::Holder::Instance().OnVirtualKey(button.vk, true);
}

- (IBAction)onButtonUp:(id)sender {
    TiledButton *button = sender;
    RemotePC::Holder::Instance().OnVirtualKey(button.vk, false);    
}

- (void)registerActionsFor:(TiledButton *)button withVirtualKey:(unsigned short)vk {
    // Remember virtual key code to use it later in handlers.
    button.vk = vk;
    // Setup handlers.
    [button addTarget:self action:@selector(onButtonDown:) forControlEvents:
        UIControlEventTouchDown];
    [button addTarget:self action:@selector(onButtonUp:) forControlEvents: 
        UIControlEventTouchUpInside | UIControlEventTouchUpOutside | UIControlEventTouchCancel]; 
}

- (void)viewDidLoad {
    // Configure handlers for all buttons.
    [self registerActionsFor:keyEsc withVirtualKey:VK_ESCAPE];
    [self registerActionsFor:keyF1 withVirtualKey:VK_F1];
    [self registerActionsFor:keyF2 withVirtualKey:VK_F2];
    [self registerActionsFor:keyF3 withVirtualKey:VK_F3];
    [self registerActionsFor:keyF4 withVirtualKey:VK_F4];
    [self registerActionsFor:keyF5 withVirtualKey:VK_F5];
    [self registerActionsFor:keyF6 withVirtualKey:VK_F6];
    [self registerActionsFor:keyF7 withVirtualKey:VK_F7];
    [self registerActionsFor:keyF8 withVirtualKey:VK_F8];
    [self registerActionsFor:keyF9 withVirtualKey:VK_F9];
    [self registerActionsFor:keyF10 withVirtualKey:VK_F10];
    [self registerActionsFor:keyF11 withVirtualKey:VK_F11];
    [self registerActionsFor:keyF12 withVirtualKey:VK_F12];
    [self registerActionsFor:keyNumLock withVirtualKey:VK_NUMLOCK];
    [self registerActionsFor:keyScrollLock withVirtualKey:VK_SCROLL];
    [self registerActionsFor:keyPrintScreen withVirtualKey:VK_SNAPSHOT];
    [self registerActionsFor:keyBreak withVirtualKey:VK_PAUSE];
    [self registerActionsFor:keyTilda withVirtualKey:VK_OEM_3];
    [self registerActionsFor:key1 withVirtualKey:'1'];
    [self registerActionsFor:key2 withVirtualKey:'2'];
    [self registerActionsFor:key3 withVirtualKey:'3'];
    [self registerActionsFor:key4 withVirtualKey:'4'];
    [self registerActionsFor:key5 withVirtualKey:'5'];
    [self registerActionsFor:key6 withVirtualKey:'6'];
    [self registerActionsFor:key7 withVirtualKey:'7'];
    [self registerActionsFor:key8 withVirtualKey:'8'];
    [self registerActionsFor:key9 withVirtualKey:'9'];
    [self registerActionsFor:key0 withVirtualKey:'0'];
    [self registerActionsFor:keyMinus withVirtualKey:VK_OEM_MINUS];
    [self registerActionsFor:keyAssign withVirtualKey:VK_OEM_PLUS];
    [self registerActionsFor:keyBackspace withVirtualKey:VK_BACK];
    [self registerActionsFor:keyHome withVirtualKey:VK_HOME];
    [self registerActionsFor:keyTab withVirtualKey:VK_TAB];
    [self registerActionsFor:keyQ withVirtualKey:'Q'];
    [self registerActionsFor:keyW withVirtualKey:'W'];
    [self registerActionsFor:keyE withVirtualKey:'E'];
    [self registerActionsFor:keyR withVirtualKey:'R'];
    [self registerActionsFor:keyT withVirtualKey:'T'];
    [self registerActionsFor:keyY withVirtualKey:'Y'];
    [self registerActionsFor:keyU withVirtualKey:'U'];
    [self registerActionsFor:keyI withVirtualKey:'I'];
    [self registerActionsFor:keyO withVirtualKey:'O'];
    [self registerActionsFor:keyP withVirtualKey:'P'];
    [self registerActionsFor:keyOpenSquareBracket withVirtualKey:VK_OEM_4];
    [self registerActionsFor:keyCloseSquareBracket withVirtualKey:VK_OEM_6];
    [self registerActionsFor:keyBackSlash withVirtualKey:VK_OEM_5];
    [self registerActionsFor:keyPgUp withVirtualKey:VK_PRIOR];
    [self registerActionsFor:keyCapsLock withVirtualKey:VK_CAPITAL];
    [self registerActionsFor:keyA withVirtualKey:'A'];
    [self registerActionsFor:keyS withVirtualKey:'S'];
    [self registerActionsFor:keyD withVirtualKey:'D'];
    [self registerActionsFor:keyF withVirtualKey:'F'];
    [self registerActionsFor:keyG withVirtualKey:'G'];
    [self registerActionsFor:keyH withVirtualKey:'H'];
    [self registerActionsFor:keyJ withVirtualKey:'J'];
    [self registerActionsFor:keyK withVirtualKey:'K'];
    [self registerActionsFor:keyL withVirtualKey:'L'];
    [self registerActionsFor:keySemicolumn withVirtualKey:VK_OEM_1];
    [self registerActionsFor:keyQuotes withVirtualKey:VK_OEM_7];
    [self registerActionsFor:keyEnter withVirtualKey:VK_RETURN];
    [self registerActionsFor:keyPgDn withVirtualKey:VK_NEXT];
    [self registerActionsFor:keyLShift withVirtualKey:VK_LSHIFT];
    [self registerActionsFor:keyZ withVirtualKey:'Z'];
    [self registerActionsFor:keyX withVirtualKey:'X'];
    [self registerActionsFor:keyC withVirtualKey:'C'];
    [self registerActionsFor:keyV withVirtualKey:'V'];
    [self registerActionsFor:keyB withVirtualKey:'B'];
    [self registerActionsFor:keyN withVirtualKey:'N'];
    [self registerActionsFor:keyM withVirtualKey:'M'];
    [self registerActionsFor:keyOpenAngleBracket withVirtualKey:VK_OEM_COMMA];
    [self registerActionsFor:keyCloseAngleBracket withVirtualKey:VK_OEM_PERIOD];
    [self registerActionsFor:keySlash withVirtualKey:VK_OEM_2];
    [self registerActionsFor:keyRShift withVirtualKey:VK_RSHIFT];
    [self registerActionsFor:keyUp withVirtualKey:VK_UP];
    [self registerActionsFor:keyEnd withVirtualKey:VK_END];
    [self registerActionsFor:keyLCtrl withVirtualKey:VK_LCONTROL];
    [self registerActionsFor:keyLWin withVirtualKey:VK_LWIN];
    [self registerActionsFor:keyLAlt withVirtualKey:VK_LMENU];
    [self registerActionsFor:keySpace withVirtualKey:VK_SPACE];
    [self registerActionsFor:keyRAlt withVirtualKey:VK_RMENU];
    [self registerActionsFor:keyRWin withVirtualKey:VK_RWIN];
    [self registerActionsFor:keyPopup withVirtualKey:VK_APPS];
    [self registerActionsFor:keyIns withVirtualKey:VK_INSERT];
    [self registerActionsFor:keyDel withVirtualKey:VK_DELETE];
    [self registerActionsFor:keyLeft withVirtualKey:VK_LEFT];
    [self registerActionsFor:keyDown withVirtualKey:VK_DOWN];
    [self registerActionsFor:keyRight withVirtualKey:VK_RIGHT];
}

@end
