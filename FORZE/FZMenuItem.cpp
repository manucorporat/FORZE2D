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

#include "FZMenuItem.h"
#include "FZActionInterval.h"
#include "FZSprite.h"
#include "FZDirector.h"


using namespace STD;

namespace FORZE {
    
    enum {
        kCurrentItem = 0xc0c05001,
        kZoomActionTag = 0xc0c05002,
    };
    
    enum {
        kFZMenuItem_batchMode,
        kFZMenuItem_selfMode
    };
    
    
#pragma mark - MenuItem
    
    MenuItem::MenuItem(SELProtocol *target, SELECTOR_PTR selector)
    : Node()
    , m_mode(kFZMenuItem_selfMode)
    , p_targetCallback(target)
    , m_selector(selector)
    , m_isEnabled(true)
    , m_isSelected(false)
    , m_isWaiting(true)
    {
        setAnchorPoint(0.5f, 0.5f);
        setPosition(Director::Instance().getCanvasSize()/2);
    }
    
    
    void MenuItem::useBatchMode()
    {
        if(m_mode == kFZMenuItem_batchMode)
            return;
        
        m_mode = kFZMenuItem_batchMode;
    }
  
    
    void MenuItem::useSelfMode()
    {
        if(m_mode == kFZMenuItem_selfMode)
            return;
        
        m_mode = kFZMenuItem_selfMode;
    }
    
    
    void MenuItem::setIsEnabled(bool e)
    {
        m_isEnabled = e;
    }
    
    
    bool MenuItem::isEnabled() const
    {
        return m_isEnabled;
    }
    
    
    bool MenuItem::isSelected() const
    {
        return m_isSelected;
    }
    
    
    void MenuItem::activate()
    {
        if(m_isEnabled)
            (p_targetCallback->*m_selector)(this);
    }
    
    
    void MenuItem::selected()
    {
        m_isSelected = true;
    }
    
    
    void MenuItem::unselected()
    {
        m_isSelected = false;
    }
    
    
    bool MenuItem::event(Event& event)
    {
        FZ_ASSERT(event.isType(kFZEventType_Tap), "Event is a touch event.");
        
        if(!isVisible())
            return false;
        
        switch (event.getState())
        {
            case kFZEventState_Began:
            {
                if( !m_isWaiting || !isVisible() )
                    return false;
                
                if(getBoundingBox().contains(event.getPoint())) {
                    selected();
                    m_isWaiting = false;
                    return true;
                }
                break;
            }
            case kFZEventState_Updated:
            {
                FZ_ASSERT(!m_isWaiting, "[Event updated] -- invalid state.");
                
                bool isSelected = getBoundingBox().contains(event.getPoint());
                if (isSelected != m_isSelected) {
                    if(isSelected)
                        selected();
                    else
                        unselected();
                }
                
                break;
            }
            case kFZEventState_Ended:
                if(m_isSelected)
                    activate();
                
            case kFZEventState_Cancelled:
                if(m_isSelected)
                    unselected();
                
                m_isWaiting = true;
                
                break;
            default: break;
        }
        
        return false;
    }
    
    
    void MenuItem::onEnter()
    {
        if(m_mode == kFZMenuItem_selfMode)
            EventManager::Instance().addDelegate(this, kFZEventType_Tap, 1);
        
        Node::onEnter();
    }
    
    
    void MenuItem::onExit()
    {
        if(m_mode == kFZMenuItem_selfMode) {
            unselected();
            m_isWaiting = true;
            EventManager::Instance().removeDelegate(this);
        }
        
        Node::onExit();
    }
    
    
#pragma mark - MenuItemLabel
    
