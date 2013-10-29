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

#import <UIKit/UIKit.h>

@interface NetworkStatsController : UIViewController< UITableViewDelegate, UITableViewDataSource > {
	
	IBOutlet UITableViewCell *durationCell_;
	IBOutlet UITableViewCell *sentCell_;
	IBOutlet UITableViewCell *receivedCell_;
	IBOutlet UITableViewCell *trafficCell_;
	IBOutlet UITableViewCell *speedCell_;
	IBOutlet UITableViewCell *resetCell_;		
	
	IBOutlet UILabel *duration_;
	IBOutlet UILabel *sent_;
	IBOutlet UILabel *received_;
	IBOutlet UILabel *traffic_;
	IBOutlet UILabel *speed_;
	
	NSTimer *refreshTimer_;

}

- (id)initController;
- (IBAction)reset;

@end
