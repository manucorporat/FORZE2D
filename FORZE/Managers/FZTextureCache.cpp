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

#include "FZTextureCache.h"
#include "FZTexture2D.h"
#include "FZMacros.h"
#include "FZIO.h"

using namespace STD;

namespace FORZE {
    
    TextureCache* TextureCache::p_instance = NULL;
    
    TextureCache& TextureCache::Instance()
    {
        if (p_instance == NULL)
            p_instance = new TextureCache();
        
        return *p_instance;
    }
    
    
    TextureCache::TextureCache()
    : m_textures()
    { }
    
    
    Texture2D* TextureCache::addImage(const char* filename)
    {
        FZ_ASSERT(filename != NULL, "fileimage argument must be non-NULL");
        
        // Make string mutable
        char *filenameCpy = fzStrcpy(filename);
        
        // Remove "-x" suffix
        IO::removeFileSuffix(filenameCpy);
        
        
        int32_t hash = fzHash(filenameCpy);
        Texture2D *tex = getTextureForHash(hash);
        
        if( ! tex ) {
            
            try {
                tex = new Texture2D(filenameCpy);
                tex->retain();
                m_textures.insert(texturesPair(hash, tex));
                
            }catch(std::exception& error) {
                free(filenameCpy);
                FZLOGERROR("%s", error.what());
                return NULL;
            }
        }
        delete filenameCpy;
        return tex;
    }
    
    
    Texture2D* TextureCache::getTextureForHash(int32_t hash) const
    {
        texturesMap::const_iterator it(m_textures.find(hash));
        if(it == m_textures.end())
            return NULL;
        
        return it->second;   
    }
    
    
    Texture2D* TextureCache::getTextureForFilename(const char* filename) const
    {
        FZ_ASSERT(filename, "Filename can not be NULL");
        return getTextureForHash(fzHash(filename)); 
    }
    
    
    void TextureCache::removeTexture(Texture2D *tex)
    {
        texturesMap::iterator it(m_textures.begin());
        for(; it != m_textures.end(); ++it) {
            if(it->second == tex) {
                tex->release();
                m_textures.erase(it);
                break;
            }
        }
    }
    
    
    void TextureCache::removeTextureForFilename(const char* filename)
    {
        FZ_ASSERT(filename, "Filename can not be NULL");

        int32_t hash = fzHash(filename);
        Texture2D *tex = getTextureForHash(hash);
        if(tex) {
            m_textures.erase(hash);
            tex->release();
        }
    }
    
    
    void TextureCache::removeUnusedTextures()
    {
        texturesMap::iterator it(m_textures.begin());
        for(; it != m_textures.end(); )
        {
            if(it->second->retainCount() <= 1) {
                it->second->release();
                m_textures.erase(it++);
            }else
                ++it;
        }
    }
    
    
    void TextureCache::removeAllTextures()
    {
        texturesMap::const_iterator it(m_textures.begin());
        for(; it != m_textures.end(); ++it)
            it->second->release();
        
        m_textures.clear();
    }
}
