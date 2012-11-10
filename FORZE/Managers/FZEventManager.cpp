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

#include "FZEventManager.h"
#include "FZDirector.h"
#include "FZMacros.h"

using namespace STD;

namespace FORZE {
    
    bool EventDelegate::event(Event&)
    {
        FZLOGERROR("EventDelegate: bool event(Event&) should be overwritten.");
        return false;
    }
    
#define FZEVENT_INTERNAL_MASK (kFZEventType_Stick-1)
    
    EventManager* EventManager::p_instance = NULL;
    
    EventManager& EventManager::Instance()
    {
        if (p_instance == NULL)
            p_instance = new EventManager();
        
        return *p_instance;
    }
    
    
    EventManager::EventManager()
    : m_events()
    , m_handlers(0)
    , m_flags(0)
    , m_enabled(true)
    {
    }
    
    
    EventManager::~EventManager()
    { }
    
    
    void EventManager::setIsEnabled(bool isEnabled)
    {
        m_enabled = isEnabled;
    }
    
    
    bool EventManager::isEnabled() const
    {
        return m_enabled;
    }
    
    
    void EventManager::setAccelGyroInterval(fzFloat interval)
    {
        FZ_ASSERT(interval >= 0, "Interval must be positive.");
        OSW::setEventsInterval(interval);
    }
    
    
    EventManager::handlerList::iterator EventManager::indexForPriority(fzInt priority)
    {
        handlerList::iterator handler(m_handlers.begin());
        for(; handler != m_handlers.end(); ++handler) {
            if( handler->priority > priority )
                return handler;
        }
        return handler;
    }
    
    
    EventManager::fzEventHandler* EventManager::getHandlerForTarget(void* target)
    {
        FZ_ASSERT(target, "Target can not be NULL");
        
        handlerList::iterator handler(m_handlers.begin());
        for (; handler != m_handlers.end(); ++handler) {
            if(handler->delegate == target)
                return &(*handler);
        }
        return NULL;
    }
    
    
    void EventManager::updateHandlerFlags(fzEventHandler* handler, uint16_t flags)
    {
        FZ_ASSERT(handler, "Handler can not be NULL");
        
        handler->flags = flags;
        
        eventList::iterator event(m_events.begin());
        for(; event != m_events.end(); ++event) {
            if(event->getDelegate() == handler->delegate && !(event->getType() & flags))
                event->setDelegate(NULL);
        }
    }
    
    
    bool EventManager::invalidateDelegate(EventDelegate *target)
    {
        FZ_ASSERT(target, "Target can not be NULL");

        fzEventHandler *handler = getHandlerForTarget(target);
        if(handler) {
            updateHandlerFlags(handler, 0);
            return true;
        }
        return false;
    }
    
    
    void EventManager::updateFlags()
    {
        // GENERATE NEW FLAGS
        uint16_t newFlags = 0;
        handlerList::const_iterator handler(m_handlers.begin());
        for (; handler != m_handlers.end(); ++handler)
            newFlags |= handler->flags;
        
        uint16_t updatedFlags = m_flags ^ newFlags;
        OSW::configEvents(updatedFlags, newFlags);
        
        m_flags = newFlags;
    }
    
    
    uint16_t EventManager::checkCompatibility(uint16_t flags) const
    {
        uint16_t capacities = fzDevice_getCapacities();
        uint16_t compatibility = (flags & (~capacities)) & FZEVENT_INTERNAL_MASK;
        
        return compatibility;
    }
    
    
    void EventManager::addDelegate(EventDelegate *target, uint16_t flags, fzInt priority, fzEventHandlerMode mode)
    {
        FZ_ASSERT(target != NULL, "Target argument must be non-NULL");
        
        fzEventHandler *handler = getHandlerForTarget(target);
        if(handler) {
            handler->mode = mode;
            
            if(priority != handler->priority) {
                updateHandlerFlags(handler, 0);
                handler = NULL;
            }else{
                
                if(flags != handler->flags)
                    updateHandlerFlags(handler, flags);
            }
        }
        
        if(handler == NULL && flags != 0) {
            fzEventHandler newHandle;
            newHandle.flags = flags;
            newHandle.mode = mode;
            newHandle.priority = priority;
            newHandle.delegate = target;
            m_handlers.insert(indexForPriority(priority), newHandle);
        }
        
#if defined (FORZE_DEBUG) && FORZE_DEBUG > 0
        uint16_t compatibility = checkCompatibility(flags);
        
        if((flags & kFZEventType_Tap) != kFZEventType_Tap) {
            if(compatibility & kFZEventType_Touch)
                FZLOGERROR("EventManager: Touch events are not available in this device.");
            
            if(compatibility & kFZEventType_Mouse)
                FZLOGERROR("EventManager: Mouse events are not available in this device.");
        }
        if(compatibility & kFZEventType_MouseRight)
            FZLOGERROR("EventManager: Mouse right events are not available in this device.");
        if(compatibility & kFZEventType_Keyboard)
            FZLOGERROR("EventManager: Keyboard events are not available in this device.");
        if(compatibility & kFZEventType_Trackpad)
            FZLOGERROR("EventManager: Trackpad events are not available in this device.");
        if(compatibility & kFZEventType_Accelerometer)
            FZLOGERROR("EventManager: Accelerometer events are not available in this device.");
        if(compatibility & kFZEventType_Gyro)
            FZLOGERROR("EventManager: Gyroscope events are not available in this device.");
#endif
        
        updateFlags();
    }
    
    
    void EventManager::removeDelegate(EventDelegate *target)
    {
        FZ_ASSERT(target != NULL, "Target argument must be non-NULL");
        
        // INVALIDATE
        if(invalidateDelegate(target))
            updateFlags();
    }
    
    
    Event* EventManager::addEvent(const Event& newEvent)
    {   
        if(!(newEvent.getType() & m_flags))
            return NULL;

        FZ_ASSERT(newEvent.getOwner(), "Event owner can not be NULL");
            
        switch (newEvent.getState()) {
                
            case kFZEventState_Began:
            case kFZEventState_Indifferent:
            {
                FZ_ASSERT(newEvent.getDelegate() == NULL, "Event delegate should be NULL");
                m_events.push_back(newEvent);
                return &(m_events.back());
            }
            case kFZEventState_Updated:
            case kFZEventState_Ended:
            case kFZEventState_Cancelled:
            {
                eventList::iterator it(m_events.begin());
                for(; it != m_events.end(); ++it) {
                    Event *e = &(*it);
                    if(newEvent.getIdentifier() == e->getIdentifier() &&
                       newEvent.getOwner() == e->getOwner() &&
                       newEvent.getType() == e->getType())
                    {
                        e->update(newEvent);
                        return e;
                    }
                }
                return NULL;
            }
            default:
            {
                FZ_ASSERT(false, "Invalid event state");
                return NULL;
            }
        }
    }
    
    
    void EventManager::catchEvent(const Event& newEvent)
    {        
        if(!m_enabled)
            return;
        
        // the events are cached in order to dispatch them correctly.
        m_buffer.push(newEvent);
    }
    
    
    static bool isEventFinished(const Event& event)
    {
        return (event.getDelegate() == NULL);
    }
    
    
    bool isHandlerFinished(const EventManager::fzEventHandler &handler)
    {
        return (handler.flags == 0);
    }
    
    
    void EventManager::dispatchEvents()
    {
        while(!m_buffer.empty())
        {
            Event *event = addEvent(m_buffer.front());
            
            if(event) {
                
                switch (event->getState()) {
                    case kFZEventState_Indifferent:
                    {
                        handlerList::const_reverse_iterator handler(m_handlers.rbegin());
                        for (; handler != m_handlers.rend(); ++handler) {
                            if(handler->flags & event->getType())
                                handler->delegate->event(*event);
                        }
                        
                        break;
                    }
                    case kFZEventState_Began:
                    {
                        handlerList::const_reverse_iterator handler(m_handlers.rbegin());
                        for (; handler != m_handlers.rend(); ++handler) {
                            if(handler->flags & event->getType()) {
                                if(handler->delegate->event(*event)) {
                                    if(handler->flags & event->getType()) {
                                        event->setDelegate(handler->delegate);
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                    }
                    case kFZEventState_Updated:
                    {
                        if(event->getDelegate())
                            event->getDelegate()->event(*event);
                        
                        break;
                    }
                    case kFZEventState_Ended:
                    case kFZEventState_Cancelled:
                    {
                        if(event->getDelegate()) {
                            event->getDelegate()->event(*event);
                            event->setDelegate(NULL);
                        }
                        break;
                    }
                    default:
                    {
                        FZ_ASSERT(false, "Invalid state");
                        break;
                    }
                }
            }
            m_buffer.pop();
        }
        m_handlers.remove_if(isHandlerFinished);
        m_events.remove_if(isEventFinished);
    }
    
    
    void EventManager::cancelAllEvents()
    {
        eventList::iterator it(m_events.begin());
        for(; it != m_events.end(); ++it) {
            if(it->getState() == kFZEventState_Began || it->getState() == kFZEventState_Updated)
            {
                Event event = Event(it->getOwner(), it->getIdentifier(),
                                    it->getType(), kFZEventState_Cancelled,
                                    it->m_x, it->m_y, it->m_z);
                
                catchEvent(event);
            }
        }
    }
    
    
    fzUInt EventManager::getNumberOfEvents(uint16_t type) const
    {
        fzUInt count = 0;
        eventList::const_iterator it(m_events.begin());
        for(; it != m_events.end(); ++it) {
            if(it->getType() & type)
                ++count;
        }
        return count;
    }
}
