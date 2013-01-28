/*
 * FORZE ENGINE: http://forzefield.com
 *
 * Copyright (c) 2011-2012 FORZEFIELD Studios S.L.
 * Copyright (c) 2012 Manuel Martínez-Almeida
 * Copyright (c) 2008-2010 Ricardo Quesada
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

/**
 @author Manuel Martínez-Almeida
 */

#include "FZMacros.h"
#include "FZPrimitives.h"
#include "FZGLState.h"
#include "FZShaderCache.h"
#include "FZDirector.h"
#include "FZMath.h"
#include "FZMS.h"

namespace FORZE {

    static fzColor4F _fzColor(fzWHITE);
#if FZ_GL_SHADERS
    static GLProgram* _fzShader = NULL;
    
    static inline void lazyInitialized()
    {
        if(!_fzShader ) {
            _fzShader = ShaderCache::Instance().getProgramByKey(kFZShader_mat_uC4);
            _fzShader->retain();
            fzGLColor(fzWHITE);
            fzGLLineWidth(1);
        }
    }
    
    static inline void prepare()
    {
        lazyInitialized();
        _fzShader->setUniform4x4f(kFZUniformMVMatrix_s, 1, GL_FALSE, MS::getMatrix());
    }
    
#endif
    
    
    void fzGLColor(const fzColor4F& color)
    {
        if(_fzColor != color) {
            _fzColor = color;
#if FZ_GL_SHADERS
            lazyInitialized();
            _fzShader->setUniform4f(kFZUniformColor_s, color.r, color.g, color.b, color.a);
#else
            glColor4f(color.r, color.g, color.b, color.a);
#endif
        }
    }
    
    
    void fzGLLineWidth(fzFloat points)
    {
        glLineWidth(points * Director::Instance().getContentScaleFactor());
    }
    
    
#pragma mark - Drawing algorythms
    
