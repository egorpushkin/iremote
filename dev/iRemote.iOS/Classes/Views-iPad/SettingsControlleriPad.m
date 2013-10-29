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

#import "SettingsControlleriPad.h"
#import "../State/UISettings.h"

@implementation SettingsControlleriPad

#pragma mark Changes handling section

- (void)applySettings {
	controlAreaAbsoluteCursorSwitch_.on = [[UISettings instance] asBool:@"areaAbsolute"];
	previewZoomSlider_.value = [[UISettings instance] asFloat:@"previewZoom"];
}

- (IBAction)controlAreaAbsoluteCursorChanged:(id)sender {
	[[UISettings instance] setBool:controlAreaAbsoluteCursorSwitch_.on name:@"areaAbsolute"];	
}

- (IBAction)previewPanelZoomChanged:(id)sender {
	[[UISettings instance] setFloat:previewZoomSlider_.value name:@"previewZoom"];
}

#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    // Return the number of sections.
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	switch ( section ) {
	// Control area.
	case 0:
		return 2;
	// Preview panel.		
	case 1:
		return 1;
	default:
		return 1;
	}	
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    // Configure the cell.
	UITableViewCell *cell = nil;
	switch ( indexPath.section ) {
	// Control area.
	case 0:
		switch ( indexPath.row ) {
		case 0:
			cell = controlAreaMagicCursor_;
			break;
		case 1:
			cell = controlAreaAbsoluteCursor_;
			break;				
		}
		break;
	// Preview panel.		
	case 1:
		switch ( indexPath.row ) {
		case 0:
			cell = previewZoom_;
			break;
		}	
		break;
	}
    return cell;
}
	
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section {
	switch ( section ) {
	// Control area.
	case 0:
		return @"Touch Pad";
	case 1:
		return @"Preview panel";
	default:
		return nil;
	}
}	
	
#pragma mark Table view delegate

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
	UITableViewCell *cell = [self tableView:tableView cellForRowAtIndexPath:indexPath];
	return cell.frame.size.height;
}

#pragma mark View controller life cycle

- (void)viewDidLoad {
	[super viewDidLoad];
	[self applySettings];
}

#pragma mark Life cycle section
	
- (id)initController {
	return [self initWithNibName:@"Settings-iPad" bundle:[NSBundle mainBundle]];
}

- (id)initWithNibName:(NSString *)nibName bundle:(NSBundle *)nibBundle {
	if (self = [super initWithNibName:nibName bundle:nibBundle]) {
		self.contentSizeForViewInPopover = CGSizeMake(320.0f, 460.0f);
	}
	return self;	
}

- (void)dealloc {
	[super dealloc];
}
	
@end
