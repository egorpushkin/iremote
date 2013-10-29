//
//  AdMobDelegate.m
//  iRemoteSuite
//
//  Created by Egor Pushkin on 8/4/10.
//  Copyright 2010 TikGames. All rights reserved.
//

#import "AdMobDelegate.h"
#import "AdMobView.h"

@implementation AdMobDelegate

- (id)initWithViewController:(UIViewController *)controller {
    if ( self = [super init] ) {
        viewController = [controller retain];
    }
    return self;
}

- (void)dealloc {
    [super dealloc];
    [viewController release];    
}

#pragma mark AdMobDelegate methods

- (NSString *)publisherIdForAd:(AdMobView *)adView {
    return @"a14c46ae5d2297a"; // this should be prefilled; if not, get it from www.admob.com
}

- (UIViewController *)currentViewControllerForAd:(AdMobView *)adView {
    return viewController;
}

- (UIColor *)adBackgroundColorForAd:(AdMobView *)adView {
    return [UIColor colorWithRed:0 green:0 blue:0 alpha:1]; // this should be prefilled; if not, provide a UIColor
}

- (UIColor *)primaryTextColorForAd:(AdMobView *)adView {
    return [UIColor colorWithRed:1 green:1 blue:1 alpha:1]; // this should be prefilled; if not, provide a UIColor
}

- (UIColor *)secondaryTextColorForAd:(AdMobView *)adView {
    return [UIColor colorWithRed:0.8 green:0.8 blue:0.8 alpha:1]; // this should be prefilled; if not, provide a UIColor
}


// To receive test ads rather than real ads...
/*
 // Test ads are returned to these devices.  Device identifiers are the same used to register
 // as a development device with Apple.  To obtain a value open the Organizer 
 // (Window -> Organizer from Xcode), control-click or right-click on the device's name, and
 // choose "Copy Device Identifier".  Alternatively you can obtain it through code using
 // [UIDevice currentDevice].uniqueIdentifier.
 //
 // For example:
 //    - (NSArray *)testDevices {
 //      return [NSArray arrayWithObjects:
 //              ADMOB_SIMULATOR_ID,                             // Simulator
 //              //@"28ab37c3902621dd572509110745071f0101b124",  // Test iPhone 3GS 3.0.1
 //              //@"8cf09e81ef3ec5418c3450f7954e0e95db8ab200",  // Test iPod 2.2.1
 //              nil];
 //    }
 
 /* 
 - (NSArray *)testDevices {
 return [NSArray arrayWithObjects: ADMOB_SIMULATOR_ID, nil];
 }
 
 - (NSString *)testAdActionForAd:(AdMobView *)adMobView {
 return @"url"; // see AdMobDelegateProtocol.h for a listing of valid values here
 }
 */

// Sent when an ad request loaded an ad; this is a good opportunity to attach
// the ad view to the hierachy.
- (void)didReceiveAd:(AdMobView *)adView {
}

// Sent when an ad request failed to load an ad.
- (void)didFailToReceiveAd:(AdMobView *)adView {
}

// Sent when a AdMobView successfully makes a subsequent ad request (via requestFreshAd).
- (void)didReceiveRefreshedAd:(AdMobView *)adView {
}

// Sent when subsequent AdMobView ad requests fail (via requestFreshAd).
- (void)didFailToReceiveRefreshedAd:(AdMobView *)adView {
}

@end
