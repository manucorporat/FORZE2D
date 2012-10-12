#ifndef __FZ_MENU_H_INCLUDED__
#define __FZ_MENU_H_INCLUDED__
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
#include "FZEventManager.h"

namespace FORZE {
    
    class Menu;
    
    /** A Menu
     * Features and Limitation:
     *  - You can add MenuItem objects in runtime using addChild()
     *  - But the only accecpted children are MenuItem objects
     */
    class Menu : public Layer
    {
    protected:
        //! menu state
        bool m_isWaiting;
        
        //! selected item
        MenuItem *p_selectedItem;
        
        //! Returns a menuitem placed at the given position
        MenuItem* itemForPosition(const fzPoint& position);
        
        virtual void _addChild(Node*) override;
        virtual void _detachChild(Node*, bool) override;
        //virtual void registerWithEventDispatcher() override;
    
        
    public:
        //! Constructs an empty menu
        explicit Menu();
        
        
        //! Construct a menu with a list of MenuItem
        Menu(MenuItem *item, ...);
        
        void alignItemsInColumns(fzPoint paddings, fzUInt *sizes, fzUInt nuColumns);
        
        // Redefined functions
        virtual bool fzEvent(Event& event) override;
        virtual void onExit() override;
    };
}
#endif
