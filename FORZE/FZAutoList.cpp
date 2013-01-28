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

#include "FZAutoList.h"
#include "FZMacros.h"


namespace FORZE {
    
    AutoList::AutoList()
    : m_count(0)
    , p_front(NULL)
    , p_back(NULL)
    { }
    
    void AutoList::insert(fzListItem *position, fzListItem *newItem)
    {
        FZ_ASSERT(newItem != NULL, "New item cannot be NULL.");

        if(position != NULL)
        {
            FZ_ASSERT(m_count > 0, "Strange bug: List is empty.");
            
            // POSITION != NULL -> INSERT AT POSITION
            newItem->p_next = position;
            newItem->p_prev = position->p_prev;
            if(position->p_prev)
                position->p_prev->p_next = newItem;
            
            position->p_prev = newItem;

        }else
        {
            // POSITION == NULL -> PUSH_BACK
            if(p_back)
                p_back->p_next = newItem;
            
            newItem->p_prev = p_back;
            newItem->p_next = NULL;
            
            p_back = newItem;
        }
        if(newItem->p_prev == NULL)
            p_front = newItem;
        
        ++m_count;
    }
    
    
    void AutoList::remove(fzListItem *position)
    {   
        FZ_ASSERT(position != NULL, "Position cannot be NULL.");
        FZ_ASSERT(m_count > 0, "List is empty.");
        
        fzListItem *prev = position->p_prev;
        fzListItem *next = position->p_next;
        
        if(prev)
            prev->p_next = next;
        else
            p_front = next;
        
        if(next)
            next->p_prev = prev;
        else
            p_back = prev;
        
        position->p_prev = NULL;
        position->p_next = NULL;
        
        --m_count;
    }
    
    
    void AutoList::move(fzListItem *object, fzListItem *newPosition)
    {
        FZ_ASSERT(object != NULL, "Object cannot be NULL.");
        FZ_ASSERT(m_count > 0, "List is empty.");
        
        if(object == newPosition || (newPosition == NULL && p_back == object))
            return;
        
        if(object == p_front)
            p_front = object->p_next;
        
        if(object->p_prev)
            object->p_prev->p_next = object->p_next;
        
        if(object->p_next)
            object->p_next->p_prev = object->p_prev;
        
        
        if(newPosition == NULL) {
            object->p_prev = p_back;
            object->p_next = NULL;
            p_back->p_next = object;
            p_back = object;
        }else{
            if(object == p_back)
                p_back = object->p_prev;
            
            object->p_prev = newPosition->p_prev;
            object->p_next = newPosition;
            if(newPosition->p_prev)
                newPosition->p_prev->p_next = object;
            
            newPosition->p_prev = object;
            
            if(object->p_prev == NULL)
                p_front = object;
        }
    }
}
