/*
 * FORZE ENGINE: http://forzefield.com
 *
 * Copyright (c) 2011-2012 FORZEFIELD Studios S.L.
 * Copyright (c) 2012 Manuel Martínez-Almeida
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

#include "FZHUD.h"
#include "FZSprite.h"
#include "FZLabel.h"
#include "FZCommon.h"
#include "FZGLState.h"
#include "FZDirector.h"
#include "FZPrimitives.h"
#include "FZLayer.h"
#include "FZFontCache.h"
#include "FZTextureCache.h"
#include "FZTexture2D.h"
#define FPS_MAX 60.0f
#define FPS_SAMPLES 50


namespace FORZE {
    
    static const fzRect _fpsGraph(15, 8, 156, 39);
    static const fzFloat _deltaX = _fpsGraph.size.width / FPS_SAMPLES;
    static const fzFloat _deltaY = _fpsGraph.size.height / FPS_MAX;
    
    HUD::HUD()
    : m_count(0)
    , m_index(0)
    , m_vertices(NULL)
    , p_FPSLabel(NULL)
    {
        setIsRelativeAnchorPoint(true);
        setAnchorPoint(1, 0);

        // FPS label
        Font *font = FontCache::Instance().addFont("FPSFont.fnt", 0);
        if(font) {
            p_FPSLabel = new Label();
            p_FPSLabel->setFont(font);
            p_FPSLabel->setAnchorPoint(1, 0);
            addChild(p_FPSLabel);
        }
        
        onEnter();
        updateLayout();
    }
    
    
    HUD::~HUD()
    {
        if(m_vertices)
        delete [] m_vertices;
    }
    
    
    void HUD::updateLayout()
    {
        // HACK: HUD doesn't use the same projection that the rest of code.
        const fzSize& size = Director::Instance().getRenderingRect().size;
        setPosition(size.width-5, 5);
                
        bool background = size.width > 500 && size.height > 500;
        
        if(background) {
            Texture2D *texture = TextureCache::Instance().addImage("HUD.pvr");
            if(texture == NULL)
                background = false;
        }
        
        
        if(background) {
            
            if(m_vertices == NULL) {
                
                // alloc samples
                m_vertices = new fzVec2[FPS_SAMPLES];
                
                // Background
                Sprite *sprite = new Sprite("HUD.pvr");
                sprite->setIsRelativeAnchorPoint(false);
                sprite->setAnchorPoint(FZPointZero);
                sprite->getTexture()->setAliasTexParameters();
                sprite->setTag(10);
                addChild(sprite, -1);
            }
            
            Node *sprite = getChildByTag(10);
            setContentSize(sprite->getContentSize());
            
        }else{
            
            if(m_vertices != NULL) {
                delete [] m_vertices;
                m_vertices = NULL;
                m_count = 0;
                m_index = 0;
                removeChildByTag(10);
            }
            
            setContentSize(100, 30);
        }
        
        if(p_FPSLabel)
        p_FPSLabel->setPosition(m_contentSize.width-14, -8);
        
        Node::updateLayout();
    }
  
    
    void HUD::setFrame(fzFloat fps)
    {
        if(p_FPSLabel)
        p_FPSLabel->setString(FZT("%2.1f", fps));
        
        if(m_vertices == NULL)
            return;
        
        // add sample
        if(m_index >= FPS_SAMPLES)
            m_index = 0;
                
        m_vertices[m_index].x = _fpsGraph.origin.x + _deltaX * m_index;
        m_vertices[m_index].y = _fpsGraph.origin.y + _deltaY * fps;
        ++m_index;
        
        if(m_count < FPS_SAMPLES)
            ++m_count;
    }
    
    
    void HUD::draw()
    {
        if(m_count <= 1)
            return;
        
        // RENDERING GRAPH
        fzGLColor(fzColor3B(20, 20, 20));
        fzDrawPoly(m_vertices, m_count, false);

        // RENDERING RED GUIDE
        const fzPoint& lastVertex = m_vertices[m_index-1];
        fzVec2 lines[4] = {
            fzVec2(lastVertex.x, _fpsGraph.origin.y + _fpsGraph.size.height), fzVec2(lastVertex.x, _fpsGraph.origin.y),
            fzVec2(lastVertex.x-5, lastVertex.y), fzVec2(lastVertex.x+5, lastVertex.y)
        };
        
        fzGLColor(fzRED);
        fzDrawLines(lines, 4);        
    }
    
    void HUD::updateStuff()
    {
        // UPDATE TRANSFORM
        if( m_dirtyFlags & kFZDirty_transform_absolute ) {
            fzMat4 projection;
            
            fzSize displaySize = Director::Instance().getRenderingRect().size;
            fzMath_mat4OrthoProjection(0, displaySize.width, 0, displaySize.height, -1024, 1024, projection);
            fzMath_mat4Multiply(projection, getNodeToParentTransform(), m_transformMV);
        }

    }

    
    bool HUD::needsFPS() const
    {
        return true;
    }
    
    
    bool HUD::needsMemory() const
    {
        return false;
    }
}
