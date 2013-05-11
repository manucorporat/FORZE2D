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

#include <string.h>

#include "FZFontCache.h"
#include "FZFont.h"
#include "FZMacros.h"
#include "FZIO.h"

using namespace STD;


namespace FORZE {
    
    FontCache* FontCache::p_instance = NULL;
    
    FontCache& FontCache::Instance()
    {
        if (p_instance == NULL)
            p_instance = new FontCache();
        
        return *p_instance;
    }
    
    
    FontCache::FontCache()
    : m_fonts()
    { }
    
    
    Font* FontCache::addFont(const char* filename, fzFloat lineHeight)
    {
        FZ_ASSERT(filename != NULL, "Filename argument must be non-NULL.");
        
        // Make string mutable
        char *filenameCpy = fzStrcpy(filename);
        
        // Remove "-x" suffix
        IO::removeFileSuffix(filenameCpy);
        
        uint32_t hash = fzHash(filenameCpy);
        Font *font = getFontForHash(hash);
        
        if(font != NULL && strcmp(IO::getExtension(filenameCpy), "ttf") == 0) {
            // Rewritting
            FZ_ASSERT(lineHeight > 0, "Line height must me positive.");
            FZLog("NOT IMPLEMENTED");
        }
        
        if(font == NULL) {
            
            try {
                font = new Font(filenameCpy, lineHeight);
                font->retain();
                m_fonts.insert(fontsPair(hash, font));

            } catch(std::exception& error) {
                delete filenameCpy;

                FZLOGERROR("%s", error.what());
                return NULL;
            }
        }
        delete filenameCpy;
        
        return font;
    }
    
    
    Font* FontCache::getFontForHash(uint32_t hash) const
    {
        fontsMap::const_iterator it(m_fonts.find(hash));
        if(it == m_fonts.end())
            return NULL;
        
        return it->second;      
    }
    
    
    Font* FontCache::getFontByName(const char* filename) const
    {
        FZ_ASSERT(filename, "Filename can not be NULL.");
        return getFontForHash(fzHash(filename));
    }
    
    
    void FontCache::removeFont(Font *font)
    {
        FZ_ASSERT(font, "Font can not be NULL.");

        fontsMap::iterator it(m_fonts.begin());
        for(; it != m_fonts.end(); ++it) {
            if(it->second == font) {
                font->release();
                m_fonts.erase(it);
                break;
            }
        }
    }
    
    
    void FontCache::removeFontByName(const char* filename)
    {
        FZ_ASSERT(filename, "Filename can not be NULL.");

        uint32_t hash = fzHash(filename);
        Font *font = getFontForHash(hash);
        if(font) {
            m_fonts.erase(hash);
            font->release();
        }
    }
    
    
    void FontCache::removeUnusedFonts()
    {
        fontsMap::iterator it(m_fonts.begin());
        for(; it != m_fonts.end(); ) {
            if(it->second->retainCount() <= 1) {
                it->second->release();
                m_fonts.erase(it++);
            }else
                ++it;
        }
    }
    
    
    void FontCache::removeAllFonts()
    {
        fontsMap::iterator it(m_fonts.begin());
        for(; it != m_fonts.end(); ++it)
            it->second->release();
        
        m_fonts.clear();
    }
}
