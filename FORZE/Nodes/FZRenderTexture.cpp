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

#include "FZRenderTexture.h"
#include "FZSprite.h"
#include "FZMacros.h"
#include "FZGLState.h"
#include "FZMath.h"
#include "matrixStack.h"
#include "FZLayer.h"


namespace FORZE {
    
    RenderTexture::RenderTexture(const fzSize& size, fzTextureFormat format)
    : m_grabber(format, size)
    , m_format(format)
    {
        // ATTACH TEXTURE TO SPRITE
        p_sprite = new Sprite(m_grabber.getTexture());
        p_sprite->setAnchorPoint(FZPointZero);
        p_sprite->setIsRelativeAnchorPoint(false);
        p_sprite->setBlendFunc(fzBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));  
        addChild(p_sprite);
        
        setContentSize(size);
    }
    
    
    RenderTexture::~RenderTexture()
    { }
    
    
    void RenderTexture::setSprite(Sprite *s)
    {
        p_sprite = s;
    }
    
    
    Sprite* RenderTexture::getSprite() const
    {
        return p_sprite;
    }
    
    
    void RenderTexture::begin()
    {
        m_grabber.begin();
        fzMath_mat4Multiply(fzMS_getMatrix(), getParentToNodeTransform(), m_renderTransform);
        fzMS_pushMatrix(m_renderTransform);
    }
    
    
    void RenderTexture::beginWithClear(const fzColor4F& c)
    {
        m_grabber.beginWithClear(c);
        fzMath_mat4Multiply(fzMS_getMatrix(), getParentToNodeTransform(), m_renderTransform);
        fzMS_pushMatrix(m_renderTransform);
    }
    
    
    void RenderTexture::clear(const fzColor4F& c)
    {
        fzMS_pop();
        m_grabber.clear(c);
    }
    
    
    void RenderTexture::end()
    {
        fzMS_pop();
        m_grabber.end();
    }
}
