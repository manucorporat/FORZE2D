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

#include "FZAnimationCache.h"
#include "FZSprite.h"
#include "FZMacros.h"


using namespace STD;

namespace FORZE {

    AnimationCache* AnimationCache::p_instance = NULL;
    
    AnimationCache& AnimationCache::Instance()
    {
        if (p_instance == NULL)
            p_instance = new AnimationCache();
        
        return *p_instance;
    }
    
    AnimationCache::AnimationCache()
    : m_animations()
    { }
    
    
    void AnimationCache::addAnimation(Animation *animation, const char* name)
    {
        m_animations.insert(animationPair(fzHash(name), animation));
    }
    
    
    void AnimationCache::removeAnimationByName(const char* name)
    {
        FZ_ASSERT(name != NULL, "AnimationCache: Name can't be empty");
        m_animations.erase(fzHash(name));
    }
    
    
    Animation* AnimationCache::getAnimationByName(const char* name) const
    {
        FZ_ASSERT(name != NULL, "AnimationCache: Name can't be empty");

        animationMap::const_iterator it(m_animations.find(fzHash(name)));
        if(it == m_animations.end())
            return NULL;
        
        return it->second;
    }
}
