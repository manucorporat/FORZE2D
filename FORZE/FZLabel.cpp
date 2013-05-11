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

#include "FZLabel.h"
#include "FZSprite.h"
#include "FZMacros.h"
#include "FZFontCache.h"
#include "FZFont.h"


using namespace STD;

namespace FORZE {
    
    // Auxiliar function to print special charaters properly, suck as /n /t...
    static void printChar(char *array, char c)
    {
        switch(c)
        {
            case '\n':
                array[0] = '\\';
                array[1] = 'n';
                array[2] = '\0';
                break;
            case '\t':
                array[0] = '\\';
                array[1] = 't';
                array[2] = '\0';
                break;
            case '\v':
                array[0] = '\\';
                array[1] = 'v';
                array[2] = '\0';
                break;
            case '\b':
                array[0] = '\\';
                array[1] = 'b';
                array[2] = '\0';
                break;
            case '\r':
                array[0] = '\\';
                array[1] = 'r';
                array[2] = '\0';
                break;
            case '\f':
                array[0] = '\\';
                array[1] = 'f';
                array[2] = '\0';
                break;
            case '\a':
                array[0] = '\\';
                array[1] = 'f';
                array[2] = '\0';
                break;
            default:
                array[0] = c;
                array[1] = '\0';
                break;
        }
    }
    
    Label::Label()
    : SpriteBatch(NULL)
    , m_string()
    , m_color(fzWHITE)
    , m_alignment(kFZLabelAlignment_left)
    , m_verticalPadding(2)
    , m_letterSpacing(0)
    , p_font(NULL)
    {
        setIsRelativeAnchorPoint(true);
        setAnchorPoint(0.5f, 0.5f);
    }
    
    
    Label::Label(const char* text, const char* fontFilename, fzFloat lineHeight)
    : Label()
    {
        Font *font = FontCache::Instance().addFont(fontFilename, lineHeight);
        setFont(font);
        setString(text);
    }
               
    
    Label::Label(const char* str, const char* fontFilename)
    : Label(str, fontFilename, 0)
    { }
    
    
    Label::~Label()
    {
        FZ_SAFE_RELEASE(p_font);
    }
    
    
    void Label::setFont(Font* font)
    {
        FZRETAIN_TEMPLATE(font, p_font);

        if(font) {
            setTexture(font->getTexture());
            createFontChars();
        }
    }
    
    
    void Label::setString(const char* str)
    {
        if(str == NULL)
            m_string.clear();
        else
            m_string.assign(str);
        
        createFontChars();
    }
    
    
    void Label::setVerticalPadding(fzFloat vertical)
    {
        if(vertical != m_verticalPadding) {
            m_verticalPadding = vertical;
            createFontChars();
        }
    }
    
    
    void Label::setLetterSpacing(fzFloat horizontal)
    {
        if(horizontal != m_letterSpacing) {
            m_letterSpacing = horizontal;
            createFontChars();
        }
    }
    
    
    void Label::setAlignment(fzLabelAlignment alignment)
    {
        if(alignment != m_alignment) {
            m_alignment = alignment;
            createFontChars();
        }
    }
    
    
    void Label::setColor(const fzColor3B& color)
    {
        m_color = color;
        Sprite *sprite = static_cast<Sprite*>(m_children.front());
        for(fzUInt i = 0; (i < m_string.size()) && sprite; ++i, sprite = static_cast<Sprite*>(sprite->next())) {
            sprite->setColor(color);
        }
    }
    
    
    const fzColor3B& Label::getColor() const
    {
        return m_color;
    }
    
    
    void Label::createFontChars()
    {
        // Get string length
        fzUInt m_stringLen = m_string.size();
    
        Sprite *fontChar = static_cast<Sprite*>(m_children.front());
        
        if(m_stringLen == 0) {
            setContentSize(FZSizeZero);
            goto clean;
        
        }else
        {
            if(p_font == NULL) {
                FZLOGERROR("Label: Impossible to generate label, font config is missing.");
                return;
            }
            
            // Precalculate label size
            const char *string = m_string.c_str();
            char charId = 0, prevId = 0;
            fzUInt i;
            fzUInt currentLine = 0;
            fzFloat longestLine = 0;
            fzFloat lineWidth[100];
            lineWidth[0] = 0;
            
            for(i = 0; i <= m_stringLen; ++i) {
                charId = string[i];
                if(charId < 0)
                    FZ_RAISE("Label: CHAR[] doesn't exist. It's negative.");
                
                if(charId == '\n' || charId == '\0') {
                    longestLine = fzMax(longestLine, lineWidth[currentLine]);
                    lineWidth[++currentLine] = 0;
                }else{
                    lineWidth[currentLine] += p_font->getCharInfo(charId).xAdvance + m_letterSpacing + p_font->getKerning(prevId, charId);
                    prevId = charId;
                }
            }
            
            
            fzFloat lineHeight = p_font->getLineHeight() + m_verticalPadding;
            fzFloat totalHeight = lineHeight * currentLine;
            
            fzFloat nextFontPositionY = totalHeight - lineHeight;
            fzFloat nextFontPositionX = -1;
            
            prevId = 0; currentLine = 0;
            for(i = 0; i < m_stringLen; ++i)
            {
                charId = string[i];

                // line jump
                if (charId == '\n') {
                    nextFontPositionY -= lineHeight;
                    nextFontPositionX = -1;
                    ++currentLine;
                    continue;
                }
                
                // config line start point
                if (nextFontPositionX == -1)
                {
                    switch (m_alignment) {
                        case kFZLabelAlignment_right:
                            nextFontPositionX = longestLine - lineWidth[currentLine];
                            break;
                        case kFZLabelAlignment_center:
                            nextFontPositionX = (longestLine - lineWidth[currentLine])/2.0f;
                            break;
                        default:
                            FZLOGERROR("Label: %d is not a valid aligment.", m_alignment);
                        case kFZLabelAlignment_left:
                            nextFontPositionX = 0;
                            break;
                    }
                }
                
                // get font def
                const fzCharDef& fontDef = p_font->getCharInfo(charId);
                if(fontDef.xAdvance == 0) {
                    char toPrint[3];
                    printChar(toPrint, charId);
                    FZLOGERROR("Label: CHAR[%d] '%s' is not included.", charId, toPrint);
                    nextFontPositionX += p_font->getLineHeight();
                    continue;
                }
                
                // get sprite
                if( fontChar == NULL ) {
                    fontChar = new Sprite();
                    addChild(fontChar);
                    
                }else {
                    // reusing sprites
                    fontChar->setIsVisible(true);
                }
                
                // config sprite
                nextFontPositionX += p_font->getKerning(prevId, charId);
                fzFloat yOffset = p_font->getLineHeight() - fontDef.yOffset;
                fzPoint fontPos = fzPoint(nextFontPositionX + fontDef.xOffset + fontDef.width * 0.5f,
                                          nextFontPositionY + yOffset - fontDef.height * 0.5f );
                
                fontChar->setTextureRect(fontDef.getRect());
                fontChar->setPosition(fontPos);
                fontChar->setColor(m_color);
                
                
                // next sprite
                nextFontPositionX += fontDef.xAdvance + m_letterSpacing;
                prevId = charId;
                fontChar = static_cast<Sprite*>(fontChar->next());
            }
            
            // new content size
            setContentSize(fzSize(longestLine, totalHeight));
        }
        
    clean:
        
        // make sprites not longer used hidden.
        for(; fontChar; fontChar = static_cast<Sprite*>(fontChar->next()))
            fontChar->setIsVisible(false);
    }
}
