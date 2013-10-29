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

- (UITextField *)createPasswordField {
	UITextField *returnTextField = [[UITextField alloc] initWithFrame:CGRectMake(22.0, 100.0, 240, 31)];
    returnTextField.borderStyle = UITextBorderStyleRoundedRect;
    returnTextField.textColor = [UIColor blackColor];
	returnTextField.font = [UIFont systemFontOfSize:14.0];
    returnTextField.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
    returnTextField.placeholder = @"<password>";
    returnTextField.backgroundColor = [UIColor clearColor];	
	returnTextField.keyboardType = UIKeyboardTypeDefault;
	returnTextField.returnKeyType = UIReturnKeyDefault;	
	returnTextField.secureTextEntry = YES;	// make the text entry secure (bullets).	
	returnTextField.clearButtonMode = UITextFieldViewModeWhileEditing;	// has a clear 'x' button to the right
	return returnTextField;
}

- (id)initWithPass:(NSNumber*)wrongPass andDelegate:(id)delegate {
    NSString *alertTitle = nil;
    NSString *alertText = nil;
    if ( ![wrongPass boolValue] ) {
        alertTitle = @"Enter Password";
        alertText = @"Enter password to get control over host machine.\n\n\n";
    } else {
        alertTitle = @"Wrong Password";
        alertText = @"Enter correct password to get control over host machine.\n\n\n";
    }
    
    if (self = [super initWithTitle:alertTitle message:alertText
        delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil]) {     
        hostDelegate = delegate;
        // Construct and add password field.
        passwordField = [self createPasswordField];
        passwordField.delegate = self;
        [self addSubview:passwordField];      
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

- (void)willPresentAlertView:(UIAlertView *)alertView {
	if ( ![Controls isTablet] ) {
		self.frame = CGRectMake(20.0f, 40.0f, 286.0f, 210.0f);    
	}
    [passwordField becomeFirstResponder];                
}

#pragma mark Text fields methods

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    if ( textField != passwordField )
        return NO;
    [hostDelegate alertView:self clickedButtonAtIndex:1];
    return YES;    
}

- (NSString*)password {
    return passwordField.text;
}

- (void)dealloc {
    [passwordField release];
    [super dealloc];
}

@end
