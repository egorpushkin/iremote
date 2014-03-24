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

#import "HostsViewController.h"
#import "../Controls/Controls.h"
#import "../../Tools/Resource.h"

// Hosts cells
#import "RegularHostCell.h"
#import "NoHostsCell.h"
#import "HostsHeaderCell.h"

// Login form
#import "../Main/iRemoteAppDelegate.h"
#import "../Main/iRemoteViewController.h"

@interface HostsViewController(PrivateMethods)

- (size_t)hostIndexForIndexPath:(NSIndexPath *)indexPath;
- (size_t)interfaceIndexForIndexPath:(NSIndexPath *)indexPath;

@end

// Hosts manager support.
#include "Common/Common.h"
#include "Connector/Hosts/Hosts.h"

@implementation HostsViewController

- (id)initWithStyle:(UITableViewStyle)style {
    if (self = [super initWithStyle:style]) {
        // Configure view controller
		self.title = NSLocalizedString(@"Hosts", @"Hosts");	
        self.view.backgroundColor = [UIColor colorWithWhite:20.0f/256.0f alpha:1.0f];
        self.tableView.separatorStyle = UITableViewCellSeparatorStyleNone;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
	// Update hosts table.
	[self updateHosts];    
    // Configure back control
	self.navigationItem.leftBarButtonItem = [Controls backButtonItem:self action:@selector(onBack:)];    
    self.navigationItem.rightBarButtonItem.tintColor = [UIColor whiteColor];
}

- (void)onBack:(id)sender {
    [self.navigationController popViewControllerAnimated:YES];    
}

- (void)updateHosts {
	// Update table header.
	if ( 0 != RemotePC::HostsManager::Instance().GetHostsCount() ) {
		// Display an Edit button in the navigation bar for this view controller.
		self.navigationItem.rightBarButtonItem = self.editButtonItem;
		// Display table header informing user on allowed actions on this page.
		self.tableView.tableHeaderView = [Resource objectFromNib:@"HostCells" owner:self index:2];		
	} else {
		// Display 'no hosts' message.
		self.tableView.tableHeaderView = [Resource objectFromNib:@"HostCells" owner:self index:1];		
	}
	// Update table data.	
	[self.tableView reloadData];	
}

#pragma mark Table view methods

// Customize the appearance of table view cells.
- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {    
	static NSString *cellIdentifier = @"hostCell";
	NSUInteger cellIndex = 0;
	
	// Construct cell.
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
	if (cell == nil) {
		cell = [Resource objectFromNib:@"HostCells" owner:self index:cellIndex];	
	}
	
	// Configure cell if necessary.
	RegularHostCell *hostCell = (RegularHostCell *)cell;
	RemotePC::Host& host = RemotePC::HostsManager::Instance().GetHost( [self hostIndexForIndexPath:indexPath] );
	const RemotePC::NetInterface& netInterface = host.GetInterface( [self interfaceIndexForIndexPath:indexPath] );
	[hostCell setHost:host andInterface:netInterface];		

    return cell;
}

// Override to support selecting row of the table view.
- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
	// Check whether there are any hosts.
	if ( 0 == RemotePC::HostsManager::Instance().GetHostsCount() )
		return;
	
    // Prepopulate host parameters.
    iRemoteViewController *loginController = [[iRemoteAppDelegate delegate] loginController];
    RemotePC::Host& host = RemotePC::HostsManager::Instance().GetHost( [self hostIndexForIndexPath:indexPath] );
	const RemotePC::NetInterface& netInterface = host.GetInterface( [self interfaceIndexForIndexPath:indexPath] );
    [loginController 
        showIp:[NSString stringWithUTF8String:netInterface.GetIp().c_str()] 
        andPort:[NSString stringWithUTF8String:netInterface.GetPort().c_str()] ];
    
    // Go back to login form.
	[self onBack:self];		
	// It is up to login form to decide whether to attempt to login automatically or no.	
}

// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
	size_t hostsCount = RemotePC::HostsManager::Instance().GetHostsCount();
	if ( hostsCount > 0 ) {
		return true;
	} else {
		return false;
	}
}

// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
	if ( editingStyle == UITableViewCellEditingStyleDelete ) {
		// Delete the row from object model.
		RemotePC::HostsManager::Instance().RemoveInterface(
			[self hostIndexForIndexPath:indexPath],
			[self interfaceIndexForIndexPath:indexPath]);		

		// Single row should be removed, if a number of rows in the section is
		// greater than one or, if table consists of the only section. 
		if ( ( [tableView numberOfRowsInSection:indexPath.section] > 1 ) || ( [tableView numberOfSections] <= 1 ) ) {		 
			// Delete the row from the table.
			[tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationLeft];		
		} else {			
			// Delete the entire section from the table.
			[tableView deleteSections:[NSIndexSet indexSetWithIndex:indexPath.section] withRowAnimation:UITableViewRowAnimationLeft];		
		}
		
		if ( 0 == RemotePC::HostsManager::Instance().GetHostsCount() ) {
			// Disable editing mode when there are no more hosts.
			self.editing = NO;
			// Hide Edit button and table header.
			self.navigationItem.rightBarButtonItem = nil;	
			// Display table header ("no hosts") instead of current one.
			self.tableView.tableHeaderView = [Resource objectFromNib:@"HostCells" owner:self index:1];			
		}
    }
}

#pragma mark Table view number of cells and cells' height

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    size_t hostsCount = RemotePC::HostsManager::Instance().GetHostsCount();
	if ( hostsCount > 0 ) {
		return hostsCount;
	} else {
		return 1;
	}	
}

// Customize the number of rows in the table view.
- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
	size_t hostsCount = RemotePC::HostsManager::Instance().GetHostsCount();
	if ( hostsCount > 0 ) {
		return RemotePC::HostsManager::Instance().GetHost(section).GetInterfacesCount();
	} else {
		return 0;
	}
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
	return [RegularHostCell height];
}

#pragma mark Private tools

- (size_t)hostIndexForIndexPath:(NSIndexPath *)indexPath {
	return indexPath.section;
}

- (size_t)interfaceIndexForIndexPath:(NSIndexPath *)indexPath {
	return indexPath.row;
}

- (void)dealloc {
    [super dealloc];
}

@end
