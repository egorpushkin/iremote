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

#import "PasswordAlert.h"
#import "Controls.h"

@implementation PasswordAlert

- (id)initWithPass:(NSNumber*)wrongPass andDelegate:(id)delegate {
    NSString *alertTitle = nil;
    NSString *alertText = nil;
    if ( ![wrongPass boolValue] ) {
        alertTitle = @"Enter Password";
        alertText = @"Enter password to get control over host machine.";
    } else {
        alertTitle = @"Wrong Password";
        alertText = @"Enter correct password to get control over host machine.";
    }
    
    if (self = [super initWithTitle:alertTitle message:alertText
        delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil]) {     
        hostDelegate = delegate;
        
        // Configure password field.
        self.alertViewStyle = UIAlertViewStyleSecureTextInput;
        [self textFieldAtIndex:0].delegate = self;
    }
    return self;
}

- (void)hide {
    [self dismissWithClickedButtonIndex:0 animated:YES];
}

#pragma mark Alert view methods

- (void)alertView:(UIAlertView *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
    [hostDelegate alertView:self clickedButtonAtIndex:buttonIndex];
}

#pragma mark Text fields methods

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [hostDelegate alertView:self clickedButtonAtIndex:1];
    return YES;    
}

- (NSString*)password {
    return [self textFieldAtIndex:0].text;
}

- (void)dealloc {
    [super dealloc];
}

@end
