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

#import "Painter2d.h"

#import "Texture.h"

@interface Painter2d (PrivateMethods) 
+ (void)before2dTexture;	
+ (void)after2dTexture;	
@end

@implementation Painter2d
@end

@implementation Painter2d (Frames)

+ (void)drawTexture:(Texture*)texture atPoint:(CGPoint)point {
    [Painter2d drawTexture:texture inRect:CGRectMake(point.x, point.y, texture.width, texture.height)];
}

+ (void)drawTexture:(Texture*)texture inRect:(CGRect)rect { 
	[Painter2d before2dTexture];
	
    GLfloat	coordinates[] = { 
        0.0f,	0.0f,
        1.0f,   0.0f, 
        0.0f,	1.0f,
        1.0f,	1.0f };
    
    GLfloat	vertices[] = {	
        rect.origin.x,						rect.origin.y,						
        rect.origin.x + rect.size.width,	rect.origin.y,						
        rect.origin.x,						rect.origin.y + rect.size.height,	
        rect.origin.x + rect.size.width,	rect.origin.y + rect.size.height	};
     
    glBindTexture(GL_TEXTURE_2D, texture.textureId);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, coordinates);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	
	
	[Painter2d after2dTexture];	        
}

+ (void)drawLineFromP1:(CGPoint)p1 toP2:(CGPoint)p2 withColorR:(GLubyte)r G:(GLubyte)g B:(GLubyte)b A:(GLubyte)a andWidth:(CGFloat)width { 
    
    const GLubyte colors[] = {
        r, g, b, a,
        r, g, b, a
    };    
    
	GLfloat	vertices[] = {	
        p1.x, p1.y,
        p2.x, p2.y
    };    
    
    glLineWidth( width );
    
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_LINES, 0, 2);       
    
    glDisableClientState(GL_COLOR_ARRAY);
}

+ (void)before2dTexture {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glColor4ub(255, 255, 255, 255);	 
}

+ (void)after2dTexture {
	glColor4ub(255, 255, 255, 255);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

@end
