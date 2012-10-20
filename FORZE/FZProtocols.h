// DO NOT MODIFY THE HEADERS IF FORZE IS ALREADY COMPILED AS A STATIC LIBRARY
#ifndef __FZPROTOCOLS_H_INCLUDED__
#define __FZPROTOCOLS_H_INCLUDED__
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

#include "FZGLConfig.h"

namespace FORZE {
    
#pragma mark - ApplicationProtocol
    
    class ApplicationProtocol
    {
    public:
        virtual ~ApplicationProtocol(){}

        virtual void applicationLaunched(void *options) = 0;
        virtual void applicationPaused() {}
        virtual void applicationResumed() {}
        virtual void applicationTerminate() {}
        
        virtual GLConfig fzGLConfig()
        {
            GLConfig config;
            return config;
        }
    };
    
    
#pragma mark - BlendProtocol
    
    class BlendProtocol
    {
    public:
        virtual ~BlendProtocol(){}
        
        virtual const fzBlendFunc& getBlendFunc() const = 0;
        virtual void setBlendFunc(const fzBlendFunc&) = 0;
    };
    
    
#pragma mark - RGBAProtocol
    
    class RGBAProtocol
    {
    public:
        virtual ~RGBAProtocol(){}

        virtual const fzColor3B& getColor() const {}
        virtual void setColor(const fzColor3B&) {}
    };
    
    
#pragma mark - TextureProtocol
    
    class GLProgram;
    class GLProgramProtocol
    {
    protected:
        GLProgram *p_glprogram;
        
    public:
        GLProgramProtocol() : p_glprogram(NULL) {}
        
        virtual ~GLProgramProtocol() {
#if FZ_GL_SHADERS
            setGLProgram((GLProgram*)NULL);
#endif
        }
        
        //! Sets the GLProgram used by the node to render his opengl stuff (NOT INCLUDING children).
        //! This parammeter doesn't make sense in void elements such as "Node", "Scene" or "Layer".
        //! @see getGLProgram
        //! @see setFilter
        //! @see getFilter
        virtual void setGLProgram(GLProgram*);
        
        
        //! Sets the GLProgram by a tag value
        virtual void setGLProgram(fzUInt programKey);
        
        

        virtual GLProgram* getGLProgram() const {
            return p_glprogram;
        }
    };
    
    class Texture2D;
    class TextureProtocol
    {
    public:
        virtual ~TextureProtocol(){}

        virtual Texture2D* getTexture() const = 0;
        virtual void setTexture(Texture2D*) = 0;
    };
}
#endif
