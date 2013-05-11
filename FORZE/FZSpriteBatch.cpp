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

#include "FZSpriteBatch.h"
#include "FZSprite.h"
#include "FZMacros.h"
#include "FZTextureCache.h"
#include "FZShaderCache.h"
#include "FZGLState.h"
#include "FZPrimitives.h"
#include "FZDirector.h"


using namespace STD;

namespace FORZE {
    
    SpriteBatch::SpriteBatch(Texture2D *texture, fzUInt capacity)
    : m_textureAtlas(texture, capacity)
    , m_blendFunc()
    {
#if FZ_GL_SHADERS
        setGLProgram(kFZShader_nomat_aC4_TEX);
#endif
    }
    
    
    SpriteBatch::SpriteBatch(const string& filename, fzUInt capacity)
    : SpriteBatch(TextureCache::Instance().addImage(filename.c_str()), capacity)
    { }
    
    
    void SpriteBatch::insertChild(Node* node)
    {
        FZ_ASSERT( dynamic_cast<Sprite*>(node), "SpriteBatch's children must be Sprites.");

        Sprite *sprite = static_cast<Sprite*>(node);
        
        Node::insertChild(sprite);
        sprite->useBatchRender(this);
        
        
        Sprite *sprite_child;
        FZ_LIST_FOREACH(sprite->getChildren(), sprite_child) {
            sprite_child->useBatchRender(this);
        }
    }
    
    
    void SpriteBatch::setTexture(Texture2D* t)
    {
        m_textureAtlas.setTexture(t);
    }
    
    
    Texture2D* SpriteBatch::getTexture() const
    {
        return m_textureAtlas.getTexture();
    }
    
    
    void SpriteBatch::setBlendFunc(const fzBlendFunc& b)
    {
        m_blendFunc = b;
    }
    
    
    const fzBlendFunc& SpriteBatch::getBlendFunc() const
    {
        return m_blendFunc;
    }
    
    
    void SpriteBatch::render(unsigned char dirtyFlags)
    {
        FZ_ASSERT(p_parent != NULL, "SpriteBatch should NOT be root node.");
        
        // RESERVE MEMORY
        m_textureAtlas.reserveCapacity(m_children.size());
        
        // ITERATE SPRITES
        fzV4_T2_C4_Quad *quad = m_textureAtlas.getQuads();

        Sprite *child;
        FZ_LIST_FOREACH(m_children, child)
        {
            child->makeDirty(dirtyFlags);
#if FZ_VBO_STREAMING
            if(child->updateTransform(&quad))
                m_textureAtlas.updateQuad(quad);
#else
            child->updateTransform(&quad);
#endif
        }
        
        // SETS THE LAST QUAD USED
        m_textureAtlas.setLastQuad(quad);

        // RENDERING
        draw();
    }
    
    
    void SpriteBatch::draw()
    {         
        if( m_textureAtlas.getCount() == 0 )
            return;
        
#if FZ_SPRITE_DEBUG_DRAW
        Sprite *sprite;
        FZ_LIST_FOREACH(m_children, sprite) {
            if(sprite->isVisible())
            fzDrawRect( sprite->getLocalBoundingBox() );
        }
#endif
        
#if FZ_GL_SHADERS
        p_glprogram->use();
#endif

        fzGLBlendFunc(m_blendFunc);        
        m_textureAtlas.drawQuads();
    }
}
