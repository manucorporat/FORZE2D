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
#include "FZMacros.h"
#include "FZMath.h"


namespace FORZE {
    
    template <typename T>
    inline T min(T a, T b) {
        return (a<b) ? a : b;
    }
    
    template <typename T>
    inline T max(T a, T b) {
        return (a>b) ? a : b;
    }
    
    
#pragma mark - fzColor3B implementation
    
    fzColor3B::fzColor3B(const _fzColor4<unsigned char, 255>& c)
    : r(c.r), g(c.g), b(c.b) {}
    
    fzColor3B::fzColor3B(const _fzColor4<fzFloat, 1>& c)
    : r(c.r * 255), g(c.g * 255), b(c.b * 255) {}


#pragma mark - fzRect implementation
    
    fzRect& fzRect::applyTransform(const float *matrix)
    {
        register float output[8];
        {
            const float vertices[8] = {
                origin.x, origin.y,
                origin.x + size.width, origin.y,
                origin.x + size.width, origin.y + size.height,
                origin.x, origin.y + size.height
            };
            
            // Hardware accelerated vertices calculation
            fzMath_mat4Vec2(matrix, vertices, output);
        }
        
        origin.x = min(min(min(output[0], output[2]), output[4]), output[6]);
        origin.y = min(min(min(output[1], output[3]), output[5]), output[7]);
        
        size.width = (max(max(max(output[0], output[2]), output[4]), output[6])) - origin.x;
        size.height = (max(max(max(output[1], output[3]), output[5]), output[7])) - origin.y;
        
        return *this;
    }
    
    
    fzRect& fzRect::applyTransform(const fzAffineTransform& t)
    {
        register fzMat4 matrix;
        t.generateGLMatrix(matrix);
        
        return applyTransform(matrix);
    }
    
    
    bool fzRect::intersect(const fzRect& r) const
    {
        return
        (r.origin.x < (origin.x + size.width)) &&
        ((r.origin.y + r.size.width) > origin.x) &&
        (r.origin.y < (origin.y + size.height)) &&
        ((r.origin.y + r.size.height) > origin.y);
    }
    
    
#pragma mark - fzAffineTransform implementation
    
    void fzAffineTransform::generateGLMatrix(float *O) const
    {
        O[2] = O[3] = O[6] = O[7] = O[8] = O[9] = O[11] = 0.0f;
        O[10] = O[15] = 1.0f;
        O[0] = m[0];
        O[1] = m[1];
        O[4] = m[2];
        O[5] = m[3];
        O[12] = m[4];
        O[13] = m[5];
        O[14] = m[6];
    }
    
    
    void fzAffineTransform::assign(fzFloat *data)
    {
        fzFloat sina = fzMath_sin(data[0]);
        fzFloat cosa = fzMath_cos(data[0]);
        
        m[0] = cosa * data[1];
        m[1] = sina * data[1];
        m[2] = -sina * data[2];
        m[3] = cosa * data[2];
        m[4] = data[3];
        m[5] = data[4];
        m[6] = data[5]; 
    }
    
    
    fzAffineTransform fzAffineTransform::getInverse() const
    {
        if(m[0]==0 || m[3]==0)
            return *this;
        
        const fzFloat det = 1 / (m[0]*m[3] - m[1]*m[2]);
        return fzAffineTransform(m[3] * det,
                                 -m[1] * det,
                                 -m[2] * det,
                                 m[0] * det,
                                 (m[5]*m[2]-m[4]*m[3]) * det,
                                 (m[4]*m[1]-m[5]*m[0]) * det
                                 );
    }
    
    
    fzAffineTransform& fzAffineTransform::rotate(fzFloat radians)
    {
        const fzFloat sina = fzMath_sin(radians);
        const fzFloat cosa = fzMath_cos(radians);
        
        fzFloat aa = cosa * m[0]; aa += sina * m[2];
        fzFloat bb = cosa * m[1]; bb += sina * m[3];
        m[2] *= cosa; m[2] -= sina * m[0];
        m[3] *= cosa; m[3] -= sina * m[1];
        m[0] = aa; m[1] = bb;
        
        return *this;
    }
    
    
    fzAffineTransform& fzAffineTransform::concat(const fzAffineTransform& t)
    {
        fzFloat aa = m[0] * t.m[0] + m[1] * t.m[2];
        fzFloat bb = m[0] * t.m[1] + m[1] * t.m[3];
        fzFloat cc = m[2] * t.m[0] + m[3] * t.m[2];
        fzFloat dd = m[2] * t.m[1] + m[3] * t.m[3];
        fzFloat txx = m[4] * t.m[0] + m[5] * t.m[2] + t.m[4];
        fzFloat tyy = m[4] * t.m[1] + m[5] * t.m[3] + t.m[5];
        m[0] = aa; m[1] = bb; m[2] = cc;
        m[3] = dd; m[4] = txx; m[5] = tyy;
        
        return *this;
    }

    
    fzBlendFunc::fzBlendFunc()
    : src(GL_SRC_ALPHA), dst(GL_ONE_MINUS_SRC_ALPHA)
    {}
}
