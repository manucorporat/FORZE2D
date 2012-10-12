#ifndef __FZ_BUFFERMANAGER_H_INCLUDED__
#define __FZ_BUFFERMANAGER_H_INCLUDED__
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

#include "FZConfig.h"
#include "FZTypes.h"


namespace FORZE {
    
    class BufferManager;
    class FZBuffer
    {
    protected:
        void *ptr_;
        fzUInt length_;
        unsigned char index_;
        BufferManager *manager_;
        
        
    public:
        FZBuffer();
        FZBuffer(const FZBuffer&);
        FZBuffer& operator = (const FZBuffer&);
        
        void setPointer(void *p);
        void* const getPointer() const;
        
        void setLength(fzUInt);
        fzUInt getLength() const;
        
        void setIndex(unsigned char);
        unsigned char getIndex() const;
        
        BufferManager *getManager() const;
        
        // Close buffer
        void close();
    };
    
    
    class BufferManager
    {
    private:
        // Director's instance
        static BufferManager* pinstance_;
        
        // Buffer
        char *buffer_;
        
        // Buffer's stack
        FZBuffer requests_[2];
        
        // Total buffer length
        fzUInt length_;
        
        // Number of filled stacks
        fzUInt num_;
        
        
    protected:
        // Constructors
        BufferManager();
        BufferManager(const BufferManager& ) ;
        BufferManager &operator = (const BufferManager& ) ;
        
        // Destructor
        ~BufferManager();

        
    public:
        // Get and alloc instance
        static BufferManager& Instance();
        
        // Preload buffer
        bool preloadBuffer(fzUInt length);
        
        // open a buffer
        FZBuffer* open(fzUInt length);
        
        // close a buffer
        void close(FZBuffer* buffer);
        
        fzUInt getMemoryUsed() const;
    };

}
#endif
