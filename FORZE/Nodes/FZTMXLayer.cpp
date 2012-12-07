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

#include "FZTMXLayer.h"
#include "FZMacros.h"
#include "FZMS.h"
#include "FZSprite.h"
#include "FZBitOrder.h"
#include "FZTMXTiledMap.h"


using namespace STD;

using namespace FORZE;
namespace FORZE {
    
    TMXLayer::TMXLayer(TMXTiledMap *tiledMap, TMXLayerInfo *layerInfo)
    : m_minGID(INT_MAX)
    , m_maxGID(0)
    , m_orientation(tiledMap->m_mapInfo.getOrientation())
    , m_mapTileSize(tiledMap->m_mapInfo.getTileSize())
    , p_tiles(layerInfo->getTiles())
    , m_layerSize(layerInfo->getSize())
    , vertexZvalue_(0)
    , p_batch(tiledMap)
    {     
        setTag(layerInfo->getHashName());
        
        fzPoint offset = calculateLayerOffset(layerInfo->getOffset());
        setPosition(offset);
        setContentSize(fzSize(m_layerSize.width * m_mapTileSize.width, m_layerSize.height * m_mapTileSize.height));
        
        setOpacity(layerInfo->getOpacity());
        if(layerInfo->getOpacity() == 0)
        {
            setIsVisible(false);
        }
        
        setupTiles();
    }
    
    
    TMXLayer::~TMXLayer()
    {
        if(p_tiles)
            free( p_tiles);
    }
    
    
    void TMXLayer::insertChild(Node *node)
    {
        FZ_ASSERT( dynamic_cast<Sprite*>(node), "SpriteBatch's children must be Sprites");
        
        Sprite *sprite = static_cast<Sprite*>(node);
        
        Node::insertChild(sprite);
        sprite->useBatchRender(p_batch);
    }
    
    
    const TMXTilesetInfo* TMXLayer::getTileset() const
    {
        return &p_batch->m_mapInfo.getTileset();
    }
    
    
    void TMXLayer::appendTileForGID(uint32_t GID, const fzPoint& coord)
    {
        fzInt idx = coord.x + coord.y * m_layerSize.width;
        fzRect rect = getTileset()->rectForGID(GID);
        
        Sprite *newTile = new Sprite(rect);
        addChild(newTile, idx);

        newTile->setTag(idx);
        newTile->setAnchorPoint(FZPointZero);
        newTile->setPosition(positionAt(coord));

        newTile->setFlipX((GID & kFlippedHorizontallyFlag));
        newTile->setFlipY((GID & kFlippedVerticallyFlag));
    }

    
    void TMXLayer::setupTiles()
    {
        for( fzUInt y = 0; y < m_layerSize.height; ++y ) {
            for( fzUInt x = 0; x < m_layerSize.width; ++x ) {
                
                fzUInt idx = x + y * m_layerSize.width;
                uint32_t GID = fzBitOrder_int32LittleToHost(p_tiles[idx]);
                
                // XXX: gid == 0 --> empty tile
                if( GID != 0 ) {
                    appendTileForGID(GID, fzPoint(x, y));
                    
                    // Optimization: update min and max GID rendered by the layer
                    m_minGID = fzMin(GID, m_minGID);
                    m_maxGID = fzMax(GID, m_maxGID);
                }
            }
        }
        
        //FZ_ASSERT(m_maxGID >= getTileset()->getFirstGID() &&
         //         m_minGID >= getTileset()->getFirstGID(), "TMX: Only 1 tilset per layer is supported");
    }
    
    
    void TMXLayer::releaseMap()
    {
        if( p_tiles) {
            free( p_tiles);
            p_tiles = NULL;
        }
    }
    
    
    Sprite* TMXLayer::tileAt(const fzPoint& coord)
    {
        FZ_ASSERT( coord.x < m_layerSize.width && coord.y < m_layerSize.height && coord.x >=0 && coord.y >=0, "TMXLayer: invalid position");
        FZ_ASSERT( p_tiles, "TMXLayer: the tiles map has been released");
        
        Sprite *tile = NULL;
        uint32_t gid = tileGIDAt(coord);
        
        // if GID == 0, then no tile is present
        if( gid ) {
            fzInt idx = coord.x + coord.y * m_layerSize.width;
            tile = static_cast<Sprite*>( getChildByTag(idx) );
            
            // tile not created yet. create it
            if( ! tile ) {
                const fzRect& rect = getTileset()->rectForGID(gid);
                
                tile = new Sprite(rect);
                tile->setTag(idx);
                tile->setAnchorPoint(FZPointZero);
                tile->setPosition(positionAt(coord));
                //tile->setVertexZ(vertexZAt(coord));

                addChild(tile, idx);
            }
        }
        return tile;
    }
    
    
    uint32_t TMXLayer::tileGIDAt(const fzPoint& coord, bool flags) const
    {
        FZ_ASSERT( coord.x < m_layerSize.width && coord.y < m_layerSize.height && coord.x >=0 && coord.y >=0, "TMXLayer: invalid position");
        FZ_ASSERT( p_tiles, "TMXLayer: the tiles map has been released");
        
        fzInt idx = coord.x + coord.y * m_layerSize.width;
        return (flags) ? p_tiles[ idx ] : (p_tiles[ idx ] & kFlippedMask);
    }
    
    
    void TMXLayer::setTileGID(uint32_t GID, const fzPoint& coord, bool flags)
    {
        FZ_ASSERT( coord.x < m_layerSize.width && coord.y < m_layerSize.height && coord.x >=0 && coord.y >=0, "TMXLayer: invalid position");
        FZ_ASSERT( p_tiles, "TMXLayer: the tiles map has been released");
        FZ_ASSERT( GID == 0 || GID >= getTileset()->getFirstGID(), "TMXLayer: invalid gid" );
        
        uint32_t currentGID = tileGIDAt(coord, flags);
        
        if (currentGID != GID) 
        {
            // setting gid=0 is equal to remove the tile
            if( GID == 0 )
                removeTileAt(coord);
            
            // empty tile. create a new one
            else if( currentGID == 0 )
                appendTileForGID(GID, coord);
            
            // modifying an existing tile with a non-empty tile
            else {
                
                fzInt idx = coord.x + coord.y * m_layerSize.width;
                Sprite *sprite = static_cast<Sprite*>(getChildByTag(idx));
                
                FZ_ASSERT(sprite, "Sprite was not found");
                
                const fzRect& rect = getTileset()->rectForGID(GID);
                sprite->setTextureRect(rect);
            }
        }
    }
    
    
    void TMXLayer::removeTileAt(const fzPoint& coord)
    {
        FZ_ASSERT( coord.x < m_layerSize.width && coord.y < m_layerSize.height && coord.x >=0 && coord.y >=0, "TMXLayer: invalid position");
        FZ_ASSERT( p_tiles, "TMXLayer: the tiles map has been released");
        
        fzInt idx = coord.x + coord.y * m_layerSize.width;
        uint32_t GID = p_tiles[idx];
        
        if( GID != 0 ) {
            // remove tile from GID map
            p_tiles[idx] = 0;
            
            // remove it from sprites and/or texture atlas
            removeChildByTag(idx);
        }
    }
    
    
    fzPoint TMXLayer::positionAt(const fzPoint& coord) const
    {
        switch( m_orientation ) {
            case kFZTMXOrientationOrtho:
                
                return fzPoint(coord.x * m_mapTileSize.width,
                               (m_layerSize.height - coord.y - 1) * m_mapTileSize.height);

                break;
            case kFZTMXOrientationIso:
                
                
                return fzPoint(m_mapTileSize.width /2 * ( m_layerSize.width + coord.x - coord.y - 1),
                               m_mapTileSize.height /2 * (( m_layerSize.height * 2 - coord.x - coord.y) - 2));
                break;
                
            case kFZTMXOrientationHex:
            {
                fzFloat diffY = 0;
                if( (int)coord.x % 2 == 1 )
                    diffY = -m_mapTileSize.height/2 ;
                
                return fzPoint(coord.x * m_mapTileSize.width * 3/4,
                               (m_layerSize.height - coord.y - 1) * m_mapTileSize.height + diffY);
                
                break;
            }
            default:
                FZ_ASSERT(false, "Invalid map orientation");
                break;
        }
        return FZPointZero;
    }
    
