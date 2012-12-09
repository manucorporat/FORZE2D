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

#include "FZSpriteFrame.h"
#include "FZMacros.h"
#include "FZTexture2D.h"
#include "FZTextureCache.h"


namespace FORZE {
    
    fzSpriteFrame::fzSpriteFrame()
    : p_texture(NULL)
    , m_rect(FZRectZero)
    , m_offset(FZPointZero)
    , m_originalSize(FZSizeZero)
    , m_isRotated(false)
    { }
    

    fzSpriteFrame::fzSpriteFrame(Texture2D *t, const fzRect& r, const fzPoint& o, const fzSize& s, bool ro)
    : p_texture(NULL)
    , m_rect(r)
    , m_offset(o)
    , m_originalSize(s)
    , m_isRotated(ro)
    {
        FZ_ASSERT(m_originalSize.width >= m_rect.size.width && m_originalSize.height >= m_rect.size.height, "Original size can not be smaller than the trimmed size.");

        setTexture(t);
    }
    
    
    fzSpriteFrame::fzSpriteFrame(Texture2D *texture, const fzRect& rect, const fzPoint& offset)
    : fzSpriteFrame(texture, rect, offset, rect.size, false)
    { }
    
    
    fzSpriteFrame::fzSpriteFrame(Texture2D *texture, const fzRect& rect)
    : fzSpriteFrame(texture, rect, FZPointZero, rect.size, false)
    { }
    
    
    fzSpriteFrame::fzSpriteFrame(const fzSpriteFrame& f)
    : fzSpriteFrame(f.getTexture(), f.getRect(), f.getOffset(), f.getOriginalSize(), f.isRotated())
    { }
    
    
    fzSpriteFrame::~fzSpriteFrame()
    {
        setTexture(NULL);
    }
    
    
    void fzSpriteFrame::setRect(const fzRect& r)
    {
        m_rect = r;
    }
    
    
    void fzSpriteFrame::setIsRotated(bool r)
    {
        m_isRotated = r;
    }
    
    
    void fzSpriteFrame::setOffset(const fzPoint& o)
    {
        m_offset = o;
    }
    
    
    void fzSpriteFrame::setOriginalSize(const fzSize& os)
    {
        m_originalSize = os;
    }
    
    
    void fzSpriteFrame::setTexture(Texture2D *texture)
    {
        FZRETAIN_TEMPLATE(texture, p_texture);
    }
    
    
    const fzRect& fzSpriteFrame::getRect() const
    {
        return m_rect;
    }
    
    
    bool fzSpriteFrame::isRotated() const
    {
        return m_isRotated;
    }
    
    
    const fzPoint& fzSpriteFrame::getOffset() const
    {
        return m_offset;
    }
    
    
    const fzSize& fzSpriteFrame::getOriginalSize() const
    {
        return m_originalSize;
    }
    
    
    Texture2D* fzSpriteFrame::getTexture() const
    {
        return p_texture;
    }
    
    
    bool fzSpriteFrame::operator == (const fzSpriteFrame& frame) const
    {
        return (m_rect == frame.getRect()
        && m_offset == frame.getOffset()
        && m_originalSize == frame.getOriginalSize()
        && p_texture->getName() == frame.getTexture()->getName());
    }
    
    
    bool fzSpriteFrame::operator != (const fzSpriteFrame& frame) const
    {
        return !(operator == (frame));
    }
}
