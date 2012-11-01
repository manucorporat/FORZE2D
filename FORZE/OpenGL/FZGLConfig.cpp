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

#include "FZGLConfig.h"
#include "FZMacros.h"


namespace FORZE {
        
    GLConfig::GLConfig()
    : preserveBackBuffer(false)
    , depthFormat(kFZGLConfigDepthNone)
    , colorFormat(kFZGLConfigColorRGB565)
    , multiSampling(0)
    , quality(1)
    { }
    
    
    GLConfig::GLConfig(fzColorFormat color, fzDepthFormat depth, bool backBuffer, fzUInt sampling, fzFloat qualitya)
    : preserveBackBuffer(backBuffer)
    , depthFormat(depth)
    , colorFormat(color)
    , multiSampling(sampling)
    , quality(qualitya)
    {
        FZ_ASSERT(quality > 0 && quality <= 1.0f, "Quality out of bounds (0, 1]");
    }
    
    
    void GLConfig::validate() const
    {
        if(quality <= 0 || quality > 1.0f)
            FZ_RAISE_STOP("GLConfig: Quality out of bounds (0, 1].");
    }
}
