/****************************************************************************
 Author: Luma (stubma@gmail.com)
 
 https://github.com/stubma/cocos2dx-common
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "CCDrawingPrimitivesEx.h"

NS_CC_BEGIN

static bool s_bInitialized = false;
static CCGLProgram* s_pShader = NULL;
static int s_nColorLocation = -1;
static ccColor4F s_tColor = {1.0f,1.0f,1.0f,1.0f};

static void lazy_init( void )
{
    if( ! s_bInitialized ) {
		
        //
        // Position and 1 color passed as a uniform (to simulate glColor4ub )
        //
        s_pShader = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_Position_uColor);
		
        s_nColorLocation = glGetUniformLocation( s_pShader->getProgram(), "u_color");
		CHECK_GL_ERROR_DEBUG();
		
        s_bInitialized = true;
    }
}

void ccDrawColor4BEx( GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
    s_tColor.r = r/255.0f;
    s_tColor.g = g/255.0f;
    s_tColor.b = b/255.0f;
    s_tColor.a = a/255.0f;
}

void ccDrawSolidCircle( const CCPoint& center, float radius, float angle, unsigned int segments, bool drawLineToCenter, float scaleX, float scaleY)
{
    lazy_init();
	
    int additionalSegment = 1;
    if (drawLineToCenter)
        additionalSegment++;
	
    const float coef = 2.0f * (float)M_PI/segments;
	
    GLfloat *vertices = (GLfloat*)calloc( sizeof(GLfloat)*2*(segments+2), 1);
    if( ! vertices )
        return;
	
    for(unsigned int i = 0;i <= segments; i++) {
        float rads = i*coef;
        GLfloat j = radius * cosf(rads + angle) * scaleX + center.x;
        GLfloat k = radius * sinf(rads + angle) * scaleY + center.y;
		
        vertices[i*2] = j;
        vertices[i*2+1] = k;
    }
    vertices[(segments+1)*2] = center.x;
    vertices[(segments+1)*2+1] = center.y;
	
    s_pShader->use();
    s_pShader->setUniformsForBuiltins();
    s_pShader->setUniformLocationWith4fv(s_nColorLocation, (GLfloat*) &s_tColor.r, 1);
	
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );
	
    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei) segments+additionalSegment);
	
    free( vertices );
	
    CC_INCREMENT_GL_DRAWS(1);
}

NS_CC_END