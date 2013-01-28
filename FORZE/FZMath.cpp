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
#include "Optimized/FZMathInline.h"


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
        FZ_ASSERT(m != NULL, "Input matrix 1 cannot be NULL.");

        m[1] = m[2] = m[3] = m[4] =
        m[6] = m[7] = m[8] = m[9] =
        m[11] = m[12] = m[13] = m[14] = 0;
        m[0] = m[5] = m[10] = m[15] = 1;
    }
    
    
    void fzMath_mat4Rotate(float* m1, float radians)
    {
        FZ_ASSERT(m1 != NULL, "Input matrix 1 cannot be NULL.");

        _inline_mat4Rotate(m1, radians);
    }
    
    
    void fzMath_mat4Copy(const float* src, float* dst)
    {
        FZ_ASSERT(src != NULL, "Source matrix cannot be NULL.");
        FZ_ASSERT(dst != NULL, "Destination matrix cannot be NULL.");

        _inline_mat4Copy(src, dst);
    }
    
    
    void fzMath_mat4MultiplySafe(const float* m1, const float* m2, float* mOut)
    {
        FZ_ASSERT(m1 != NULL, "Input matrix 1 cannot be NULL.");
        FZ_ASSERT(m2 != NULL, "Input matrix 2 cannot be NULL.");
        FZ_ASSERT(mOut != NULL, "Output matrix cannot be NULL.");
        FZ_ASSERT(m1 != mOut, "Input and output can not have the same pointer.");

        _inline_mat4MultiplySafe(m1, m2, mOut);
    }
    
    
    void fzMath_mat4Multiply(const float* m1, const float* m2, float* mOut)
    {
        FZ_ASSERT(m1 != NULL, "Input matrix 1 cannot be NULL.");
        FZ_ASSERT(m2 != NULL, "Input matrix 2 cannot be NULL.");
        FZ_ASSERT(mOut != NULL, "Output matrix cannot be NULL.");
        FZ_ASSERT(m1 != mOut && m2 != mOut, "Input and output can not have the same pointer.");
        
        _inline_mat4Multiply(m1, m2, mOut);
    }
    
    
    void fzMath_mat4Vec2(const float* m1, const float* v1, float* vOut)
    {
        FZ_ASSERT(m1 != NULL, "Input matrix 1 cannot be NULL.");
        FZ_ASSERT(v1 != NULL, "Input matrix 2 cannot be NULL.");
        FZ_ASSERT(vOut != NULL, "Output vertices cannot be NULL.");
        
        _inline_mat4Vec2(m1, v1, vOut);
    }
    
    
    void fzMath_mat4Vec4(const float* m1, const float* v1, float* vOut)
    {
        FZ_ASSERT(m1 != NULL, "Input matrix 1 cannot be NULL.");
        FZ_ASSERT(v1 != NULL, "Input matrix 2 cannot be NULL.");
        FZ_ASSERT(vOut != NULL, "Output vertices cannot be NULL.");
        
        _inline_mat4Vec4(m1, v1, vOut);
    }
    
    
    bool fzMath_mat4Invert(const float *m, float *mOut)
    {
        double det;
        int i;
        
        mOut[0] = m[5] * m[10] -
        m[9]  * m[6];
        
        mOut[4] = -m[4] * m[10] +
        m[8]  * m[6];
        
        mOut[8] = m[4] * m[9] -
        m[8]  * m[5];
        
        mOut[12] = -m[4] * m[9] * m[14] +
        m[4]  * m[10] * m[13] +
        m[8]  * m[5] * m[14] -
        m[8]  * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];
        
        mOut[1] = -m[1] * m[10] +
        m[9]  * m[2];
        
        mOut[5] = m[0] * m[10] -
        m[8]  * m[2];
        
        mOut[9] = -m[0] * m[9] +
        m[8]  * m[1];
        
        mOut[13] = m[0] * m[9] * m[14] -
        m[0]  * m[10] * m[13] -
        m[8]  * m[1] * m[14] +
        m[8]  * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];
        
        mOut[2] = m[1]  * m[6] -
        m[5]  * m[2];
        
        mOut[6] = -m[0]  * m[6] +
        m[4]  * m[2];
        
        mOut[10] = m[0]  * m[5] -
        m[4]  * m[1];
        
        mOut[14] = -m[0]  * m[5] * m[14] +
        m[0]  * m[6] * m[13] +
        m[4]  * m[1] * m[14] -
        m[4]  * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];
        
        mOut[3] = 0;
        mOut[7] = 0;
        mOut[11] = 0;
        mOut[15] = 1;
        
        det = m[0] * mOut[0] + m[1] * mOut[4] + m[2] * mOut[8] + m[3] * mOut[12];
        
        if (det == 0) {
            FZLOGERROR("fzAffineTransform: Imposible to get inverse.");
            return false;
        }
        
        det = 1.0 / det;
        
        for (i = 0; i < 15; i++)
            mOut[i] *= det;
        
        return true;
    }
    
    
    void fzMath_mat4Print(const float* O)
    {
        FZ_ASSERT(O != NULL, "Input matrix cannot be NULL.");

        FZLog("Matrix 4x4 %p {\n"
              "\t{%f, %f, %f, %f}\n"
              "\t{%f, %f, %f, %f}\n"
              "\t{%f, %f, %f, %f}\n"
              "\t{%f, %f, %f, %f}\n}", O,
              O[0],O[4],O[8],O[12],
              O[1],O[5],O[9],O[13],
              O[2],O[6],O[10],O[14],
              O[3],O[7],O[11],O[15]);
    }
    
    
    void fzMath_mat4PerspectiveProjection(fzFloat fovY, fzFloat aspect,
                                          fzFloat zNear, fzFloat zFar,
                                          float *output)
    {
        FZ_ASSERT(output != NULL, "Output matrix cannot be NULL.");

        fzFloat r = FZ_DEGREES_TO_RADIANS(fovY / 2);
        fzFloat deltaZ = zFar - zNear;
        fzFloat cotangent = 1/tanf(r);
        
        if (deltaZ == 0 || cotangent == 0 || aspect == 0) {
            FZLOGERROR("Perpertive impossible.");
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
        FZ_ASSERT(output != NULL, "Output pointer cannot be NULL.");

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
    
  
    void fzMath_mat4LookAt(const fzPoint3& eye, const fzPoint3& center, const fzPoint3& up, float *output)
    {     
        FZ_ASSERT(output != NULL, "Output pointer cannot be NULL.");

        fzPoint3 f(center);
        f -= eye;
        f.normalize();

        fzPoint3 up2(up);
        up2.normalize();

        fzPoint3 s = f.getCrossed(up2);
        s.normalize();
        
        fzPoint3 u(s.getCrossed(f));
        // u.normalize();
        //s.normalize();
        
        fzMath_mat4Identity(output);
        
        output[0] = s.x;  //1
        output[4] = s.y;
        output[8] = s.z;
        
        output[1] = u.x;
        output[5] = u.y; // 1
        output[9] = u.z;
        
        output[2] = -f.x;
        output[6] = -f.y;
        output[10] = -f.z; // 1
    

        float x = -eye.x; //mat[12]
        float y = -eye.y; //mat[13]
        float z = -eye.z; //mat[14]


        float mat[4];
        mat[0] = output[0] * x + output[4] * y + output[8] * z + output[12];
        mat[1] = output[1] * x + output[5] * y + output[9] * z + output[13];
        mat[2] = output[2] * x + output[6] * y + output[10] * z + output[14];
        mat[3] = output[3] * x + output[7] * y + output[11] * z + output[15];
        
        
        memcpy(&output[12], mat, sizeof(float)*4);        
    }
}
