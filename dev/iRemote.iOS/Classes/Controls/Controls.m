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

#import "Controls.h"
#import "Content.h"

@implementation Controls

+ (UIBarButtonItem *)backButtonItem:(id)target action:(SEL)action {
	// Create and configure button.
	UIButton *backButton = [UIButton buttonWithType:UIButtonTypeCustom];
	backButton.frame = CGRectMake(0, 0, 49, 30);
    [backButton setImage:[[Content instance] imageNamed:@"BackNormal"] forState:UIControlStateNormal];
    [backButton setImage:[[Content instance] imageNamed:@"BackPressed"] forState:UIControlStateHighlighted];    
	[backButton addTarget:target action:action forControlEvents:UIControlEventTouchUpInside];
	// Wrap button into bar button item.	
	return [[[UIBarButtonItem alloc] initWithCustomView:backButton] autorelease];
}

+ (void)showOldRemotePCAlert {
	UIAlertView *alert = [[UIAlertView alloc] 
		initWithTitle:@"Software Update" 
		message:@"Your Remote PC Suite desktop software is too old. Please, follow to http://www.scientific-soft.com/mobile/apps/iremote/ to download latest application package."
		delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];	
	[alert release];	
}

+ (void)showEULADeclinedAlert {
    UIAlertView *alert = [[UIAlertView alloc] 
        initWithTitle:@"EULA Declined" 
        message:@"You may either Accept this EULA and proceed to the next step, or press Home button to quit the application."
        delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];	
	[alert release];    
}

+ (NSString *)pathForResource:(NSString *)name ofType:(NSString *)extension {
	if ( [Controls isTablet] ) {
		name = [NSString stringWithFormat:@"%@-ipad", name];
	}
	return [[NSBundle mainBundle] pathForResource:name ofType:extension];
}

+ (BOOL)isTablet {
	return 
		( UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad );
}

@end
