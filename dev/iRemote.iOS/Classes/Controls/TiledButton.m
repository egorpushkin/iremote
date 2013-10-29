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

#import "TiledButton.h"
#import "Content.h"

@implementation TiledButton

@synthesize vk;
@synthesize leftArrow;
@synthesize rightArrow;

- (id)init {
    if (self = [super init]) {
        // Initialize flags.
        leftArrow = NO;
        rightArrow = NO;
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    UIImage* left = nil;
    UIImage* center = nil;
    UIImage* right = nil;

    if ( !self.highlighted ) {
        if ( leftArrow )
            left = [[Content instance] imageNamed:@"BackArrowLeftNormal"];
        else
            left = [[Content instance] imageNamed:@"ButtonNormalLeft"];
        center = [[Content instance] imageNamed:@"ButtonNormalCenter"];
        if ( rightArrow )
            right = [[Content instance] imageNamed:@"BackArrowRightNormal"];
        else
            right = [[Content instance] imageNamed:@"ButtonNormalRight"];
    } else {
        if ( leftArrow )
            left = [[Content instance] imageNamed:@"BackArrowLeftPressed"];
        else
            left = [[Content instance] imageNamed:@"ButtonPressedLeft"];
        center = [[Content instance] imageNamed:@"ButtonPressedCenter"];
        if ( rightArrow )
            right = [[Content instance] imageNamed:@"BackArrowRightPressed"];        
        else
            right = [[Content instance] imageNamed:@"ButtonPressedRight"];
    }
    
    [left drawAtPoint:CGPointMake(0, 0)];
    [center drawInRect:CGRectMake(left.size.width, 0, rect.size.width - left.size.width - right.size.width, rect.size.height)];
    [right drawAtPoint:CGPointMake(rect.size.width - right.size.width, 0)];    
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesBegan:touches withEvent:event];
    [self setNeedsDisplay];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {  
    [super touchesMoved:touches withEvent:event];
    [self setNeedsDisplay];    
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesEnded:touches withEvent:event];    
    [self setNeedsDisplay];    
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesCancelled:touches withEvent:event];    
    [self setNeedsDisplay];    
}

@end
