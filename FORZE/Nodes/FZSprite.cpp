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

#include "FZSprite.h"
#include "FZSpriteBatch.h"
#include "FZAnimation.h"
#include "FZMacros.h"
#include "FZPrimitives.h"
#include "FZGLState.h"
#include "FZSpriteFrameCache.h"
#include "FZAnimationCache.h"
#include "FZTextureCache.h"
#include "FZShaderCache.h"
#include "FZDirector.h"
#include "FZMS.h"
#include "FZTexture2D.h"


using namespace STD;

namespace FORZE {
    
    enum {
        kFZSprite_Uninitialized,
        kFZSprite_SelfRendering,
        kFZSprite_BatchRendering
    };
    
#pragma mark - Initialization
    
    Sprite::Sprite()
    : m_mode            (kFZSprite_Uninitialized)
    , m_textureRect     (FZRectZero)
    , m_offset          (FZPointZero)
    , m_unflippedOffset (FZPointZero)
    , m_color           (fzWHITE)
    , m_alpha           (255)
    , m_rectRotated     (false)
    , m_flipX           (false)
    , m_flipY           (false)
    , mode              ()
    {
        useSelfRender();

        setIsRelativeAnchorPoint(true);
        setAnchorPoint(0.5f, 0.5f);
        
#if FZ_GL_SHADERS
        // shader program
        setGLProgram(kFZShader_mat_uC4_TEX);
#endif
    }
    
    
    Sprite::Sprite(Texture2D* texture, const fzRect& rect)
    : Sprite()
    {        
        setTexture(texture);
        setTextureRect(rect);
    }
    
    
    Sprite::Sprite(Texture2D* texture)
    : Sprite(texture, ((texture) ? fzRect(FZPointZero, texture->getContentSize()) : FZRectZero))
    { }
    

    Sprite::Sprite(const string& name)
    : Sprite(TextureCache::Instance().addImage(name.c_str()))
    { }
    
    
    Sprite::Sprite(const string& filename, const fzRect& rect)
    : Sprite(TextureCache::Instance().addImage(filename.c_str()), rect)
    { }
    
    
    Sprite::Sprite(const fzRect& rect)
    : Sprite()
    {
        setTextureRect(rect);    
    }
    
    Sprite::Sprite(const fzSpriteFrame& spriteFrame)
    : Sprite()
    {
        setDisplayFrame(spriteFrame);
    }
    
    
    Sprite::~Sprite()
    {
        if(m_mode == kFZSprite_SelfRendering)
            setTexture(NULL);
    }
    
    
    void Sprite::useSelfRender()
    {
        if(m_mode == kFZSprite_SelfRendering)
            return;
        
        makeDirty(kFZDirty_all);
        m_blendFunc = fzBlendFunc();    
        m_mode = kFZSprite_SelfRendering;
    }
    
    
    void Sprite::useBatchRender(SpriteBatch* batch)
    {
        FZ_ASSERT(batch != NULL, "Batch node cannot be NULL.");
        
        if(m_mode == kFZSprite_BatchRendering)
            return;
        
        if(m_mode == kFZSprite_SelfRendering)
            setTexture(NULL);
        
        makeDirty(kFZDirty_all);
        mode.B.p_currentQuad = NULL;
        mode.B.p_batchNode = batch;
        m_mode = kFZSprite_BatchRendering;
        updateTextureCoords(m_textureRect);
    }
    
    
    void Sprite::insertChild(Node *child)
    {
#if FZ_SPRITE_CHILDREN
        FZ_ASSERT(dynamic_cast<Sprite*>(child), "Child should be a sprite.");
        
        Node::insertChild(child);
        
        if(m_mode == kFZSprite_BatchRendering) {
            Sprite *sprite = static_cast<Sprite*>(child);
            sprite->useBatchRender(mode.B.p_batchNode);
        }
#else
        (void)child;
        FZ_ASSERT(false, "Children in Sprite are disabled. Set FZ_SPRITE_CHILDREN 1.");
#endif
    }
    
    
#pragma mark - Setters
    