    fzFloat TMXLayer::vertexZAt(const fzPoint& coord) const
    {
        fzFloat ret = 0;
        fzFloat maxVal = 0;
        
        if( useAutomaticVertexZ_ ) {
            switch( m_orientation ) {
                case kFZTMXOrientationIso:
                    maxVal = m_layerSize.width + m_layerSize.height;
                    ret = coord.x + coord.y - maxVal;
                    break;
                case kFZTMXOrientationOrtho:
                    ret = coord.y - m_layerSize.height;
                    break;
                case kFZTMXOrientationHex:
                    FZ_ASSERT(false, "TMX Hexa zOrder not supported");
                    break;
                default:
                    FZ_ASSERT(false, "TMX invalid value");
                    break;
            }
        } else
            ret = vertexZvalue_;
            
            return ret;
    }
    
    
    fzPoint TMXLayer::calculateLayerOffset(const fzPoint& coord) const
    {
        switch( m_orientation ) {
            case kFZTMXOrientationOrtho:
                
                return fzPoint(coord.x * m_mapTileSize.width,
                               -coord.y * m_mapTileSize.height);
                break;
            case kFZTMXOrientationIso:
                
                return fzPoint((m_mapTileSize.width /2) * (coord.x - coord.y),
                               (m_mapTileSize.height /2 ) * (-coord.x - coord.y));
                
                break;
            case kFZTMXOrientationHex:
                FZ_ASSERT(coord == FZPointZero, "Offset calculation is not implemented for hexagonal maps");
                break;
        }
        return FZPointZero;
    }
    
    
    char* TMXLayer::propertyNamed(const char* name) const
    {
        return NULL;
    }
    
    
    void TMXLayer::visitTMXLayer(fzV4_T2_C4_Quad **quadp)
    {
        if (!m_isVisible)
            return;
        
        unsigned char dirtyFlags = m_dirtyFlags & kFZDirty_recursive;
        
        updateStuff();
        MS::pushMatrix(m_transformMV);
        
        Sprite *child;
        FZ_LIST_FOREACH(m_children, child)
        {
            child->makeDirty(dirtyFlags);
            child->updateTransform(quadp);
        }
        MS::pop();
    }
    
    void TMXLayer::render(unsigned char dirtyFlags)
    {
        FZ_ASSERT(false, "You can not render a TMXLayer manually.");
    }
}
