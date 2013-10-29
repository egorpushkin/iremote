//
//  AdMobDelegate.h
//  iRemoteSuite
//
//  Created by Egor Pushkin on 8/4/10.
//  Copyright 2010 TikGames. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "AdMobDelegateProtocol.h"
@class AdMobView;

@interface AdMobDelegate : NSObject< AdMobDelegate > {

    UIViewController *viewController;
    
}

- (id)initWithViewController:(UIViewController *)controller;

@end
