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

#include "FZNodeManager.h"
#include "FZMacros.h"
#include "FZNode.h"


using namespace STD;

namespace FORZE {
    
    NodeManager* NodeManager::pinstance_ = NULL;
    
    NodeManager& NodeManager::Instance()
    {
        if (pinstance_ == NULL)
            pinstance_ = new NodeManager();
        
        return *pinstance_; 
    }
    
    
    NodeManager::NodeManager()
    : queues_()
    { }
    
    
    void NodeManager::addChild(Node *p, Node *c, fzInt z)
    {
        FZ_ASSERT( p != NULL, "Parent must be non-NULL");
        FZ_ASSERT( c != NULL, "Child must be non-nil");
        FZ_ASSERT( c->getParent() == NULL, "child already added. It can't be added again");
        
        c->setZOrder(z);
        c->setParent(p);
        
        nodeQueue queue;
        queue.parent = const_cast<Node*>(p);
        queue.child = const_cast<Node*>(p);
        queue.add = true;
        
        queues_.push(queue);
    }
    
    void NodeManager::removeChildByTag(const Node *p, fzInt t, bool cl)
    {
        FZ_ASSERT( p != NULL, "Parent must be non-NULL");
        FZ_ASSERT( t != kFZNodeTagInvalid, "Invalid tag");
        
        nodeQueue queue;
        queue.parent = const_cast<Node*>(p);
        queue.tag = t;
        queue.data = cl;
        queue.add = false;
        
        queues_.push(queue);
    }
    
    
    void NodeManager::removeChild(const Node *p, const Node *c, bool cl)
    {
        FZ_ASSERT( p != NULL, "Parent must be non-NULL");
        FZ_ASSERT( c != NULL, "Child must be non-nil");
        FZ_ASSERT( c->getParent() == p, "this child isn't subnode of this node");
        
        nodeQueue queue;
        queue.parent = const_cast<Node*>(p);
        queue.child = const_cast<Node*>(c);
        queue.data = cl;
        queue.add = false;

        queues_.push(queue);
    }
    
    
    void NodeManager::removeAllChildren(const Node *p, bool cl)
    {
        FZ_ASSERT( p != NULL, "Parent must be non-NULL");
        
        nodeQueue queue;
        queue.parent = const_cast<Node*>(p);
        queue.child = NULL;
        queue.data = cl;
        queue.add = false;

        queues_.push(queue);
    }
    
    
    void NodeManager::perform()
    {
        while ( !queues_.empty() ) {
            
            nodeQueue q = queues_.front();
            if(q.child == NULL)
                q.parent->_removeAllChildren(q.data);
            {
                if(q.add)
                    q.parent->_addChild(q.child);
                else
                    q.parent->_removeChild(q.child, q.data);
            }
            
            queues_.pop();
        }  
    }
}
