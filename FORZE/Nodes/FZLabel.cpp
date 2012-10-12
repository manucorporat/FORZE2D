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
    void printChar(char *toPrint, char c)
    {
        switch(c)
        {
            case '\n':
                toPrint[0] = '\\';
                toPrint[1] = 'n';
                toPrint[2] = '\0';
                break;
            case '\t':
                toPrint[0] = '\\';
                toPrint[1] = 't';
                toPrint[2] = '\0';
                break;
            case '\v':
                toPrint[0] = '\\';
                toPrint[1] = 'v';
                toPrint[2] = '\0';
                break;
            case '\b':
                toPrint[0] = '\\';
                toPrint[1] = 'b';
                toPrint[2] = '\0';
                break;
            case '\r':
                toPrint[0] = '\\';
                toPrint[1] = 'r';
                toPrint[2] = '\0';
                break;
            case '\f':
                toPrint[0] = '\\';
                toPrint[1] = 'f';
                toPrint[2] = '\0';
                break;
            case '\a':
                toPrint[0] = '\\';
                toPrint[1] = 'f';
                toPrint[2] = '\0';
                break;
            default:
                toPrint[0] = c;
                toPrint[1] = '\0';
                break;
        }
    }
    
    Label::Label()
    : SpriteBatch(NULL)
    , m_string()
    , m_color(fzWHITE)
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
        setFont(NULL);
        setTexture(NULL);
    }
    
    
    void Label::setFont(Font* font)
    {
        FZRETAIN_TEMPLATE(font, p_font);

        if(font)
            setTexture(font->getTexture());
        
        createFontChars();
    }
    
    
    void Label::setString(const char* str)
    {
        if(str == NULL)
            m_string.clear();
        else
            m_string.assign(str);
        
        createFontChars();
    }
    
    
    void Label::setColor(const fzColor3B& color)
    {
        m_color = color;
        Sprite *sprite = static_cast<Sprite*>(m_children.front());
        for(fzUInt i = 0; i < m_string.size() && sprite; ++i, sprite = static_cast<Sprite*>(sprite->next()))
        {
            sprite->setColor(color);
        }
    }
    
    
    void Label::createFontChars()
    {
        fzUInt m_stringLen = m_string.size();
        if(m_stringLen == 0)
            return;
    
        if(p_font == NULL) {
            FZLOGERROR("Label: Impossible to generate label, font config is missing.");
            return;
        }        
        
        fzUInt quantityOfLines = 1;
        for(fzUInt i = 0; i < m_stringLen; ++i) {
            char charID = m_string[i];
            if( charID == '\n')
                ++quantityOfLines;
        }
        
        fzUInt totalHeight = p_font->getLineHeight() * quantityOfLines;
        fzInt nextFontPositionY = p_font->getLineHeight() * (quantityOfLines - 1);
        fzInt nextFontPositionX = 0;
        fzInt longestLine = 0;
        
        char charId = 0, prevId = 0;
        
        Sprite *fontChar = static_cast<Sprite*>(m_children.front());
        for(fzUInt i = 0; i < m_stringLen; ++i)
        {
            charId = m_string.at(i);
            if(charId < 0) {
                FZLOGERROR("Label: CHAR[%d] doesn't exist. It's negative.", charId);
                continue;
            }

            
            if (charId == '\n') {
                nextFontPositionX = 0;
                nextFontPositionY -= p_font->getLineHeight();
                continue;
            }
            
            const fzCharDef& fontDef = p_font->getCharInfo(static_cast<unsigned char>(charId));
            if(fontDef.xAdvance == 0) {
                char toPrint[3];
                printChar(toPrint, charId);
                FZLOGERROR("Label: CHAR[%d] '%s' is not included.", charId, toPrint);
                nextFontPositionX += p_font->getLineHeight();
                continue;
            }
            
            
            if( fontChar == NULL ) {
                fontChar = new Sprite(fontDef.getRect());
                addChild(fontChar);
                
            }else {
                // reusing sprites
                fontChar->setTextureRect(fontDef.getRect());
                fontChar->setIsVisible(true);
                fontChar->setAlpha(255);
            }
            
            // font kerning
            nextFontPositionX += p_font->getKerning(prevId, charId);
            
            fzFloat yOffset = p_font->getLineHeight() - fontDef.yOffset;
            fzPoint fontPos = fzPoint(nextFontPositionX + fontDef.xOffset + fontDef.width * 0.5f,
                                    nextFontPositionY + yOffset - fontDef.height * 0.5f );
            
            fontChar->setPosition(fontPos);
            fontChar->setColor(m_color);
            
            
            // update kerning
            nextFontPositionX += fontDef.xAdvance;
            prevId = charId;
            
            if (longestLine < nextFontPositionX)
                longestLine = nextFontPositionX;
            
            fontChar = static_cast<Sprite*>(fontChar->next());
        }
        
        // make sprites not longer used hidden.
        for(; fontChar; fontChar = static_cast<Sprite*>(fontChar->next()))
            fontChar->setIsVisible(false);
        
        // new content size
        setContentSize(fzSize(longestLine, totalHeight));
    }
}
