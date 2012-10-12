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

#include <stdlib.h>

#include "FZBufferManager.h"
#include "FZMacros.h"


namespace FORZE {
    
#pragma mark - FZBuffer implementation
    
    FZBuffer::FZBuffer()
    : ptr_(NULL), length_(0), index_(0), manager_(NULL)
    { }
    
    FZBuffer::FZBuffer(const FZBuffer& b)
    : ptr_(b.getPointer()), length_(b.getLength()), index_(b.getIndex()), manager_(b.getManager())
    { }
    
    FZBuffer& FZBuffer::operator = (const FZBuffer& b)
    {
        ptr_ = b.getPointer();
        length_ = b.getLength();
        index_ = b.getIndex();
        manager_ = b.getManager();
        
        return *this;
    }
    
    
    void FZBuffer::setPointer(void *p)
    {
        ptr_ = p;
    }
    
    
    void FZBuffer::setLength(fzUInt l)
    {
        length_ = l;
    }
    
    
    void FZBuffer::setIndex(unsigned char i)
    {
        index_ = i;
    }
    
    
    void* const FZBuffer::getPointer() const
    {
        return ptr_;
    }
    
    
    fzUInt FZBuffer::getLength() const
    {
        return length_;
    }
    
    
    unsigned char FZBuffer::getIndex() const
    {
        return index_;
    }
    
    
    BufferManager* FZBuffer::getManager() const
    {
        return manager_; 
    }
    
    
    void FZBuffer::close()
    {
        BufferManager::Instance().close(this);
    }
    
    
#pragma mark - BufferManager implementation
    
    BufferManager* BufferManager::pinstance_ = NULL;
    
    BufferManager& BufferManager::Instance()
    {
        if (pinstance_ == NULL)
            pinstance_ = new BufferManager();
        
        return *pinstance_;
    }
    
    BufferManager::BufferManager()
    : buffer_(NULL), length_(0), num_(0)
    {
        preloadBuffer(8 * 2048 * 2048);
    }
    
    BufferManager::~BufferManager()
    {
        free(buffer_);
        buffer_ = NULL;
    }
    
    
    bool BufferManager::preloadBuffer(fzUInt m)
    {
        if(num_ != 0)
        {
            FZLog("You can't resize the buffer while it is using");
            return false;
        }
        
        if(length_ < m)
        {
            free(buffer_);
            length_ = m;
            buffer_ = (char*) malloc(length_);
            return true;
        }
        return false;
    }
    
    
    FZBuffer* BufferManager::open(fzUInt l)
    {
        if(num_ >= 2)
        {
            FZLog("You can't open more buffers");
            exit(-1);
        }
        else if(num_ == 1)
        {
            char *oldPtr = (char*)requests_[0].getPointer();
            void *newPtr = NULL;
            
            if ((fzUInt)(oldPtr - buffer_) <= l)
                newPtr = buffer_;
            
            else
            {
                char *end = oldPtr + requests_[0].getLength();
                if((fzUInt)(buffer_-end+length_) <= l)
                    newPtr = end;
                
            }
            if(newPtr != NULL)
            {
                requests_[1].setPointer(newPtr);
                requests_[1].setLength(l);
                requests_[1].setIndex(1);
                num_ = 2;
            }else{
                FZLog("NO AVAILABLE SIZE");
                exit(EXIT_FAILURE);  
            }
            
        }else{
            if(length_ >= l)
            {
                requests_[0].setPointer(buffer_);
                requests_[0].setLength(l);
                requests_[0].setIndex(0);
                num_ = 1;
            }else{
                FZLog("NO AVAILABLE SIZE");
                exit(EXIT_FAILURE);  
            }
        }
        return &requests_[num_-1];
    }
    
    
    void BufferManager::close(FZBuffer* b)
    {
        if(num_ == 1)
            num_ = 0;
        
        else if(num_ == 2)
        {
            if(b->getIndex() == 0)
            {
                requests_[0] = requests_[1];
                requests_[0].setIndex(0);
            }
            
            num_ = 1;
        }
        b->setIndex(0);
        b->setLength(0);
        b->setPointer(NULL);
    }
    
    
    fzUInt BufferManager::getMemoryUsed() const
    {
        return length_;
    }
}