    void Sprite::setTexture(Texture2D* texture)
    {
        FZ_ASSERT( m_mode == kFZSprite_SelfRendering, "Sprite mode is not kFZSprite_SelfRendering.");

        if(m_mode != kFZSprite_SelfRendering)
            return;
        
        FZRETAIN_TEMPLATE(texture, mode.A.p_texture);
        
        updateTextureCoords(m_textureRect);
    }
    
    
    void Sprite::setTextureRect(const fzRect& rect, const fzSize& untrimmedSize, bool rotated)
    {    
        m_textureRect = rect;
        m_rectRotated = rotated;
        
        setContentSize(untrimmedSize);
        updateTextureCoords(rect);

        
        // Calculate final offset
        fzPoint relativeOffset(m_unflippedOffset);
        if( m_flipX )
            relativeOffset.x *= -1;
        
        if( m_flipY )
            relativeOffset.y *= -1;

        m_offset = relativeOffset + (m_contentSize - m_textureRect.size)/2;
        
        
        // Generate untransformed vertices
        fzPoint xy2 = m_offset + m_textureRect.size;        

        m_vertices[0] = m_offset;
        m_vertices[1] = fzVec2(xy2.x, m_offset.y);
        m_vertices[2] = fzVec2(m_offset.x, xy2.y);
        m_vertices[3] = xy2;

        makeDirty(kFZDirty_transform_absolute);
    }
    
    
    void Sprite::updateTextureCoords(fzRect rect)
    {
        Texture2D *texture = getTexture();
        if(texture == NULL)
            return;
        
        fzFloat wide = texture->getPixelsWide();
        fzFloat high = texture->getPixelsHigh();
        
        rect *= texture->getFactor();
        
#if FZ_FIX_ARTIFACTS_BY_STRECHING_TEXEL
        wide *= 2;
        high *= 2;
        rect.origin.x       = rect.origin.x*2+1;
        rect.origin.y       = rect.origin.y*2+1;
        rect.size.width     = rect.size.width*2-2;
        rect.size.height    = rect.size.height*2-2;
#endif // ! FZ_FIX_ARTIFACTS_BY_STRECHING_TEXEL
        
        
        fzFloat A = rect.origin.x / wide;
        fzFloat B = A + rect.size.width / wide;
        fzFloat C = rect.origin.y / high;
        fzFloat D = C + rect.size.height / high;
        
        if(m_rectRotated) {
            
            if( m_flipX )
                FZ_SWAP(D, C);
            
            if( m_flipY )
                FZ_SWAP(A, B);
            
            m_texCoords[0] = fzVec2(A, D);
            m_texCoords[1] = fzVec2(A, C);
            m_texCoords[2] = fzVec2(B, D);
            m_texCoords[3] = fzVec2(B, C);
            
        } else {
            if( m_flipX )
                FZ_SWAP(A, B);
            
            if( m_flipY )
                FZ_SWAP(C, D);
            
            m_texCoords[0] = fzVec2(A, D);
            m_texCoords[1] = fzVec2(B, D);
            m_texCoords[2] = fzVec2(A, C);
            m_texCoords[3] = fzVec2(B, C);
        }

        makeDirty(kFZDirty_texcoords);
    }
    
    
    void Sprite::setBlendFunc(const fzBlendFunc& blend)
    {
        FZ_ASSERT( m_mode == kFZSprite_SelfRendering, "Sprite mode is not kFZSprite_SelfRendering.");
        m_blendFunc = blend;
    }
    
    
    void Sprite::setFlipX(bool fX)
    {
        if( m_flipX != fX ) {
            m_flipX = fX;
            setTextureRect(m_textureRect, m_contentSize, m_rectRotated);
        }
    }
    
    
    void Sprite::setFlipY(bool fY)
    {
        if( m_flipY != fY ) {
            m_flipY = fY;
            setTextureRect(m_textureRect, m_contentSize, m_rectRotated);
        }	
    }
    
    
    void Sprite::setBatch(SpriteBatch *batch)
    {
        FZ_ASSERT( m_mode == kFZSprite_BatchRendering, "Sprite mode is not kFZSprite_BatchRendering.");
        mode.B.p_batchNode = batch;
    }
    
    
    void Sprite::setColor(const fzColor3B& color)
    {
        if(color != m_color) {
            m_color = color;
            makeDirty(kFZDirty_color);
        }
    }
    
    
#pragma mark - Getters
    
