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

#include "FZSpriteFrameCache.h"
#include "FZTextureCache.h"
#include "FZResourcesManager.h"
#include "FZMacros.h"
#include "FZSprite.h"
#include "FZIO.h"
#include "rapidxml.hpp"


using namespace rapidxml;
using namespace STD;

namespace FORZE {
    
    SpriteFrameCache* SpriteFrameCache::p_instance = NULL;
    
    SpriteFrameCache& SpriteFrameCache::Instance()
    {
        if (p_instance == NULL)
            p_instance = new SpriteFrameCache();
        
        return *p_instance;
    }
    
    SpriteFrameCache::SpriteFrameCache()
    : m_frames()
    { }
    
    
    void SpriteFrameCache::addSpriteFrames(const char* coordsFilename, Texture2D *texture)
    {        
        fzUInt factor;
        fzBuffer data = ResourcesManager::Instance().loadResource(coordsFilename, &factor);
        if(data.isEmpty())
            return;
        
        xml_document<> doc;
        
        try {
            doc.parse<parse_fastest | parse_validate_closing_tags>(data.getPointer());
            
        } catch(std::exception &error) {
            data.free();
            FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". rapidXML exception: %s", coordsFilename, error.what());
            
            return;
        }
        
        xml_node<> *rootNode, *subNode, *node;

        rootNode = doc.first_node("metadata");
        
        if(texture == NULL) {
            subNode = rootNode->first_node("textureFileName");
            if(subNode) {
                char *filename = fzStrcpy(subNode->value(), subNode->value_size());
                texture = TextureCache::Instance().addImage(filename);
                delete filename;
            }
        }
        
