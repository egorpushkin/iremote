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

#import "NetworkStatsController.h"

#include "Connector/Server.h"

@implementation NetworkStatsController

#pragma mark Formatters

- (NSString *)formatDuration:(long)seconds {
	long minutes = seconds / 60;
	if ( seconds < 60 )
		return [NSString stringWithFormat:@"%ld sec", seconds];
	else if ( minutes < 60 ) {
		long secondsLastMinute = seconds % 60;		
		return [NSString stringWithFormat:@"%ld:%ld m:s", minutes, secondsLastMinute];
	} else {
		long hours = minutes / 60;		
		long minutesLastHour = minutes % 60;		
		return [NSString stringWithFormat:@"%ld:%ld h:m", hours, minutesLastHour];
	}
}

- (NSString *)formatSize:(long)bytes {
	long kbytes = bytes / 1024;
	if ( bytes < 1024 )
		return [NSString stringWithFormat:@"%ld B", bytes];
	else if ( kbytes < 1024 ) {		
		return [NSString stringWithFormat:@"%.2f kB", bytes / 1024.0f];
	} else {
		return [NSString stringWithFormat:@"%.2f mB", kbytes / 1024.0f];		
	}
}

- (NSString *)formatSpeed:(float)kbytesPerSec {
	return [NSString stringWithFormat:@"%.2f kB/sec", kbytesPerSec];
}

#pragma mark Events handlers

- (void)update:(id)sender {
	mc::INetworkStatsPtr stats = RemotePC::Holder::Instance().GetService()->GetStats();
	duration_.text = [self formatDuration:stats->GetSessionDuration() / 1000];
	sent_.text = [self formatSize:stats->GetTotalSent()];
	received_.text = [self formatSize:stats->GetTotalReceived()];
	traffic_.text = [self formatSize:stats->GetTotalTraffic()];
	speed_.text = [self formatSpeed:stats->GetSpeed() / 1024.0f];
}

- (IBAction)reset {
	RemotePC::Holder::Instance().GetService()->GetStats()->Reset();
	[self update:nil]; 	
}

#pragma mark View controller section

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    refreshTimer_ = [[NSTimer scheduledTimerWithTimeInterval:1.0f
		target:self selector:@selector(update:)
		userInfo:nil repeats:YES] retain];
	[self update:nil];
}

- (void)viewWillDisappear:(BOOL)animated {
	[super viewWillDisappear:animated];
	[refreshTimer_ invalidate];
	[refreshTimer_ release];
	refreshTimer_ = nil;
}

#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    // Return the number of sections.
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    // Return the number of rows in the section.
	if ( 0 == section )
		return 5;
	else
		return 1;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    // Configure the cell.
	UITableViewCell *cell = nil;
	if ( 0 == indexPath.section ) {
		if ( 0 == indexPath.row ) {
			cell = durationCell_;
		} else if ( 1 == indexPath.row ) { 
			cell = sentCell_;
		} else if ( 2 == indexPath.row ) { 
			cell = receivedCell_;
		} else if ( 3 == indexPath.row ) { 
			cell = trafficCell_;
		} else if ( 4 == indexPath.row ) { 
			cell = speedCell_;
		}
	} else {
		if ( 0 == indexPath.row ) {
			cell = resetCell_;
		} 
	}
	cell.selectionStyle = UITableViewCellSelectionStyleNone;
    return cell;
}

- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section {
	if ( 1 == section ) {
		return @"You may reduce traffic by switching to gray-scaled mode, reducing image quality and/or FPS in Remote PC Suite settings.";
	} else {
		return nil;
	}
}

#pragma mark Life cycle section

- (id)initController {
	return [self initWithNibName:@"NetworkStats" bundle:[NSBundle mainBundle]];
}

- (id)initWithNibName:(NSString *)nibName bundle:(NSBundle *)nibBundle {
	if (self = [super initWithNibName:nibName bundle:nibBundle]) {
		self.contentSizeForViewInPopover = CGSizeMake(320.0f, 375.0f);
    }
    return self;	
}

- (void)dealloc {
    [super dealloc];
}

@end
