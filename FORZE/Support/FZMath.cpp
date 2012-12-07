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

#include <stdio.h>
#include <string.h>

#include "FZMath.h"
#include "FZMacros.h"
#include "neon_support.h"
#include "SSE_support.h"


namespace FORZE {

    uint32_t fzMath_nextPOT(uint32_t nu)
    {
        --nu;
        nu |= nu >> 1;
        nu |= nu >> 2;
        nu |= nu >> 4;
        nu |= nu >> 8;
        nu |= nu >> 16;
        ++nu;
        
        return nu;
    }
    
    
    bool fzMath_isPOT(uint32_t nu)
    {
        return (nu != 0) && ((nu & (nu - 1)) == 0);
    }
    
    
#pragma mark Matrixes    
    
    void fzMath_mat4Identity(float *m)
    {
        FZ_ASSERT(m != NULL, "Input matrix 1 cannot be NULL");

        m[1] = m[2] = m[3] = m[4] =
        m[6] = m[7] = m[8] = m[9] =
        m[11] = m[12] = m[13] = m[14] = 0;
        m[0] = m[5] = m[10] = m[15] = 1;
    }
    
    
    void fzMath_mat4Copy(const float* m1, float* mat)
    {
#if defined(__ARM_NEON__)
        _NEON_mat4Copy(m1, mat);
#else
        memcpy(mat, m1, sizeof(fzMat4));
#endif
    }
    
    
    void fzMath_mat4Multiply(const float* m1, const fzAffineTransform &affine, float* mat)
    {
        FZ_ASSERT(m1 != NULL, "Input matrix 1 cannot be NULL");
        FZ_ASSERT(mat != NULL, "Output pointer cannot be NULL");
        FZ_ASSERT(m1 != mat, "Input matrix and output matrix cannot be the same one.");

        const float* m2 = affine.m;

#if defined(__ARM_NEON__)
        
        _NEON_mat4Multiply(m1, m2, mat);
        
#else        
        
        // first column
        mat[0] = m1[0] * m2[0] + m1[4] * m2[1];
        mat[1] = m1[1] * m2[0] + m1[5] * m2[1];
        mat[2] = m1[2] * m2[0] + m1[6] * m2[1];
        mat[3] = 0;
        
        // second column
        mat[4] = m1[0] * m2[2] + m1[4] * m2[3];
        mat[5] = m1[1] * m2[2] + m1[5] * m2[3];
        mat[6] = m1[2] * m2[2] + m1[6] * m2[3];
        mat[7] = 0;
        
        // the third column does not change
        memcpy(&mat[8], &m1[8], sizeof(float) * 4); // vmov in NEON
        
        // fourth column
        mat[12] = m1[0] * m2[4] + m1[4] * m2[5] + m1[12];
        mat[13] = m1[1] * m2[4] + m1[5] * m2[5] + m1[13];
        mat[14] = m1[2] * m2[4] + m1[6] * m2[5] + m1[14] + m2[6];
        mat[15] = 1;
#endif
    }
    
    
    void fzMath_mat4Multiply(const float* m1, const float* m2, float* mat)
    {
        FZ_ASSERT(m1 != NULL, "Input matrix 1 cannot be NULL");
        FZ_ASSERT(m2 != NULL, "Input matrix 2 cannot be NULL");
        FZ_ASSERT(m1 != mat, "Input matrix and output matrix cannot be the same one.");

        
        mat[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
        mat[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
        mat[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
        mat[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];
        
        mat[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
        mat[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
        mat[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
        mat[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];
        
        mat[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
        mat[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
        mat[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
        mat[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];
        
        mat[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
        mat[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
        mat[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
        mat[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
    }
    
    
    void fzMath_mat4Vec2(const float* m1, const float* m2, float* mat)
    {
        FZ_ASSERT(m1 != NULL, "Input matrix 1 cannot be NULL");
        FZ_ASSERT(m2 != NULL, "Input matrix 2 cannot be NULL");
        FZ_ASSERT(mat != NULL, "Output pointer cannot be NULL");
        
#if defined(__ARM_NEON__) && 0
        
        _NEON_mat4Vec2(m1, m2, mat);
        
#else
        
        mat[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[12];
        mat[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[13];
        
        mat[2] = m1[0] * m2[2] + m1[4] * m2[3] + m1[12];
        mat[3] = m1[1] * m2[2] + m1[5] * m2[3] + m1[13];
        
        mat[4] = m1[0] * m2[6] + m1[4] * m2[7] + m1[12];
        mat[5] = m1[1] * m2[6] + m1[5] * m2[7] + m1[13];
        
        mat[6] = m1[0] * m2[4] + m1[4] * m2[5] + m1[12];
        mat[7] = m1[1] * m2[4] + m1[5] * m2[5] + m1[13];
        
#endif
    }
    
    
    void fzMath_mat4Vec4(const float* m1, const float* m2, float* mat)
    {
        FZ_ASSERT(m1 != NULL, "Input matrix 1 cannot be NULL");
        FZ_ASSERT(m2 != NULL, "Input matrix 2 cannot be NULL");
        FZ_ASSERT(mat != NULL, "Output pointer cannot be NULL");
        
#if defined(__ARM_NEON__)
        
        _NEON_mat4Vec4(m1, m2, mat);
        
#else
        
        mat[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[12];
        mat[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[13];
        mat[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[14];
        
        mat[4] = m1[0] * m2[2] + m1[4] * m2[3] + m1[12];
        mat[5] = m1[1] * m2[2] + m1[5] * m2[3] + m1[13];
        mat[6] = m1[2] * m2[2] + m1[6] * m2[3] + m1[14];
        
        mat[8] = m1[0] * m2[4] + m1[4] * m2[5] + m1[12];
        mat[9] = m1[1] * m2[4] + m1[5] * m2[5] + m1[13];
        mat[10] = m1[2] * m2[4] + m1[6] * m2[5] + m1[14];
        
        mat[12] = m1[0] * m2[6] + m1[4] * m2[7] + m1[12];
        mat[13] = m1[1] * m2[6] + m1[5] * m2[7] + m1[13];
        mat[14] = m1[2] * m2[6] + m1[6] * m2[7] + m1[14];
        
#endif
    }
    
    
    void fzMath_mat4Vec4Affine(const float* matrixInput, const fzAffineTransform& affine, const float* vertices2DInput, float* vertices4DOutput) {
        FZ_ASSERT(matrixInput != NULL, "Input matrix 1 cannot be NULL");
        FZ_ASSERT(vertices2DInput != NULL, "Input matrix 2 cannot be NULL");
        FZ_ASSERT(vertices4DOutput != NULL, "Output pointer cannot be NULL");
        
#if defined(__ARM_NEON__)
        
        _NEON_mat4Vec4Affine(matrixInput, affine.m, vertices2DInput, vertices4DOutput);
        
#else
        register fzMat4 finalMatrix;
        fzMath_mat4Multiply(matrixInput, affine, finalMatrix);
        fzMath_mat4Vec4(finalMatrix, vertices2DInput, vertices4DOutput);
        
#endif
        
    }
    
    
    void fzMath_mat4PerspectiveProjection(fzFloat fovY, fzFloat aspect,
                                          fzFloat zNear, fzFloat zFar,
                                          float *output)
    {
        FZ_ASSERT(output != NULL, "Output pointer cannot be NULL");

        fzFloat r = FZ_DEGREES_TO_RADIANS(fovY / 2);
        fzFloat deltaZ = zFar - zNear;
        fzFloat cotangent = 1/tanf(r);
        
        if (deltaZ == 0 || cotangent == 0 || aspect == 0) {
            FZLOGERROR("Perpertive impossible");
            return;
        }
        
        fzMath_mat4Identity(output);
        output[0] = cotangent / aspect;
        output[5] = cotangent;
        output[10] = -(zFar + zNear) / deltaZ;
        output[11] = -1;
        output[14] = -2 * zNear * zFar / deltaZ;
    }
    

    void fzMath_mat4OrthoProjection(fzFloat left, fzFloat right,
                                    fzFloat bottom, fzFloat top,
                                    fzFloat nearVal, fzFloat farVal,
                                    float* output )
    {
        FZ_ASSERT(output != NULL, "Output pointer cannot be NULL");

        fzFloat tx = -((right + left) / (right - left));
        fzFloat ty = -((top + bottom) / (top - bottom));
        fzFloat tz = -((farVal + nearVal) / (farVal - nearVal));
        
        fzMath_mat4Identity(output);
        output[0] = 2 / (right - left);
        output[5] = 2 / (top - bottom);
        output[10] = -2 / (farVal - nearVal);
        output[12] = tx;
        output[13] = ty;
        output[14] = tz;        
    }
    
    
    void fzMath_mat4print(const float* O)
    {
        printf("Matrix 4x4 {\n  {%f, %f, %f, %f}\n  {%f, %f, %f, %f}\n  {%f, %f, %f, %f}\n  {%f, %f, %f, %f}}\n", 
               O[0],O[4],O[8],O[12],O[1],O[5],O[9],O[13],O[2],O[6],O[10],O[14],O[3],O[7],O[11],O[15]);
    }
    
  
    void fzMath_mat4LookAt(const fzPoint3& eye, const fzPoint3& center, const fzPoint3& up, float *output)
    {     
        FZ_ASSERT(output != NULL, "Output pointer cannot be NULL");

        fzPoint3 f(center);
        f.x -= eye.x;
        f.y -= eye.y;
        f.z -= eye.z;
        f.normalize();

        fzPoint3 up2(up);
        up2.normalize();

        fzPoint3 s(f.getCrossed(up2));
        s.normalize();
        
        fzPoint3 u(s.getCrossed(f));
        //s.normalize();
        u.normalize();
        
        fzMath_mat4Identity(output);
        
        
        output[0] = s.x;
        output[4] = s.y;
        output[8] = s.z;
        
        output[1] = u.x;
        output[5] = u.y;
        output[9] = u.z;
        
        output[2] = -f.x;
        output[6] = -f.y;
        output[10] = -f.z;
        

        
        /*
        fzMat4 translationMatrix;
        fzAffineTransform translate(FZAffineTransformIdentity);
        translate.translate(-eye.x, -eye.y);
        translate.setZ(-eye.y);
         */
        
        /*
        fzMath_mat4Multiply(<#const fzFloat *mIn1#>, <#const fzFloat *mIn2#>, <#fzFloat *mOut#>)
        fzMath_mat4Multiply(output, translate, output);
        */
        
        // kmMat4 translate;
        // kmMat4Translation(&translate, -pEye->x, -pEye->y, -pEye->z);
        // kmMat4Multiply(pOut, pOut, &translate);
    }
}
