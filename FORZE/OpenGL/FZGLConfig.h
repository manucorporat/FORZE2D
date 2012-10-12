// DO NOT MODIFY THE HEADERS IF FORZE IS ALREADY COMPILED AS A STATIC LIBRARY
#ifndef __FZGLCONFIG_H_INCLUDED__
#define __FZGLCONFIG_H_INCLUDED__
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
    
    enum fzDepthFormat
    {
        kFZGLConfigDepthNone = 0,
        kFZGLConfigDepth16 = 16,
        kFZGLConfigDepth24 = 24
    };
    
    
    enum fzColorFormat
    {
        kFZGLConfigColorRGB565 = 16,
        kFZGLConfigColorRGB888 = 24,
        kFZGLConfigColorRGBA8888 = 32
    };
    
    
    class GLConfig 
    {
    public:
        bool preserveBackBuffer;
        fzDepthFormat depthFormat;
        fzColorFormat colorFormat;
        fzUInt multiSampling;
        fzFloat quality;
        
        
        //! Constructs a default opengl context config.
        //! - Color format: RGB565 (16 bits)
        //! - Depth format: 0
        //! - Preserve back buffering: false
        //! - MultiSampling: false
        //! - Render buffer size: Auto
        GLConfig();
        
        
        //! Constructs a custom opengl context config.
        GLConfig(fzColorFormat color,
                 fzDepthFormat depth,
                 bool backBuffer,
                 fzUInt multiSampling,
                 fzFloat quality);
        
        void validate() const;
    };
}
#endif
