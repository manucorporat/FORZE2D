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

#include "FZTMXTiledMap.h"
#include "FZMacros.h"
#include "FZTMXParser.h"
#include "FZTMXLayer.h"
#include "FZTextureCache.h"
#include "FZTexture2D.h"


using namespace STD;

namespace FORZE {
    
    TMXTiledMap::TMXTiledMap(const char *tmxfilename)
    : SpriteBatch(NULL, 0)
    , m_mapInfo()
    {
        // PARSE TMX FILE
        try {
            m_mapInfo.parseTMXFile(tmxfilename);
            
        } catch(std::exception &error) {
            FZLOGERROR("%s", error.what());
            return;
        }
        
        // LOAD TILESET
        const char *textureFilename = m_mapInfo.getTileset().getFilename().c_str();
        Texture2D *texture = TextureCache::Instance().addImage(textureFilename);
        
        setTexture(texture);
        m_mapInfo.getTileset().m_textureSize = texture->getContentSize();
        
        
        // LOAD LAYERS
        fzFloat maxWidth = 0, maxHeight = 0;
        vector<TMXLayerInfo>::iterator it(m_mapInfo.m_layers.begin());
        for(; it != m_mapInfo.m_layers.end(); ++it)
        {
            TMXLayerInfo *info = (TMXLayerInfo*) &(*it);
            TMXLayer *layer = new TMXLayer(this, info);
            addChild(layer);
            
            maxWidth = fzMax(maxWidth, layer->getContentSize().width);
            maxHeight = fzMax(maxHeight, layer->getContentSize().height);
        }
        setContentSize(maxWidth, maxHeight);
    }
    

    void TMXTiledMap::insertChild(Node* node)
    {
        FZ_ASSERT( dynamic_cast<TMXLayer*>(node), "SpriteBatch's children must be Sprites.");
        Node::insertChild(node);
    }

    
    void TMXTiledMap::render(unsigned char dirtyFlags)
    {        
        // RESERVE MEMORY
        TMXLayer *layer;
        fzUInt totalSize = 0;
        FZ_LIST_FOREACH(m_children, layer) {
            totalSize += layer->getChildren().size();
        }
        
        m_textureAtlas.reserveCapacity(totalSize);
        
        // RENDERING
        fzV4_T2_C4_Quad *quad = m_textureAtlas.getQuads();
        FZ_LIST_FOREACH(m_children, layer) {
            layer->makeDirty(dirtyFlags);
            layer->visitTMXLayer(&quad);
        }
        
        m_textureAtlas.setLastQuad(quad);
        
        draw();        
    }
}
