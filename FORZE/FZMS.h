// DO NOT MODIFY THE HEADERS IF FORZE IS ALREADY COMPILED AS A STATIC LIBRARY
#ifndef __FZMATRIXSTACK_H_INCLUDED__
#define __FZMATRIXSTACK_H_INCLUDED__
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

#include "FZTypes.h"


namespace FORZE {
    
    class MS
    {
        friend class Director;
        
    private:
        static void initialize();
        static void dealloc();
        
    public:
        static void loadMatrix(float *matrix);
        static void loadBaseMatrix(float *matrix);
        
        static void pushMatrix(float *matrix);
        static void pop();
        
        static fzUInt getLevel();
        static float* getMatrix();
        static float* getBaseMatrix();
    };
    
#define FZ_PROGRAM_APPLY_TRANSFORM(__SHADER__) { \
FZ_ASSERT(__SHADER__ != NULL, "Shader can not be NULL."); \
FZ_ASSERT(MS::getMatrix() != NULL, "Stack base can not be NULL."); \
(__SHADER__)->setUniform4x4f(kFZUniformMVMatrix, 1, GL_FALSE, MS::getMatrix()); \
}

}

#endif