    Texture2D* Sprite::getTexture() const
    {
        if(m_mode == kFZSprite_BatchRendering)
            return mode.B.p_batchNode->getTexture();
        else if(m_mode == kFZSprite_SelfRendering)
            return mode.A.p_texture;
        
        else return NULL;
    }
    
    
    const fzBlendFunc& Sprite::getBlendFunc() const
    {
        FZ_ASSERT( m_mode == kFZSprite_SelfRendering, "Sprite mode is not kFZSprite_SelfRendering.");
        return m_blendFunc;
    }
    
    
    SpriteBatch* Sprite::getBatch() const
    {
        if(m_mode != kFZSprite_BatchRendering) {
            FZ_ASSERT( false, "Sprite mode is not kFZSprite_BatchRendering.");
            return NULL;
        }            
        return mode.B.p_batchNode;
    }
    
    
    TextureAtlas* Sprite::getTextureAtlas() const
    {
        if(m_mode != kFZSprite_BatchRendering) {
            FZ_ASSERT( false, "Sprite mode is not kFZSprite_BatchRendering.");
            return NULL;
        }    
        return mode.B.p_batchNode->getTextureAtlas();
    }
    
    
    const fzColor3B& Sprite::getColor() const
    {
        return m_color;
    }
    
    
    void Sprite::getVertices(float *vertices) const
    {
        fzMath_mat4Vec2(m_transformMV, reinterpret_cast<const float*>(m_vertices), vertices);
    }
    
    
    fzSpriteFrame Sprite::getDisplayFrame() const
    {        
        return fzSpriteFrame(getTexture(),
                             m_textureRect,
                             m_offset,
                             m_contentSize,
                             m_rectRotated);
    }
    
    
#pragma mark - Updating protocols
    
    bool Sprite::updateTransform(fzV4_T2_C4_Quad **quadp)
    {        
        FZ_ASSERT( m_mode == kFZSprite_BatchRendering, "Sprite mode is not kFZSprite_BatchRendering.");
        
        if(!m_isVisible)
            return false;
        
        
        fzV4_T2_C4_Quad *quad = *quadp;
        ++(*quadp);
        
        FZ_ASSERT( quad != NULL, "Quad cannot be NULL.");

        if( mode.B.p_currentQuad != quad) {
            mode.B.p_currentQuad = quad;
            m_dirtyFlags |= kFZDirty_transform_absolute | kFZDirty_color | kFZDirty_texcoords;
            
        }else if(m_dirtyFlags == 0)
            return false;
        
        
        // UPDATING ABSOLUTE TRANSFORM
        if( m_dirtyFlags & kFZDirty_transform_absolute )
        {
            register fzVec4 output[4];

#if FZ_SPRITE_CHILDREN || 1
            fzMath_mat4Multiply(MS::getMatrix(), getNodeToParentTransform(), m_transformMV);            
            fzMath_mat4Vec4(m_transformMV, (float*)m_vertices, (float*)output);
#else
            fzMath_mat4Vec4Affine(MS::getMatrix(), getNodeToParentTransform(), m_vertices, (float*)output);
#endif
            quad->bl.vertex = output[0];
            quad->br.vertex = output[1];
            quad->tl.vertex = output[2];
            quad->tr.vertex = output[3];
        }
        
        
        // UPDATING TEXTURE COORDS
        if( m_dirtyFlags & kFZDirty_texcoords ) {
            
            quad->bl.texCoord = m_texCoords[0];
            quad->br.texCoord = m_texCoords[1];
            quad->tl.texCoord = m_texCoords[2];
            quad->tr.texCoord = m_texCoords[3];
        }
        
        
        // UPDATING RECURSIVE OPACITY
        if(m_dirtyFlags & kFZDirty_opacity) {
            
            m_cachedOpacity = p_parent->getCachedOpacity() * m_opacity;
            m_dirtyFlags |= kFZDirty_color;
        }
        
        if(m_dirtyFlags & kFZDirty_color) {
            
            const GLubyte cachedAlpha = static_cast<GLubyte>(m_cachedOpacity * m_alpha);
            const fzColor4B color4(m_color.r, m_color.g, m_color.b, cachedAlpha);
            quad->bl.color = color4;
            quad->br.color = color4;
            quad->tl.color = color4;
            quad->tr.color = color4;            
        }
        
#if FZ_SPRITE_CHILDREN
        Sprite *child = static_cast<Sprite*>(m_children.front());
        if(child) {
            unsigned char flags = m_dirtyFlags & kFZDirty_recursive;
            
            MS::pushMatrix(m_transformMV);
            for(; child; child = static_cast<Sprite*>(child->next())) {
                child->makeDirty(flags);
                child->updateTransform(quadp);
            }
            MS::pop();
        }
#endif
        m_dirtyFlags = 0;
        
        return true;
    }
    
    
    void Sprite::updateStuff()
    {
        Node::updateStuff();
        
        if(m_dirtyFlags & kFZDirty_transform_absolute)
            fzMath_mat4Vec4(m_transformMV, reinterpret_cast<const float*>(m_vertices), (float*)mode.A.m_finalVertices);
    }
    
    
#pragma mark - Rendering
    
