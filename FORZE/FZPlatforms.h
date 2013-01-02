// DO NOT MODIFY THE HEADERS IF FORZE IS ALREADY COMPILED AS A STATIC LIBRARY
#ifndef __FZPLATFORMS_H_INCLUDED__
#define __FZPLATFORMS_H_INCLUDED__
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

#include "FZPlatformsHeader.h"

#ifndef FZ_OS

//#warning FORZE: The operative system was not specified.
//#warning You must define FZ_OS. See FZPlatformsHeader.h

#define FZ_OS kFZPLATFORM_MODEL
#endif

#include "FZTypes.h"

#if (FZ_OS == kFZPLATFORM_IOS_GL_1)
#define FZ_OS_MOBILE 1
#include "Wrappers/iOSGL1_support.h"

#elif (FZ_OS == kFZPLATFORM_IOS_GL_2)
#define FZ_OS_MOBILE 1
#include "Wrappers/iOSGL2_support.h"

#elif (FZ_OS == kFZPLATFORM_MAC)
#define FZ_OS_DESKTOP 1
#include "Wrappers/macosx_support.h"

#elif (FZ_OS == kFZPLATFORM_PSVITA)
#include "Wrappers/psvita_support.h"

#elif (FZ_OS == kFZPLATFORM_MODEL)
#include "Wrappers/model_support.h"

#else

#error "FORZE: Invalid operative system."

#endif

#include "FZDefaultOpenGLAPI.h"


namespace FORZE {
    
    class GLConfig;
    class GLManager;
    class EventManager;
        
    // OS WRAPPER
    // FORZE allocates an private class (Objective-C or C++ class) that connects the OS API with FORZE.
    // All this methods are low level, and they have a different implementation for each platform.
    // The common use never should have to use this methods.
    class OSW
    {        
    public:
        static void* p_oswrapper;
        static void* Instance();
        static void setInstance(void *instance);
        
        
        //! Initializes the wrapper class, initialize all OS runtime tasks and call notify that to the Director.
        static void init(int argc, char *argv[]);
        
        //! Starts the loop thread giving the OSWRAPPER pointer
        static void startRendering(fzFloat interval);
        
        //! Stops the loop thread giving the OSWRAPPER pointer
        static void stopRendering();
        
        //! Updates the window size and the rendering rect.
        static void updateWindow();
        
        //! Enables or disables the IO events, (touch, accelerometer) giving the OSWRAPPER
        static void configEvents(uint16_t dirtyFlags, uint16_t flags);
        
        //! Sets event internval
        static void setEventsInterval(fzFloat interval);
        
        //! Sets orientation
        static void setOrientation(int orientation);
    };
    
    

    // DEVICE
    //! Returns the OS version.
    void fzDevice_getOSVersion(unsigned int *os);
  
    //! Returns the screen size in points and the factor scale.
    void fzDevice_getScreenSize(fzSize *size, fzFloat *factor);
    
    //! Returns system capacities (accelerometer...)
    uint16_t fzDevice_getCapacities();
    
    //! Returns the device's user interface idiom.
    int fzDevice_getUserInterfaceIdiom();
  
    //! Returns the device identifier code.
    void fzDevice_getDeviceCode(char *deviceCode, fzUInt maxLength);
  
    //! Returns the product name.
    bool fzDevice_getProductName(char *path, fzUInt bufferLength);
  
    //! Returns the application's absolute resources path.
    bool fzDevice_getResourcesPath(char *path, fzUInt maxLength);
  
    //! Returns the application persistent resources path.
    bool fzDevice_getPersistentPath(const char *filename, char *absolutePath, fzUInt bufferLength);
    
    //! Returns the application persistent resources path.
    bool fzDevice_createDirectory(const char *path, bool pathIsDirectory);
    
    //! Removes the specified path.
    bool fzDevice_removePath(const char *path);
}
#endif
