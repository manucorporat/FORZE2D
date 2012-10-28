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

#include "FZMenu.h"
#include "FZMenuItem.h"
#include "FZMacros.h"
#include "FZDirector.h"
#include "FZEventManager.h"



namespace FORZE {
    
    enum {
        kFZMenuDefaultPriority  = 1,
        kFZMenuDefaultPadding   = 5,
    };
    
    
    Menu::Menu()
    : p_selectedItem(NULL)
    , m_isWaiting(true)
    {
        setTrackedEvents(kFZEventType_Tap);
        setPriority(kFZMenuDefaultPriority);
        
        // menu in the center of the screen
        setIsRelativeAnchorPoint(true);
        setPosition(Director::Instance().getCanvasSize()/2);
    }
    
    
    Menu::Menu(MenuItem *item, ...)
    : Menu()
    {
        if(item == NULL)
            return;
        
        va_list args;
        va_start(args, item);

        while(item) {
            addChild(item);
            item = va_arg(args, MenuItem*);
        }
        
        va_end(args);
    }
    
    
    void Menu::onExit()
    {
        if(!m_isWaiting)
        {
            p_selectedItem->unselected();
            p_selectedItem = NULL;
            m_isWaiting = true;
        }
        Layer::onExit();
    }
    
    
    void Menu::insertChild(Node* node)
    {
        FZ_ASSERT(dynamic_cast<MenuItem*>(node), "Child must be subclass of MenuItem");
        
        MenuItem *item = static_cast<MenuItem*>(node);
        item->useBatchMode();
        Node::insertChild(node);
    }
    
    
    bool Menu::detachChild(Node* node, bool cleanup)
    {
        ((MenuItem*)node)->useSelfMode();
        return Node::detachChild(node, cleanup);
    }
    
    
    MenuItem* Menu::itemForPosition(const fzPoint& point)
    {	
        MenuItem *item;
        FZ_LIST_FOREACH(m_children, item)
        {            
            if ( item->isVisible() && item->isEnabled() ) {
                fzRect rect = item->getBoundingBox();
                if( rect.contains(point) )
                    return item;
            }
        }
        return NULL;
    }
    
    
    void Menu::alignItemsInColumns(fzPoint paddings, fzUInt *sizes, fzUInt nuColumns)
    {
        // Calculate total size
        fzSize size(-paddings.x, 0);
        Node *node = (Node*)m_children.front();
        
        fzFloat widths[nuColumns];
        
        
        for(fzUInt i = 0; i < nuColumns; ++i) {
            
            fzFloat width = 0;
            fzFloat height = -paddings.y;
            for(fzUInt w = 0; w < sizes[i]; ++i)
            {
                if(node == NULL) {
                    FZ_ASSERT(false, "Too many items.");
                    return;
                }
                
                width = MAX(width, node->getContentSize().width);
                height += node->getContentSize().height + paddings.y;
                node = (Node*)node->next();
            }
            
            size.width += width + paddings.x;
            widths[i] = size.width + width/2 + paddings.x;
            size.height = MAX(height, size.height);
        }
        setContentSize(size);
        
        
        fzUInt index = 0;
        for(fzUInt i = 0; i < nuColumns; ++i) {
            alignVertically(paddings.y, fzPoint(widths[i], m_contentSize.height/2), fzRange(index, sizes[i]));
            index += sizes[i];
        }
    }
    
    
    bool Menu::event(Event& event)
    {
        if(!event.isType(kFZEventType_Tap))
            return false;
        
        fzPoint point = event.getPoint();
        
        switch (event.getState())
        {
            case kFZEventState_Began:
            {
                if( m_isWaiting == false || !isVisible() )
                    return false;
                
                p_selectedItem = itemForPosition(point);
                
                if( p_selectedItem ) {
                    p_selectedItem->selected();
                    m_isWaiting = false;

                    return true;
                }
                
                break;
            }
            case kFZEventState_Updated:
            {
                FZ_ASSERT(!m_isWaiting, "[Event updated] -- invalid state.");
                
                MenuItem *currentItem = itemForPosition(point);
                
                if (currentItem != p_selectedItem) {
                    if(p_selectedItem)
                        p_selectedItem->unselected();
                        
                    p_selectedItem = currentItem;
                        
                    if(p_selectedItem)
                        p_selectedItem->selected();
                }
                
                break;
            }
            case kFZEventState_Ended:
                if(p_selectedItem)
                    p_selectedItem->activate();
                
            case kFZEventState_Cancelled:
                if(p_selectedItem)
                    p_selectedItem->unselected();
                
                m_isWaiting = true;
                
                break;
            default: break;
        }
        
        return false;
    }
}
