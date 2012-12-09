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

#include "FZMath.h"
#include "FZMacros.h"
#include "FZMS.h"

namespace FORZE {
    
#define STACK_SIZE 30
#define STACK_INCREMENT 40
    
    static struct
    {
        fzUInt capacity;
        fzUInt num;
        float **stack;
        
    } _modelview_stack = {0, 0, NULL};    
    
    void MS::initialize()
    {
        if (_modelview_stack.stack != NULL)
            return;
        
        // Initialize stack
        _modelview_stack.stack = (float**)malloc(sizeof(float*)*STACK_SIZE);
        if(_modelview_stack.stack == NULL)
            FZ_RAISE("MatrixStack: Imposible to allocate memory.");
        
        _modelview_stack.capacity = STACK_SIZE;
        _modelview_stack.num = 1;
        _modelview_stack.stack[0] = NULL;        
    }
    
    
    void MS::dealloc()
    {
        if(_modelview_stack.stack) {
            free(_modelview_stack.stack);
            _modelview_stack.capacity = 0;
            _modelview_stack.stack = NULL;
        }
    }
    
    
    void MS::pushMatrix(float *matrix)
    {
        FZ_ASSERT(_modelview_stack.stack, "The matrix stack is not initialized.");
        FZ_ASSERT(matrix != NULL, "Matrix cannot be NULL.");
        
        _modelview_stack.stack[_modelview_stack.num++] = matrix;
        
        if(_modelview_stack.num >= _modelview_stack.capacity) {
            _modelview_stack.capacity += STACK_INCREMENT;
            realloc(_modelview_stack.stack, _modelview_stack.capacity * sizeof(float*));
        }
    }
    
    
    void MS::pop()
    {
        FZ_ASSERT(_modelview_stack.stack, "The matrix stack is not initialized.");
        FZ_ASSERT(_modelview_stack.num > 1, "Cannot pop an empty stack.");
        
        --_modelview_stack.num;
    }
    
    
    void MS::loadBaseMatrix(float *matrix)
    {
        FZ_ASSERT(_modelview_stack.stack, "The matrix stack is not initialized.");
        FZ_ASSERT(matrix != NULL, "Matrix cannot be NULL.");
        
        _modelview_stack.stack[0] = matrix;
    }
    
    
    void MS::loadMatrix(float *matrix)
    {
        FZ_ASSERT(_modelview_stack.stack, "The matrix stack is not initialized.");
        FZ_ASSERT(matrix != NULL, "Input matrix can not be NULL.");
        FZ_ASSERT(_modelview_stack.num > 1, "Use MS::loadBaseMatrix() instead.");
        
        _modelview_stack.stack[_modelview_stack.num-1] = matrix;
    }
    
    
    fzUInt MS::getLevel()
    {
        FZ_ASSERT(_modelview_stack.stack, "The matrix stack is not initialized.");
        return _modelview_stack.num;
    }
    
    
    float* MS::getBaseMatrix()
    {
        FZ_ASSERT(_modelview_stack.stack, "The matrix stack is not initialized.");
        return _modelview_stack.stack[0];  
    }
    
    
    float* MS::getMatrix()
    {
        FZ_ASSERT(_modelview_stack.stack, "The matrix stack is not initialized.");
        return _modelview_stack.stack[_modelview_stack.num-1];
    }
}
