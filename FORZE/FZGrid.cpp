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

#include "FZGrid.h"
#include "FZTexture2D.h"
#include "FZMacros.h"
#include "FZGLState.h"

#include "FZNode.h"
#include "FZDirector.h"
#include "FZShaderCache.h"
#include "FZMath.h"


namespace FORZE {
    
#pragma mark - GridBase
    
    GridBase::GridBase(const fzGridSize& size, Texture2D *texture, bool fliped)
    : m_isActive(false)
    , m_reuseGrid(0)
    {
        setTexture(texture);
        setGridSize(size);
        
        const fzSize& texSize = texture->getContentSize();
        m_step = fzPoint(texSize.width/m_gridSize.x, texSize.height/m_gridSize.y);
    }

    
    GridBase::~GridBase()
    {
        setTexture(NULL);
        setIsActive(false);
    }
    
    
    void GridBase::setIsActive(bool a)
    {
        m_isActive = a;
        if( ! m_isActive ) {
            Director &director = Director::Instance();
            fzProjection proj = director.getProjection();
            director.setProjection(proj);
        }
    }
    
    
    void GridBase::setReuseGrid(fzInt r)
    {
        m_reuseGrid = r;
    }
    
    
    void GridBase::setGridSize(const fzGridSize& size)
    {
        m_gridSize = m_gridSize_1 = size;
        
        ++m_gridSize_1.x;
        ++m_gridSize_1.y;
    }
    
    
    void GridBase::setStep(const fzPoint& step)
    {
        m_step = step;
    }
    
    
    void GridBase::setTexture(Texture2D *texture)
    {
        FZRETAIN_TEMPLATE(texture, p_texture);
    }
    
    
    bool GridBase::getIsActive() const
    {
        return m_isActive;
    }
    
    
    fzInt GridBase::getReuseGrid() const
    {
        return m_reuseGrid;
    }
    
    
    const fzGridSize& GridBase::getGridSize() const
    {
        return m_gridSize;
    }
    
    
    const fzPoint& GridBase::getStep() const
    {
        return m_step;
    }
    
    
    Texture2D* GridBase::getTexture() const
    {
        return p_texture;
    }
    
    
#pragma mark - Grid3D
    
    Grid3D::~Grid3D()
    {
        delete [] p_vertices;
        delete [] p_originalVertices;
        delete [] p_texCoords;
    }
    
    
    const fzVec3& Grid3D::getVertex(const fzGridSize& pos) const
    {
        fzUInt index = m_gridSize_1.y * pos.x * + pos.y;
        return p_vertices[index];
    }
    
    
    void Grid3D::setVertex(const fzGridSize& pos, const fzVec3& newVertex)
    {
        fzUInt index = m_gridSize_1.y * pos.x * + pos.y;
        p_vertices[index] = newVertex;
    }
    
    
    const fzVec3& Grid3D::getOriginalVertex(const fzGridSize& pos) const
    {
        fzUInt index = m_gridSize_1.y * pos.x * + pos.y;
        return p_originalVertices[index];
    }
    
    
    void Grid3D::calculateVertexPoints()
    {
        fzFloat width = p_texture->getPixelsWide();
        fzFloat height = p_texture->getPixelsHigh();
        fzUInt len = m_gridSize_1.x * m_gridSize_1.y;
        
        p_vertices          = new fzVec3[len];
        p_originalVertices  = new fzVec3[len];
        p_texCoords         = new fzVec2[len];
        GLushort *indices   = new GLushort[m_gridSize.x * m_gridSize.y * 6];
        
        fzUInt x, y, i;
        for(x = 0; x < m_gridSize.x; ++x ) {
            for(y = 0; y < m_gridSize.y; ++y )
            {
                fzInt idx = (y * m_gridSize.x) + x;
                
                fzFloat x1 = x * m_step.x;
                fzFloat x2 = x1 + m_step.x;
                fzFloat y1 = y * m_step.y;
                fzFloat y2 = y1 + m_step.y;
                
                // Indices
                GLushort i1[4] = {
                    static_cast<GLushort>(x * m_gridSize_1.y + y),
                    static_cast<GLushort>((x+1) * m_gridSize_1.y + y),
                    static_cast<GLushort>((x+1) * m_gridSize_1.y + (y+1)),
                    static_cast<GLushort>(x * m_gridSize_1.y+ + (y+1))
                };
                
                // Vertexes
                fzVec3 l2[4] = {
                    {(float)x1, (float)y1, 0},
                    {(float)x2, (float)y1, 0},
                    {(float)x2, (float)y2, 0},
                    {(float)x1, (float)y2, 0}
                };
                
                
                GLushort tempidx[6] = { i1[0], i1[1], i1[3], i1[1], i1[2], i1[3] };
                memcpy(&indices[6*idx], tempidx, 6 * sizeof(GLushort));
                
                for( i = 0; i < 4; ++i ) {
                    
                    p_vertices[i1[i]] = l2[i];
                    p_texCoords[i1[i]] = fzVec2(l2[i].x / width, l2[i].y / height);
                }
            }
        }
        
        memcpy(p_originalVertices, p_vertices, len * sizeof(fzVec3));
        
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * m_gridSize.x * m_gridSize.y * 6, indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        delete [] indices;
    }
    
    
    void Grid3D::blit()
    {    
        // OpenGL settings
        fzGLSetMode(kFZGLMode_Texture);
        
#if FZ_GL_SHADERS
        // Attributes
        glVertexAttribPointer(kFZAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, p_vertices);
        glVertexAttribPointer(kFZAttribTexCoords, 2, GL_FLOAT, GL_FALSE, 0, p_texCoords);
#else
        // Attributes
        glVertexPointer(3, GL_FLOAT, 0, p_vertices);
        glTexCoordPointer(2, GL_FLOAT, 0, p_texCoords);
#endif
        
        
        // Rendering
        GLsizei n = m_gridSize.x * m_gridSize.y * 6;
        glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_SHORT, 0);		
    }
    
    
    void Grid3D::reuse()
    {
        if ( m_reuseGrid > 0 ) {
            memcpy(p_originalVertices, p_vertices, m_gridSize_1.x * m_gridSize_1.y * sizeof(fzVec3));
            m_reuseGrid--;
        }
    }
    
        