    void Sprite::draw()
    {
        FZ_ASSERT(m_mode == kFZSprite_SelfRendering, "If Sprite is being rendered by SpriteBatch, Sprite::draw SHOULD NOT be called.");

        // Bind texture
        fzGLSetMode(kFZGLMode_TextureNoColor);
        if(mode.A.p_texture)
            mode.A.p_texture->bind();
        
        fzGLBlendFunc( m_blendFunc );
        
#if FZ_GL_SHADERS
        
        p_glprogram->use();
        p_glprogram->setUniform4f(kFZUniformColor_s, m_color.r/255.0f, m_color.g/255.0f, m_color.b/255.0f, (m_cachedOpacity * m_alpha)/255.0f);
        
        // atributes
        glVertexAttribPointer(kFZAttribTexCoords, 2, GL_FLOAT, GL_FALSE, 0, m_texCoords);
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, sizeof(fzVec4), mode.A.m_finalVertices);
        
#else
        glLoadIdentity();
        
        // atributes
        fzGLColor(m_color);
        glVertexPointer(3, GL_FLOAT, sizeof(fzVec4), mode.A.m_finalVertices);	
        glTexCoordPointer(2, GL_FLOAT, 0, m_texCoords);
#endif
        
        // Rendering
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	
    }
    
    
#pragma mark - Sprite Frames
    
    void Sprite::setDisplayFrame(const fzSpriteFrame& spriteFrame)
    {
        if(spriteFrame.isValid()) {
            
            m_unflippedOffset = spriteFrame.getOffset();
            setTexture(spriteFrame.getTexture());
            setTextureRect(spriteFrame.getRect(), spriteFrame.getOriginalSize(), spriteFrame.isRotated());
        }
    }
    
    
    void Sprite::setDisplayFrame(const char *spriteFrameName)
    {
        setDisplayFrame(SpriteFrameCache::Instance().getSpriteFrameByKey(spriteFrameName));
    }
   
    
//    void Sprite::setDisplayFrameWithAnimationName(const char* animationName, fzUInt index)
//    {
//        FZ_ASSERT( animationName, "Sprite#setDisplayFrameWithAnimationName. animationName must not be nil");
//        
//        Animation *animation = AnimationCache::Instance().animationByName(animationName);
//        fzSpriteFrame frame = animation->getFrames().at(frameIndex);
//        setDisplayFrame(frame);
//    }
}
