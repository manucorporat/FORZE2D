#ifndef __FZ_NODEMANAGER_H_INCLUDED__
#define __FZ_NODEMANAGER_H_INCLUDED__
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

#include "FZTypes.h"
#include STL_QUEUE


using namespace STD;

namespace FORZE {
    
    
    class NodeManager
    {
    private:
        class Node;
        typedef struct nodeQueue
        {
            union{
                Node *child;
                fzInt tag;
            };
            Node *parent;
            fzInt data;
            bool add;
        }nodeQueue;
        
        
        /* Cache's instance */
        static NodeManager* pinstance_;
        queue<nodeQueue> queues_;
        
        NodeManager();
        NodeManager(const NodeManager& ) ;
        NodeManager &operator = (const NodeManager& );
        
        
        void addChild(Node *parent, Node *child, fzInt zOrder);
        void removeChildByTag(const Node *parent, fzInt tag, bool cleanup);
        void removeChild(const Node *parent, const Node *child, bool cleanup);
        void removeAllChildren(const Node *parent, bool cleanup);
        void perform();
        
    public:
        static NodeManager& Instance();
        friend class Node;
        
    };
}
#endif
