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

#import "IndicatorsProvider.h"

@implementation IndicatorsProvider

- (void)subscribe:(id<IIndicatorsView>)indicator {
    [indicators addObject:indicator];
}

- (void)unsubscribe:(id<IIndicatorsView>)indicator {
    [indicators removeObject:indicator];
}

- (void)volumeChanged:(unsigned long)value {
    for ( id<IIndicatorsView> indicator in indicators )
        [indicator volumeChanged:value]; 
}

- (id)init {	
	if ( self = [super init] ) {	
        indicators = [[NSMutableArray alloc] init];
    }
    return self;
}

- (void)dealloc {
    [indicators release];
    [super dealloc];
}

// Singleton section
static IndicatorsProvider *globalIndicatorsProvider = nil;

+ (IndicatorsProvider *)instance {
    @synchronized(self) {
        if ( globalIndicatorsProvider == nil ) {
            [[self alloc] init]; // assignment not done here
        }
    }
    return globalIndicatorsProvider;
}

+ (id)allocWithZone:(NSZone *)zone {
    @synchronized(self) {
        if ( globalIndicatorsProvider == nil ) {
            globalIndicatorsProvider = [super allocWithZone:zone];
            return globalIndicatorsProvider;  // assignment and return on first allocation
        }
    }
    return nil; //on subsequent allocation attempts return nil
}

@end
