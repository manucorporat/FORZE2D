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

#include "FZActionInstant.h"
#include "FZMacros.h"
#include "FZSprite.h"


namespace FORZE {
    
#pragma mark - ActionInstant
    
    ActionInstant::ActionInstant()
    {
        m_duration = 0;
    }
    
    
    bool ActionInstant::isDone() const
    {
        return true;
    }
    
    
    void ActionInstant::step(fzFloat dt)
    {
        update(1);
    }
    
    
    ActionInstant* ActionInstant::copy() const
    {
        FZLOGERROR("ActionInstant: Copying is not implemented. Override this method.");
        return NULL;
    }
    
    
    ActionInstant* ActionInstant::reverse() const
    {
        return copy();
    }
    
    
#pragma mark - Show
    
    void Show::startWithTarget(void *t)
    {
        Action::startWithTarget(t);
        ((Node*)p_target)->setIsVisible(true);
    }
    
    Show* Show::copy() const
    {
        return new Show();
    }
    
    
#pragma mark - Hide
    
    void Hide::startWithTarget(void *target)
    {
        Action::startWithTarget(target);
        ((Node*)p_target)->setIsVisible(false);
    }
    
    
    Hide* Hide::copy() const
    {
        return new Hide();
    }
    
    
#pragma mark - ToggleVisibility
    
    void ToggleVisibility::startWithTarget(void *target)
    {
        Action::startWithTarget(target);
        
        Node *node = (Node*)p_target;
        node->setIsVisible(!node->isVisible());
    }
    
    
    ActionInstant* ToggleVisibility::copy() const
    {
        return new ToggleVisibility();
    }
    
    
#pragma mark - FlipX
    
    FlipX::FlipX(bool x)
    : m_flipX(x)
    { }
    
    
    void FlipX::startWithTarget(void *target)
    {
        Action::startWithTarget(target);
        ((Sprite*)p_target)->setFlipX(m_flipX);
    }
    
    
    ActionInstant* FlipX::copy() const
    {
        return new FlipX(m_flipX);
    }
    
    
#pragma mark - FlipY
    
    FlipY::FlipY(bool y)
    : m_flipY(y)
    { }
    
    
    void FlipY::startWithTarget(void *target)
    {
        Action::startWithTarget(target);
        ((Sprite*)p_target)->setFlipY(m_flipY);
    }
    
    
    ActionInstant* FlipY::copy() const
    {
        return new FlipY(m_flipY);
    }
    
    
#pragma mark - Place
    
    Place::Place(const fzPoint& p)
    : m_position(p)
    { }
    
    Place::Place(fzFloat x, fzFloat y)
    : Place(fzPoint(x, y))
    { }
    
    
    void Place::startWithTarget(void *target)
    {
        Action::startWithTarget(target);
        ((Node*)p_target)->setPosition(m_position);
    }
    
    
    ActionInstant* Place::copy() const
    {
        return new Place(m_position);
    }
    
    
#pragma mark - CallFunc
    
    CallFunc::CallFunc(SELProtocol *t, SELECTOR_VOID s)
    : p_targetCallback(t)
    , p_selector(s)
    {
        FZ_ASSERT( p_targetCallback != NULL, "Selector must be non-NULL");
        FZ_ASSERT( p_selector != NULL, "Target must be non-NULL");
    }
    
    
    void CallFunc::startWithTarget(void *target)
    {
        Action::startWithTarget(target);
        execute();
    }
    
    
    void CallFunc::execute()
    {
        (p_targetCallback->*p_selector)();
    }
    
    
    ActionInstant* CallFunc::copy() const
    {
        return new CallFunc(p_targetCallback, p_selector);
    }
    
    
#pragma mark - CallFuncN
    
    CallFuncN::CallFuncN(SELProtocol *t, SELECTOR_PTR s)
    : p_targetCallback(t)
    , p_selector(s)
    {
        FZ_ASSERT( p_targetCallback != NULL, "Selector must be non-NULL");
        FZ_ASSERT( p_selector != NULL, "Target must be non-NULL");
    }
    
    
    void CallFuncN::startWithTarget(void *target)
    {
        Action::startWithTarget(target);
        execute();
    }
    
    
    void CallFuncN::execute()
    {
        (p_targetCallback->*p_selector)(p_target);
    }
    
    
    ActionInstant* CallFuncN::copy() const
    {
        return new CallFuncN(p_targetCallback, p_selector);
    }
    
    
#pragma mark - CallFuncND
    
    CallFuncND::CallFuncND(SELProtocol *t, SELECTOR_2PTR s, void* d)
    : p_targetCallback(t)
    , p_selector(s)
    , p_pointer(d)
    {
        FZ_ASSERT( p_targetCallback != NULL, "Selector must be non-NULL");
        FZ_ASSERT( p_selector != NULL, "Target must be non-NULL");
    }
    
    
    void CallFuncND::startWithTarget(void *target)
    {
        Action::startWithTarget(target);
        execute();
    }
    
    
    void CallFuncND::execute()
    {
        (p_targetCallback->*p_selector)(p_target, p_pointer);
    }
    
    
    ActionInstant* CallFuncND::copy() const
    {
        return new CallFuncND(p_targetCallback, p_selector, p_pointer);
    }
    
    
#pragma mark - CallFuncO
    
    CallFuncO::CallFuncO(SELProtocol *t, SELECTOR_PTR s, void* o)
    : CallFuncN(t, s)
    , p_pointer(o)
    { }
    
    
    void CallFuncO::execute()
    {
        (p_targetCallback->*p_selector)(p_pointer);
    }
    
    
    ActionInstant* CallFuncO::copy() const
    {
        return new CallFuncO(p_targetCallback, p_selector, p_pointer);
    }
}
