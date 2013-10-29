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

#import "RegularHostCell.h"

#import "../Controls/Content.h"

// Persistent UI config.
#include "Common/Common.h"
#include "../State/UIConfig.h"

// Hosts manager support.
#include "Connector/Hosts/Hosts.h"

@implementation RegularHostCell

- (id)initWithCoder:(NSCoder*)coder {    
    if ((self = [super initWithCoder:coder])) {
        // Configure the cell.
        // self.selectionStyle = UITableViewCellSelectionStyleGray;
    }
    return self;
}

- (void)setHost:(const RemotePC::Host&)host andInterface:(const RemotePC::NetInterface&)netInterface {
	ipAddress.text = [NSString stringWithFormat:@"%s : %s", netInterface.GetIp().c_str(), netInterface.GetPort().c_str()];
	if ( host.HasPassword() ) {
        authImage.image = [[Content instance] imageNamed:@"AuthLocked"];
		authMessage.text = @"Authorized";
	} else {
        authImage.image = [[Content instance] imageNamed:@"AuthUnlocked"];
		authMessage.text = @"Not authorized";		
	}	
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
	if ( selected )
		self.contentView.backgroundColor = [UIColor colorWithWhite:90.0f/256.0f alpha:1.0f];
	else
		self.contentView.backgroundColor = [UIColor colorWithWhite:20.0f/256.0f alpha:1.0f];
}

- (void)setEditing:(BOOL)editing animated:(BOOL)animated {	
	[super setEditing:editing animated:animated];
	[UIView beginAnimations:@"editing" context:nil];
	[UIView setAnimationDuration:0.2f];
	if ( editing )
		underline.frame = CGRectMake(20.0f, RemotePC::UIConfig::hostCellHeight_ - 2.0f, 248.0f, 1.0f);
	else
		underline.frame = CGRectMake(20.0f, RemotePC::UIConfig::hostCellHeight_ - 2.0f, 280.0f, 1.0f);
	[UIView commitAnimations];		
}

+ (CGFloat)height {
	return RemotePC::UIConfig::hostCellHeight_;
}

- (void)dealloc {
    [super dealloc];
}

@end
