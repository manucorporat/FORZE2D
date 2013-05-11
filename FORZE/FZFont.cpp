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

#include <string.h>
#include "FZFont.h"
#include "FZCommon.h"
#include "FZMacros.h"
#include "FZIO.h"
#include "FZTexture2D.h"
#include "FZTextureCache.h"
#include "FZResourcesManager.h"


namespace FORZE {
    
    static uint16_t generateKey(uint8_t first, uint8_t second)
    {
        uint16_t key = first; key <<= 8;
        key |= second;
        return key;
    }
    
    static void firstWord(const char *str, char *output)
    {
        while(*str != ' ') {
            *output = *str;
            ++output;
            ++str;
        }
        *output = '\0';
    }
    
    
    Font::Font(const char* filename, fzFloat fontHeight)
    : p_texture(NULL)
    {
        FZ_ASSERT(filename != NULL, "Filename cannot be empty.");
        
        const char *extension = IO::getExtension(filename);
        if(extension == NULL)
            FZ_RAISE_STOP("Font: Extension is missing.");
        
        
        if(strcasecmp(extension, "fnt") == 0 )
            loadFNTFile(filename);
        
        else if(strcasecmp(extension, "ttf") == 0 )
            loadTTFFile(filename, fontHeight);
        
        else
            FZ_RAISE_STOP("Font: Invalid font extension.");
    }
    
    
    Font::~Font()
    {
        // if pointer is non-NULL, we release it.
        FZ_SAFE_RELEASE(p_texture);
    }
    
    
    void Font::loadFNTFile(const char* filename)
    {
        fzBuffer buffer = ResourcesManager::Instance().loadResource(filename, &m_factor);
        
        // Unpack FNT Data
        try {
            loadFNTData(buffer.getPointer());
            buffer.free();
            
        } catch(...) {
            buffer.free();
            throw;
        }
    }
    
    
    void Font::loadTTFFile(const char* filename, fzFloat fontHeight)
    {
        fzBuffer buffer = ResourcesManager::Instance().loadResource(filename, &m_factor);
        
        // Unpack TTF Data
        try {
            loadTTFData(buffer.getPointer(), fontHeight);
            buffer.free();
            
        } catch(...) {
            buffer.free();
            throw;
        }
    }
    
    
    void Font::loadFNTData(char* data)
    {
        if(data == NULL)
            FZ_RAISE("Font:FNT: Imposible to load FNT data. Pointer is NULL.");
        
        int line = 1;
        char word[30];
        bool parsedCommon = false;
        bool parsedPage = false;
        
        while(*data != '\0') {
        
            firstWord(data, word);
            
            switch(fzHash(word)) {
                case "common"_hash:
                {
                    int nuPages = 0;
                    int nu_arg = sscanf(data, "common lineHeight=%f base=%*d scaleW=%*d scaleH=%*d pages=%d", &m_lineHeight, &nuPages);
                    
                    if(nu_arg != 2)
                        FZ_RAISE_STOP("Font:FNT: Line 2. Sintax error. Error parsing FNT common data.");
                    
                    if(nuPages != 1)
                        FZ_RAISE_STOP("Font:FNT: Line 2. Number of pages must be 1.");
                    
                    if(m_lineHeight == 0)
                        FZ_RAISE_STOP("Font:FNT: Line 2. Line height parsing error.");
                    
                    m_lineHeight /= m_factor;
                    parsedCommon = true;
                    
                    break;
                }
                case "page"_hash:
                {
                    char filename[256];
                    int nu_arg = sscanf(data, "page id=%*d file=\"%s\"", filename);
                    
                    if(nu_arg != 1)
                        FZ_RAISE_STOP("Font:FNT: Line 3. Sintax erro. Error parsing FNT page data.");
                    
                    if(filename[0] == '\0')
                        FZ_RAISE_STOP("Font:FNT: Line 3. texture's path is missing.");
                    
                    filename[strlen(filename)-1] = '\0'; // remove last "
                    p_texture = TextureCache::Instance().addImage(filename);
                    if(p_texture == NULL)
                        FZ_RAISE("Font:FNT: Font's texture is missing.");
                    
                    p_texture->retain();
                    parsedPage = true;
                    
                    break;
                }
                case "char"_hash:
                {
                    // CHAR DATA PARSING
                    int charID = 0;
                    fzCharDef temp_char;
                    
                    int nu_arg = sscanf(data, "char id=%d x=%f y=%f width=%f height=%f xoffset=%f yoffset=%f xadvance=%f",
                                    &charID,
                                    &temp_char.x, &temp_char.y,
                                    &temp_char.width, &temp_char.height,
                                    &temp_char.xOffset, &temp_char.yOffset,
                                    &temp_char.xAdvance);
                    
                    temp_char.x         /= m_factor;
                    temp_char.y         /= m_factor;
                    temp_char.width     /= m_factor;
                    temp_char.height    /= m_factor;
                    temp_char.xOffset   /= m_factor;
                    temp_char.yOffset   /= m_factor;
                    temp_char.xAdvance  /= m_factor;
                    
                    if(nu_arg != 8) {
                        FZLOGERROR("Font:FNT: Line %d. Error parsing FNT char data, syntax is not correct.", line);
                        break;
                    }
                    
                    if(charID >= 256) {
                        FZLOGERROR("Font:FNT: Line %d. CharID is out of bounds [0, 255].", line);
                        break;
                    }
                    
                    m_chars[charID] = temp_char;
                    break;
                }
                case "kerning"_hash:
                {
                    // KERNING DATA PARSING
                    int first = 0;
                    int second = 0;
                    fzFloat amount = 0;
                    
                    int nu_arg = sscanf(data, "kerning first=%d second=%d amount=%f",
                                    &first, &second, &amount);
                    
                    if(first < 0 || second < 0 || first > 255 || second > 255) {
                        FZLOGERROR("Font:FNT: Line %d. Invalid indexes.", line);
                        break;
                    }
                    if(nu_arg != 3) {
                        FZLOGERROR("Font:FNT: Line %d. Error parsing FNT kerning data.", line);
                        break;
                    }
                    
                    uint16_t key = generateKey((uint8_t)first, (uint8_t)second);
                    m_kerning.insert(pair<uint16_t, fzFloat>(key, amount));
                    
                    break;
                }
            }
            
            
            while(true) {
                if(*data != '\0') {
                    if(*(data++) == '\n'){
                        ++line;
                        break;
                    }
                }
            }
        }
        if(!parsedCommon)
            FZ_RAISE_STOP("Font:FNT: FNT common data not found.");
        
        if(!parsedPage)
            FZ_RAISE_STOP("Font:FNT: FNT page data not found.");
    }
    
    
    void Font::loadTTFData(char* data, fzFloat fontHeight)
    {
        if(data == NULL)
            FZ_RAISE("Font:TTF: Imposible to load TTF data. Pointer is NULL.");
        
        FZ_RAISE_STOP("Font:TTF: TTF NOT IMPLEMENTED.");
    }
    
    
    fzFloat Font::getKerning(unsigned char first, unsigned char second) const
    {
        uint16_t key = generateKey(first, second);
        map<uint16_t, fzFloat>::const_iterator it(m_kerning.find(key));
        return (it == m_kerning.end()) ? 0.0f : it->second;
    }
    
    
    void Font::log() const
    {
        printf(FORZE_SIGN "Font ( %p ):\n"
              " - Texture: %p\n"
              " - Line height: %.2f\n"
              " - Factor: %d\n"
              " - Chars:", this, p_texture, m_lineHeight, m_factor);
        
        for(int i = 0; i < 256; ++i) {
            if(m_chars[i].xAdvance > 0)
                printf("%c", i);
        }
        printf("\n\n");
    }
    
}
