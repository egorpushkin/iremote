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

#import "ActiveButton.h"
#import "Content.h"

@implementation ActiveButton

- (id)initWithCoder:(NSCoder*)coder {
    if ((self = [super initWithCoder:coder])) {
        state = FALSE;
        originalFrame = self.frame;
        originalFont = self.titleLabel.font;
    }
    return self;
}

- (void)updateButton {
    if ( state != self.highlighted ) {
        if ( self.highlighted ) {
            [UIView beginAnimations:@"activation" context:nil];
            [UIView setAnimationDuration:0.1f];
            self.frame = CGRectMake( 
                 self.frame.origin.x - self.frame.size.width / 2.0f,
                 self.frame.origin.y - self.frame.size.height * 1.5f, 
                 self.frame.size.width * 2.0f,
                 self.frame.size.height * 2.0f);
            [UIView commitAnimations];            
            self.titleLabel.font = [Content instance].largeButtonFont;
            
            [self.superview bringSubviewToFront:self];
        } else {
            self.frame = originalFrame;
            self.titleLabel.font = originalFont;
        }     
        state = self.highlighted;
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesBegan:touches withEvent:event];
    [self updateButton];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {  
    [super touchesMoved:touches withEvent:event];
    [self updateButton];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesEnded:touches withEvent:event];    
    [self updateButton];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    [super touchesCancelled:touches withEvent:event];    
    [self updateButton];
}

@end
