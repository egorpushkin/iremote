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

#import "ShellView.h"

@implementation ShellView

const CGFloat topOffset = 10.0f;
const CGFloat bottomOffset = 10.0f;
const CGFloat leftOffset = 10.0f;
const CGFloat rightOffset = 10.0f;
const CGFloat intervalOffset = 0.0f;

const NSInteger maxShellLines = 128;

#pragma mark Messages layouting.

- (void)addMessageLabel:(NSString *)message inRect:(CGRect)rect {
	UILabel *messageLabel = [[UILabel alloc] initWithFrame:rect];
	messageLabel.font = messageFont;
	messageLabel.text = message;
	messageLabel.textAlignment = UITextAlignmentLeft;
	messageLabel.textColor = [UIColor greenColor];
	messageLabel.backgroundColor = [UIColor clearColor];
    // Add label to content view.
	[contentView addSubview:messageLabel];
	[messageLabel release];	
}

- (void)removeMessageLabels {
	// Mark all messages as not shown.
	messagesShown = 0;
	// Remove all message labels.
	for ( UIView *view in contentView.subviews ) 
		[view removeFromSuperview];
	// Specify initial content size.
	self.contentSize = CGSizeMake(0.0f, topOffset);  
}

- (void)removeObsoleteMessages:(NSUInteger)obsoleteCount {
/*
    New solution, which is based on updating label text for existing labels.
    In practice, it bacame even slower.
*/
/*    
    for ( NSUInteger i = 0 ; i < messagesShown ; ++i ) {
        UILabel *messageLabel = [contentView.subviews objectAtIndex:i];
        NSString *message = [messages objectAtIndex:i];
        
        messageLabel.text = message;
        CGSize messageSize = [message sizeWithFont:messageFont];
        messageLabel.frame = CGRectMake(
            messageLabel.frame.origin.x, 
            messageLabel.frame.origin.y, 
            messageSize.width, 
            messageSize.height);   
    }
*/    
    
/*    
    Old solution, which is based on shifting labels inside content view. It is 
    very soow in practice.
*/
	// Remove all messages from superview, if it is all obsolete.
	if ( obsoleteCount > [contentView.subviews count] ) {
		[self removeMessageLabels];
		return;
	}
	
	// Remove obsolete messages from content view.
	CGFloat contentHeightDiff = obsoleteCount * intervalOffset;
	for ( NSUInteger i = 0 ; i < obsoleteCount ; ++i ) {
        // The first(!) label should be extracted on each iteration (not i-th).
        UIView *messageLabel = [contentView.subviews objectAtIndex:0];
		contentHeightDiff += messageLabel.frame.size.height;
        // Remove label from content view.
		[messageLabel removeFromSuperview];
	}				
	// Shift current messages. 
	CGFloat contentWidth = 0.0f;
    NSUInteger labelsCount = [contentView.subviews count];
	// for ( UIView *view in contentView.subviews ) {
    for ( NSUInteger i = 0 ; i < labelsCount ; ++i ) {
        UIView *view = [contentView.subviews objectAtIndex:i];
		view.frame = CGRectMake(
			view.frame.origin.x,
			view.frame.origin.y - contentHeightDiff,
			view.frame.size.width,
			view.frame.size.height);
        if ( view.frame.size.width > contentWidth )
            contentWidth = view.frame.size.width;
	}
	// Update content size.
	self.contentSize = CGSizeMake(contentWidth, self.contentSize.height - contentHeightDiff);
	// Update show messages count.
	messagesShown -= obsoleteCount;		
	// Note. Content view frame should be updated in layoutMessages, which is 
	// always invoked after this method.
}

- (void)updateContent:(CGSize)size {
	// Position of contentView is changed, if content size is less than screen
    // size. This workaround is applied to show not numerous message at the
    // bottom of the screen.
    if ( size.height < self.frame.size.height ) {
        // Fit the the bottom of the screen if it is not enough messages to 
        // cover it.
        contentView.frame = CGRectMake(
			0.0, 
			self.frame.size.height - size.height, 
			self.frame.size.width, 
			size.height);
    } else {
        // Fit the entire screen if there are enough messages to cover it.
        contentView.frame = CGRectMake(
			0.0, 0.0, self.frame.size.width, self.frame.size.height);
    }    
	
	// Update size of the content.
    self.contentSize = size;        
    // Update content offset.
	if ( self.contentSize.height - contentView.frame.size.height > 0.0f )
		self.contentOffset = CGPointMake(0.0f, self.contentSize.height - contentView.frame.size.height);
}
 
