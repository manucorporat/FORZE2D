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

#include "FZSpriterNode.h"
#include "FZResourcesManager.h"
#include "rapidxml.hpp"
#include "FZMacros.h"


using namespace rapidxml;

namespace FORZE {
    
    SpriterNode::SpriterNode(const char *spriterFile)
    {        
        fzUInt factor;
        fzBuffer buffer = ResourcesManager::Instance().loadResource(spriterFile, &factor);
        if(!buffer.empty()) {
            loadData(buffer.getPointer());
            buffer.free();
        }
    }
    
    
    bool SpriterNode::loadData(char *data)
    {        
        xml_document<> doc;
        doc.parse<parse_fastest>(data);
        
        xml_node<> *rootNode, *node;        
        rootNode = doc.first_node("spriterdata");
        
        // GET ROOT NODE
        if(rootNode == NULL) {
            FZ_ASSERT(false, "<spriterdata> tag was not found.");
            return false;
        }
        
        {
            // GET FIRST CHAR
            node = rootNode->first_node("char");
            if(node == NULL) {
                FZ_ASSERT(false, "<char> tag was not found.");
                return false;
            }
            
            node = node->first_node("anim");
            if(node != NULL) {
                FZLOGINFO("PARSING ANIMATION");
                
                
            }
        }
    }
}
