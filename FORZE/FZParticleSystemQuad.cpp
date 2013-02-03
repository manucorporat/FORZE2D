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
#include "FZMS.h"


namespace FORZE {
    
    ParticleSystemQuad::ParticleSystemQuad(fzUInt number)
    : ParticleSystem(number)
    , p_texture(NULL)
    , p_quads(NULL)
    , m_blendFunc()
    {
        // allocating quads
        p_quads = new fzC4_T2_V2_Quad[getTotalParticles()];
        
        // init indices
        initIndices();
        
#if FZ_GL_SHADERS
        setGLProgram(kFZShader_mat_aC4_TEX);
#endif
    }
    
    
    ParticleSystemQuad::ParticleSystemQuad(fzUInt number, Texture2D *texture)
    : ParticleSystemQuad(number)
    {
        // set texture
        setTexture(texture);
    }
    
    
    ParticleSystemQuad::~ParticleSystemQuad()
    {
        delete [] p_quads;
        if(p_texture)
            p_texture->release();

        glDeleteBuffers(1, &m_indicesVBO);
#if FZ_VBO_STREAMING
        glDeleteBuffers(1, &m_quadsVBO);
#endif 
    }
    
    
    void ParticleSystemQuad::setDisplayFrame(const fzSpriteFrame& s)
    {
        FZ_ASSERT( s.getOffset() == FZPointZero, "QuadParticle only supports SpriteFrames with no offsets.");
        
        // update texture before updating texture rect
        if ( s.getTexture() != p_texture )
            setTexture(s.getTexture());
    }
    
    
    void ParticleSystemQuad::setTexture(Texture2D *texture, const fzRect& rect)
    {
        FZ_ASSERT(texture, "Texture can not be NULL.");
        
        // Only update the texture if is different from the current one
        if( texture != p_texture) {
            FZRETAIN_TEMPLATE(texture, p_texture);
            initTexCoordsWithRect(rect);
        }
    }
    
    
    void ParticleSystemQuad::setTexture(Texture2D *texture)
    {
        FZ_ASSERT(texture, "Texture can not be NULL.");
        setTexture(texture, fzRect(FZPointZero, texture->getContentSize()));
    }
    
    
    Texture2D* ParticleSystemQuad::getTexture() const
    {
        return p_texture;
    }
    
    
    void ParticleSystemQuad::setBlendFunc(const fzBlendFunc& b)
    {
        m_blendFunc = b;
    }
    
    
    const fzBlendFunc& ParticleSystemQuad::getBlendFunc() const
    {
        return m_blendFunc;
    }
    
    
    void ParticleSystemQuad::initIndices()
    {        
        GLushort *indices = new GLushort[m_totalParticles * 6];
        
        fzUInt i = 0;
        for(; i < m_totalParticles; ++i)
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_totalParticles * 6, indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        CHECK_GL_ERROR_DEBUG();
        
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
    }
    
    
    void ParticleSystemQuad::initTexCoordsWithRect(fzRect rect)
    {
        FZ_ASSERT(p_texture, "Texture was not created.");

        // convert to Tex coords
        fzUInt wide = p_texture->getPixelsWide();
        fzUInt high = p_texture->getPixelsHigh();
        
        rect *= p_texture->getFactor();
        
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
        for(; i < m_totalParticles; ++i)
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
    
    
    void ParticleSystemQuad::updateQuadWithParticle(const fzParticle& p)
    {
        // colors
        fzC4_T2_V2_Quad& quad = p_quads[m_particleIdx];
        
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
            const fzFloat& x = p.pos.x;
            const fzFloat& y = p.pos.y;
            
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
            quad.bl.vertex.x = p.pos.x - size_2;
            quad.bl.vertex.y = p.pos.y - size_2;
            
            // bottom-right vertex:
            quad.br.vertex.x = p.pos.x + size_2;
            quad.br.vertex.y = p.pos.y - size_2;
            
            // top-left vertex:
            quad.tl.vertex.x = p.pos.x - size_2;
            quad.tl.vertex.y = p.pos.y + size_2;
            
            // top-right vertex:
            quad.tr.vertex.x = p.pos.x + size_2;
            quad.tr.vertex.y = p.pos.y + size_2;				
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
        FZ_ASSERT(p_texture, "Texture was not created.");

        if(m_particleIdx == 0)
            return;
        
        // Default Attribs & States: GL_TEXTURE0, k,CCAttribVertex, kCCAttribColor, kCCAttribTexCoords
        // Needed states: GL_TEXTURE0, k,CCAttribVertex, kCCAttribColor, kCCAttribTexCoords
        // Unneeded states: -
        
        fzGLSetMode(kFZGLMode_Texture);
        fzGLBlendFunc( m_blendFunc.src, m_blendFunc.dst );
        p_texture->bind();
        
#if FZ_GL_SHADERS
        
        p_glprogram->use();
        FZ_SAFE_APPLY_MATRIX(p_glprogram);
        
        // atributes
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, sizeof(_fzC4_T2_V2), &p_quads->bl.vertex);
        glVertexAttribPointer(kFZAttribTexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(_fzC4_T2_V2), &p_quads->bl.texCoord);
        glVertexAttribPointer(kFZAttribColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(_fzC4_T2_V2), &p_quads->bl.color);
#else
        
        //glLoadMatrixf(get);

        glVertexPointer(2, GL_FLOAT, sizeof(_fzC4_T2_V2), &p_quads->bl.vertex);
        glTexCoordPointer(2, GL_FLOAT, sizeof(_fzC4_T2_V2), &p_quads->bl.texCoord);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(_fzC4_T2_V2), &p_quads->bl.color);
#endif
                
        FZ_ASSERT( m_particleIdx == m_particleCount, "Abnormal error in particle quad.");
    
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO);
        glDrawElements(GL_TRIANGLES, m_particleIdx * 6, GL_UNSIGNED_SHORT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}
