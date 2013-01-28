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
#include <stdlib.h>
#include "FZHash.h"


namespace FORZE {
    
    int32_t fzHash(const char *str, size_t length)
    {
        int32_t hash = 5381;
        while (length > 0) {
            hash = __HASH_FUNCTION(hash, *str);
            ++str;
            --length;
        }
        return hash;
    }
    
    
    uint32_t fzVersion(const char *str)
    {
        uint32_t hash = 0;
		uint32_t idx = 0x01000000;
        char *end = NULL;
        do {
			int value = strtol(str, &end, 10);
			hash += value * idx;
			idx = idx >> 8;
            str = end+1;
		}while(*end != '\0');
        
        return hash;
    }
    
    
    int32_t fzHash(const char *str)
    {
        return fzHash(str, strlen(str));
    }
}
