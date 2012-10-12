// DO NOT MODIFY THE HEADERS IF FORZE IS ALREADY COMPILED AS A STATIC LIBRARY
#ifndef __FZ_SUPPORT_MAC_H_INCLUDED__
#define __FZ_SUPPORT_MAC_H_INCLUDED__
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


#if defined(FZ_OS) && (FZ_OS == kFZPLATFORM_WINDOWS)


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include "targetver.h"
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <thread>

// Opengl stuff
#include <GL/GL.h>
#include <GL/GLU.h>


#define FZ_OS_DESKTOP 1

#pragma mark - OpenGL extensions

#define FZ_EXTENSION_PVRTC "GL_IMG_texture_compression_pvrtc"
#define FZ_EXTENSION_POT "GL_ARB_texture_non_power_of_two"
#define FZ_EXTENSION_BGRA "GL_EXT_bgra"
#define FZ_EXTENSION_DISCARD_FRAMEBUFFER "GL_EXT_discard_framebuffer"


#pragma mark - OpenGL config

#define FZ_GL_PVRTC 0
#define FZ_GL_SHADERS 1



#pragma mark - OS WRAPPER

namespace FORZE {
    class Director;
    class EventManager;

    
    class _FZOSWRAPPER
    {
    private:
        HINSTANCE instance_;
        int cmdShow_;
        
        // managers
        FORZE::Director *mgrDirector_;
        FORZE::EventManager *mgrEvents_;
        
        // UI
        
        // Threading
        std::thread thread_;
        std::mutex eventsMutex_;
        bool isRunning_;
        
        // OpenGl
        float   m_theta;
        HWND    window_;
        HDC     deviceContext_;
        HGLRC   openGLContext_;
        
        
        
        
        void setupUI();
        void setupOpenGL();
        void startThreading();
        
    public:
        _FZOSWRAPPER();
        
        void startRendering(float interval);
        void stopRendering();
        void updateEvents(uint16_t dirtyFlags, uint16_t flags);
        void updateWindow(float sizeX, float sizeY, float rectOx, float rectOy, float rectSx, float rectSy);
        void loop(void*);
    };
}

#endif
#endif
