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

#import "FBModePickerController.h"
#import "FBControlArea.h"

@implementation FBModePickerController

#pragma mark Mode update helpers

- (void)updateModeFeatures {
	// Apply feature currents. 
	panning_.on = [controlArea_ panningAllowed];
	zooming_.on = [controlArea_ zoomingAllowed];		
	// Update features visibility.
	FBControlAreaMode mode = [controlArea_ mode];
	if ( FBControlAreaModeScaleToFit == mode ) {
		panning_.enabled = YES;
		zooming_.enabled = NO;
	} else if ( FBControlAreaModeScaleToFill == mode ) {
		panning_.enabled = YES;
		zooming_.enabled = NO;		
	} else if ( FBControlAreaModeFreeform == mode )	{
		panning_.enabled = YES;
		zooming_.enabled = YES;		
	}
}

- (IBAction)panningChanged {
	[controlArea_ setPanningAllowed:panning_.on];	
}

- (IBAction)zoomingChanged {
	[controlArea_ setZoomingAllowed:zooming_.on];		
}

#pragma mark View lifecycle

- (void)viewWillAppear:(BOOL)animated {
	[super viewWillAppear:animated];
	
	// Apply mode.
	FBControlAreaMode mode = [controlArea_ mode];
	NSUInteger row = 0;
	if ( FBControlAreaModeScaleToFit == mode )
		row = 0;
	else if ( FBControlAreaModeScaleToFill == mode )
		row = 1;
	else if ( FBControlAreaModeFreeform == mode )	
		row = 2;			
	[tableView_ reloadData];
	[tableView_ cellForRowAtIndexPath:[NSIndexPath indexPathForRow:row inSection:0]].accessoryType = UITableViewCellAccessoryCheckmark;
	// Apply mode features.
	[self updateModeFeatures];
}

#pragma mark Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    // Return the number of sections.
    return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    // Return the number of rows in the section.
	if ( 0 == section )
		return 3;
	else
		return 2;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    // Configure the cell.
	UITableViewCell *cell = nil;
	if ( 0 == indexPath.section ) {
		static NSString *cellIdentifier = @"modeCell";
		cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
		if (cell == nil) {
			cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier] autorelease];
		}		
		if ( 0 == indexPath.row ) {
			cell.textLabel.text = @"Scale to Fit";
		} else if ( 1 == indexPath.row ) {
			cell.textLabel.text = @"Scale to Fill";
		} else if ( 2 == indexPath.row ) {
			cell.textLabel.text = @"Freeform";
		}    
	} else {
		if ( 0 == indexPath.row ) {
			cell = panningCell_;
		} else if ( 1 == indexPath.row ) {
			cell = zoomingCell_;
		}
	}
	cell.selectionStyle = UITableViewCellSelectionStyleNone;
	cell.accessoryType = UITableViewCellAccessoryNone;
    return cell;
}

#pragma mark Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	if ( 0 == indexPath.section ) {
		if ( 0 == indexPath.row ) {
			[controlArea_ setMode:FBControlAreaModeScaleToFit];
		} else if ( 1 == indexPath.row ) {
			[controlArea_ setMode:FBControlAreaModeScaleToFill];
		} else if ( 2 == indexPath.row ) {
			[controlArea_ setMode:FBControlAreaModeFreeform];
		}	
		[tableView_ reloadData];		
		[tableView_ cellForRowAtIndexPath:indexPath].accessoryType = UITableViewCellAccessoryCheckmark;
		[self updateModeFeatures];
	}
}

#pragma mark Life cycle section

- (id)initWithControlArea:(FBControlArea *)controlArea {
	if (self = [super initWithNibName:@"FBModePicker" bundle:[NSBundle mainBundle]]) {
		self.contentSizeForViewInPopover = CGSizeMake(320.0f, 265.0f);
		controlArea_ = controlArea;
    }
    return self;	
}

- (void)dealloc {
    [super dealloc];
}

@end
