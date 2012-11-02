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

#include "FZTMXParser.h"
#include "FZMacros.h"
#include "FZResourcesManager.h"
#include "FZData.h"
#include "rapidxml.hpp"


using namespace rapidxml;
using namespace STD;

namespace FORZE {

    TMXMapInfo::TMXMapInfo()
    : m_tileset()
    { }
    
    TMXMapInfo::TMXMapInfo(const char* filename)
    : TMXMapInfo()
    {
        parseTMXFile(filename);
    }
    
    
    void TMXMapInfo::parseTMXFile(const char* filename)
    {
        fzBuffer data = ResourcesManager::Instance().loadResource(filename, &m_factor);
        if(data.isEmpty())
            FZ_RAISE("TMXParser: Impossible to load file.");
        
        try {
            parseTMXData(data.getPointer());
            data.free();
            
        } catch(std::exception& error) {
            data.free();
            throw;
        }
    }
    
    void TMXMapInfo::parseTMXData(char* data)
    {
        xml_document<> doc;
        doc.parse<parse_fastest | parse_trim_whitespace>(data);
        
        xml_attribute<> *attribute;
        xml_node<> *rootNode, *subNode;        
        
        // MAP ROOT
        rootNode = doc.first_node("map");
        if(rootNode == NULL)
            FZ_RAISE_STOP("TMXParser: <map> tag wasn't found.");
        
        
        // VERSION
        attribute = rootNode->first_attribute("version");
        if(attribute == NULL || atoi(attribute->value()) != 1)
            FZLOGERROR("TMXParser: <map version> attribute invalid.");
        
        
        // ORIENTATION
        attribute = rootNode->first_attribute("orientation");
        if(attribute == NULL)
            FZ_RAISE_STOP("TMXParser: <map orientation> attribute wasn't found.");
        
        
        const char* orientation = attribute->value();
        if(strncmp(orientation, "orthogonal", attribute->value_size()) == 0)
            m_orientation = kFZTMXOrientationOrtho;
        else if(strncmp(orientation, "isometric", attribute->value_size()) == 0)
            m_orientation = kFZTMXOrientationIso;
        else if(strncmp(orientation, "hexagonal", attribute->value_size()) == 0)
            m_orientation = kFZTMXOrientationHex;
        else {
            m_orientation = kFZTMXOrientationOrtho; // default value
            FZ_RAISE_STOP("TMXParser: Invalid orientation.");
        }        
        
        // MAP WIDTH
        attribute = rootNode->first_attribute("width");
        if(attribute == NULL)
            FZ_RAISE_STOP("TMXParser: <map width attribute wasn't found.");
            
        m_mapSize.width = atoi(attribute->value());
        
        // MAP HEIGHT
        attribute = rootNode->first_attribute("height");
        if(attribute == NULL)
            FZ_RAISE_STOP("TMXParser: <map height> attribute wasn't found.");
            
        m_mapSize.height = atoi(attribute->value());
        
        // TILE WIDTH
        attribute = rootNode->first_attribute("tilewidth");
        if(attribute == NULL)
            FZ_RAISE_STOP("TMXParser: <map tilewidth> attribute wasn't found.");
            
        m_tileSize.width = atof(attribute->value()) / m_factor;
        
        // TILE HEIGHT
        attribute = rootNode->first_attribute("tileheight");
        if(attribute == NULL)
            FZ_RAISE_STOP("TMXParser: <map tileheight> attribute wasn't found.");
        
        m_tileSize.height = atof(attribute->value()) / m_factor;
        
        
        // REST
        for(subNode = rootNode->first_node(); subNode; subNode = subNode->next_sibling())
        {
            if(strncmp(subNode->name(), "layer", subNode->name_size()) == 0) {
                
                TMXLayerInfo layer = TMXLayerInfo();
                if(parseLayer(subNode, layer)) 
                    m_layers.push_back(layer);
                
                
                
            }else if(strncmp(subNode->name(), "tileset", subNode->name_size()) == 0) {
                
                parseTileset(subNode, m_tileset);
                
            }else if(strncmp(subNode->name(), "objectgroup", subNode->name_size()) == 0) {
                
                FZLog("TMX Objects are not implemented");

            }
        }
        
        return true;
    }
    
    
    bool TMXMapInfo::parseLayer(void* outputData, TMXLayerInfo& info)
    {
        xml_node<> *rootNode = static_cast<xml_node<>*>(outputData);
        xml_attribute<> *attribute;
        
        // name
        attribute = rootNode->first_attribute("name");
        info.m_nameHash = (attribute) ? fzHash(attribute->value(), attribute->value_size()) : -1;
        
        // x coord
        attribute = rootNode->first_attribute("x");
        info.m_offset.x = (attribute) ? (atof(attribute->value()) / m_factor) : 0;
        
        // y coord
        attribute = rootNode->first_attribute("y");
        info.m_offset.y = (attribute) ? (atof(attribute->value()) / m_factor) : 0;
        
        // opacity
        attribute = rootNode->first_attribute("opacity");
        info.m_opacity = (attribute) ? (atof(attribute->value()) / m_factor) : 1;
        
        // width
        attribute = rootNode->first_attribute("width");
        if(attribute == NULL) {
            FZLOGERROR("TMXParser: <layer width> attribute wasn't found.");
            return false;
        }
        info.m_size.width = atoi(attribute->value());
        
        // height
        attribute = rootNode->first_attribute("height");
        if(attribute == NULL) {
            FZLOGERROR("TMXParser: <layer height> attribute wasn't found.");
            return false;
        }
        info.m_size.height = atoi(attribute->value());
        
        
        // GET RAW DATA
        xml_node<> *dataNode = rootNode->first_node("data");
        if(dataNode == NULL) {
            FZLOGERROR("TMXParser: <data> tag wasn't found.");
            return false;
        }
        
        //fzUInt decodedLength;
        fzBuffer buffer2;
        fzBuffer buffer1 = fzBuffer(dataNode->value(), dataNode->value_size());
        if(buffer1.getPointer() == NULL || buffer1.getLength() == 0) {
            FZLOGERROR("TMXParser: Layer data is missing.");
            return false;
        }
        
        
        // DECODE RAW DATA
        attribute = dataNode->first_attribute("encoding");
        if(attribute != NULL && strncmp(attribute->value(), "base64", attribute->value_size()) == 0) {
            
            buffer2 = Data::B64Decode(buffer1.getPointer(), buffer1.getLength());

        }else {
            
            FZLOGERROR("TMXParser: Layer data encoding is not supported.");
            return false;
        }
        
        
        // UNCOMPRESS DECODED DATA
        attribute = dataNode->first_attribute("compression");
        if(attribute != NULL &&
           (strncmp(attribute->value(), "gzip", attribute->value_size()) == 0 ||
            strncmp(attribute->value(), "zlib", attribute->value_size()) == 0))
        {
            fzUInt expectedSize = info.m_size.width * info.m_size.height * sizeof(uint32_t);
            buffer1 = Data::inflateZIPWithHint((unsigned char*)buffer2.getPointer(), buffer2.getLength(), expectedSize);
            buffer2.free();
            
            
            
            if(expectedSize != buffer1.getLength()) {
                buffer1.free();
                FZLOGERROR("TMXParser: TMX data looks corrupted, hint failed while inflating data.");
                return false;
            }
            
        }else
            buffer1 = buffer2;
        
        info.p_tiles = reinterpret_cast<uint32_t*>(buffer1.getPointer());
        return true;
    }