    MenuItemLabel::MenuItemLabel(Label *label, SELProtocol *target, SELECTOR_PTR selector)
    : MenuItem(target, selector)
    , m_originalScale(1)
    , m_colorBackup(fzWHITE)
    , m_disabledColor(fzGRAY)
    , p_label(NULL)
    {
        setLabel(label);
        setIsEnabled(true);
    }
    
    
    MenuItemLabel::MenuItemLabel(const string& text, const string& filename, SELProtocol *target, SELECTOR_PTR selector)
    : MenuItemLabel(new Label(text.c_str(), filename.c_str()), target, selector)
    { }
    
    
    void MenuItemLabel::setDisabledColor(const fzColor3B& d)
    {
        m_disabledColor = d;
    }
    
    
    void MenuItemLabel::setLabel(Label *label)
    {
        if( label != p_label ) {
            
            if(p_label)
                removeChild((Node*)p_label, true);
            
            if(label) {
                addChild(label);
                
                label->setAnchorPoint(FZPointZero);
                setContentSize(label->getContentSize());
            }
            p_label = label;
        }    
    }
    
    
    void MenuItemLabel::setString(const char* str)
    {
        p_label->setString(str);
        setContentSize( ((Node*)p_label)->getContentSize() );
    }
    
    
    const fzColor3B& MenuItemLabel::getDisabledColor() const
    {
        return m_disabledColor;
    }
    
    
    Label* MenuItemLabel::getLabel() const
    {
        return p_label;
    }
    
    
    void MenuItemLabel::setIsEnabled(bool e)
    {
        if( m_isEnabled != e ) {
            if(e == false) {
                m_colorBackup = p_label->getColor();
                p_label->setColor(m_disabledColor);
            }
            else
                p_label->setColor(m_colorBackup);
        }
        MenuItem::setIsEnabled(e);
    }
    
    
    void MenuItemLabel::selected()
    {
        // subclass to change the default action
        if(m_isEnabled) {

            MenuItem::selected();
            
            Action *action = getActionByTag(kZoomActionTag);
            if( action )
                stopAction(action);
            else
                m_originalScale = getScale();
            
            Action *zoomAction = new ScaleTo(0.1f, m_originalScale * 1.2f);
            zoomAction->setTag(kZoomActionTag);
            runAction(zoomAction);
        }
    }
    
    
    void MenuItemLabel::unselected()
    {
        // subclass to change the default action
        if(m_isEnabled) {
            MenuItem::unselected();
            stopActionByTag(kZoomActionTag);
            
            Action *zoomAction = new ScaleTo(0.1f, m_originalScale);
            zoomAction->setTag(kZoomActionTag);
            runAction(zoomAction);
        }
    }
    
    
    void MenuItemLabel::activate()
    {
        if(m_isEnabled) {
            stopAllActions();
            setScale(m_originalScale);
            MenuItem::activate();
        }
    }
    
    
    void MenuItemLabel::setColor(const fzColor3B& c)
    {
        p_label->setColor(c);
    }
    
    
    const fzColor3B& MenuItemLabel::getColor() const
    {
        return p_label->getColor();
    }
    
    
#pragma mark - MenuItemImage
    
    MenuItemImage::MenuItemImage(Sprite *sprite, SELProtocol *target, SELECTOR_PTR selector)
    : MenuItem(target, selector)
    , m_colorNormal(fzWHITE)
    , m_colorSelected(100, 220, 255)
    , m_colorDisabled(fzGRAY)
    , p_sprite(sprite)
    {
        FZ_ASSERT(sprite, "Sprite cannot be NULL.");
        setColorNormal(sprite->getColor());
        
        p_sprite->setAnchorPoint(FZPointZero);
        addChild(p_sprite);
        setContentSize(p_sprite->getContentSize());
    }
    

    MenuItemImage::MenuItemImage(const string& filename, SELProtocol *target, SELECTOR_PTR selector)
    : MenuItemImage(new Sprite(filename), target, selector)
    { }

    
    void MenuItemImage::setColorNormal(const fzColor3B& color)
    {
        m_colorNormal = color;
        if(!m_isSelected && m_isEnabled)
            p_sprite->setColor(color);
    }
    
    
    void MenuItemImage::setColorSelected(const fzColor3B& color)
    {
        m_colorSelected = color;
        if(m_isSelected && m_isEnabled)
            p_sprite->setColor(color);
    }
    
    
    void MenuItemImage::setColorDisabled(const fzColor3B& color)
    {
        m_colorDisabled = color;
        if(!m_isEnabled)
            p_sprite->setColor(color);
    }
    
    
    const fzColor3B& MenuItemImage::getColorNormal() const
    {
        return m_colorNormal;
    }
    
    
    const fzColor3B& MenuItemImage::getColorSelected() const
    {
        return m_colorSelected;
    }
    
    
    const fzColor3B& MenuItemImage::getColorDisabled() const
    {
        return m_colorDisabled;
    }
    
    
    void MenuItemImage::selected()
    {
        if(!m_isEnabled)
            return;
        
        MenuItem::selected();
        p_sprite->setColor(m_colorSelected);
    }
    
    
    void MenuItemImage::unselected()
    {
        if(!m_isEnabled)
            return;
        
        MenuItem::unselected();
        p_sprite->setColor(m_colorNormal);
    }
    
    
    void MenuItemImage::setIsEnabled(bool enabled)
    {
        MenuItem::setIsEnabled(enabled);
        if(enabled)
            p_sprite->setColor(m_colorNormal);
        else
            p_sprite->setColor(m_colorDisabled);
        
    }
    
    
#pragma mark - MenuItemSprite
    
