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

#include <string.h>
#include "FZDeviceConfig.h"
#include "FZMacros.h"
#include "FZMath.h"


namespace FORZE {
    
    static inline const char* boolToText(bool b)
    {
        return (b) ? "YES" : "NO";
    }
    
    
    static inline const char* userInterfaceIdiomToText(fzUserInterface b)
    {
        switch (b) {
            case kFZUserInterfaceIdiomPhone: return "Phone";
            case kFZUserInterfaceIdiomPad: return "Pad";
            case kFZUserInterfaceIdiomPC: return "PC";
            default: return "Unknow";
        }
    }
    
    
    DeviceConfig* DeviceConfig::p_instance = NULL;
    
    DeviceConfig& DeviceConfig::Instance()
    {
        if (p_instance == NULL)
            p_instance = new DeviceConfig(-1);
        
        return *p_instance;
    }
    
    
    void DeviceConfig::SimulateInterface(int interface)
    {
        if (p_instance == NULL) {
            p_instance = new DeviceConfig(interface);
        }else{
            FZ_ASSERT(false, "You can not call after the singleton was initialized.");
        }
    }
    
    
    DeviceConfig::DeviceConfig(int interface)
    : p_glExtensions(NULL)
    , p_deviceCode(NULL)
    , m_OSVersion(0)
    , m_maxTextureSize(0)
    , m_maxSamplesAllowed(0)
    , m_supportsPVRTC(false)
    , m_supportsNPOT(false)
    , m_supportsBGRA8888(false)
    , m_supportsDiscardFB(false)
    {
        char tmp[128];

        // GET OPENGL EXTENSIONS
        p_glExtensions = fzStrcpy((char*)glGetString(GL_EXTENSIONS));
        
        // GET DEVICE CODE
        fzOSW_getDeviceCode(tmp, 128);
        p_deviceCode = fzStrcpy(tmp);
        
        // GET OS VERSION (not needed yet)
        //fzDevice_getOSVersion(&m_OSVersion);
        
        // GET OPENGL MAX TEXTURE SIZE
        glGetIntegerv(FZ_MAX_TEXTURE_SIZE, &m_maxTextureSize);
        
        // GET OPENGL MAX MULTISAMPLING
        glGetIntegerv(FZ_MAX_SAMPLES, &m_maxSamplesAllowed);
        
        // GET OPENGL EXTENSIONS
        m_supportsNPOT       = checkForGLExtension(FZ_EXTENSION_POT);
        m_supportsBGRA8888   = checkForGLExtension(FZ_EXTENSION_BGRA);
        m_supportsDiscardFB  = checkForGLExtension(FZ_EXTENSION_DISCARD_FRAMEBUFFER);
        m_supportsPVRTC      = checkForGLExtension(FZ_EXTENSION_PVRTC);

        // USER INTERFACE IDIOM
        if(interface == -1)
            m_userInterfaceIdiom = static_cast<fzUserInterface>(fzOSW_getUserInterfaceIdiom());
        else
            m_userInterfaceIdiom = static_cast<fzUserInterface>(interface);
        
        CHECK_GL_ERROR_DEBUG();
    }
    
    
    DeviceConfig::~DeviceConfig()
    {
        delete p_glExtensions;
    }
    
    
    void DeviceConfig::logDeviceInfo() const
    {
        FZLog("DeviceConfig: System Info:\n"
              " - DEVICE CODE:                     %s\n"
              " - USER INTERFACE IDIOM:            %s\n"
              " - GL_VENDOR:                       %s\n"
              " - GL_RENDERER:                     %s\n"
              " - GL_VERSION:                      %s\n"
              " - GL_SHADING_LANGUAGE_VERSION:     %s\n"
              " - GL_MAX_TEXTURE_SIZE:             %d %cx%d\n"
              " - GL_MAX_SAMPLES:                  %d\n"
              " - GL supports PVRTC textures:      %s\n"
              " - GL supports BGRA8888 textures:   %s\n"
              " - GL supports discard_framebuffer: %s\n"
              " - VBO streaming in TextureAtlas:   %s\n",
              getDeviceCode(),
              userInterfaceIdiomToText(getUserInterfaceIdiom()),
              glGetString (GL_VENDOR),
              glGetString (GL_RENDERER),
              glGetString (GL_VERSION),
              glGetString (GL_SHADING_LANGUAGE_VERSION),
              getMaxTextureSize(), FZ_IO_SUBFIX_CHAR, getMaxFactor(),
              getMaxSamplesAllowed(),
              boolToText(isPVRTCSupported()),
              boolToText(isBGRA8888Supported()),
              boolToText(isNPOTSupported()),
              boolToText(isDiscardFramebufferSupported()),
              boolToText(FZ_VBO_STREAMING));

    }
    
    const char* DeviceConfig::getDeviceCode() const
    {
        return p_deviceCode;
    }
    
    
    bool DeviceConfig::checkForGLExtension(const char* searchName)
    {
        if(p_glExtensions == NULL)
            return false;
        
        char *found = strstr(p_glExtensions, searchName);
        return (found != NULL);
    }
    
    
    GLint DeviceConfig::getMaxTextureSize() const
    {
        return m_maxTextureSize;
    }
    
    
    fzUInt DeviceConfig::getMaxFactor() const
    {
        return fzMax(m_maxTextureSize >> 10, 1);
    }
    
    
    GLint DeviceConfig::getMaxSamplesAllowed() const
    {
        return m_maxSamplesAllowed;
    }
    
    
    bool DeviceConfig::isNPOTSupported() const
    {
        return m_supportsNPOT;
    }
    
    
    bool DeviceConfig::isPVRTCSupported() const
    {
        return m_supportsPVRTC;
    }
    
    
    bool DeviceConfig::isBGRA8888Supported() const
    {
        return m_supportsBGRA8888;
    }
    
    
    fzUserInterface DeviceConfig::getUserInterfaceIdiom() const
    {
        return m_userInterfaceIdiom;
    }
    
    
    bool DeviceConfig::isDiscardFramebufferSupported() const
    {
        return m_supportsDiscardFB;
    }
    
    
    fzUInt DeviceConfig::getOSVersion() const
    {
        return m_OSVersion;
    }
}
