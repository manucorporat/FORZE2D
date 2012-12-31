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

#include "FZGLState.h"
#include "FZGLProgram.h"
#include "FZMacros.h"


namespace FORZE {

    static fzColor4F _fzCurrentClearColor(-1,-1,-1,-1);
    static fzGLMode	_fzCurrentMode = kFZGLMode_Primitives;
    static GLuint	_fzCurrentTextureID = 0;
    static GLuint	_fzCurrentFramebufferID = 0;
    static GLuint	_fzCurrentShaderProgram = 0;
    static GLenum	_fzBlendingSource = 0;
    static GLenum	_fzBlendingDest = 0;
    
    
#if FZ_GL_SHADERS
    
#define GL_ENABLE_COLOR() glEnableVertexAttribArray(kFZAttribColor);
#define GL_ENABLE_TEXCOORDS() glEnableVertexAttribArray(kFZAttribTexCoords);
#define GL_DISABLE_COLOR() glDisableVertexAttribArray(kFZAttribColor);
#define GL_DISABLE_TEXCOORDS() glDisableVertexAttribArray(kFZAttribTexCoords);
    
#else
    
#define GL_ENABLE_COLOR() glEnableClientState(GL_COLOR_ARRAY);
#define GL_ENABLE_TEXCOORDS() glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#define GL_DISABLE_COLOR() glDisableClientState(GL_COLOR_ARRAY);
#define GL_DISABLE_TEXCOORDS() glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
#endif
    
    void fzGLSetMode(fzGLMode mode)
    {
#if FZ_GL_SHADERS
        return;
#endif
        if(_fzCurrentMode == mode)
            return;
        
        // COLOR ARRAY
        bool flagEnabled = (mode & kGLColors);
        if(flagEnabled != (_fzCurrentMode & kGLColors))
        {
            if(flagEnabled)
                GL_ENABLE_COLOR()
            else
                GL_DISABLE_COLOR();
        }
        
        // TEXTURES
        flagEnabled = (mode & kGLTexcoords);
        if(flagEnabled != (_fzCurrentMode & kGLTexcoords))
        {
            if(flagEnabled) {
                glEnable(GL_TEXTURE_2D);
                GL_ENABLE_TEXCOORDS();
            }else{
                glDisable(GL_TEXTURE_2D);
                GL_DISABLE_TEXCOORDS();
            }
        }
        //CHECK_GL_ERROR_DEBUG();
        _fzCurrentMode = mode;
    }
    
    
    void fzGLBindTexture2D( GLuint textureID )
    {
        if(_fzCurrentTextureID != textureID) {
            _fzCurrentTextureID = textureID;
            glBindTexture(GL_TEXTURE_2D, textureID);
        }
    }
    
    
    GLuint fzGLGetFramebuffer()
    {
        //if(_fzCurrentFramebufferID == 0)
        //    glGetIntegerv(FZ_FRAMEBUFFER_BINDING, (GLint*)&_fzCurrentFramebufferID);

        return _fzCurrentFramebufferID;
    }
    
    
    void fzGLBindFramebuffer(GLuint framebuffer)
    {
        if(_fzCurrentFramebufferID != framebuffer)
        {
            _fzCurrentFramebufferID = framebuffer;
            _fzGLBindFramebuffer(FZ_FRAMEBUFFER, framebuffer);
        }
    }
    
    void fzGLClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
    {
        fzGLClearColor(fzColor4F(r, g, b, a));
    }
    
    
    void fzGLClearColor(const fzColor4F& color)
    {
        if(_fzCurrentClearColor != color)
        {
            _fzCurrentClearColor = color;
            glClearColor(color.r, color.g, color.b, color.a);
        }
    }
        
    
    void fzGLUseProgram( GLuint program )
    {
        if( program != _fzCurrentShaderProgram ) {
            _fzCurrentShaderProgram = program;
            glUseProgram(program);
        }
    }
    
    
    void fzGLBlendFunc(const fzBlendFunc& blend)
    {
        fzGLBlendFunc(blend.src, blend.dst);
    }
    
    
    void fzGLBlendFunc(GLenum sfactor, GLenum dfactor)
    {
        if( sfactor != _fzBlendingSource || dfactor != _fzBlendingDest ) {

            _fzBlendingSource = sfactor;
            _fzBlendingDest = dfactor;
            glBlendFunc( sfactor, dfactor );
            
            CHECK_GL_ERROR_DEBUG();
        }
    }
    
    
#pragma mark - Deleting

    void fzGLDeleteProgram( GLuint program )
    {
        if( program == _fzCurrentShaderProgram )
            _fzCurrentShaderProgram = 0;
        
        glDeleteProgram( program );
    }
    
    
    void fzGLDeleteTexture( GLuint textureID )
    {
        if( _fzCurrentTextureID == textureID )
            _fzCurrentTextureID = 0;
        
        glDeleteTextures(1, &textureID);
    }
}
