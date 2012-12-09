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

#include "FZGrabber.h"
#include "FZTexture2D.h"
#include "FZMacros.h"
#include "FZGLState.h"
#include "FZDirector.h"
#include "FZMath.h"
#include "FZMS.h"


namespace FORZE {
        
    Grabber::Grabber()
    : m_fbo(0), m_oldFBO(0xffffff), p_texture(NULL)
    { }
    
    
    Grabber::~Grabber()
    {
        if(m_fbo != 0)
            fzGLDeleteFramebuffers(1, &m_fbo);
        
        // if pointer is non-NULL, we release it.
        FZ_SAFE_RELEASE(p_texture);
    }
    
    
    Texture2D* Grabber::getTexture() const
    {
        return p_texture;
    }
    
    
    void Grabber::grab(Texture2D* texture)
    {
        FZ_ASSERT(texture, "Texture can not be NULL.");
        
        FZRETAIN_TEMPLATE(texture, p_texture);
        
        // generate FBO
        if(m_fbo == 0)
            fzGLGenFramebuffers(1, &m_fbo);

        // cache current framebuffer
        m_oldFBO = fzGLGetFramebuffer();
        
        // bind FBO
        fzGLBindFramebuffer(m_fbo);
        
        // associate texture with FBO
        fzGLFramebufferTexture2D(FZ_FRAMEBUFFER, FZ_COLOR_ATTACHMENT0, GL_TEXTURE_2D, p_texture->getName(), 0);
        
        // check if it worked
        GLuint status = fzGLCheckFramebufferStatus(FZ_FRAMEBUFFER);
        if (status != FZ_FRAMEBUFFER_COMPLETE) {
            FZLOGERROR("Grabber: Could not attach texture to framebuffer. Status: 0x%04X.", status);
        }

        fzGLClearColor(fzColor4F(0,0,0,0));
        glClear(GL_COLOR_BUFFER_BIT);
        
        fzGLBindFramebuffer(m_oldFBO);
        m_oldFBO = 0xffffff;
        
        CHECK_GL_ERROR_DEBUG();
    }
    
    
    void Grabber::begin()
    {
        FZ_ASSERT(m_fbo != 0, "FBO is uninitilialez, you must grab a texture first.");  
        FZ_ASSERT(m_oldFBO == 0xffffff, "This FBO is already opened.");
        
        // cache current framebuffer
        m_oldFBO = fzGLGetFramebuffer();
        
        // bind FBO
        fzGLBindFramebuffer(m_fbo);
        
        CHECK_GL_ERROR_DEBUG();
    }
    
    
    void Grabber::beginWithClear(const fzColor4F& color)
    {
        begin();
        
        fzGLClearColor(color);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    
    void Grabber::end()
    {
        fzGLBindFramebuffer(m_oldFBO);
        m_oldFBO = 0xffffff;
    }
    
    
    void Grabber::clear(const fzColor4F& color)
    {
        beginWithClear(color);
        end();
    }
    
    
#pragma mark - FBO Texture
    
    FBOTexture::FBOTexture(fzTextureFormat format, const fzSize& size, fzFloat quality)
    {
        config(format, size, FZPointZero, quality);
    }
    
    
    void FBOTexture::config(fzTextureFormat format, fzSize size, const fzPoint& anchorPoint, fzFloat quality)
    {
        FZ_ASSERT(getTexture() == NULL, "FBOTexture is already configured.");
        
        fzFloat factor = Director::Instance().getResourcesFactor() * quality;
        Texture2D *texture = new Texture2D(format, size * factor);
        texture->setFactor(factor);
        texture->setAliasTexParameters();
        m_grabber.grab(texture);
        
        // generate transform
        fzSize size2(size.width * anchorPoint.x, size.height * anchorPoint.y);
        
        // Inverted projection
        fzMath_mat4OrthoProjection(-size2.width, size.width-size2.width, size.height-size2.height, -size2.height, -1500, 1500, m_transform);
    }

    
    Texture2D* FBOTexture::getTexture() const
    {
        return m_grabber.getTexture();
    }
    
    
    void FBOTexture::begin()
    {
        m_grabber.begin();
        
        const fzSize& viewPort = getTexture()->getContentSizeInPixels();
        
        glViewport(0, 0, (GLsizei)viewPort.width, (GLsizei)viewPort.height);
        
        MS::pushMatrix(m_transform);
    }
    
    
    void FBOTexture::beginWithClear(const fzColor4F& color)
    {
        m_grabber.beginWithClear(color);
        
        const fzSize& viewPort = getTexture()->getContentSizeInPixels();
        glViewport(0, 0, (GLsizei)viewPort.width, (GLsizei)viewPort.height);
        
        MS::pushMatrix(m_transform);
    }
    
    
    void FBOTexture::end()
    {
        MS::pop();
        m_grabber.end();
        
        fzSize viewPort = Director::Instance().getViewPort();
        glViewport(0, 0, (GLsizei)viewPort.width, (GLsizei)viewPort.height);
    }
    
    
    void FBOTexture::clear(const fzColor4F& color)
    {
        m_grabber.clear(color);
    }
}
