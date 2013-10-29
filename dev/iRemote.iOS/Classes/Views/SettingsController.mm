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

#import "SettingsController.h"

// Persistent UI config.
#include "Common/Common.h"
#include "../State/UIConfig.h"

@implementation SettingsController

- (IBAction)panningModeChanged:(id)sender {
    RemotePC::UIConfig::Instance().SetPanningMode(inversePanning.on);
}

- (IBAction)autorotationModeChanged:(id)sender {
    RemotePC::UIConfig::Instance().EnableAutorotation(autorotation.on);
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];    
    
    // Apply current settings.
    inversePanning.on = RemotePC::UIConfig::Instance().PanningMode();
    autorotation.on = RemotePC::UIConfig::Instance().Autorotation();
}

@end
