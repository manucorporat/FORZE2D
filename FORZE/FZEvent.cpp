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

#include "FZEvent.h"
#include "FZMacros.h"


namespace FORZE {
    
    void Event::update(const Event& event)
    {
        FZ_ASSERT(m_state != kFZEventState_Indifferent, "Events with indifferent state can not be updated.");
        m_state = event.getState();
        m_x = event.m_x;
        m_y = event.m_y;
        m_z = event.m_z;
    }
    
    
    void Event::setDelegate(EventDelegate* d)
    {
        FZ_ASSERT(m_state != kFZEventState_Indifferent, "Events with indifferent state can not have a delegate.");
        p_delegate = d;
    }
    
    
    void Event::log() const
    {
        const char *t;
        switch (m_type) {
            case kFZEventType_Touch:        t = "Touch"; break;
            case kFZEventType_Mouse:        t = "Mouse"; break;
            case kFZEventType_Tap:          t = "Tap"; break;
            case kFZEventType_MouseRight:   t = "Mouse right"; break;
            case kFZEventType_MouseMoved:   t = "Mouse moved"; break;
            case kFZEventType_Keyboard:     t = "Key"; break;
            case kFZEventType_Stick:        t = "Stick"; break;
            case kFZEventType_Accelerometer:t = "Accelerometer"; break;
            default: t = "Unknow"; break;
        }
        
        const char *s;
        switch (m_state) {
            case kFZEventState_Began:        s = "Began"; break;
            case kFZEventState_Updated:      s = "Updated"; break;
            case kFZEventState_Ended:        s = "Ended"; break;
            case kFZEventState_Cancelled:    s = "Cancelled"; break;
            case kFZEventState_Indifferent:  s = "Indifferent"; break;
            default: s = "Unknow"; break;
        }
        FZLog("Event( %d ): \n - Type: %s \n - State: %s \n - Values: { %f, %f } * %f", m_identifier, t, s, m_x, m_y, m_z);
    }
}
