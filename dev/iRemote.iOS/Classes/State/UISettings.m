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

#import "UISettings.h"
#import "LocalStorage.h"

NSString *UISettingsAreaAbsolute = @"areaAbsolute";
NSString *UISettingsPreviewZoom = @"previewZoom";

@implementation UISettings

#pragma mark Initialization section

- (void)initSettings {
	// Control area.
	if ( ![self hasSetting:UISettingsAreaAbsolute] )
		[self setBool:YES name:UISettingsAreaAbsolute];
	// Preview panel.
	if ( ![self hasSetting:UISettingsPreviewZoom] )	
		[self setFloat:0.5 name:UISettingsPreviewZoom];
}

#pragma mark Modifying settings

- (void)setId:(id)value name:(NSString *)name {
	[settings_ setObject:value forKey:name];		
	[self notifyOnChanged];
}

- (void)setString:(NSString*)value name:(NSString *)name {
	[self setId:value name:name];		
}

- (void)setNumber:(NSNumber*)value name:(NSString *)name {
	[self setId:value name:name];		
}

- (void)setFloat:(float)value name:(NSString *)name {
	[self setId:[NSNumber numberWithFloat:value] name:name];		
}

- (void)setBool:(BOOL)value name:(NSString *)name {
	[self setId:[NSNumber numberWithBool:value] name:name];			
}

#pragma mark Accessing settings

- (NSString *)asString:(NSString*)name {
	return (NSString *)[settings_ objectForKey:name];	
}

- (NSNumber *)asNumber:(NSString*)name {
	return (NSNumber *)[settings_ objectForKey:name];
}

- (float)asFloat:(NSString*)name {
	return [((NSNumber *)[settings_ objectForKey:name]) floatValue];	
}

- (BOOL)asBool:(NSString*)name {
	return [((NSNumber *)[settings_ objectForKey:name]) boolValue];	
}

- (BOOL)hasSetting:(NSString*)name {
	return ( nil != [settings_ objectForKey:name] );
}

#pragma mark Serialization section

- (void)saveSettings {
	[LocalStorage save:settings_ toFile:@"uiSettings.dat"];
}

- (void)loadSettings {
	NSDictionary *settings = [LocalStorage loadDataFromFile:@"uiSettings.dat"];
	if ( settings ) {
		// Constract container from read data.
		settings_ = [[NSMutableDictionary alloc] initWithDictionary:settings];
	} else {
		// Allocate new container to store settings.
		settings_ = [[NSMutableDictionary alloc] initWithCapacity:0];
	}
	// Intended to register properties added in subsequent versions.
	[self initSettings];	
}

#pragma mark Listeners section

- (void)subscribe:(id<ISettingsListener>)listener {
	[listeners_ addObject:listener];
	[listener settingsChanged];
}
	
- (void)unsubscribe:(id<ISettingsListener>)listener {
	[listeners_ removeObject:listener];
}

- (void)notifyOnChanged {
	for ( id<ISettingsListener> listener in listeners_ ) {
		[listener settingsChanged];
	}	
}

#pragma mark Life cycle section

- (id)init {	
	if ( (self = [super init]) ) {				
		listeners_ = [[NSMutableArray alloc] initWithCapacity:0];
	} 	
	return self;	
}

- (void)dealloc {
	[listeners_ release];
	[settings_ release];
	[super dealloc];	
}

#pragma mark Singleton section

static UISettings *globalUISettings = nil;

+ (UISettings *)instance {
    @synchronized(self) {
        if ( globalUISettings == nil ) {
            [[self alloc] init]; // assignment not done here
        }
    }
    return globalUISettings;
}

+ (id)allocWithZone:(NSZone *)zone {
    @synchronized(self) {
        if ( globalUISettings == nil ) {
            globalUISettings = [super allocWithZone:zone];
            return globalUISettings;  // assignment and return on first allocation
        }
    }
    return nil; //on subsequent allocation attempts return nil
}

@end
