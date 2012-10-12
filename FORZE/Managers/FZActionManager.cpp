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

#include "FZActionManager.h"
#include "FZScheduler.h"
#include "FZMacros.h"


using namespace STD;

namespace FORZE {
    
    ActionManager* ActionManager::p_instance = NULL;
    
    ActionManager& ActionManager::Instance()
    {
        if (p_instance == NULL)
            p_instance = new ActionManager();
        
        return *p_instance;
    }
    
    
    ActionManager::ActionManager()
    : m_actions()
    { }
    
    
    Action* ActionManager::getActionByTag(fzInt tag, void *target)
    {
        FZ_ASSERT( tag != kFZActionTagInvalid, "Invalid tag");
        FZ_ASSERT( target != NULL, "Argument target must be non-NULL");
        
        const pairSearch& p = m_actions.equal_range(target);
        actionsMap::const_iterator it(p.first);
        for (; it != p.second; ++it) {
            Action *action = it->second.action;
            if(action->getTag() == tag)
                return action;
        }
        return NULL;
    }
    
    
    fzUInt ActionManager::getNumberActions(void *target) const
    {
        FZ_ASSERT( target != NULL, "Argument target must be non-NULL");
        return m_actions.count(target);
    }
    
    
    fzUInt ActionManager::getNumberActions() const
    {
        return m_actions.size();
    }
    
    
    void ActionManager::addAction(Action *action, void *target, bool paused)
    {
        FZ_ASSERT( action != NULL, "Argument action must be non-nil");
        FZ_ASSERT( target != NULL, "Argument target must be non-NULL");
        FZ_ASSERT( action->getTarget() == NULL, "This action is already used.");
        
        action->startWithTarget(target);
        action->retain();
        
        fzActionHandler container = {action, paused};
        m_actions.insert(pairAction(target, container));
    }
    
    
    void ActionManager::pauseTarget(void *target)
    {
        FZ_ASSERT( target != NULL, "Argument target must be non-NULL");
        
        const pairSearch& p = m_actions.equal_range(target);
        actionsMap::iterator it(p.first);
        for (; it != p.second; ++it)
            it->second.isPaused = true;
    }
    
    
    void ActionManager::resumeTarget(void *target)
    {
        FZ_ASSERT( target != NULL, "Argument target must be non-NULL");
        
        const pairSearch& p = m_actions.equal_range(target);
        actionsMap::iterator it(p.first);
        for (; it != p.second; ++it)
            it->second.isPaused = false;
    }
    
    
    void ActionManager::removeAction(const Action* action)
    {
        FZ_ASSERT( action != NULL, "Argument action must be non-NULL");
        
        const pairSearch& p = m_actions.equal_range(action->getTarget());
        actionsMap::const_iterator it(p.first);
        for (; it != p.second; ++it)
        {
            Action *a = it->second.action;
            if(a == action) {
                it->second.action->stop();
                return;
            }
        }
    }
    
    
    void ActionManager::removeAction(fzInt tag, void *target)
    {
        FZ_ASSERT( tag != kFZActionTagInvalid, "Invalid tag");
        FZ_ASSERT( target != NULL, "Argument target must be non-NULL");
        
        const pairSearch& p = m_actions.equal_range(target);
        actionsMap::const_iterator it(p.first);
        for (; it != p.second; ++it)
        {
            Action *action = it->second.action;
            if(action->getTag() == tag) {
                it->second.action->stop();
                return;
            }
        }
    }
    
    
    void ActionManager::removeAllActions(void *target)
    {
        FZ_ASSERT( target != NULL, "Argument target must be non-NULL");
        
        const pairSearch& p = m_actions.equal_range(target);
        actionsMap::const_iterator it(p.first);
        for (; it != p.second; ++it)
            it->second.action->stop();
    }
    
    
    void ActionManager::removeAllActions()
    {
        actionsMap::const_iterator it(m_actions.begin());
        for (; it != m_actions.end(); ++it)
            it->second.action->stop();
    }
    
    
    void ActionManager::update(fzFloat dt)
    {
        actionsMap::iterator it(m_actions.begin());
        for(; it != m_actions.end();) {
            Action *action = it->second.action;

            if(action->getTarget() == NULL) {
                
                m_actions.erase(it++);
                action->release();
                
            }else{
                
                if(!it->second.isPaused) {
                    
                    action->step(dt);
                    if(action->isDone())
                        action->stop();
                }
                ++it;
            }
        }
    }
}
