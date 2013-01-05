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

#include "FZLayer.h"
#include "FZMacros.h"
#include "FZDirector.h"
#include "FZShaderCache.h"
#include "FZGLProgram.h"
#include "FZGLState.h"
#include "FZMS.h"


namespace FORZE {
    
#pragma mark - Layer
    
    Layer::Layer()
    : m_trackedEvents(kFZEventType_None)
    , m_priority(0)
    {
        setAnchorPoint(0.5f, 0.5f);
        setIsRelativeAnchorPoint(false);
        setContentSize(Director::Instance().getCanvasSize());
    }
    
    
    uint16_t Layer::setTrackedEvents(uint16_t flags)
    {
        if( m_trackedEvents != flags ) {
            m_trackedEvents = flags;
            if( isRunning() )
                registerWithEventDispatcher();
        }
        
        return EventManager::Instance().checkCompatibility(m_trackedEvents);
    }
    
    
    uint16_t Layer::getTrackedEvents() const
    {
        return m_trackedEvents;
    }
    
    
    void Layer::setPriority(fzInt priority)
    {
        if(priority != m_priority) {
            m_priority = priority;
            if( isRunning() )
                registerWithEventDispatcher();
        }
    }
    
    
    fzInt Layer::getPriority() const
    {
        return m_priority;
    }

    
    void Layer::onEnter()
    {
        registerWithEventDispatcher();
        Node::onEnter();
    }
    
    
    void Layer::onExit()
    {
        EventManager::Instance().removeDelegate(this);
        Node::onExit();
    }
    
    
    void Layer::registerWithEventDispatcher()
    {
        EventManager::Instance().addDelegate(this, m_trackedEvents, m_priority);
    }
    
    
#pragma mark - LayerColor
    
    LayerColor::LayerColor(const fzColor4B& color, const fzSize& size)
    : m_color(color)
    , m_alpha(color.a)
    , m_blendFunc()
    {        
        setContentSize(size);
#if FZ_GL_SHADERS
        setGLProgram(kFZShader_mat_aC4);
#endif
    }
    
    
    LayerColor::LayerColor(const fzColor4B& color)
    : LayerColor(color, Director::Instance().getCanvasSize())
    { }
    
    
    void LayerColor::setColor(const fzColor3B& color)
    {
        m_color = color;
        makeDirty(kFZDirty_color);
    }
    
    
    const fzColor3B& LayerColor::getColor() const
    {
        return m_color;
    }
    

    void LayerColor::setBlendFunc(const fzBlendFunc& blend)
    {
        m_blendFunc = blend;
    }
    
    
    const fzBlendFunc& LayerColor::getBlendFunc() const
    {
        return m_blendFunc;
    }
    
    
    void LayerColor::updateStuff()
    {        
        Node::updateStuff();
        
        if(m_dirtyFlags & kFZDirty_transform_absolute)
        {
            float vertices[8] = {
                0, 0,
                m_contentSize.width, 0,
                0, m_contentSize.height,
                m_contentSize.width, m_contentSize.height
            };
                
            fzMath_mat4Vec4(m_transformMV, vertices, p_squareVertices);
        }
        
        if(m_dirtyFlags & kFZDirty_color) {
            GLubyte cacheAlpha = static_cast<GLubyte>(m_cachedOpacity * m_alpha);
            
            fzColor4B color4B(m_color.r, m_color.g, m_color.b, cacheAlpha);            
            p_squareColors[0] = color4B;
            p_squareColors[1] = color4B;
            p_squareColors[2] = color4B;
            p_squareColors[3] = color4B;            
        }
    }
    
    
    void LayerColor::draw()
    {
        fzGLBlendFunc( m_blendFunc.src, m_blendFunc.dst );
        fzGLSetMode(kFZGLMode_NoTexture);

#if FZ_GL_SHADERS
        p_glprogram->use();
        
        glVertexAttribPointer(kFZAttribPosition, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), p_squareVertices);
        glVertexAttribPointer(kFZAttribColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, p_squareColors);
        
#else
        glLoadIdentity();
        
        glVertexPointer(2, GL_FLOAT, 4 * sizeof(float), p_squareVertices);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, p_squareColors);
        
#endif

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
    
