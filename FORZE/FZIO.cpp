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

#include <zlib.h>
#include <stdio.h>
#include <string.h>

#include "FZIO.h"
#include "FZBitOrder.h"
#include "FZDeviceConfig.h"
#include "FZMacros.h"
#include "FZDirector.h"

using namespace STD;


namespace FORZE {
    
    fzBuffer IO::loadFile(const char *absolutePath)
    {
        FZ_ASSERT(absolutePath, "Absolute path cannot be NULL.");
        if(absolutePath[0] == '\0')
            return fzBuffer::empty();
        
        FILE *f = fopen(absolutePath, "rb");
        if( f == NULL )
            return fzBuffer::empty();
        
        
        fseek(f, 0, SEEK_END);
        size_t size = (size_t)ftell(f);
        fseek(f, 0, SEEK_SET);
        
        
        char *buffer = new(std::nothrow) char[size+1];
        if(buffer == NULL) {
            fclose(f);
            FZLOGERROR("IO: Impossible to allocate memory.");
            return fzBuffer::empty();
        }
        size_t read = fread(buffer, 1, size, f);
        fclose(f);
        if( read != size ) {
            FZLOGERROR("IO: Abnormal reading error. Probably the path is not a file.");
            delete [] buffer;
            return fzBuffer::empty();
        }
        // NULL TERMINATED
        buffer[size] = '\0';
        
        return fzBuffer(buffer, size+1);
    }
    
    
    bool IO::checkFile(const char *absolutePath)
    {
        if(absolutePath == NULL || absolutePath[0] == '\0')
            return false;
        
        FILE *f = fopen(absolutePath, "rb");
        if( f != NULL ) {
            fclose(f);
            return true;
        }
        return false;
    }
    
    
    bool IO::writeFile(const char *data, const char *absolutePath)
    {
        FZ_ASSERT(data, "Data can not be NULL.");
        FZ_ASSERT(absolutePath, "absolutePath can not be NULL.");
        
        FILE *f = fopen(absolutePath, "w");
        if( f == NULL ) {
            FZLOGERROR("IO: \"%s\" can not be opened.", absolutePath);
            return false;
        }
        
        int error = fputs(data, f);
        fclose (f);
        
        if(error <= 0) {
            FZLOGERROR("IO: Error writing data. Error code: %d.", error);
            return false;
        }
        return true;
    }
    
    
    void IO::removeFileSuffix(char* filename)
    {
        char *start = strchr(filename, FZ_IO_SUBFIX_CHAR);
        if(start) {
            const char *extension = IO::getExtension(start);
            if(extension)
                memmove(start, extension-1, strlen(extension)+2);
            else
                *start = '\0';
        }
    }
    
    
    const char* IO::getExtension(const char* filename)
    {
        if(filename == NULL)
            return NULL;
        
        const char *extension = strchr(filename, '.');
        if(extension == NULL)
            return NULL;
        
        return (++extension);
    }
    
    
    char* IO::appendPaths(const char* path1, const char* path2, char *output)
    {
        if(path1 == NULL || *path1 == '\0')
            strcpy(output, path2);
        
        else {
            strcpy(output, path1);
            strcat(output, "/");
            strcat(output, path2);
        }
        return output;
    }
}
