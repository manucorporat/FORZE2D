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

#include "FZFont.h"
#include "FZCommon.h"
#include "FZMacros.h"
#include "FZIO.h"
#include "FZTexture2D.h"
#include "FZTextureCache.h"
#include "FZResourcesManager.h"


namespace FORZE {
    
    uint16_t generateKey(uint8_t first, uint8_t second)
    {
        uint16_t key = first; key <<= 8;
        key |= second;
        return key;
    }
    
    fzUInt getSubtrings(char *str, char** lines, fzUInt max)
    {
        fzUInt line = 0;
        bool next = true;
        
        while(*str != '\0') {
            if(next) {
                FZ_ASSERT(line < max, "Too many lines.");
                lines[line] = str;
                ++line;
                next = false;
            }
            
            if( *str == '\n') {
                *str = '\0';
                next = true;
            }
            ++str;
        }
        return line;
    }
    
    
    Font::Font(const char* filename, fzFloat fontHeight)
    : p_texture(NULL)
    {
        FZ_ASSERT(filename != NULL, "Filename cannot be empty.");
        
        const char *extension = fzIO_getExtension(filename);
        if(extension == NULL)
            throw std::runtime_error("Font: Extension is missing");
        
        if(strcasecmp( extension, "fnt") == 0 )
            loadFNTFile(filename);
        
        else if(strcasecmp( extension, "ttf") == 0 )
            loadTTFFile(filename, fontHeight);
        
        else
            throw std::runtime_error("Font: Invalid texture extension");
    }
    
    
    Font::~Font()
    {
        // if pointer is non-NULL, we release it.
        FZ_SAFE_RELEASE(p_texture);
    }
    
    
    void Font::loadFNTFile(const char* filename)
    {
        fzBuffer buffer = ResourcesManager::Instance().loadResource(filename, &m_factor);
        if(buffer.empty())
            throw std::runtime_error("Font: File was not found.");
        
        // Unpack FNT Data
        try {
            loadFNTData(buffer.getPointer());
            buffer.free();
        } catch(std::runtime_error &error) {
            buffer.free();
            throw error;
        }
    }
    
    
    void Font::loadTTFFile(const char* filename, fzFloat fontHeight)
    {
        fzBuffer buffer = ResourcesManager::Instance().loadResource(filename, &m_factor);
        if(buffer.empty())
            throw std::runtime_error("Font: File was not found.");
        
        // Unpack TTF Data
        try {
            loadTTFData(buffer.getPointer(), fontHeight);
            buffer.free();
        } catch(std::runtime_error &error) {
            buffer.free();
            throw error;
        }
    }
    
    
    void Font::loadFNTData(char* data)
    {
#define FNT_NU_LINES 512
        char *lines[FNT_NU_LINES];
        fzUInt nuLines = getSubtrings(data, lines, FNT_NU_LINES);
        int nu_arg;
#undef FNT_NU_LINES
        
        if(nuLines <= 4)
            throw std::runtime_error("Font: Invalid FNT. Missing info.");
        
        // COMMON DATA PARSING
        int nuPages = 0;
        nu_arg = sscanf(lines[1], "common lineHeight=%f base=%*d scaleW=%*d scaleH=%*d pages=%d", &m_lineHeight, &nuPages);
        
        if(nu_arg != 2)
            throw std::runtime_error("Font: Error parsing FNT common data, syntax is not correct.");

        if(nuPages != 1)
            throw std::runtime_error("Font: Number of pages must be 1");

        if(m_lineHeight == 0)
            throw std::runtime_error("Font: Line height parsing error");

        m_lineHeight /= m_factor;
        
        
        // PAGE + TEXTURE DATA PARSING
        char filename[256];
        nu_arg = sscanf(lines[2], "page id=%*d file=\"%s\"", filename);
        
        if(nu_arg != 1)
            throw std::runtime_error("Font: Error parsing FNT page data, syntax is not correct.");
        
        if(filename[0] == '\0')
            throw std::runtime_error("Font: Atlas path parsing error");
        
        filename[strlen(filename)-1] = '\0'; // remove last "
        p_texture = TextureCache::Instance().addImage(filename);
        p_texture->retain();
        
        
        // CHARACTERS DATA PARSING
        for(fzUInt i = 4; i < nuLines; ++i)
        {
            char *l = lines[i];
            
            try {
                
                if(l[0]=='c' && l[1]=='h' && l[2]=='a' &&
                   l[3]=='r' && l[4]==' ') {
                    
                    // CHAR DATA PARSING
                    int charID = 0;
                    fzCharDef temp_char;
                    
                    nu_arg = sscanf(l, "char id=%d x=%f y=%f width=%f height=%f xoffset=%f yoffset=%f xadvance=%f",
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
                    
                    if(nu_arg != 8)
                        throw std::runtime_error("Error parsing FNT char data, syntax is not correct.");
                    
                    if(charID >= 256)
                        throw std::runtime_error("CharID is out of bounds [0, 255]");

                    
                    m_chars[charID] = temp_char;
                    
                }else if(l[0]=='k' && l[1]=='e' && l[2]=='r' && l[3]=='n' &&
                         l[4]=='i' && l[5]=='n' && l[6]=='g' && l[7]==' ') {
                    
                    // KERNING DATA PARSING
                    int first = 0;
                    int second = 0;
                    fzFloat amount = 0;
                    
                    nu_arg = sscanf(l, "kerning first=%d second=%d amount=%f",
                                    &first, &second, &amount);
                    
                    if(first < 0 || second < 0)
                        throw std::runtime_error("Invalid indexes");
                    
                    if(nu_arg != 3)
                        throw std::runtime_error("Error parsing FNT kerning data");
                    
                    
                    uint16_t key = generateKey(first, second);
                    m_kerning.insert(pair<uint16_t, fzFloat>(key, amount));
                }
                
            } catch (std::runtime_error& error) {
                FZLOGERROR("%s", error.what());
            }
        }
    }
    
    
    void Font::loadTTFData(char* data, fzFloat fontHeight)
    {
        FZLog("TTF NOT IMPLEMENTED");
    }
    
    
    fzFloat Font::getKerning(unsigned char first, unsigned char second) const
    {
        uint16_t key = generateKey(first, second);
        map<uint16_t, fzFloat>::const_iterator it(m_kerning.find(key));
        return (it == m_kerning.end()) ? 0.0f : it->second;
    }
    
}
