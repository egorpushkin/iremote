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

#import "LocalStorage.h"

// Persistent UI config.
#include "Common/Common.h"
#include "Versions/Versions.h"

@implementation LocalStorage

static LocalStorage *globalStorage = nil;

@synthesize ip;
@synthesize port;
@synthesize acceptedEulaVersion;

#pragma mark Serialization tools

- (void)saveData_v2 {	
    // Construct data structure.
    NSMutableDictionary *appData = [[NSMutableDictionary alloc] init];
    [appData setObject:ip forKey:@"ip"];
    [appData setObject:port forKey:@"port"];
    [appData setObject:[NSNumber numberWithUnsignedInteger:acceptedEulaVersion] forKey:@"eula"];    
    
    // Save data to file.
	[LocalStorage save:appData toFile:@"appState2.dat"];	
    
    [appData release];
}

/**
 * This data format is supported since iRemote 1.2.2.        
 */
- (void)loadData_v2 {	
	NSDictionary *appData = [LocalStorage loadDataFromFile:@"appState2.dat"];
	if ( appData ) {
		self.ip = [appData objectForKey:@"ip"];
		self.port = [appData objectForKey:@"port"];        
        NSNumber *eula = (NSNumber *)[appData objectForKey:@"eula"];
        self.acceptedEulaVersion = [eula unsignedIntegerValue];
	} 
}

/**
 * This data format was supported prior to iRemote 1.2.1.
 */ 
- (void)loadData_v1 {	
	NSArray *appData = [LocalStorage loadDataFromFile:@"appState.dat"];
	if ( appData ) {
		self.ip = [appData objectAtIndex:0];
		self.port = [appData objectAtIndex:1];        
		BOOL eulaAccepted = [[appData objectAtIndex:2] boolValue];        
        self.acceptedEulaVersion = ( eulaAccepted ) ? ( 1 ) : ( 0 );
	} 
}

#pragma mark Public tools

- (void)loadData {	
    if ( [LocalStorage isFilenameExists:@"appState.dat"] ) {
        // Load data from old format.
        [self loadData_v1];
        // Remove old config.
        [LocalStorage removeFile:@"appState.dat"];
    } else {
        // Load data from current format.
        [self loadData_v2];
    }
}

- (void)saveData { 	
    [self saveData_v2];
}

- (void)acceptEula {
    self.acceptedEulaVersion = RemotePC::iREMOTE_LICENSE;
    [self saveData];
}

- (BOOL)eulaAccepted {
    return ( self.acceptedEulaVersion >= RemotePC::iREMOTE_LICENSE );
}

#pragma mark Serialization helpers

+ (BOOL)save:(id)data toFile:(NSString *)filename {
	NSString *filepath = [LocalStorage pathForName:filename];
	return [NSKeyedArchiver archiveRootObject:data toFile:filepath];
}

+ (id)loadDataFromFile:(NSString *)filename {	
	NSString *filepath = [LocalStorage pathForName:filename];
    if ( ![self isFilepathExists:filepath] )
        return nil;
	return [NSKeyedUnarchiver unarchiveObjectWithFile:filepath];
}

+ (BOOL)isFilepathExists:(NSString *)filepath {
	NSFileManager *filemanager = [NSFileManager defaultManager];	
	return [filemanager fileExistsAtPath:filepath];
}

+ (BOOL)isFilenameExists:(NSString *)filename {
	NSString *filepath = [LocalStorage pathForName:filename];
    return [LocalStorage isFilepathExists:filepath];
}

+ (BOOL)removeFile:(NSString *)filename {
    NSString *filepath = [LocalStorage pathForName:filename];
	NSFileManager *filemanager = [NSFileManager defaultManager];	
	return [filemanager removeItemAtPath:filepath error:NULL];
}

+ (NSString *)pathForName:(NSString *)filename {
    return [NSString stringWithFormat:@"%@/Documents/%@", NSHomeDirectory(), filename];
}

#pragma mark Singleton section

+ (LocalStorage *)instance {
    @synchronized(self) {
        if ( globalStorage == nil ) {
            [[self alloc] init]; // assignment not done here
        }
    }
    return globalStorage;
}

+ (id)allocWithZone:(NSZone *)zone {
    @synchronized(self) {
        if ( globalStorage == nil ) {
            globalStorage = [super allocWithZone:zone];
            return globalStorage;  // assignment and return on first allocation
        }
    }
    return nil; //on subsequent allocation attempts return nil
}

- (id)init {	
	if ( (self = [super init]) ) {		
        self.acceptedEulaVersion = 0;
		self.ip = @"192.168.1.1";		
		self.port = @"8444";		
	} 	
	return self;	
}

- (void)dealloc {
	[ip release];
	[port release];
	[super dealloc];	
}

@end