#pragma mark - TiledGrid3D
    
    GridTiled3D::~GridTiled3D()
    {
        delete [] p_vertices;
        delete [] p_originalVertices;
        delete [] p_texCoords;
    }
    
    
    const fzQuad3& GridTiled3D::getTile(const fzGridSize& pos) const
    {
        fzUInt index = m_gridSize.y * pos.x + pos.y;
        return p_vertices[index];
    }
    
    
    const fzQuad3& GridTiled3D::getOriginalTile(const fzGridSize& pos) const
    {
        fzUInt index = m_gridSize.y * pos.x + pos.y;
        return p_originalVertices[index];
    }
    
    
    void GridTiled3D::setTile(const fzGridSize& pos, const fzQuad3& newQuad)
    {
        fzUInt index = m_gridSize.y * pos.x + pos.y;
        p_vertices[index] = newQuad;
    }
    
    
    void GridTiled3D::calculateVertexPoints()
    {
        fzFloat width = static_cast<fzFloat>(p_texture->getPixelsWide());
        fzFloat height = static_cast<fzFloat>(p_texture->getPixelsHigh());
        
        fzUInt numQuads = m_gridSize.x * m_gridSize.y;
        
        p_vertices          = new fzQuad3[numQuads];
        p_originalVertices  = new fzQuad3[numQuads];
        p_texCoords         = new fzQuad2[numQuads];
        GLushort *indices   = new GLushort[numQuads * 6];
        
        float *vertArray = (float*)p_vertices;
        float *texArray = (float*)p_texCoords;
        GLushort *idxArray = (GLushort *)indices;
        
        fzUInt x = 0, y;
        for(; x < m_gridSize.x; ++x )
        {
            for( y = 0; y < m_gridSize.y; ++y )
            {
                fzFloat x1(x * m_step.x);
                fzFloat x2(x1 + m_step.x);
                fzFloat y1(y * m_step.y);
                fzFloat y2(y1 + m_step.y);
                
                *vertArray = x1;
                *(++vertArray) = y1;
                *(++vertArray) = 0;
                *(++vertArray) = x2;
                *(++vertArray) = y1;
                *(++vertArray) = 0;
                *(++vertArray) = x1;
                *(++vertArray) = y2;
                *(++vertArray) = 0;
                *(++vertArray) = x2;
                *(++vertArray) = y2;
                *(++vertArray) = 0;

                
                *texArray   = x1 / width;
                *(++texArray) = y1 / height;
                *(++texArray) = x2 / width;
                *(++texArray) = y1 / height;
                *(++texArray) = x1 / width;
                *(++texArray) = y2 / height;
                *(++texArray) = x2 / width;
                *(++texArray) = y2 / height;
            }
        }
        
        for( x = 0; x < numQuads; ++x)
        {
            const GLushort x4 = x*4;
            const GLushort x6 = x*6;
            
            idxArray[x6+0] = x4+0;
            idxArray[x6+1] = x4+1;
            idxArray[x6+2] = x4+2;
            
            idxArray[x6+3] = x4+1;
            idxArray[x6+4] = x4+2;
            idxArray[x6+5] = x4+3;
        }
        
        memcpy(p_originalVertices, p_vertices, numQuads * sizeof(fzQuad3));
        
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesVBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numQuads * 6, indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        delete [] indices;
    }
    
    
    void GridTiled3D::blit()
    {
        // OpenGL config
        fzGLSetMode(kFZGLMode_Texture);
        
#if FZ_GL_SHADERS        
        // Attributes
        glVertexAttribPointer(kFZAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, p_vertices);
        glVertexAttribPointer(kFZAttribTexCoords, 2, GL_FLOAT, GL_FALSE, 0, p_texCoords);
#else
        
#endif
        
        // Rendering
        fzUInt n = m_gridSize.x * m_gridSize.y * 6;
        glDrawElements(GL_TRIANGLES, (GLsizei) n, GL_UNSIGNED_SHORT, 0);
    }
    
    
    void GridTiled3D::reuse()
    {
        if ( m_reuseGrid > 0 ) {
            memcpy(p_originalVertices, p_vertices, m_gridSize.x * m_gridSize.y * sizeof(fzQuad3));
            m_reuseGrid--;
        }
    }
}
