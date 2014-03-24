/**
 * Copyright (c) 2014 Egor Pushkin. All rights reserved.
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

#import "AdMobHelper.h"

// AdMob includes
#import "../../3rdParty/AdMob/GADBannerView.h"

// Config
#include "Common/Common.h"
#include "../State/UIConfig.h"

@implementation AdMobHelper

- (id)initWithController:(UIViewController*)controller andView:(UIView*)view {
    if ( self = [super init] ) {
        hostController = [controller retain];
        hostView = [view retain];
        [self initAdMob];
    }
    return self;
}

- (void)dealloc {
    [hostController release];
    [hostView release];
    [banner release];
 	[super dealloc];
}

#pragma mark AdMob tools

- (void)initAdMob {
    banner = [[GADBannerView alloc] initWithAdSize:kGADAdSizeBanner];
    banner.frame = CGRectMake(0.0f, 0.0f, 320.0f, 50.0f);
    banner.adUnitID = @"a14c46ae5d2297a";
    banner.rootViewController = hostController;
    [hostView addSubview:banner];
}

- (void)refreshAd {
    [banner loadRequest:[GADRequest request]];
}

- (void)startRefreshingAds {
    if ( timer ) {
        return;
    }
    
    // Schedule timer.
    timer = [[NSTimer alloc] initWithFireDate:
        [NSDate dateWithTimeIntervalSinceNow:(RemotePC::UIConfig::adRefreshInterval_)]
        interval:RemotePC::UIConfig::adRefreshInterval_
        target:self selector:@selector(refreshAd) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
    
    // Refresh right away.
    [self refreshAd];
}

- (void)stopRefreshingAds {
    if ( !timer ) {
        return;
    }
    
    [timer invalidate];
    [timer release];
    timer = nil;
}

@end
