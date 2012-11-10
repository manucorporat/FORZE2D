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

#include "FZAnimation.h"
#include "FZTextureCache.h"
#include "FZTexture2D.h"


using namespace STD;

namespace FORZE {
    
    Animation::Animation(const vector<fzAnimationFrame>& frames, fzFloat delayPerUnit, fzUInt loops)
    : m_frames(frames)
    , m_delayPerUnit(delayPerUnit)
    , m_loops(loops)
    , m_totalDelayUnits(0)
    {
        vector<fzAnimationFrame>::const_iterator it(m_frames.begin());
        for(; it != m_frames.end(); ++it)
			m_totalDelayUnits += it->delay;
    }
    
    
    Animation::Animation(const vector<fzSpriteFrame>& frames, fzFloat delay)
    : m_loops(1)
    , m_delayPerUnit(delay)
    , m_frames()
    {
        vector<fzSpriteFrame>::const_iterator it(frames.begin());
        for(; it != frames.end(); ++it)
            addFrame(*it); 
    }
    
    
    void Animation::setDelay(fzFloat d)
    {
        m_delayPerUnit = d; 
    }
    
    
    fzFloat Animation::getDelay() const
    {
        return m_delayPerUnit;
    }
    
    
    fzUInt Animation::getLoops() const
    {
        return m_loops;
    }
    
    
    fzFloat Animation::getTotalDelayUnit() const
    {
        return m_totalDelayUnits;
    }
    
    
    bool Animation::restoreOriginalFrame() const
    {
        return m_restoreOriginalFrame;
    }
    
    
    fzFloat Animation::getDuration() const
    {
        return m_totalDelayUnits * m_delayPerUnit;
    }
    
    
    const vector<fzAnimationFrame>& Animation::getFrames() const
    {
        return m_frames;
    }
    
    
    void Animation::addFrame(const fzSpriteFrame& f)
    {
        addFrame(fzAnimationFrame(f, 1, NULL));
    }
    
    
    void Animation::addFrame(const fzAnimationFrame& f)
    {
        m_frames.push_back(f);
        m_totalDelayUnits += f.delay;
    }
    
    
    void Animation::addFrameWithFilename(const char* filename)
    {
        Texture2D *texture = TextureCache::Instance().addImage(filename);
        fzRect rect = FZRectZero;
        rect.size = texture->getContentSize();
        
        addFrameWithTexture(texture, rect);
    }
    
    
    void Animation::addFrameWithTexture(Texture2D *texture, const fzRect& rect)
    {
        addFrame(fzSpriteFrame(texture, rect));
    }
}
