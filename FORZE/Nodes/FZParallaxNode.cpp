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

#include "FZParallaxNode.h"
#include "FZDirector.h"
#include "FZMacros.h"


namespace FORZE {
    
    ParallaxNode::ParallaxNode()
    : m_lastPosition(-100, -100)
    , p_parallaxArray(NULL)
    , m_count(0)
    {
        p_parallaxArray = new fzParallaxObject[5];
    }
    
    
    ParallaxNode::~ParallaxNode()
    {
        if( p_parallaxArray )
            delete [] p_parallaxArray;
    }
    
    
    void ParallaxNode::addChild(Node *child, fzInt zOrder, fzPoint ratio, fzPoint offset)
    {
        FZ_ASSERT(child, "Argument must be non-nil");        
        
        fzParallaxObject &obj = p_parallaxArray[m_count];
        obj.ratio = ratio;
        obj.offset = offset;
        obj.child = child;
        ++m_count;
        

        fzPoint pos = m_position.compMult(ratio) + offset;
        child->setPosition(pos);
        Node::addChild(child, zOrder);
    }


    bool ParallaxNode::detachChild(Node *child, bool cleanup)
    {
        for( fzUInt i = 0; i < m_count; ++i) {
            fzParallaxObject &point = p_parallaxArray[i];
            if(point.child == child) {
                point = p_parallaxArray[m_count-1];
                break;
            }
        }
        return Node::detachChild(child, cleanup);
    }
    
    
    fzPoint ParallaxNode::absolutePosition() const
    {
        fzPoint ret(m_position);
        
        const Node *cn = this;
        while ((cn = cn->getParent()))
            ret += cn->getPosition();
        
        return ret;
    }
    
    
//    void ParallaxNode::visit()
//    {
//        fzPoint pos = absolutePosition();
//        
//        if( pos != m_lastPosition) {
//            for( fzUInt i = 0; i < m_count; ++i) {
//                fzParallaxObject &point = p_parallaxArray[i];
//                fzPoint newPos = pos.compMult(point.ratio) + point.offset - pos;	
//                point.child->setPosition(newPos);
//            }
//            m_lastPosition = pos;
//        }
//        Node::visit();
//    }
}
