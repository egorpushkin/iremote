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

@class ControlArea;
@class ActiveButton;

@interface FullKeyboardController : UIViewController {
    
    IBOutlet ControlArea *controlArea;   
    
    BOOL rotated;
    
    IBOutlet ActiveButton *keyEsc;
    IBOutlet ActiveButton *keyF1;
    IBOutlet ActiveButton *keyF2;
    IBOutlet ActiveButton *keyF3;
    IBOutlet ActiveButton *keyF4;
    IBOutlet ActiveButton *keyF5;
    IBOutlet ActiveButton *keyF6;
    IBOutlet ActiveButton *keyF7;
    IBOutlet ActiveButton *keyF8;
    IBOutlet ActiveButton *keyF9;
    IBOutlet ActiveButton *keyF10;
    IBOutlet ActiveButton *keyF11;
    IBOutlet ActiveButton *keyF12;
    IBOutlet ActiveButton *keyNumLock;
    IBOutlet ActiveButton *keyScrollLock;
    IBOutlet ActiveButton *keyPrintScreen;
    IBOutlet ActiveButton *keyBreak;
    IBOutlet ActiveButton *keyTilda;
    IBOutlet ActiveButton *key1;
    IBOutlet ActiveButton *key2;
    IBOutlet ActiveButton *key3;
    IBOutlet ActiveButton *key4;
    IBOutlet ActiveButton *key5;
    IBOutlet ActiveButton *key6;
    IBOutlet ActiveButton *key7;
    IBOutlet ActiveButton *key8;
    IBOutlet ActiveButton *key9;
    IBOutlet ActiveButton *key0;
    IBOutlet ActiveButton *keyMinus;
    IBOutlet ActiveButton *keyAssign;
    IBOutlet ActiveButton *keyBackspace;
    IBOutlet ActiveButton *keyHome;
    IBOutlet ActiveButton *keyTab;
    IBOutlet ActiveButton *keyQ;
    IBOutlet ActiveButton *keyW;
    IBOutlet ActiveButton *keyE;
    IBOutlet ActiveButton *keyR;
    IBOutlet ActiveButton *keyT;
    IBOutlet ActiveButton *keyY;
    IBOutlet ActiveButton *keyU;
    IBOutlet ActiveButton *keyI;
    IBOutlet ActiveButton *keyO;
    IBOutlet ActiveButton *keyP;
    IBOutlet ActiveButton *keyOpenSquareBracket;
    IBOutlet ActiveButton *keyCloseSquareBracket;
    IBOutlet ActiveButton *keyBackSlash;
    IBOutlet ActiveButton *keyPgUp;
    IBOutlet ActiveButton *keyCapsLock;
    IBOutlet ActiveButton *keyA;
    IBOutlet ActiveButton *keyS;
    IBOutlet ActiveButton *keyD;
    IBOutlet ActiveButton *keyF;
    IBOutlet ActiveButton *keyG;
    IBOutlet ActiveButton *keyH;
    IBOutlet ActiveButton *keyJ;
    IBOutlet ActiveButton *keyK;
    IBOutlet ActiveButton *keyL;
    IBOutlet ActiveButton *keySemicolumn;
    IBOutlet ActiveButton *keyQuotes;
    IBOutlet ActiveButton *keyEnter;
    IBOutlet ActiveButton *keyPgDn;
    IBOutlet ActiveButton *keyLShift;
    IBOutlet ActiveButton *keyZ;
    IBOutlet ActiveButton *keyX;
    IBOutlet ActiveButton *keyC;
    IBOutlet ActiveButton *keyV;
    IBOutlet ActiveButton *keyB;
    IBOutlet ActiveButton *keyN;
    IBOutlet ActiveButton *keyM;
    IBOutlet ActiveButton *keyOpenAngleBracket;
    IBOutlet ActiveButton *keyCloseAngleBracket;
    IBOutlet ActiveButton *keySlash;
    IBOutlet ActiveButton *keyRShift;
    IBOutlet ActiveButton *keyUp;
    IBOutlet ActiveButton *keyEnd;
    IBOutlet ActiveButton *keyLCtrl;
    IBOutlet ActiveButton *keyLWin;
    IBOutlet ActiveButton *keyLAlt;
    IBOutlet ActiveButton *keySpace;
    IBOutlet ActiveButton *keyRAlt;
    IBOutlet ActiveButton *keyRWin;
    IBOutlet ActiveButton *keyPopup;
    IBOutlet ActiveButton *keyIns;
    IBOutlet ActiveButton *keyDel;
    IBOutlet ActiveButton *keyLeft;
    IBOutlet ActiveButton *keyDown;
    IBOutlet ActiveButton *keyRight;
    
}

- (IBAction)onBack;

@end
