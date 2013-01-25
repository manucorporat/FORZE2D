// DO NOT MODIFY THE HEADERS IF FORZE IS ALREADY COMPILED AS A STATIC LIBRARY
#ifndef __FZNEONSUPPORT_H_INCLUDED__
#define __FZNEONSUPPORT_H_INCLUDED__
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

#ifdef __arm__
#include <arm/arch.h>
#endif

#ifdef __ARM_NEON__

#ifdef __cplusplus
extern "C" {
#endif
    
    // THIS FUNCTIONS ONLY WORKS WITH OPENGL MATRICES
    // Because math simplifications were applied.
    // {a b c d}
    // {e f g h}
    // {i j k l}
    // {0 0 0 1}
    // M = {a, e, i, 0,  b, f, j, 0,  c, g, k, 0,  d, h, l, 1}
    
    
    inline void _NEON_mat4Copy(const float *__restrict__ src,
                               float *__restrict__ dst)
    {
        __asm__ volatile
        (
         "PLD [%0, #512]\n\t" // preloading src
         "vldmia %0, { q0-q3 }\n\t" // q0-q3 = src
         "vstmia %1, { q0-q3 }"     // dst = q0-q3
         :
         : "r" (src), "r" (dst)
         : "memory", "q0", "q1", "q2", "q3" //clobber
         );
    }
    
    
    inline void _NEON_mat4Rotate(float *__restrict__ mat,
                                 const float *__restrict__ values)
    {
        __asm__ volatile
        (
         // Load matrix into NEON registers
         "vldmia %0, { q8-q9 }\n\t" // q8-q12 = mat (128bits)
         "vldmia %1, { d0 }\n\t" // d0 = values (128bits)
         
         
         "vmul.f32    q10, q8, d0[0]\n\t" // mOut[0-2] = m[0-2] * x
         "vmla.f32    q10, q9, d0[1]\n\t" // mOut[0-2] += m[2-4] * y
         
         "vmul.f32    q11, q9, d0[0]\n\t" // mOut[2-4] = m[2-4] * x
         "vmls.f32    q11, q8, d0[1]\n\t" // mOut[2-4] -= m[0-2] * y
         
         "vstmia %0, { q10-q11 }" // mOut = mat = q10-q11
         
         : // no explicit output
         : "r" (mat), "r" (values)
         : "memory", "d0",   "q8", "q9",  "q10", "q11" //clobber
         );
    }
    
    
    inline void _NEON_mat4Translate(float *__restrict__ mat,
                                    const float *__restrict__ values)
    {
        __asm__ volatile
        (
         // Load matrix into NEON registers
         
         "vldmia %0, { q8-q11 }\n\t" // q8-q12 = mat (128bits)
         "vldmia %1, { d0-d1 }\n\t" // d0 = values (128bits)
         
         "vmla.f32    q11, q8, d0[0]\n\t" // mOut[0-2] = m[0-2] * x
         "vmla.f32    q11, q9, d0[1]\n\t" // mOut[0-2] += m[2-4] * y
         "vmla.f32    q11, q10, d1[0]\n\t" // mOut[0-2] += m[2-4] * y
         
         "vstmia %0, { q8-q11 }" // mOut = mat = q10-q11
         
         : // no explicit output
         : "r" (mat), "r" (values)
         : "memory", "d0", "d1",   "q8", "q9",  "q10", "q11" //clobber
         );
    }
    
    
    inline void _NEON_mat4Multiply(const float *__restrict__ m1,
                                   const float *__restrict__ m2,
                                   float *__restrict__ mOut)
    {
        __asm__ volatile
        (
         "vldmia %0, { q8-q11 }\n\t" // q8-q12 = m1 (128bits)
         "vldmia %1, { q0-q3 }\n\t" // q0-q3 = m2 (128bits)
         
         "vmul.f32    q12, q8, d0[0]\n\t" // col0  = (m1 col0) * (m2 col0 elt0)
         "vmul.f32    q13, q8, d2[0]\n\t" // col1  = (m1 col0) * (m2 col1 elt0)
         "vmul.f32    q14, q8, d4[0]\n\t" // col2  = (m1 col0) * (m2 col2 elt0)
         "vmul.f32    q15, q8, d6[0]\n\t" // col3  = (m1 col0) * (m2 col3 elt0)
         
         "vmla.f32    q12, q9, d0[1]\n\t" // col0 += (m1 col1) * (m2 col0 elt1)
         "vmla.f32    q13, q9, d2[1]\n\t" // col1 += (m1 col1) * (m2 col1 elt1)
         "vmla.f32    q14, q9, d4[1]\n\t" // col2 += (m1 col1) * (m2 col2 elt1)
         "vmla.f32    q15, q9, d6[1]\n\t" // col3 += (m1 col1) * (m2 col3 elt1)
         
         "vmla.f32    q12, q10, d1[0]\n\t" // col0 += (m1 col2) * (m2 col0 elt2)
         "vmla.f32    q13, q10, d3[0]\n\t" // col1 += (m1 col2) * (m2 col1 elt2)
         "vmla.f32    q14, q10, d5[0]\n\t" // col2 += (m1 col2) * (m2 col2 elt2)
         "vmla.f32    q15, q10, d7[0]\n\t" // col3 += (m1 col2) * (m2 col2 elt2)
         
         // Optimization for OpenGL
         "vadd.f32    q15, q15, q11\n\t"   // col3 += (mat0 col3)
         
         
         "vstmia %2, { q12-q15 }" // mOut = q12-q15
         
         : // no explicit output
         : "r" (m1), "r" (m2), "r" (mOut)
         : "memory", "q0", "q1", "q2", "q3",  "q8", "q9", "q10", "q11",  "q12", "q13", "q14", "q15" //clobber
         );
    }
    
    inline void _NEON_mat4Vec2(const float *__restrict__ m1,
                               const float *__restrict__ v1,
                               float *__restrict__ vOut)
    {
        __asm__ volatile
        (
         "vldmia %0, { q4-q7 }\n\t"	// q8-q11 = matrix
         "vldmia %1, { q0-q1 }\n\t"	// q0-q1  = input vertices
         
         // result = first column of A x V.x
         "vmov.f32 d4, d14\n\t"
         "vmla.f32 d4, d8, d0[0]\n\t"
         "vmla.f32 d4, d10, d0[1]\n\t"
         
         "vmov.f32 d5, d14\n\t"
         "vmla.f32 d5, d8, d1[0]\n\t"
         "vmla.f32 d5, d10, d1[1]\n\t"
         
         // reverse order
         "vmov.f32 d7, d14\n\t"
         "vmla.f32 d7, d8, d2[0]\n\t"
         "vmla.f32 d7, d10, d2[1]\n\t"
         
         "vmov.f32 d6, d14\n\t"
         "vmla.f32 d6, d8, d3[0]\n\t"
         "vmla.f32 d6, d10, d3[1]\n\t"
         
         // output = result registers
         "vstmia %2, { q2-q3 }"
         
         : // no output
         : "r" (m1), "r" (v1), "r" (vOut)
         : "memory", "q0", "q1",    "q2", "q3",     "q4", "q5",  "q6", "q7"
         );
    }
    
    
    inline void _NEON_mat4Vec4(const float *__restrict__ m1,
                               const float *__restrict__ v1,
                               float *__restrict__ vOut)
    {
        __asm__ volatile
        (
         "vldmia %0, { q6-q9 }	\n\t"	// q6-q9 = matrix
         "vldmia %1, { q0-q1 }	\n\t"	// q0-q1  = vertices
         
         // result = first column of A x V.x
         "vmov.f32 q2, q9\n\t"
         "vmla.f32 q2, q6, d0[0]\n\t"
         "vmla.f32 q2, q7, d0[1]\n\t"
         
         "vmov.f32 q3, q9\n\t"
         "vmla.f32 q3, q6, d1[0]\n\t"         
         "vmla.f32 q3, q7, d1[1]\n\t"
         
         "vmov.f32 q4, q9\n\t" 
         "vmla.f32 q4, q6, d2[0]\n\t"         
         "vmla.f32 q4, q7, d2[1]\n\t"
         
         "vmov.f32 q5, q9\n\t" 
         "vmla.f32 q5, q6, d3[0]\n\t"         
         "vmla.f32 q5, q7, d3[1]\n\t"
         
         // output = result registers
         "vstmia %2, { q2-q5 }"
         
         : // no output
         : "r" (m1), "r" (v1), "r" (vOut) 	// input - note *value* of pointer doesn't change
         : "memory", "q0", "q1",   "q2", "q3", "q4", "q5",   "q6", "q7", "q8", "q9" //clobber
         );
    }

#ifdef __cplusplus
    extern "C" {
#endif

#endif
#endif
