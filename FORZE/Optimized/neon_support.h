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

inline void _NEON_mat4Multiply(const float *m1, const float *m2, float* mat)
{
    __asm__ volatile
    (
     "vldmia %1, { q0-q1 }\n\t" // q0-q1 = mIn2
     "vldmia %2, { q2-q5 }\n\t" // q2-q5 = mIn1
     
     // first column
     "vmul.f32 q6, q2, d0[0]\n\t"
     "vmla.f32 q6, q3, d0[1]\n\t"
     
     // second column
     "vmul.f32 q7, q2, d1[0]\n\t"
     "vmla.f32 q7, q3, d1[1]\n\t"

     // the third column does not change
     "vmov.f32 q8, q4\n\t"
     
     // fourth column
     "vmov.f32 q9, q5\n\t"
     "vmla.f32 q9, q2, d2[0]\n\t"
     "vmla.f32 q9, q3, d2[1]\n\t"    
     
     
     // output = result registers
     "vstmia %0, { q6-q9 }" // mOut = q6-q9
     
     : // no explicit output
     : "r" (mat), "r" (m2), "r" (m1) 	// input - note *value* of pointer doesn't change
     : "memory", "q0", "q1",   "q2", "q3", "q4", "q5",   "q6", "q7", "q8", "q9" //clobber
     );
}


inline void _NEON_mat4Vec2(const float* m1, const float* m2, float* mat)
{
    __asm__ volatile
    (
     // Store m & v - avoiding q4-q7 which need to be preserved - q0 = result
     "vldmia %1, { q4-q7 }	\n\t"	// q8-q11 = matrix
     "vldmia %2, { q0-q1 }	\n\t"	// q0-q1  = input vertices
     
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
     "vstmia %0, { q2-q3 }"
     
     : // no output
     : "r" (mat), "r" (m1), "r" (m2) 	// input - note *value* of pointer doesn't change
     : "memory", "q0", "q1",    "q2", "q3",     "q4", "q5",  "q6", "q7" //clobber
     );
}


inline void _NEON_mat4Vec4Affine(const float* m1, const float* m2, const float* m3, float* mat)
{
    //m1 = matrix
    //m2 = affine transform
    //m3 = 2d vertices
    //mat = ourput vertices
    
    __asm__ volatile
    (
     "vldmia %3, { q0-q1 }\n\t" // q0-q1 = m3 (input vertices)
     "vldmia %2, { q2-q3 }\n\t" // q2-q3 = m2 (input affine transform)
     "vldmia %1, { q4-q7 }\n\t" // q4-q7 = m1 (input matrix)

     // COMPUTE MATRIX (4x3) q8-q10
     // first column
     "vmul.f32 q8, q4, d4[0]\n\t"
     "vmla.f32 q8, q5, d4[1]\n\t"
     
     // second column
     "vmul.f32 q9, q4, d5[0]\n\t"
     "vmla.f32 q9, q5, d5[1]\n\t"
     
     // second column
     "vmov.f32 q10, q6\n\t"

     // fourth column
     "vmov.f32 q11, q7\n\t"
     "vmla.f32 q11, q4, d6[0]\n\t"
     "vmla.f32 q11, q5, d6[1]\n\t"
     //"vadd.f32 d21[0], d21[0], d3[0]\n\t"
     
     
     // COMPUTE VERTICES (4x4) q4-q7 (reuse matrix registers)
     // result = first column of A x V.x
     "vmov.f32 q4, q11\n\t" 
     "vmla.f32 q4, q8, d0[0]\n\t"         
     "vmla.f32 q4, q9, d0[1]\n\t"
     
     "vmov.f32 q5, q11\n\t" 
     "vmla.f32 q5, q8, d1[0]\n\t"         
     "vmla.f32 q5, q9, d1[1]\n\t"
     
     "vmov.f32 q6, q11\n\t" 
     "vmla.f32 q6, q8, d2[0]\n\t"         
     "vmla.f32 q6, q9, d2[1]\n\t"
     
     "vmov.f32 q7, q11\n\t" 
     "vmla.f32 q7, q8, d3[0]\n\t"         
     "vmla.f32 q7, q9, d3[1]\n\t"
     

     // output = result registers
     "vstmia %0, { q4-q7 }" // vertices = q4-q7
     
     : // no explicit output
     : "r" (mat), "r" (m1), "r" (m2), "r" (m3) 	// input - note *value* of pointer doesn't change
     : "memory", "q0", "q1",   "q2", "q3",    "q4", "q5", "q6", "q7",   "q8", "q9", "q10", "q11"
     );
}


inline void _NEON_mat4Vec4(const float* m1, const float* m2, float* mat)
{
    __asm__ volatile
    (
     // Store m & v - avoiding q4-q7 which need to be preserved - q0 = result
     "vldmia %1, { q6-q9 }	\n\t"	// q8-q11 = matrix
     "vldmia %2, { q0-q1 }	\n\t"	// q0-q1  = vertices
     
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
     "vstmia %0, { q2-q5 }"
     
     : // no output
     : "r" (mat), "r" (m1), "r" (m2) 	// input - note *value* of pointer doesn't change
     : "memory", "q0", "q1",   "q2", "q3", "q4", "q5",   "q6", "q7", "q8", "q9" //clobber
     );
}


inline void _NEON_mat4Copy(const float* m1, float* mat)
{
    __asm__ volatile
    (
     "vldmia %1, { q0-q3 }\n\t" // q0-q3 = m1         
     "vstmia %0, { q0-q3 }" // mat = q0-q3
     :
     : "r" (mat), "r" (m1)
     : "memory", "q0", "q1", "q2", "q3" //clobber
     );
}


#endif
#endif
