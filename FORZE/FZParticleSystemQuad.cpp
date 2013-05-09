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
    
    ParticleSystemQuad::ParticleSystemQuad(ParticleSystemLogic *logic)
    : m_textureAtlas(NULL)
    , p_logic(logic)
    , m_blendFunc()
    {        
#if FZ_GL_SHADERS
        setGLProgram(kFZShader_mat_aC4_TEX);
#endif
        m_textureAtlas.reserveCapacity(logic->getTotalParticles());
        schedule();
    }
    
    
    ParticleSystemQuad::ParticleSystemQuad(ParticleSystemLogic *logic, Texture2D *texture)
    : ParticleSystemQuad(logic)
    {
        // set texture
        setTexture(texture);
    }
    
    
    ParticleSystemQuad::~ParticleSystemQuad()
    { }
    
    
    void ParticleSystemQuad::setDisplayFrame(const fzSpriteFrame& s)
    {
        FZ_ASSERT( s.getOffset() == FZPointZero, "QuadParticle only supports SpriteFrames with no offsets.");
        
        // update texture before updating texture rect
        setTexture(s.getTexture());
    }
    
    
    void ParticleSystemQuad::setTexture(Texture2D *texture, const fzRect& rect)
    {
        FZ_ASSERT(texture, "Texture can not be NULL.");
        
        // Only update the texture if is different from the current one
        if( texture != getTexture()) {
            m_textureAtlas.setTexture(texture);
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
        return m_textureAtlas.getTexture();
    }
    
    
    void ParticleSystemQuad::setBlendFunc(const fzBlendFunc& b)
    {
        m_blendFunc = b;
    }
    
    
    const fzBlendFunc& ParticleSystemQuad::getBlendFunc() const
    {
        return m_blendFunc;
    }
    
    
    void ParticleSystemQuad::initTexCoordsWithRect(fzRect rect)
    {
        Texture2D *texture = getTexture();
        FZ_ASSERT(texture, "Texture was not created.");
        
        // convert to Tex coords
        fzUInt wide = texture->getPixelsWide();
        fzUInt high = texture->getPixelsHigh();
        rect *= texture->getFactor();
        
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
        
        fzV4_T2_C4_Quad quad;
        quad.bl.texCoord.x = left;
        quad.bl.texCoord.y = bottom;
        quad.br.texCoord.x = right;
        quad.br.texCoord.y = bottom;
        quad.tl.texCoord.x = left;
        quad.tl.texCoord.y = top;
        quad.tr.texCoord.x = right;
        quad.tr.texCoord.y = top;
        
        fzUInt i = 0;
        fzV4_T2_C4_Quad *q = m_textureAtlas.getQuads();
        for(; i < m_textureAtlas.getCapacity(); ++i) {
            q[i] = quad;
        }
    }
    
    
    void ParticleSystemQuad::update(fzFloat dt)
    {
        p_logic->preUpdate(dt);
        fzV4_T2_C4_Quad *quad = m_textureAtlas.getQuads();
        
        if(p_logic->getParticleCount() > 0) {
            fzParticle p;
            for(fzUInt index = 0; index < p_logic->getParticleCount(); ++index)
            {
                p_logic->updateParticle(index, dt, &p);
                
                // Rotation optimization
                float c = 1, s = 0;
                if( p.rotation ) {
                    float radians = -FZ_DEGREES_TO_RADIANS(m_rotation);
                    c = fzMath_cos(radians);
                    s = fzMath_sin(radians);
                }
                
                // Updating the transform
                float data[] = {
                    c,  s,
                    -s, c,
                    p.pos.x-p.size.width/2, p.pos.y-p.size.height/2, 0
                };
                
                fzAffineTransform transform;
                transform.assign(data);
                
                
                fzRect rect = fzRect(FZPointZero, p.size);
                rect.applyTransform(transform);
                
                quad->bl.vertex.x = rect.origin.x;
                quad->bl.vertex.y = rect.origin.y;
                
                // bottom-right vertex:
                quad->br.vertex.x = rect.origin.x + rect.size.width;
                quad->br.vertex.y = rect.origin.y;
                
                // top-left vertex:
                quad->tl.vertex.x = rect.origin.x;
                quad->tl.vertex.y = rect.origin.y + rect.size.height;
                
                // top-right vertex:
                quad->tr.vertex.x = rect.origin.x + rect.size.width;
                quad->tr.vertex.y = rect.origin.y + rect.size.height;
                
                
                quad->bl.color = p.color;
                quad->br.color = p.color;
                quad->tl.color = p.color;
                quad->tr.color = p.color;
                
                ++quad;
            }
            makeDirty(0);
        }
        
        // SETS THE LAST QUAD USED
        m_textureAtlas.setLastQuad(quad);
    }
    
    
    void ParticleSystemQuad::draw()
    {
        if( m_textureAtlas.getCount() == 0 )
            return;
        
#if FZ_GL_SHADERS
        p_glprogram->use();
#endif
        fzGLBlendFunc( m_blendFunc );
        FZ_SAFE_APPLY_MATRIX(p_glprogram);
        m_textureAtlas.drawQuads();
    }
}
