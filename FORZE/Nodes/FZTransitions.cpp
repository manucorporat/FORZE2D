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

#include "FZTransitions.h"
#include "FZDirector.h"
#include "FZEventManager.h"
#include "FZMacros.h"


namespace FORZE {
    
    Transition::Transition(fzFloat duration, Scene *scene)
    {
        FZ_ASSERT(duration >= 0, "Duration cannot be negative.");
        FZ_ASSERT(scene, "Argumment cannot be NULL.");

        p_inScene = scene;
        p_inScene->retain();
        
        p_outScene = Director::Instance().getRunningScene();
        p_outScene->retain();
        
        
        EventManager::Instance().setIsEnabled(false);
    }
    
    
    Transition::~Transition()
    {
        // if pointer is non-NULL, we release it.
        FZ_SAFE_RELEASE(p_inScene);
        FZ_SAFE_RELEASE(p_outScene);
    }
    
    
    void Transition::finish()
    {
        p_inScene->setIsVisible(true);
        p_inScene->setPosition(FZPointZero);
        p_inScene->setScale(1.0f);
        p_inScene->setRotation(0);
        //[inScene_.camera restore];

        p_outScene->setIsVisible(false);
        p_outScene->setPosition(FZPointZero);
        p_outScene->setScale(1.0f);
        p_outScene->setRotation(0);
        //[outScene_.camera restore];
        
        schedule(SEL_FLOAT(Transition::setNewScene), 0);        
    }
    
    
    void Transition::setNewScene(fzFloat dt)
    {
        unschedule(SEL_FLOAT(Transition::setNewScene));
        
        Director &director = Director::Instance();
        //m_sendCleanupToScene = director.i
        // Before replacing, save the "send cleanup to scene"
        //sendCleanupToScene_ = [director sendCleanupToScene];
        director.replaceScene(p_inScene);

        // enable events while transitions
        EventManager::Instance().setIsEnabled(true);

        // issue #267
        p_outScene->setIsVisible(true);
    }
    
    
    void Transition::draw()
    {
        Scene::draw();
        if(m_inSceneOnTop) {
            p_outScene->visit();
            p_inScene->visit();
        }else{
            p_inScene->visit();
            p_outScene->visit();
        }
    }
    
    
    void Transition::onEnter()
    {
        Scene::onEnter();
        
        // outScene_ should not receive the onExit callback
        // only the onExitTransitionDidStart
        p_outScene->onExit();
        //[outScene_ onExitTransitionDidStart];
        
        p_inScene->onEnter();
    }
        
    
    void Transition::onExit()
    {
        Scene::onExit();
        p_outScene->onExit();
        
        // inScene_ should not receive the onEnter callback
        // only the onEnterTransitionDidFinish
        p_inScene->onEnter();
        //[inScene_ onEnterTransitionDidFinish];
    }
    
    
    void Transition::cleanup()
    {
        Scene::cleanup();
        if(m_sendCleanupToScene)
            p_outScene->cleanup();
    }
}
