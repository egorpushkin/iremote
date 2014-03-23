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

#import "HelpController.h"
#import "../Main/iRemoteAppDelegate.h"
#import "../Controls/Controls.h"

@implementation HelpController

+ (void)showOnTopOf:(UIViewController *)controller {
    [controller presentModalViewController:[[iRemoteAppDelegate delegate] helpController] animated:YES];
}

- (IBAction)onBack:(id)sender {
    [self dismissModalViewControllerAnimated:YES];
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType {
    NSURL* targetUrl = [request URL];
    NSString *absoluteString = [targetUrl absoluteString];
    // We should not prevent loading original page.
    if ( NSNotFound != [absoluteString rangeOfString:@"iRemoteSuite.app"].location ) {
        return YES;
    }
    // For the rest we should change default behavior and launch Safari.
    [[UIApplication sharedApplication] openURL:targetUrl];
    return NO;
}

- (void)viewDidLoad {
	// Present help content.
	NSString *resource = [Controls pathForResource:@"Help" ofType:@"htm"];
    NSURL *resourceUrl = [NSURL fileURLWithPath:resource];
    NSURLRequest *resourceRequest = [NSURLRequest requestWithURL:resourceUrl];
    [contentView loadRequest:resourceRequest];
}

- (BOOL)shouldAutorotate {
    return NO;
}

@end
