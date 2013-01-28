// DO NOT MODIFY THE HEADERS IF FORZE IS ALREADY COMPILED AS A STATIC LIBRARY
#ifndef __FZMATHINLINE_H_INCLUDED__
#define __FZMATHINLINE_H_INCLUDED__
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

#ifdef __cplusplus
extern "C" {
#endif
    
#include "neon_support.h"
#include "SSE_support.h"
    
    inline void _inline_mat4Copy(const float * __restrict__ src,
                                 float * __restrict__ dst)
    {
#if defined(__ARM_NEON__)
        _NEON_mat4Copy(src, dst);
        
#else
        memcpy(dst, src, sizeof(float)*16);
        
#endif
    }
    
    
    inline void _inline_mat4Multiply(const float *__restrict__ m1,
                                     const float *__restrict__ m2,
                                     float *__restrict__ mOut)
    {
#if defined(__ARM_NEON__)
        _NEON_mat4Multiply(m1, m2, mOut);
        
#else
        mOut[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2];
        mOut[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2];
        mOut[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2];
        mOut[3] = 0;
        
        mOut[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6];
        mOut[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6];
        mOut[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6];
        mOut[7] = 0;

        mOut[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10];
        mOut[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10];
        mOut[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10];
        mOut[11] = 0;

        mOut[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12];
        mOut[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13];
        mOut[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14];
        mOut[15] = 1;

#endif
    }
    
    
    inline void _inline_mat4MultiplySafe(const float * __restrict__ m1,
                                         const float * __restrict__ m2,
                                         float *__restrict__  mOut)
    {
#if defined(__ARM_NEON__)
        _NEON_mat4Multiply(m1, m2, mOut);
        
#else
        if(m1 == mOut) {
            register float tmp[16];
            _inline_mat4Copy(m1, tmp);
            _inline_mat4Multiply(tmp, m2, mOut);

        }else
            _inline_mat4Multiply(m1, m2, mOut);
#endif
    }
    
    
    inline void _inline_mat4Rotate(float *__restrict__ m, float angle)
    {
        float d[2] {
            fzMath_cos(angle),
            fzMath_sin(angle)
        };
        
#if defined(__ARM_NEON__)
        _NEON_mat4Rotate(m, d);
        
#else
        float a0 = m[0]*d[0] + m[4]*d[1];
        float a1 = m[1]*d[0] + m[5]*d[1];
        float a2 = m[2]*d[0] + m[6]*d[1];
        
        m[4] = m[4]*d[0] - m[0]*d[1];
        m[5] = m[5]*d[0] - m[1]*d[1];
        m[6] = m[6]*d[0] - m[2]*d[1];
        
        m[0] = a0;
        m[1] = a1;
        m[2] = a2;
#endif
    }
    
    
    inline void _inline_mat4Tranlate(float * __restrict__ m, float x, float y, float z)
    {
#if defined(__ARM_NEON__)
        float d[3] {x, y, z};
        _NEON_mat4Translate(m, d);
        
#else
        m[12] += x*m[0] + y*m[4] + z*m[8];
        m[13] += x*m[1] + y*m[5] + z*m[9];
        m[14] += x*m[2] + y*m[6] + z*m[10];
        
#endif
    }
    
    
    inline void _inline_mat4Vec2(const float * __restrict__ m1,
                                 const float * __restrict__ v1,
                                 float * __restrict__ vOut)
    {
        
#if defined(__ARM_NEON__)
        _NEON_mat4Vec2(m1, v1, vOut);
        
#else
        vOut[0] = m1[0] * v1[0] + m1[4] * v1[1] + m1[12];
        vOut[1] = m1[1] * v1[0] + m1[5] * v1[1] + m1[13];
        
        vOut[2] = m1[0] * v1[2] + m1[4] * v1[3] + m1[12];
        vOut[3] = m1[1] * v1[2] + m1[5] * v1[3] + m1[13];
        
        vOut[4] = m1[0] * v1[6] + m1[4] * v1[7] + m1[12];
        vOut[5] = m1[1] * v1[6] + m1[5] * v1[7] + m1[13];
        
        vOut[6] = m1[0] * v1[4] + m1[4] * v1[5] + m1[12];
        vOut[7] = m1[1] * v1[4] + m1[5] * v1[5] + m1[13];
        
#endif
    }

    
    inline void _inline_mat4Vec4(const float * __restrict__ m1,
                                 const float * __restrict__ v1,
                                 float * __restrict__ vOut)
    {
#if defined(__ARM_NEON__)
        _NEON_mat4Vec4(m1, v1, vOut);
        
#else
        vOut[0] = m1[0] * v1[0] + m1[4] * v1[1] + m1[12];
        vOut[1] = m1[1] * v1[0] + m1[5] * v1[1] + m1[13];
        vOut[2] = m1[2] * v1[0] + m1[6] * v1[1] + m1[14];
        vOut[3] = 0;
        
        vOut[4] = m1[0] * v1[2] + m1[4] * v1[3] + m1[12];
        vOut[5] = m1[1] * v1[2] + m1[5] * v1[3] + m1[13];
        vOut[6] = m1[2] * v1[2] + m1[6] * v1[3] + m1[14];
        vOut[7] = 0;

        vOut[8] = m1[0] * v1[4] + m1[4] * v1[5] + m1[12];
        vOut[9] = m1[1] * v1[4] + m1[5] * v1[5] + m1[13];
        vOut[10] = m1[2] * v1[4] + m1[6] * v1[5] + m1[14];
        vOut[11] = 0;

        vOut[12] = m1[0] * v1[6] + m1[4] * v1[7] + m1[12];
        vOut[13] = m1[1] * v1[6] + m1[5] * v1[7] + m1[13];
        vOut[14] = m1[2] * v1[6] + m1[6] * v1[7] + m1[14];
        vOut[15] = 1;
        
#endif
    }
    
#ifdef __cplusplus
}
#endif

#endif
