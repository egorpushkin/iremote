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

#import "FunctionsController.h"
#import "../Main/iRemoteAppDelegate.h"
#import "../Controls/TiledButton.h"

// Indicators provider
#import "../Controls/IndicatorsProvider.h"

// Help controller
#import "../Doc/HelpController.h"

#include "Connector/Server.h"
#include "KeyCodes/VirtualCodes.Win32.h"

@implementation FunctionsController

- (IBAction)onVolumeChanged {
    RemotePC::Holder::Instance().OnVolume(volumeSlider.value);
}

- (IBAction)onButtonDown:(id)sender {
    TiledButton *button = sender;
    RemotePC::Holder::Instance().OnVirtualKey(button.vk, true);
}

- (IBAction)onButtonUp:(id)sender {
    TiledButton *button = sender;
    RemotePC::Holder::Instance().OnVirtualKey(button.vk, false);    
}

// IIndicatorsView protocol section
- (void)volueChangedLocal {
    @synchronized(self) {
        volumeSlider.value = lastVolumeValue;
    }
}

- (void)volumeChanged:(unsigned long)value {
    @synchronized(self) {
        lastVolumeValue = value;
    }
    [self performSelectorOnMainThread:@selector(volueChangedLocal) withObject:nil waitUntilDone:NO];
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
    [self registerActionsFor:keyEsc withVirtualKey:VK_ESCAPE];
    [self registerActionsFor:keyTab withVirtualKey:VK_TAB];
    [self registerActionsFor:keyEnter withVirtualKey:VK_RETURN];    
    [self registerActionsFor:keyShift withVirtualKey:VK_SHIFT];
    [self registerActionsFor:keyCtrl withVirtualKey:VK_CONTROL];
    [self registerActionsFor:keyAlt withVirtualKey:VK_MENU];    
    [self registerActionsFor:keyWin withVirtualKey:VK_LWIN];
    [self registerActionsFor:keyPopup withVirtualKey:VK_APPS];
    [self registerActionsFor:keySpace withVirtualKey:VK_SPACE];    
    [self registerActionsFor:keyIns withVirtualKey:VK_INSERT];
    [self registerActionsFor:keyDel withVirtualKey:VK_DELETE];
    [self registerActionsFor:keyHome withVirtualKey:VK_HOME];    
    [self registerActionsFor:keyEnd withVirtualKey:VK_END];
    [self registerActionsFor:keyPgUp withVirtualKey:VK_PRIOR];
    [self registerActionsFor:keyPgDn withVirtualKey:VK_NEXT];    
    [self registerActionsFor:keyLeft withVirtualKey:VK_LEFT];
    [self registerActionsFor:keyRight withVirtualKey:VK_RIGHT];
    [self registerActionsFor:keyUp withVirtualKey:VK_UP];    
    [self registerActionsFor:keyDown withVirtualKey:VK_DOWN];  
    [self registerActionsFor:keyEject withVirtualKey:VK_APPS];  
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Permanently subscribe on events from different indicators.
        [[IndicatorsProvider instance] subscribe:self];
    }
    return self;
}

- (void)dealloc {
    [[IndicatorsProvider instance] unsubscribe:self];
    [super dealloc];    
}

@end
