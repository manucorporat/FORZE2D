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

#include "FZMathApprox.h"


namespace FORZE {

    fzFloat fast_sqrt(fzFloat z)
    {
        union
        {
            int tmp;
            float f;
        } u;
        
        u.f = z;
        u.tmp -= 1 << 23; /* Subtract 2^m. */
        u.tmp >>= 1; /* Divide by 2. */
        u.tmp += 1 << 29; /* Add ((b + 1) / 2) * 2^m. */
        
        return u.f;
    }
    
    
    fzFloat fast_invsqrt(fzFloat x)
    {
        float xhalf = 0.5f * x;
        union
        {
            int i;
            float x;
        } u;
        u.x = x;
        u.i = 0x5f3759df - (u.i >> 1);
        return (u.x * (1.5f - xhalf * u.x * u.x));
    }
    
    
    fzFloat fast_sin(fzFloat x)
    {
        const fzFloat B = 4/M_PI;
        const fzFloat C = -4/(M_PI*M_PI);
        
        fzFloat y = B * x + C * x * fabsf(x);
        
        const float P = 0.225f;
        y = P * (y * fabsf(y) - y) + y;   // Q * y + P * y * abs(y)
        
        return y;
    }
}