    bool TMXMapInfo::parseTileset(void* outputData, TMXTilesetInfo& info)
    {
        xml_node<> *rootNode = static_cast<xml_node<>*>(outputData);
        xml_attribute<> *attribute;
        
        attribute = rootNode->first_attribute("firstgid");
        if(attribute == NULL) {
            FZLOGERROR("TMXParser: <tileset firstgid> attribute wasn't found.");
            return false;
        }
        info.m_firstGID = atoi(attribute->value());
        
        
        attribute = rootNode->first_attribute("source");

        if(attribute) {
            
            
        } else {
            attribute = rootNode->first_attribute("name");
            if(attribute != NULL)
                info.m_name = string(attribute->value(), attribute->value_size());            
            
            attribute = rootNode->first_attribute("tilewidth");
            if(attribute == NULL) {
                FZLOGERROR("TMXParser: <tileset tilewidth> attribute wasn't found.");
                return false;
            }
            info.m_tileSize.width = atof(attribute->value()) / m_factor;
            
            
            attribute = rootNode->first_attribute("tileheight");
            if(attribute == NULL) {
                FZLOGERROR("TMXParser: <tileset tileheight> attribute wasn't found.");
                return false;
            }
            info.m_tileSize.height = atof(attribute->value()) / m_factor;
            
            
            attribute = rootNode->first_attribute("spacing");
            if(attribute != NULL)
                info.m_spacing = atof(attribute->value()) / m_factor;
            
            
            attribute = rootNode->first_attribute("margin");
            if(attribute != NULL)
                info.m_margin = atof(attribute->value()) / m_factor;
            
            {
                xml_node<> *node = rootNode->first_node("image");
                if(node == NULL) {
                    FZLOGERROR("TMXParser: <image> tag wasn't found.");
                    return false;
                }
                
                attribute = node->first_attribute("source");
                if(attribute == NULL) {
                    FZLOGERROR("TMXParser: <image source> attribute wasn't found.");
                    return false;
                }
                info.m_filename = string(attribute->value(), attribute->value_size());
            }
        }
        return true;
    }
    
    
    bool TMXMapInfo::parseObjectGroup(void* outputData, TMXObjectGroup& info)
    {
        xml_node<> *rootNode = static_cast<xml_node<>*>(outputData);
        xml_attribute<> *attribute;
        
        attribute = rootNode->first_attribute("name");
        if(attribute == NULL) {
            FZLOGERROR("TMXParser: <tileset name> attribute wasn't found.");
            return false;
        }
        info.p_name = fzStrcpy(attribute->value());
        
        
        attribute = rootNode->first_attribute("x");
        if(attribute != NULL)
            info.m_offset.x = atoi(attribute->value());
        
        
        attribute = rootNode->first_attribute("y");
        if(attribute == NULL)
            info.m_offset.y = atoi(attribute->value());
    }
    
    
    bool TMXMapInfo::parseProperties(void* outputData, TMXProperties& info)
    {
        xml_node<> *rootNode = static_cast<xml_node<>*>(outputData);
        xml_node<> *node;
        
        fzUInt i = 0;
        for(node = rootNode->first_node(); node && i < 4; node = node->next_sibling(), ++i)
        {
            info.m_properties[i].key = node->first_attribute("name")->value();
            info.m_properties[i].value = node->last_attribute("value")->value();                               
        }
    }
    
    
#pragma mark - TMXTilesetInfo
    
    fzRect TMXTilesetInfo::rectForGID(uint32_t GID) const
    {
        FZ_ASSERT(m_tileSize != FZSizeZero, "Tile size info is missing.");
        FZ_ASSERT(m_textureSize != FZSizeZero, "Texture size is missing.");
        
        fzRect rect(FZPointZero, m_tileSize);
        
        GID &= kFlippedMask;
        GID = GID - m_firstGID;
        
        int max_x = (m_textureSize.width + m_spacing - m_margin*2) / (m_tileSize.width + m_spacing);
        
        rect.origin.x = (GID % max_x) * (m_tileSize.width + m_spacing) + m_margin;
        rect.origin.y = (GID / max_x) * (m_tileSize.height + m_spacing) + m_margin;
        
        return rect;        
    }
}