        // Parse metadata
        if(texture == NULL) {
            data.free();
            FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". Error pTexture could not be loaded.", coordsFilename);
            return;
        }
        
        // Parse frames
        rootNode = doc.first_node("frames");
        
        if(rootNode == NULL) {
            data.free();
            FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". <frames> tag wasn't found.", coordsFilename);
            return;
        }
        
        for(subNode = rootNode->first_node(); subNode; subNode = subNode->next_sibling())
        {
            if(strncmp(subNode->name(), "frame", subNode->name_size()) != 0) {
                FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". XML format is not correct.", coordsFilename);
                continue;
            }
            
            fzRect rect;
            fzPoint offset = FZPointZero;
            fzSize originalSize = FZSizeZero;
            bool rotated = false;
            
            // NAME
            xml_attribute<> *attribute = subNode->first_attribute("name");
            if(attribute == NULL) {
                FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". <node name> attribute wasn't found.", coordsFilename);
                continue;
            }
            int32_t hash = fzHash(attribute->value(), attribute->value_size());
            
            if(getSpriteFrameByHash(hash).isValid())
                continue;
            
            
            // ORIGIN
            node = subNode->first_node("p");
            if(node == NULL) {
                FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". <p> tag wasn't found.", coordsFilename);
                continue;
            }
            attribute = node->first_attribute("x");
            if(attribute == NULL) {
                FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". <p x> attribute wasn't found.", coordsFilename);
                continue;
            }            
            rect.origin.x = atof(attribute->value()) / factor;
            
            attribute = node->first_attribute("y");
            if(attribute == NULL) {
                FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". <p y> attribute wasn't found.", coordsFilename);
                continue;
            } 
            rect.origin.y = atof(attribute->value()) / factor;
            
            
            // SIZE
            node = subNode->first_node("s");
            if(node == NULL) {
                FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". <s> tag wasn't found.", coordsFilename);
                continue;
            }
            
            attribute = node->first_attribute("x");
            if(attribute == NULL) {
                FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". <s x> attribute wasn't found.", coordsFilename);
                continue;
            }            
            rect.size.width = atof(attribute->value()) / factor;
            
            attribute = node->first_attribute("y");
            if(attribute == NULL) {
                FZLOGERROR("SpriteFrameCache: Error parsing \"%s\". <s y> attribute wasn't found.", coordsFilename);
                continue;
            } 
            rect.size.height = atof(attribute->value()) / factor;
            
            
            // OFFSET POSITION
            node = subNode->first_node("o");
            if(node) {
                attribute = node->first_attribute("x");
                if(attribute)
                    offset.x = atof(attribute->value()) / factor;
                
                attribute = node->first_attribute("y");
                if(attribute)
                    offset.y = atof(attribute->value()) / factor;
            } 
            
            
            // UNTRIMMED SIZE (ORIGINAL)
            node = subNode->first_node("u");
            if(node) {
                attribute = node->first_attribute("x");
                if(attribute)
                    originalSize.width = atof(attribute->value()) / factor;
                
                attribute = node->first_attribute("y");
                if(attribute)
                    originalSize.height = atof(attribute->value()) / factor;
            }
            
            
            // IS ROTATED
            node = subNode->last_node("r");
            if(node) {
                attribute = node->first_attribute("x");
                if(attribute)
                    rotated = atoi(attribute->value());
            }
            
            if(originalSize == FZSizeZero)
                originalSize = rect.size;
            
            // create frame
            fzSpriteFrame spriteFrame(texture, rect, offset, originalSize, rotated);
            m_frames.insert(framesPair(hash, spriteFrame));
        }
        data.free();
    }
    
    
    void SpriteFrameCache::addSpriteFramesWithImage(const char* cName, const char* tName)
    {
        Texture2D *texture = TextureCache::Instance().addImage(tName);
        addSpriteFrames(cName, texture);
    }
    
    
    void SpriteFrameCache::addSpriteFrame(const fzSpriteFrame& frame, const char* name)
    {
        m_frames.insert(framesPair(fzHash(name), frame));
    }
    
    
    void SpriteFrameCache::removeAllSpriteFrames()
    {
        m_frames.clear();
    }
    
    
    void SpriteFrameCache::removeSpriteFrameByName(const char* name)
    {
        m_frames.erase(fzHash(name));
    }
    
    
    void SpriteFrameCache::removeSpriteFramesFromFile(const char* plist)
    {
        fzUInt factor;
        fzBuffer data = ResourcesManager::Instance().loadResource(plist, &factor);
        if(data.isEmpty())
            return;
        
        xml_document<> doc;
        doc.parse<parse_fastest>(data.getPointer());
        
        xml_node<> *rootNode, *frameNode;        
        
        // Parse frames
        rootNode = doc.first_node("frames");
        
        if(rootNode == NULL) {
            FZLOGERROR("SpriteFrameCache: <frames> tag wasn't found.");
            data.free();
            return;
        }
        
        for(frameNode = rootNode->first_node(); frameNode; frameNode = frameNode->next_sibling())
        {
            FZ_ASSERT(strncmp(frameNode->name(), "node", frameNode->name_size()) == 0, "XML tag is not \"node\".");

            // NAME
            xml_attribute<> *attribute = frameNode->first_attribute("name");
            if(attribute == NULL) {
                FZ_ASSERT(false, "SpriteFrameCache: <node name> attribute wasn't found.");
                continue;
            }
            
            m_frames.erase(fzHash(attribute->value(), attribute->value_size()));
        }
        data.free();
    }
    
    
    void SpriteFrameCache::removeSpriteFramesFromTexture(Texture2D *texture)
    {
        framesMap::iterator it(m_frames.begin());
        for(; it != m_frames.end(); ) {
            if(it->second.getTexture() == texture)
                m_frames.erase(it++);
            else
                ++it;
        }
    }
    
    
    fzSpriteFrame SpriteFrameCache::getSpriteFrameByHash(int32_t hash) const
    {
        framesMap::const_iterator it(m_frames.find(hash));
        if(it == m_frames.end())
            return fzSpriteFrame();
        
        return it->second;
    }
    
    
    
    fzSpriteFrame SpriteFrameCache::getSpriteFrameByKey(const char* key) const
    {
        fzSpriteFrame frame = getSpriteFrameByHash(fzHash(key));
        if(!frame.isValid()) {
            FZLOGERROR("SpriteFrameCache: The fzSpriteFrame was not found for the key \"%s\".", key);
        }
        return frame;
    }
}