    MenuItemSprite::MenuItemSprite(Sprite *nS,
                                   Sprite *sS,
                                   Sprite *dS, SELProtocol *target, SELECTOR_PTR selector)
    : MenuItem(target, selector)
    , p_normalSprite(NULL)
    , p_selectedSprite(NULL)
    , p_disabledSprite(NULL)
    {
        setNormalSprite(nS);
        setSelectedSprite(sS);
        setDisabledSprite(dS);
        
        setContentSize(p_normalSprite->getContentSize());
    }
    
    
    MenuItemSprite::MenuItemSprite(const string& normalFilename,
                                   const string& selectedFilename,
                                   const string& disabledFilename, SELProtocol *target, SELECTOR_PTR selector)
    : MenuItem(target, selector)
    , p_normalSprite(NULL)
    , p_selectedSprite(NULL)
    , p_disabledSprite(NULL)
    {
        setNormalSprite(new Sprite(normalFilename));
        if(!selectedFilename.empty())
            setSelectedSprite(new Sprite(selectedFilename));
        
        if(!disabledFilename.empty())
            setDisabledSprite(new Sprite(disabledFilename));
    }
    
    void MenuItemSprite::setNormalSprite(Sprite *s)
    {
        FZ_ASSERT(s, "Normal sprite can not be NULL.");
        
        if( s != p_normalSprite ) {
            s->setAnchorPoint(FZPointZero);
            s->setIsVisible(true);
            
            removeChild(p_normalSprite);
            addChild(s);

            p_normalSprite = s;
        }
    }
    
    
    void MenuItemSprite::setSelectedSprite(Sprite *s)
    {
        if( s != p_selectedSprite ) {
            removeChild(p_selectedSprite, true);
            
            if(s){
                s->setAnchorPoint(FZPointZero);
                s->setIsVisible(false);
                addChild(s);
            }
            p_selectedSprite = s;
        }
    }
    
    
    void MenuItemSprite::setDisabledSprite(Sprite *s)
    {
        if( s != p_disabledSprite ) {
            removeChild(p_disabledSprite, true);
            
            if(s){
                s->setAnchorPoint(FZPointZero);
                s->setIsVisible(false);
                addChild(s);
            }
            p_disabledSprite = s;
        }
    }
    
    
    void MenuItemSprite::selected()
    {
        MenuItem::selected();
        
        if( p_selectedSprite ) {
            p_normalSprite->setIsVisible(false);
            p_selectedSprite->setIsVisible(true);
            if(p_disabledSprite)
                p_disabledSprite->setIsVisible(false);
            
        } else {
            
            p_normalSprite->setIsVisible(true);
            if(p_disabledSprite)
                p_disabledSprite->setIsVisible(false);
        }
    }
    
    
    void MenuItemSprite::unselected()
    {
        MenuItem::unselected();
        p_normalSprite->setIsVisible(true);
        if(p_selectedSprite)
            p_selectedSprite->setIsVisible(false);
        if(p_disabledSprite)
            p_disabledSprite->setIsVisible(false);
    }
    
    
    void MenuItemSprite::setIsEnabled(bool e)
    {
        MenuItem::setIsEnabled(e);
        
        if( e ) {
            p_normalSprite->setIsVisible(true);
            if(p_selectedSprite)
                p_selectedSprite->setIsVisible(false);
            if(p_disabledSprite)
                p_disabledSprite->setIsVisible(false);
            
        } else {
            if( p_disabledSprite ) {
                p_disabledSprite->setIsVisible(true);
                p_normalSprite->setIsVisible(false);
                if(p_selectedSprite)
                    p_selectedSprite->setIsVisible(false);
		
            } else {
                p_normalSprite->setIsVisible(true);
                if(p_selectedSprite)
                    p_selectedSprite->setIsVisible(false);
            }
        }
    }
    
#pragma mark - MenuItemImage

}
