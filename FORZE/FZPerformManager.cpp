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

#include "FZPerformManager.h"
#include "FZMacros.h"
#include "external/tinythread/tinythread.h"


using namespace STD;

namespace FORZE {
    
    enum {
        kJPData_float,
        kJPData_void,
        kJPData_ptr,
        kJPData_2ptr,
    };
    
    PerformManager* PerformManager::p_instance = NULL;
    
    PerformManager& PerformManager::Instance()
    {
        if (p_instance == NULL)
            p_instance = new PerformManager();
        
        return *p_instance;
    }
    
    
    PerformManager::PerformManager()
    : m_asyncPerforms()
    , m_syncPerforms()
    {
        p_mutex = new recursive_mutex();
    }
    
    
    inline void PerformManager::execute(const fzPerform& perform)
    {
        switch (perform.valueType) {
            case kJPData_void:
                (perform.target->*perform.selector.argVoid)();
                break;
            case kJPData_float:
                (perform.target->*perform.selector.argFloat)(perform.value.valueFloat);
                break;
            case kJPData_ptr:
                (perform.target->*perform.selector.argPTR)(perform.value.ptr[0]);
                break;
            case kJPData_2ptr:
                (perform.target->*perform.selector.arg2PTR)(perform.value.ptr[0], perform.value.ptr[1]);
                break;
            default:
                break;
        }
    }
    
    
    void PerformManager::schedule(const fzPerform& perform, bool async)
    {
        p_mutex->lock();
        
        m_syncPerforms.push(perform);
        if(async == true) {
            FZLog("ASYNC performing is not implemented yet.");
        }
        
        p_mutex->unlock();
    }
    
    
    void PerformManager::clean()
    {
        p_mutex->lock();

        while (!m_syncPerforms.empty()) {
            execute(m_syncPerforms.front());
            m_syncPerforms.pop();
        }
        
        p_mutex->unlock();
    }
    
    void PerformManager::perform(SELProtocol *target, SELECTOR_FLOAT selector, float withFloat, bool async)
    {
        fzPerform perform;
        perform.target = target;
        perform.valueType = kJPData_float;
        perform.selector.argFloat = selector;
        perform.value.valueFloat = withFloat;
        
        schedule(perform, async);
    }
    
    
    void PerformManager::perform(SELProtocol *target, SELECTOR_VOID selector, bool async)
    {
        fzPerform perform;
        perform.target = target;
        perform.valueType = kJPData_void;
        perform.selector.argVoid = selector;
        
        schedule(perform, async);
    }
    
    
    void PerformManager::perform(SELProtocol *target, SELECTOR_PTR selector, void *withPointer, bool async)
    {
        fzPerform perform;
        perform.target = target;
        perform.valueType = kJPData_ptr;
        perform.selector.argPTR = selector;
        perform.value.ptr[0] = withPointer;
        
        schedule(perform, async);
    }
    
    
    void PerformManager::perform(SELProtocol *target, SELECTOR_2PTR selector, void *withPointer, void *withPointer2, bool async)
    {
        fzPerform perform;
        perform.target = target;
        perform.valueType = kJPData_2ptr;
        perform.selector.arg2PTR = selector;
        perform.value.ptr[0] = withPointer;
        perform.value.ptr[1] = withPointer2;
        
        schedule(perform, async);
    }
}