#pragma mark - LayerGradient
        
    LayerGradient::LayerGradient(const fzColor4B& start, const fzColor4B& end, const fzSize& size)
    : LayerColor(start, size)
    , m_endColor(end)
    , m_endAlpha(end.a)
    , m_startAlpha(start.a)
    , m_vector(0, 1)
    , m_compressedInterpolation(true)
    { }
    
    
    LayerGradient::LayerGradient(const fzColor4B& start, const fzColor4B& end)
    : LayerColor(start)
    , m_endColor(end)
    , m_endAlpha(end.a)
    , m_startAlpha(start.a)
    , m_vector(0, 1)
    , m_compressedInterpolation(true)
    { }
    
    
    void LayerGradient::setStartColor(const fzColor3B& color)
    {
        setColor(color);
    }
    
    
    void LayerGradient::setEndColor(const fzColor3B& color)
    {
        m_endColor = color;
        makeDirty(kFZDirty_color);
    }
    
    
    void LayerGradient::setStartAlpha(GLubyte a)
    {
        m_startAlpha = a;
        makeDirty(kFZDirty_color);
    }
    
    
    void LayerGradient::setEndAlpha(GLubyte a)
    {
        m_endAlpha =  a;
        makeDirty(kFZDirty_color);
    }
    
    
    void LayerGradient::setVector(const fzPoint& v)
    {
        m_vector = v;
        makeDirty(kFZDirty_color);
    }
    
    
    void LayerGradient::setCompressedInterpolation(bool i)
    {
        m_compressedInterpolation = i;
        makeDirty(kFZDirty_color);
    }
    
    
    const fzColor3B& LayerGradient::getStartColor() const
    {
        return getColor();
    }
    
    
    const fzColor3B& LayerGradient::getEndColor() const
    {
        return m_endColor;
    }
    
    
    GLubyte LayerGradient::getStartAlpha() const
    {
        return m_startAlpha;
    }
    
    
    GLubyte LayerGradient::getEndAlpha() const
    {
        return m_endAlpha;
    }
    
    
    const fzPoint& LayerGradient::getVector() const
    {
        return m_vector;
    }
    
    
    bool LayerGradient::getCompressedInterpolation() const
    {
        return m_compressedInterpolation;
    }
    
    
    void LayerGradient::updateStuff()
    {
        Node::updateStuff();
        
        if(m_dirtyFlags & kFZDirty_transform_absolute)
        {
            float vertices[8] = {
                0, 0,
                m_contentSize.width, 0,
                0, m_contentSize.height,
                m_contentSize.width, m_contentSize.height
            };
            
            fzMath_mat4Vec4(m_transformMV, vertices, p_squareVertices);
        }
        
        if(m_dirtyFlags & kFZDirty_color) {
            GLubyte cachedAlpha = static_cast<GLubyte>((fzFloat)m_alpha * m_cachedOpacity);
            
            fzFloat h = m_vector.length();
            if (h == 0)
                return;
            
            const fzFloat c = sqrtf(2);
            fzPoint u = m_vector / h;
            
            // compressed interpolation mode
            if( m_compressedInterpolation )
                u *= (c / (fabsf(u.x) + fabsf(u.y)) );
            
            fzFloat opacityf = static_cast<fzFloat>(cachedAlpha) / 255.0f;
            
            fzColor4F S(m_color.r,
                      m_color.g,
                      m_color.b,
                      m_startAlpha * opacityf);
            
            fzColor4F E(m_endColor.r,
                      m_endColor.g,
                      m_endColor.b,
                      m_endAlpha * opacityf);
            
            // calculate delta values
            S -= E;
            
            const fzFloat c2 = c * 2;
            
            // (-1, -1)
            fzFloat ratio = (c + u.x + u.y) / c2;
            p_squareColors[0].r = static_cast<GLubyte>(E.r + S.r * ratio);
            p_squareColors[0].g = static_cast<GLubyte>(E.g + S.g * ratio);
            p_squareColors[0].b = static_cast<GLubyte>(E.b + S.b * ratio);
            p_squareColors[0].a = static_cast<GLubyte>(E.a + S.a * ratio);
            
            // (1, -1)
            ratio = (c - u.x + u.y) / c2;
            p_squareColors[1].r = static_cast<GLubyte>(E.r + S.r * ratio);
            p_squareColors[1].g = static_cast<GLubyte>(E.g + S.g * ratio);
            p_squareColors[1].b = static_cast<GLubyte>(E.b + S.b * ratio);
            p_squareColors[1].a = static_cast<GLubyte>(E.a + S.a * ratio);
            
            // (-1, 1)
            ratio = (c + u.x - u.y) / c2;
            p_squareColors[2].r = static_cast<GLubyte>(E.r + S.r * ratio);
            p_squareColors[2].g = static_cast<GLubyte>(E.g + S.g * ratio);
            p_squareColors[2].b = static_cast<GLubyte>(E.b + S.b * ratio);
            p_squareColors[2].a = static_cast<GLubyte>(E.a + S.a * ratio);
            
            // (1, 1)
            ratio = (c - u.x - u.y) / c2;
            p_squareColors[3].r = static_cast<GLubyte>(E.r + S.r * ratio);
            p_squareColors[3].g = static_cast<GLubyte>(E.g + S.g * ratio);
            p_squareColors[3].b = static_cast<GLubyte>(E.b + S.b * ratio);
            p_squareColors[3].a = static_cast<GLubyte>(E.a + S.a * ratio);            
        }
    }
}
