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

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include "FZCommon.h"


namespace FORZE {
    
#define FZLOG_NU 6
#define FZLOG_SIZE 512
    static int _currentPacket = 0;
    static char _szBuf[FZLOG_NU][FZLOG_SIZE];

    
    const char* FZTLocalized(const char *key)
    {
        return key;
    }
    
    
    int32_t fzHash(const char *str, size_t length)
    {
        int32_t hash = 5381;
        
        while (length > 0) {
            int32_t c = *str;
            hash = ((hash << 5) + hash) + c;
            
            ++str;
            --length;
        }
        return hash;
    }
    
    
    int32_t fzHash(const char *str)
    {
        return fzHash(str, strlen(str));
    }

    
    char* FZT(const char *pszFormat, ...)
    {
        ++_currentPacket;
        if(_currentPacket >= FZLOG_NU)
            _currentPacket = 0;

        va_list ap;
        va_start(ap, pszFormat);
#if FZ_STL_CPLUSPLUS11
        vsnprintf(_szBuf[_currentPacket], FZLOG_SIZE, pszFormat, ap);
#else
        vsprintf(_szBuf[_currentPacket], pszFormat, ap);
#endif
        va_end(ap);
        return _szBuf[_currentPacket];
    }
    
    
    void FZLog(const char * pszFormat, ...)
    {
        ++_currentPacket;
        if(_currentPacket >= FZLOG_NU)
            _currentPacket = 0;
        
#define SIGN_TEXT "FORZE_E: "
#define SIGN_LEN 9
        va_list ap;
        va_start(ap, pszFormat);
        memcpy(_szBuf[_currentPacket], SIGN_TEXT, SIGN_LEN);
#if FZ_STL_CPLUSPLUS11
        vsnprintf(_szBuf[_currentPacket][SIGN_LEN], FZLOG_SIZE-SIGN_LEN, pszFormat, ap);
#else
        vsprintf(&_szBuf[_currentPacket][SIGN_LEN], pszFormat, ap);
#endif
        va_end(ap);
        
        puts(_szBuf[_currentPacket]);
    }
    
    
    char* fzStrcpy(const char *str, size_t len)
    {
        if(str == NULL)
            return NULL;
        
        char *newString = new char[len+1];
        memcpy(newString, str, len);
        newString[len] = '\0';
        return newString;
    }
    
    
    char* fzStrcpy(const char *str)
    {
        if(str == NULL)
            return NULL;
        
        return fzStrcpy(str, strlen(str));
    }
}
