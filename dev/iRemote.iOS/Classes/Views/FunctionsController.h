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
#import "../Controls/IIndicatorsView.h"

@class TiledButton;

@interface FunctionsController : UIViewController< IIndicatorsView > {

    IBOutlet UISlider *volumeSlider;    
    unsigned long lastVolumeValue;
    
    IBOutlet TiledButton *keyF1;
    IBOutlet TiledButton *keyF2;
    IBOutlet TiledButton *keyF3;    
    IBOutlet TiledButton *keyF4;
    IBOutlet TiledButton *keyF5;
    IBOutlet TiledButton *keyF6;    
    IBOutlet TiledButton *keyF7;
    IBOutlet TiledButton *keyF8;
    IBOutlet TiledButton *keyF9;    
    IBOutlet TiledButton *keyF10;
    IBOutlet TiledButton *keyF11;
    IBOutlet TiledButton *keyF12;    
    IBOutlet TiledButton *keyEsc;
    IBOutlet TiledButton *keyTab;
    IBOutlet TiledButton *keyEnter;    
    IBOutlet TiledButton *keyShift;
    IBOutlet TiledButton *keyCtrl;
    IBOutlet TiledButton *keyAlt;    
    IBOutlet TiledButton *keyWin;
    IBOutlet TiledButton *keyPopup;
    IBOutlet TiledButton *keySpace;    
    IBOutlet TiledButton *keyIns;
    IBOutlet TiledButton *keyDel;
    IBOutlet TiledButton *keyHome;    
    IBOutlet TiledButton *keyEnd;
    IBOutlet TiledButton *keyPgUp;
    IBOutlet TiledButton *keyPgDn;    
    IBOutlet TiledButton *keyLeft;
    IBOutlet TiledButton *keyRight;
    IBOutlet TiledButton *keyUp;    
    IBOutlet TiledButton *keyDown;
    IBOutlet TiledButton *keyEject;

}

- (IBAction)onVolumeChanged;

@end