    void fzDrawPoint( const fzVec2& vertices )
    {	
        fzGLSetMode(kFZGLMode_Primitives);    

#if FZ_GL_SHADERS
        prepare();
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, &vertices);
#else
        glVertexPointer(2, GL_FLOAT, 0, &vertices);
#endif
        glDrawArrays(GL_POINTS, 0, 1);	
    }
    
    
    void fzDrawPoints( const fzVec2 *vertices, fzUInt numberOfPoints )
    {
        FZ_ASSERT( numberOfPoints > 0, "Number of points can not be 0.");

        fzGLSetMode(kFZGLMode_Primitives);
        
#if FZ_GL_SHADERS
        prepare();
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#else
        glVertexPointer(2, GL_FLOAT, 0, vertices);
#endif
        glDrawArrays(GL_POINTS, 0, (GLsizei)numberOfPoints);
    }
    
    
    void fzDrawLine(const fzVec2& origin, const fzVec2& destination )
    {
        fzGLSetMode(kFZGLMode_Primitives);
        fzVec2 vertices[2] = { origin, destination };

#if FZ_GL_SHADERS
        prepare();
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#else
        glVertexPointer(2, GL_FLOAT, 0, vertices);
#endif
        glDrawArrays(GL_LINES, 0, 2);	
    }
    
    
    void fzDrawLines( const fzVec2 *vertices, fzUInt numOfVertices)    
    {    
        FZ_ASSERT( numOfVertices > 0, "Number of vertices can not be 0.");

        fzGLSetMode(kFZGLMode_Primitives);            
#if FZ_GL_SHADERS
        prepare();
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#else
        glVertexPointer(2, GL_FLOAT, 0, vertices);
#endif
        GLsizei nu = static_cast<GLsizei>(numOfVertices);
        glDrawArrays(GL_LINES, 0, nu);
    }
    
    
    void fzDrawPoly( const fzVec2 *vertices, fzUInt numOfVertices, bool closePolygon )
    {    
        FZ_ASSERT( numOfVertices > 0, "Number of vertices can not be 0.");

        fzGLSetMode(kFZGLMode_Primitives);            
#if FZ_GL_SHADERS
        prepare();
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#else
        glVertexPointer(2, GL_FLOAT, 0, vertices);
#endif

        if( closePolygon )
            glDrawArrays(GL_LINE_LOOP, 0, (GLsizei) numOfVertices);
        else
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) numOfVertices);
    }
    
    void fzDrawRect( const fzRect& rect )
    {
        fzVec2 vertices[4] = {
            rect.origin,
            fzVec2(rect.origin.x + rect.size.width, rect.origin.y),
            fzVec2(rect.origin + rect.size),
            fzVec2(rect.origin.x, rect.origin.y + rect.size.height)
        };
        fzDrawPoly(vertices, 4, true);
    }
    
    
    void fzDrawShape( const fzVec2 *vertices, fzUInt numOfVertices)
    {    
        FZ_ASSERT( numOfVertices > 0, "Number of vertices can not be 0.");
        
        fzGLSetMode(kFZGLMode_Primitives);            
#if FZ_GL_SHADERS
        prepare();
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#else
        glVertexPointer(2, GL_FLOAT, 0, vertices);
#endif
        GLsizei nu = static_cast<GLsizei>(numOfVertices);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, nu);
    }
    
    
    void fzDrawCircle( const fzVec2& center, fzFloat r, fzFloat a, fzUInt segs, bool drawLineToCenter)
    {
        int additionalSegment = 1;
        if (drawLineToCenter)
            additionalSegment++;
        
        const float coef = 2.0f * (float)M_PI/segs;
        
        fzVec2 *vertices = new fzVec2[segs+2];
        
        for(fzUInt i = 0; i <= segs; i++)
        {
            float rads  = i * coef;
            GLfloat j   = r * fzMath_cos(rads + a) + center.x;
            GLfloat k   = r * fzMath_sin(rads + a) + center.y;
            
            vertices[i] = fzPoint(j, k);
        }
        vertices[segs+1] = center;
        
        fzGLSetMode(kFZGLMode_Primitives);            
#if FZ_GL_SHADERS
        prepare();
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#else
        glVertexPointer(2, GL_FLOAT, 0, vertices);
#endif
        
        glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segs+additionalSegment);
        
        delete [] vertices;
    }
    
    
    void fzDrawQuadBezier(const fzVec2& origin, const fzVec2& control, const fzVec2& destination, fzUInt segments)
    {
        fzVec2 *vertices = new fzVec2[segments+1];
        
        float t = 0.0f;
        for(fzUInt i = 0; i < segments; i++)
        {
            vertices[i].x = powf(1 - t, 2) * origin.x + 2.0f * (1 - t) * t * control.x + t * t * destination.x;
            vertices[i].y = powf(1 - t, 2) * origin.y + 2.0f * (1 - t) * t * control.y + t * t * destination.y;
            t += 1.0f / segments;
        }
        vertices[segments] = destination;
        
        fzGLSetMode(kFZGLMode_Primitives);            
#if FZ_GL_SHADERS
        prepare();
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#else
        glVertexPointer(2, GL_FLOAT, 0, vertices);
#endif
        glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments + 1);
        
        delete [] vertices;
    }
    
    
    void fzDrawCubicBezier(const fzVec2& origin, const fzVec2& c1, const fzVec2& c2, const fzVec2& destination, fzUInt segments)
    {
        fzVec2 *vertices = new fzVec2[segments+1];

        float t = 0;
        for(fzUInt i = 0; i < segments; i++)
        {
            vertices[i].x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * c1.x + 3.0f * (1 - t) * t * t * c2.x + t * t * t * destination.x;
            vertices[i].y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * c1.y + 3.0f * (1 - t) * t * t * c2.y + t * t * t * destination.y;
            t += 1.0f / segments;
        }
        vertices[segments] = destination;
        
        fzGLSetMode(kFZGLMode_Primitives);
        
#if FZ_GL_SHADERS
        prepare();
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
#else
        glVertexPointer(2, GL_FLOAT, 0, vertices);
#endif
        glDrawArrays(GL_LINE_STRIP, 0, (GLsizei) segments + 1);
        
        delete [] vertices;
    }
}
