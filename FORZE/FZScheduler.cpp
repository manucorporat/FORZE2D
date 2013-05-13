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

#include "FZScheduler.h"
#include "FZMacros.h"


using namespace STD;

namespace FORZE {
    
    //! Used intenally to compare pointers to functions.
    template <typename T> static bool compareSEL(const T sel1, const T sel2)
    {
        union {
            T sel;
            intptr_t i[2];
        } un;
        
        un.sel = sel1;
        intptr_t id1 = un.i[0];
        
        un.sel = sel2;
        intptr_t id2 = un.i[0];
        
        return (id1 == id2);
    }
    
    
#pragma mark - Scheduler

    Scheduler* Scheduler::p_instance = NULL;
    
    Scheduler& Scheduler::Instance()
    {
        if (p_instance == NULL)
            p_instance = new Scheduler();
        
        return *p_instance;
    }
    
    Scheduler::Scheduler()
    : m_timeScale(1.0f)
    , m_timers()
    , p_currentTimer(NULL)
    { }
    
    
    Scheduler::~Scheduler()
    {
        unscheduleAllSelectors();
    }
    
    
    void Scheduler::setTimeScale(fzFloat timeScale)
    {
        FZ_ASSERT(timeScale > 0.0f, "Time scaling factor must be positive.");
        m_timeScale = timeScale;
    }
    
    
    fzFloat Scheduler::getTimeScale() const
    {
        return m_timeScale;
    }
    
    
    Timer* Scheduler::getCurrentTimer() const
    {
        return p_currentTimer;
    }
    
    
    void Scheduler::scheduleSelector(const SELECTOR_FLOAT selector, SELProtocol *target, fzFloat interval, bool paused, fzUInt priority)
    {
        FZ_ASSERT( selector != NULL, "Selector must be non-NULL.");
        FZ_ASSERT( target != NULL, "Target must be non-NULL.");
        FZ_ASSERT( interval >= 0, "Interval must be positive.");

        Timer *updatedTimer = NULL;
        fzUInt i = 0;
        timersList::iterator it(m_timers.begin());
        for (; it != m_timers.end(); ++it, ++i) {
            if ((it->getTarget() == target) && compareSEL(it->getSelector(), selector)) {
                updatedTimer = &(*it);
                break;
            }
        }
        
        if(updatedTimer != NULL) {
            
            // Update interval
            updatedTimer->setInterval(interval);
            updatedTimer->setIsPaused(paused);
            
            if (updatedTimer->getPriority() != priority) {
                updatedTimer->m_priority = priority;
                m_timers.splice(it, m_timers, indexForPriority(priority));
            }
            
        }else{
            Timer timer(target, selector, priority, interval);
            timer.setIsPaused(paused);
            
            m_timers.insert(indexForPriority(priority), timer);
        }
    }
    
    
    void Scheduler::unscheduleSelector(const SELECTOR_FLOAT selector, SELProtocol *target)
    {
        FZ_ASSERT( selector != NULL, "Selector must be non-NULL.");
        FZ_ASSERT( target != NULL, "Target must be non-NULL.");
        
        timersList::iterator it(m_timers.begin());
        for(; it != m_timers.end(); ++it) {
            Timer *timer = &(*it);
            
            if((timer->getTarget() == target) && compareSEL(timer->getSelector(), selector)) {
                timer->p_target = NULL;
                break;
            }
        }
    }
    
    
    void Scheduler::unscheduleAllSelectors(SELProtocol *target)
    {
        FZ_ASSERT(target != NULL, "Target must be non-NULL.");
        
        timersList::iterator it(m_timers.begin());
        for(; it != m_timers.end(); ++it) {
            if(it->getTarget() == target)
                it->p_target = NULL;
        }
    }
    
    
    void Scheduler::unscheduleAllSelectors()
    {
        FZLOGINFO("Scheduler: warning: unscheduleAllSelectors() is dangerous, actions could stop working.");
        timersList::iterator it(m_timers.begin());
        for(; it != m_timers.end(); ++it)
            it->p_target = NULL;
    }
    
    
    void Scheduler::pauseTarget(SELProtocol *target)
    {
        FZ_ASSERT(target, "Target can not be NULL.");
        
        timersList::iterator it(m_timers.begin());
        for(; it != m_timers.end(); ++it) {
            if(it->getTarget() == target)
                it->setIsPaused(true);
        }
    }
    
    
    void Scheduler::resumeTarget(SELProtocol *target)
    {
        FZ_ASSERT(target, "Target can not be NULL.");

        timersList::iterator it(m_timers.begin());
        for(; it != m_timers.end(); ++it) {
            if(it->getTarget() == target)
                it->setIsPaused(false);
        }
    }
    
    
    Scheduler::timersList::iterator Scheduler::indexForPriority(fzUInt priority)
    {
        timersList::iterator it(m_timers.begin());
        for(; it != m_timers.end(); ++it) {    
            if( priority <= it->getPriority())
                return it;
        }
        return it;
    }
    
    
    void Scheduler::tick(fzFloat dt)
    {
        FZ_ASSERT(dt >= 0.0f, "Tick delta must be positive.");
        FZ_ASSERT(p_currentTimer == NULL, "Scheduler::tick() can not call himself.");
        dt *= m_timeScale;
        
        timersList::iterator it(m_timers.begin());
        for(; it != m_timers.end(); ) {
            if(it->getTarget() == NULL) {
                p_currentTimer = NULL;
                m_timers.erase(it++);
            
            } else {
                if(!it->isPaused()) {
                    p_currentTimer = &(*it);
                    it->update(dt);
                }
                ++it;
            }
        }
        p_currentTimer = NULL;
    }
}