- (void)layoutMessages {
	// Check whether messages cache is empty.
	if ( 0 == [messages count] ) {
		// Remove all message labels.
		[self removeMessageLabels];
	}
	
	// Check whether there are any new messages.
	if ( messagesShown >= [messages count] ) {
		// TODO: (low priority) It is possible that some messages are removed.
		// This should be figgured out here.
		return;
	}
    
	// Initial content size.
	CGSize size = { self.contentSize.width, self.contentSize.height - bottomOffset };
    // Layout new messages.
	const NSUInteger messagesCount = [messages count];
	for ( NSUInteger i = messagesShown ; i < messagesCount ; ++i ) {
		// Acquire next message.
		NSString *message = [messages objectAtIndex:i];
		// Calculate message frame.
        CGSize messageSize = [message sizeWithFont:messageFont];
        CGRect messageFrame = CGRectMake(
            leftOffset, size.height, messageSize.width, messageSize.height);

        // Construct message label and add it to content view. 
		[self addMessageLabel:message inRect:messageFrame];
        
        // Update content size.
        CGFloat messageContentWidth = leftOffset + messageFrame.size.width + rightOffset;
        if ( messageContentWidth > size.width )
            size.width = messageContentWidth;
        size.height += ( intervalOffset + messageFrame.size.height );
    }
    size.height += bottomOffset;

	// Update content location, size and offset.
	[self updateContent:size];
	
	// Mark all messages as shown.
	messagesShown = messagesCount;
}

#pragma mark Content manipulation.

- (void)add:(NSString*)message { 
    NSArray *submessages = [message componentsSeparatedByString:@"\n"];
    for ( NSString *submessage in submessages ) {
        if ( 0 == [submessage length] )
            continue;
        submessage = [submessage stringByReplacingOccurrencesOfString:@"\t" withString:@"    "];
        [messages addObject:submessage];
    }
	// Limit messages size to maximumShellLines.
	if ( -1 != maxShellLines ) {
		if ( [messages count] > maxShellLines ) {
			NSUInteger obsoleteMessagesCount = [messages count] - maxShellLines;
			NSRange obsoleteMessagesRange = { 0, obsoleteMessagesCount };
			// Remove obsolete messages from cache.
			[messages removeObjectsInRange:obsoleteMessagesRange];
			// Remove obsolete message labels.
			[self removeObsoleteMessages:obsoleteMessagesCount];
		}
	}
    // Layout new messages.
    [self layoutMessages];
}

- (void)clear {
    // Remove all messages from cache.
    [messages removeAllObjects];
    // Update screen.
    [self layoutMessages];
}

#pragma mark View section

- (void)setFrame:(CGRect)newFrame {
    [super setFrame:newFrame];
	// Update content location and offset.
	[self updateContent:self.contentSize];
}

#pragma mark Life cycle section

- (id)initWithCoder:(NSCoder*)coder {
    if ((self = [super initWithCoder:coder])) {
        // Configure view.
        self.contentSize = CGSizeMake(1.0f, topOffset * 2);
        self.backgroundColor = [UIColor clearColor];
        self.clipsToBounds = YES;
        self.delegate = self;
        // Create messages view.
		contentView = [[UIView alloc] initWithFrame:CGRectMake(0.0f, 0.0f, 1.0f, topOffset * 2)];
        [self addSubview:contentView];
		messagesShown = 0;
        // Create messages container.
        messages = [[NSMutableArray alloc] init];
        // Crate font for displaying messages.
        messageFont = [[UIFont fontWithName:@"Courier" size:14.0f] retain];        
    }
    return self;
}

- (void)dealloc {
    [messageFont release];
    [messages release];
    [contentView release];
    [super dealloc];
}

@end
