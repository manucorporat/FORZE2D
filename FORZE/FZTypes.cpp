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
#include "Optimized/FZMathInline.h"


namespace FORZE {
    
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
        
        origin.x = fzMin(fzMin(fzMin(output[0], output[2]), output[4]), output[6]);
        origin.y = fzMin(fzMin(fzMin(output[1], output[3]), output[5]), output[7]);
        
        size.width = (fzMax(fzMax(fzMax(output[0], output[2]), output[4]), output[6])) - origin.x;
        size.height = (fzMax(fzMax(fzMax(output[1], output[3]), output[5]), output[7])) - origin.y;
        
        return *this;
    }
    
    
    fzRect& fzRect::applyTransform(const fzAffineTransform& t)
    {
        return applyTransform(t.m);
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
    
    fzAffineTransform::fzAffineTransform(const float *matrix)
    {
        _inline_mat4Copy(matrix, m);
    }
    
    
    void fzAffineTransform::assign(fzFloat data[7])
    {
        m[0] = data[0];
        m[1] = data[1];
        m[2] = 0;
        m[3] = 0;
        m[4] = data[2];
        m[5] = data[3];
        m[6] = 0;
        m[7] = 0;
        m[8] = 0;
        m[9] = 0;
        m[10] = 0;
        m[11] = 0;
        m[12] = data[4];
        m[13] = data[5];
        m[14] = data[6];
    }
    
    
    fzAffineTransform fzAffineTransform::getInverse() const
    {        
        fzAffineTransform invert;
        fzMath_mat4Invert(m, invert.m);
        return invert;
    }
    
    
    void fzAffineTransform::log() const
    {
        fzMath_mat4Print(m);
    }
    
    
    fzAffineTransform& fzAffineTransform::translate(float x, float y, float z)
    {
        if(x != 0 || y != 0 || z != 0)
            _inline_mat4Tranlate(m, x, y, z);
        
        return *this;
    }
    
    
    fzAffineTransform& fzAffineTransform::rotate(fzFloat radians)
    {
        if(radians != 0)
            _inline_mat4Rotate(m, radians);
        
        return *this;
    }
    
    
    fzAffineTransform& fzAffineTransform::concat(const fzAffineTransform& t)
    {
        _inline_mat4MultiplySafe(m, t.m, m);
        return *this;
    }

    
    fzBlendFunc::fzBlendFunc()
    : src(GL_SRC_ALPHA), dst(GL_ONE_MINUS_SRC_ALPHA)
    {}
}
