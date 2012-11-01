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

#include "FZTextureAtlas.h"
#include "FZGLState.h"
#include "FZGLProgram.h"
#include "FZMacros.h"
#include "FZTexture2D.h"


namespace FORZE {
    
    TextureAtlas::TextureAtlas(Texture2D *texture, fzUInt capacity)
    : m_capacity(capacity)
    , m_count(0)
    , m_indicesVBO(0)
    , p_quads(NULL)
    , p_texture(NULL)
#if FZ_VBO_STREAMING
    , m_quadsVBO(0)
    , m_dirtyMin((fzV4_T2_C4_Quad*)INTPTR_MAX)
    , m_dirtyMax(0)
#endif
    {
        setTexture(texture);
        
        glGenBuffers(1, &m_indicesVBO);
#if FZ_VBO_STREAMING
        glGenBuffers(1, &m_quadsVBO);
#endif
        
        if(m_capacity > 0) {
            p_quads = new fzV4_T2_C4_Quad[m_capacity];
            generateIndices();
        }
    }
    
    
    TextureAtlas::~TextureAtlas()
    {
        delete [] p_quads;
        setTexture(NULL);

        glDeleteBuffers(1, &m_indicesVBO);

#if FZ_VBO_STREAMING
        glDeleteBuffers(1, &m_quadsVBO);
#endif
    }
    
    
    void TextureAtlas::generateIndices()
    {        
        GLushort *indices = new GLushort[m_capacity * 6];

        fzUInt i = 0;
        for(; i < m_capacity; ++i) {
            
            const fzUInt i6 = i*6;
            const GLushort i4 = i*4;
#if FZ_TEXTURE_ATLAS_USE_TRIANGLE_STRIP
            indices[i6] = i4;
            indices[i6+1] = i4+0;
            indices[i6+2] = i4+2;		
            indices[i6+3] = i4+1;
            indices[i6+4] = i4+3;
            indices[i6+5] = i4+3;
#else
            indices[i6] = i4;
            indices[i6+1] = i4+1;
            indices[i6+2] = i4+2;            
            indices[i6+3] = i4+3;
            indices[i6+4] = i4+2;
            indices[i6+5] = i4+1;
#endif	
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_capacity * 6, indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        delete [] indices;
        
        initVAO();
    }


    void TextureAtlas::setTexture(Texture2D *texture)
    {
        FZRETAIN_TEMPLATE(texture, p_texture);
    }
    
    
    Texture2D* TextureAtlas::getTexture() const
    {
        return p_texture;
    }
    
    
    void TextureAtlas::initVAO()
    {
        /*
        glGenVertexArraysAPPLE(1, &m_VAO);
        glBindVertexArrayAPPLE(m_VAO);
        
        // bind index
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO);
        
        glVertexAttribPointer(kFZAttribColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(_fzV4_T2_C4), (GLvoid*) offset);
        glVertexAttribPointer(kFZAttribTexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(_fzV4_T2_C4), (GLvoid*) (offset += sizeof(fzColor4B)));
        glVertexAttribPointer(kFZAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(_fzV4_T2_C4), (GLvoid*) (offset += sizeof(fzVec2)));
        
        
        glBindVertexArrayAPPLE(0);
        */
#if FZ_VBO_STREAMING        
        glBindBuffer(GL_ARRAY_BUFFER, m_quadsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(fzV4_T2_C4_Quad) * m_capacity, p_quads, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
    }
    
    
#pragma mark TextureAtlas - Resize
    
    bool TextureAtlas::resizeCapacity(fzUInt newCapacity)
    {
        FZ_ASSERT(newCapacity > m_count, "Capacity cannot be reduced.");
        if(p_quads)
            delete [] p_quads;

        p_quads = new fzV4_T2_C4_Quad[newCapacity];
        m_capacity = newCapacity;

        generateIndices(); 
        
        return true;
    }
    
    
#pragma mark -
    
    void TextureAtlas::reserveCapacity(fzUInt count)
    {
        if(count > m_capacity)
            resizeCapacity(count+1);
    }
    
    
    void TextureAtlas::setLastQuad(fzV4_T2_C4_Quad *quad)
    {
        FZ_ASSERT(quad >= p_quads && quad <= &p_quads[m_capacity], "Quad pointer is out of buffer.");
        m_count = quad - p_quads;
    }
    
    
#pragma mark TextureAtlas - Drawing
    
    void TextureAtlas::drawQuads()
    {       
        // Opengl config
        fzGLSetMode(kFZGLMode_Texture);
        p_texture->bind();

        intptr_t offset = 0;
        
        // atributes
#if FZ_GL_SHADERS        
        glVertexAttribPointer(kFZAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(_fzV4_T2_C4), &p_quads->bl.vertex - offset);
        glVertexAttribPointer(kFZAttribTexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(_fzV4_T2_C4), &p_quads->bl.texCoord - offset);
        glVertexAttribPointer(kFZAttribColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(_fzV4_T2_C4), &p_quads->bl.color - offset);
        
#else
        glLoadIdentity();
        
        glVertexPointer(3, GL_FLOAT, sizeof(_fzV4_T2_C4), &p_quads->bl.vertex - offset);
        glTexCoordPointer(2, GL_FLOAT, sizeof(_fzV4_T2_C4), &p_quads->bl.texCoord - offset);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(_fzV4_T2_C4), &p_quads->bl.color - offset);
        
#endif      

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO);
        glDrawElements(FZ_TRIANGLE_MODE, (GLsizei)m_count * 6, GL_UNSIGNED_SHORT, 0 );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        CHECK_GL_ERROR_DEBUG();
    }
}
