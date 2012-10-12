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

#include "FZParticleSystemQuad.h"
#include "FZTexture2D.h"
#include "FZTextureCache.h"
#include "FZMacros.h"
#include "FZSpriteFrame.h"
#include "FZDirector.h"
#include "FZShaderCache.h"
#include "FZGLState.h"
#include "FZMath.h"
#include "matrixStack.h"


namespace FORZE {
    
    ParticleSystemQuad::ParticleSystemQuad(fzUInt number, Texture2D *texture)
    : ParticleSystem(number, texture)
    {
        // allocating quads
        p_quads = new fzC4_T2_V2_Quad[totalParticles_];
        
        // initialize only once the texCoords and the indices
        initTexCoordsWithRect(fzRect(0, 0, texture_->getPixelsWide(), texture_->getPixelsHigh()));
        initIndices();
        
#if FZ_GL_SHADERS
        setGLProgram(kFZShader_mat_aC4_TEX);
#endif  
    }
    
    
    ParticleSystemQuad::~ParticleSystemQuad()
    {
        delete [] p_quads;

        glDeleteBuffers(1, &m_indicesVBO);
#if FZ_VBO_STREAMING
        glDeleteBuffers(1, &m_quadsVBO);
#endif 
    }
    
    
    void ParticleSystemQuad::initIndices()
    {        
        GLushort *indices = new GLushort[totalParticles_ * 6];
        
        fzUInt i = 0;
        for(; i < totalParticles_; ++i)
        {
            const fzUInt i6 = i*6;
            const GLushort i4 = i*4;
            
            indices[i6] = i4;
            indices[i6+1] = i4+1;
            indices[i6+2] = i4+2;
            
            indices[i6+5] = i4+1;
            indices[i6+4] = i4+2;
            indices[i6+3] = i4+3;
        }
        // generate vertex buffer object
        glGenBuffers(1, &m_indicesVBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO);
        // copy indices to vram
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * totalParticles_ * 6, indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        // dealloc indices
        delete [] indices;
        
        
#if FZ_VBO_STREAMING
        // create the VBO buffer
        glGenBuffers(1, &m_quadsVBO);
        
        // initial binding
        glBindBuffer(GL_ARRAY_BUFFER, m_quadsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(fzC4_T2_V2_Quad) * totalParticles_, p_quads, GL_DYNAMIC_DRAW);	
        glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif // FZ_USES_VBO
        
        
        //fzOpenglUnLock();
    }
    
    
    void ParticleSystemQuad::initTexCoordsWithRect(fzRect rect)
    {
        // convert to Tex coords        
        fzUInt wide = texture_->getPixelsWide();
        fzUInt high = texture_->getPixelsHigh();
        
        rect.origin *= texture_->getFactor();
        rect.size *= texture_->getFactor();
        
#if FZ_FIX_ARTIFACTS_BY_STRECHING_TEXEL
        wide *= 2;
        high *= 2;
        rect.origin.x       = rect.origin.x*2+1;
        rect.origin.y       = rect.origin.y*2+1;
        rect.size.width     = rect.size.width*2-2;
        rect.size.height    = rect.size.height*2-2;
#endif // ! FZ_FIX_ARTIFACTS_BY_STRECHING_TEXEL
        
        GLfloat left    = rect.origin.x / wide;
        GLfloat right   = left + rect.size.width / wide;
        GLfloat top     = rect.origin.y / high;
        GLfloat bottom  = top + rect.size.height / high;
        
        fzUInt i = 0;
        for(; i < totalParticles_; ++i)
        {
            p_quads[i].bl.texCoord.x = left;
            p_quads[i].bl.texCoord.y = bottom;
            p_quads[i].br.texCoord.x = right;
            p_quads[i].br.texCoord.y = bottom;
            p_quads[i].tl.texCoord.x = left;
            p_quads[i].tl.texCoord.y = top;
            p_quads[i].tr.texCoord.x = right;
            p_quads[i].tr.texCoord.y = top;
        }
    }
    
    
    void ParticleSystemQuad::setDisplayFrame(const fzSpriteFrame& s)
    {
        FZ_ASSERT( s.getOffset() == FZPointZero, "QuadParticle only supports SpriteFrames with no offsets");
        
        // update texture before updating texture rect
        if ( s.getTexture()->getName() != texture_->getName() )
            setTexture(s.getTexture());
    }
    
    
    void ParticleSystemQuad::setTexture(Texture2D *t, const fzRect& rect)
    {
        // Only update the texture if is different from the current one
        if( t->getName() != texture_->getName() ) {
            ParticleSystem::setTexture(t);
            initTexCoordsWithRect(rect);
        }
    }
    
    
    void ParticleSystemQuad::setTexture(Texture2D *t)
    {
        setTexture(t, fzRect(FZPointZero, texture_->getContentSize()));
    }
    
    
    void ParticleSystemQuad::updateQuadWithParticle(const tFZParticle& p, const fzPoint& newPos)
    {
        // colors
        fzC4_T2_V2_Quad& quad = p_quads[particleIdx_];
        
        fzColor4B color4B(p.color);
        quad.bl.color = color4B;
        quad.br.color = color4B;
        quad.tl.color = color4B;
        quad.tr.color = color4B;
        
        // vertices
        fzFloat size_2 = p.size/2;
        
        if( p.rotation ) {
            fzFloat x1 = -size_2;
            fzFloat y1 = -size_2;
            
            fzFloat& x2 = size_2;
            fzFloat& y2 = size_2;
            const fzFloat& x = newPos.x;
            const fzFloat& y = newPos.y;
            
            fzFloat r = -FZ_DEGREES_TO_RADIANS(p.rotation);
            fzFloat sr = fzMath_sin(r);
            fzFloat cr = fzMath_cos(r);
            
            fzFloat ax = x1 * cr - y1 * sr + x;
            fzFloat ay = x1 * sr + y1 * cr + y;
            fzFloat bx = x2 * cr - y1 * sr + x;
            fzFloat by = x2 * sr + y1 * cr + y;
            fzFloat cx = x2 * cr - y2 * sr + x;
            fzFloat cy = x2 * sr + y2 * cr + y;
            fzFloat dx = x1 * cr - y2 * sr + x;
            fzFloat dy = x1 * sr + y2 * cr + y;
            
            // bottom-left
            quad.bl.vertex.x = ax;
            quad.bl.vertex.y = ay;
            
            // bottom-right vertex:
            quad.br.vertex.x = bx;
            quad.br.vertex.y = by;
            
            // top-left vertex:
            quad.tl.vertex.x = dx;
            quad.tl.vertex.y = dy;
            
            // top-right vertex:
            quad.tr.vertex.x = cx;
            quad.tr.vertex.y = cy;
        } else {
            // bottom-left vertex:
            quad.bl.vertex.x = newPos.x - size_2;
            quad.bl.vertex.y = newPos.y - size_2;
            
            // bottom-right vertex:
            quad.br.vertex.x = newPos.x + size_2;
            quad.br.vertex.y = newPos.y - size_2;
            
            // top-left vertex:
            quad.tl.vertex.x = newPos.x - size_2;
            quad.tl.vertex.y = newPos.y + size_2;
            
            // top-right vertex:
            quad.tr.vertex.x = newPos.x + size_2;
            quad.tr.vertex.y = newPos.y + size_2;				
        }
    }
    
    
    void ParticleSystemQuad::postStep()
    {
#if FZ_VBO_STREAMING
        glBindBuffer(GL_ARRAY_BUFFER, m_quadsVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(fzC4_T2_V2_Quad) * particleCount_, p_quads);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
    }
    
    
    void ParticleSystemQuad::draw()
    {	
        if(particleIdx_ == 0)
            return;
        
        // Default Attribs & States: GL_TEXTURE0, k,CCAttribVertex, kCCAttribColor, kCCAttribTexCoords
        // Needed states: GL_TEXTURE0, k,CCAttribVertex, kCCAttribColor, kCCAttribTexCoords
        // Unneeded states: -
        
        fzGLSetMode(kFZGLMode_Texture);
        fzGLBlendFunc( blendFunc_.src, blendFunc_.dst );

        
#if FZ_GL_SHADERS
        
        p_glprogram->use();
        APPLY_MATRIX(p_glprogram);
        
        // atributes
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, sizeof(_fzC4_T2_V2), &p_quads->bl.vertex);
        glVertexAttribPointer(kFZAttribTexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(_fzC4_T2_V2), &p_quads->bl.texCoord);
        glVertexAttribPointer(kFZAttribColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(_fzC4_T2_V2), &p_quads->bl.color);
#else
        
        glLoadMatrixf(m_transformMV);

        glVertexPointer(2, GL_FLOAT, sizeof(_fzC4_T2_V2), &p_quads->bl.vertex);
        glTexCoordPointer(2, GL_FLOAT, sizeof(_fzC4_T2_V2), &p_quads->bl.texCoord);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(_fzC4_T2_V2), &p_quads->bl.color);
#endif
                
        FZ_ASSERT( particleIdx_ == particleCount_, "Abnormal error in particle quad");
    
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO);
        glDrawElements(GL_TRIANGLES, particleIdx_ * 6, GL_UNSIGNED_SHORT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
