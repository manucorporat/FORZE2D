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

#include "FZShaderCache.h"

#if FZ_GL_SHADERS

// DEFAULT SHADERS
#include "Shaders/_fz_aC4_TEX.shader.h"
#include "Shaders/_fz_mat_TEX.shader.h"
#include "Shaders/_fz_mat_aC4.shader.h"
#include "Shaders/_fz_mat_uC4.shader.h"
#include "Shaders/_fz_mat_uC4_TEX.shader.h"


namespace FORZE {

    ShaderCache* ShaderCache::p_instance = NULL;
    
    ShaderCache& ShaderCache::Instance()
    {
        if (p_instance == NULL)
            p_instance = new ShaderCache();
        
        return *p_instance; 
    }
    
    
    ShaderCache::ShaderCache()
    {
        loadDefaultShaders();
    }
    
    
    void ShaderCache::loadDefaultShaders()
    {
#if FZ_GL_SHADERS
        GLProgram *p;
        
        /****************************************************/
        /****************** M A T R I X *********************/
        /****************************************************/

        // POSITION + ACOLOR_RGBA + TEXTURE
        p = new GLProgram(GLShader(__fz_vert_mat_aC4_TEX, GL_VERTEX_SHADER), GLShader(__fz_frag_aC4_TEX, GL_FRAGMENT_SHADER));
        p->addGenericAttributes();
        p->link();
        p->retain();
        
        m_programs[kFZShader_mat_aC4_TEX] = p;
        
        
        // POSITION + ACOLOR_RGBA
        p = new GLProgram(GLShader(__fz_vert_mat_aC4, GL_VERTEX_SHADER), GLShader(__fz_frag_aC4, GL_FRAGMENT_SHADER));
        p->addAttribute(kFZAttributeNamePosition, kFZAttribPosition);
        p->addAttribute(kFZAttributeNameColor, kFZAttribColor);
        p->link();
        p->retain();
        
        m_programs[kFZShader_mat_aC4] = p;
        
        
        // POSITION + TEXTURE
        p = new GLProgram(GLShader(__fz_vert_mat_TEX, GL_VERTEX_SHADER), GLShader(__fz_frag_TEX, GL_FRAGMENT_SHADER));
        p->addAttribute(kFZAttributeNamePosition, kFZAttribPosition);
        p->addAttribute(kFZAttributeNameTexCoord, kFZAttribTexCoords);
        p->link();
        p->retain();
        
        m_programs[kFZShader_mat_TEX] = p;
        
        
        // POSITION + UCOLOR_RGBA + TEXTURE
        p = new GLProgram(GLShader(__fz_vert_mat_uC4_TEX, GL_VERTEX_SHADER), GLShader(__fz_frag_uC4_TEX, GL_FRAGMENT_SHADER));
        p->addAttribute(kFZAttributeNamePosition, kFZAttribPosition);
        p->addAttribute(kFZAttributeNameTexCoord, kFZAttribTexCoords);
        p->link();
        p->retain();
        
        m_programs[kFZShader_mat_uC4_TEX] = p;
        
        
        // POSITION + UCOLOR_RGBA
        p = new GLProgram(GLShader(__fz_vert_mat_uC4, GL_VERTEX_SHADER), GLShader(__fz_frag_uC4, GL_FRAGMENT_SHADER));
        p->addAttribute(kFZAttributeNamePosition, kFZAttribPosition);
        p->link();
        p->retain();
        
        m_programs[kFZShader_mat_uC4] = p;
        
        
        // TEXTURE ATLAS
        p = new GLProgram(GLShader(__fz_vert_nomat_aC4_TEX, GL_VERTEX_SHADER), GLShader(__fz_frag_aC4_TEX, GL_FRAGMENT_SHADER));
        p->addGenericAttributes();
        p->link();
        p->retain();
        
        m_programs[kFZShader_nomat_aC4_TEX] = p;
                
        
        glEnableVertexAttribArray(kFZAttribPosition);
        glEnableVertexAttribArray(kFZAttribColor);
        glEnableVertexAttribArray(kFZAttribTexCoords);
#endif
    }
    
    
    GLProgram* ShaderCache::getProgramForKey(fzUInt key) const
    {
        if(key >= NUM_SHADERS)
            return NULL;
        
        return m_programs[key];
    }
}
#endif
