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

#import <Foundation/Foundation.h>
#import "../../Tools/Singleton.h"
#import "ISettingsListener.h"

extern NSString *UISettingsAreaAbsolute;
extern NSString *UISettingsPreviewZoom;

@interface UISettings : Singleton {
	
	NSMutableArray *listeners_;	
	NSMutableDictionary *settings_;

}

#pragma mark Modifying settings

- (void)setString:(NSString*)value name:(NSString *)name;
- (void)setNumber:(NSNumber*)value name:(NSString *)name;
- (void)setFloat:(float)value name:(NSString *)name;
- (void)setBool:(BOOL)value name:(NSString *)name;

#pragma mark Accessing settings

- (NSString *)asString:(NSString*)name;
- (NSNumber *)asNumber:(NSString*)name;
- (float)asFloat:(NSString*)name;
- (BOOL)asBool:(NSString*)name;
- (BOOL)hasSetting:(NSString*)name;

#pragma mark Listeners section

- (void)subscribe:(id<ISettingsListener>)listener;
- (void)unsubscribe:(id<ISettingsListener>)listener;
- (void)notifyOnChanged;

#pragma mark Serialization section

- (void)saveSettings;
- (void)loadSettings;

#pragma mark Singleton section

+ (UISettings *)instance;

@end
