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

#include "windows_support.h"

#if defined(FZ_OS) && (FZ_OS == kFZPLATFORM_WINDOWS)

#include "FZEventManager.h"
#include "FZDeviceConfig.h"
#include "FZDirector.h"
#include "FZMacros.h"

#include <sys/types.h>
#include <sys/sysctl.h>
#include <OpenGL/OpenGL.h>


using namespace FORZE;

namespace FORZE {
    
#pragma mark - OS WRAPPER communication protocol
    
    void fzOSW_init(int argc, char *argv[])
    {
        new _FZOSWRAPPER();
    }
    
    
    void fzOSW_startRendering(void *oswrapper, fzFloat interval)
    {
        ((_FZOSWRAPPER*)oswrapper)->startRendering(interval);
    }
    
    
    void fzOSW_stopRendering(void *oswrapper)
    {
        ((_FZOSWRAPPER*)oswrapper)->stopRendering();
    }
    
    
    void fzOSW_configWindow(void *oswrapper)
    {
        ((_FZOSWRAPPER*)oswrapper)->updateWindow();
    }
    
    
    void fzOSW_configOrientation(void *oswrapper, int orientation)
    {
        // ORIENTATION DOESN'T MAKE SENSE IN OSX
        // Nothing to do here :)
    }
    
    
    void fzOSW_configEvents(void *oswrapper, uint16_t dirtyFlags, uint16_t flags)
    {
        ((_FZOSWRAPPER*)oswrapper)->updateEvents(dirtyFlags, flags);
    }
    
    
    void fzOSW_configEventsInterval(void *oswrapper, fzFloat interval)
    {
        // CONSTANT TIME EVENTS DON'T EXIST IN OSX
        // Nothing to do here :)
    }
    
    
#pragma mark Device
    
    void fzDevice_getOSVersion(unsigned int *os)
    {
        FZLog("NOT IMPLEMENTED YET");
    }
    
    
    void fzDevice_getScreenSize(fzSize *size, fzFloat *factor)
    {
        NSRect rect = [[NSScreen mainScreen] frame];
        size->width = rect.size.width;
        size->height = rect.size.height;
        
        *factor = [[NSScreen mainScreen] backingScaleFactor];
    }
    
    
    uint16_t fzDevice_getCapacities()
    {
        return kFZEventType_Mouse | kFZEventType_MouseRight | kFZEventType_Keyboard;
    }
    
    
    int fzDevice_getUserInterfaceIdiom()
    {
        return kFZUserInterfaceIdiomPC;
    }
    
    
    void fzDevice_getDeviceCode(char *deviceCode, fzUInt maxLength)
    {
        size_t size;
        if(sysctlbyname("hw.model", deviceCode, &size, NULL, 0) == -1)
            FZ_RAISE("OS Wrapper: sysctlbyname() failed.");
        
        deviceCode[size] = '\0';
        
        if(size > maxLength)
            FZ_RAISE("OS Wrapper: Memory overload. You should allocate more memory");
    }
    
    
    bool fzDevice_getProductName(char *path, fzUInt bufferLength)
    {
        NSDictionary* infoDictionary = [[NSBundle mainBundle] infoDictionary];
        NSString *productName = [infoDictionary objectForKey:@"CFBundleName"];
        return [productName getCString:path maxLength:bufferLength encoding:NSUTF8StringEncoding];
    }
    
    
    bool fzDevice_getResourcesPath(char *path, fzUInt maxLength)
    {
        NSString *resourcesPath = [[NSBundle mainBundle] resourcePath];
        return [resourcesPath getCString:path maxLength:maxLength encoding:NSUTF8StringEncoding];
    }
    
    
    bool fzDevice_getPersistentPath(const char *filename, char *absolutePath, fzUInt bufferLength)
    {
        // GET APPLICATION SUPPORT DIRECTORY
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSString *nspath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
        
        // GET PRODUCT NAME
        char productName[512];
        if(!fzDevice_getProductName(productName, 512))
            FZ_RAISE("Imposible to get product name. No enough memory.");
        
        // GENERATE ABSOLUTE PATH
        nspath = [nspath stringByAppendingPathComponent:[NSString stringWithCString:productName encoding:NSUTF8StringEncoding]];
        nspath = [nspath stringByAppendingPathComponent:[NSString stringWithCString:filename encoding:NSUTF8StringEncoding]];
        
        return [nspath getCString:absolutePath maxLength:bufferLength encoding:NSUTF8StringEncoding];
    }
    
    
    bool fzDevice_createDirectory(const char *path, bool pathIsDirectory)
    {
        NSString *nspath = [NSString stringWithCString:path encoding:NSUTF8StringEncoding];
        BOOL exists = [[NSFileManager defaultManager] fileExistsAtPath:nspath isDirectory:NULL];
        
        // Create directory
        if(exists == NO) {
            nspath = (pathIsDirectory) ? nspath : [nspath stringByDeletingLastPathComponent];
            [[NSFileManager defaultManager] createDirectoryAtPath:nspath withIntermediateDirectories:YES attributes:nil error:nil];
        }
    }
    
    
    bool fzDevice_removePath(const char *path)
    {
        NSString *nspath = [NSString stringWithCString:path encoding:NSUTF8StringEncoding];
        return [[NSFileManager defaultManager] removeItemAtPath:nspath error:nil];
    }
    


#pragma mark -
#pragma mark - OS WRAPPER IMPLEMENTATION
    
    static void drawScene(void *context)
    {
        ((_FZOSWRAPPER*)contex)->loop();
    }
    
    
    
    _FZOSWRAPPER::_FZOSWRAPPER(HINSTANCE instancia, int ncmdShow)
    {
        // STEP ZERO.
        mgrDirector_ = &Director::Instance();
        mgrEvents_ = &EventManager::Instance();
        instance_ = instancia;
        cmdShow_ = cmdShow;
        
        
        // FIRST STEP. Notify Director::applicationLaunching()
        // - gets opengl context config
        // - attachs OS wrapper to director
        mgrDirector_->applicationLaunching(this); //this = OS Wrapper pointer
        
        
        // SECOND STEP. Config UI.
        // - configs view: opacity, retina display...
        // - creates the window
        setupUI();
        
        
        // THIRD STEP. OpenGL initialization.
        // - creates the opengl context.
        // - set the default window size.
        // - attachs the context to the view.
        setupOpenGL();
        
        
        // FOURTH STEP. Make window visible.
        // - "connects" the windows with the screen.
        ShowWindow(window_, cmdShow_);
        UpdateWindow(window_);
        
        
        // FIFTH STEP. Notify Directed::applicationLaunched()
        // - ends director's initialization
        // - appdelegate->applicationLaunched() is called
        // - -> game loop starts
        mgrDirector_->applicationLaunched(options);
        
        
        // WAIT FOR EVENTS
        MSG msg;
        while(GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if(thread_ != NULL)
            thread_->join();
    }
    
    
    _FZOSWRAPPER::~_FZOSWRAPPER()
    {
        // Delete the openGL rendering context
        if (m_hGLRenderingContext)
        {
            wglDeleteContext(m_hGLRenderingContext);
            m_hGLRenderingContext = NULL;
        }
        
        // Release the Windows' device context
        if (m_hDeviceContext)
        {
            ReleaseDC(m_hWindow, m_hDeviceContext);
            m_hDeviceContext = NULL;
        }
        
        m_hWindow = NULL;
    }
    
    
    void _FZOSWRAPPER::setupUI()
    {
        WNDCLASSEX wcex;
        
        wcex.cbSize = sizeof(WNDCLASSEX);
        
        wcex.style			= CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc	= WndProc;
        wcex.cbClsExtra		= 0;
        wcex.cbWndExtra		= 0;
        wcex.hInstance		= instance_;
        wcex.hIcon			= LoadIcon(instance_, MAKEINTRESOURCE(IDI_SIMPLEGL));
        wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SIMPLEGL);
        wcex.lpszClassName	= szWindowClass;
        wcex.hIconSm		= LoadIcon(instance_, MAKEINTRESOURCE(IDI_SMALL));
        
        RegisterClassEx(&wcex);
        
        // Perform application initialization:
        HWND hWnd;
        window_ = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, instance_, NULL);
        
        if(!hWnd)
            FZ_RAISE("OS Wrapper: Impossible to init instance.");
        
        
        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);
    }
    
    
    void _FZOSWRAPPER::setupOpenGL()
    {        
        // Determine the size of the hWindow
        RECT clientRect;
        GetClientRect(window_, &clientRect);
        
        // Create the pixel format descriptor
        const FORZE::GLConfig& glconfig = mgrDirector_->getGLConfig();

        PIXELFORMATDESCRIPTOR pfd;
        ZeroMemory(&pfd, sizeof(pfd));
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = glconfig.colorFormat;
        pfd.cDepthBits = glconfig.depthFormat;
        pfd.iLayerType = PFD_MAIN_PLANE;
        
        // See which format we should use
        deviceContext_ = GetDC(window_);
        int pixelFormat = ChoosePixelFormat(deviceContext_, &pfd);
        if (pixelFormat)
        {
            // Try to set the selected format
            if (SetPixelFormat(deviceContext_, pixelFormat, &pfd))
            {
                // Create the windows openGL context
                openGLContext_ = wglCreateContext(deviceContext_);
                wglMakeCurrent(deviceContext_, openGLContext_);
            }
        }
    }
    
    
    void _FZOSWRAPPER::startRendering(float interval)
    {
        if(isRunning_ == false) {
            isRunning_ = true;
            thread_ = new std::thread(drawScene, this);
        }
    }
    
    
    void _FZOSWRAPPER::stopRendering()
    {
        if(isRunning_ == true) {
            isRunning_ = false;
            delete thread_;
        }
    }
    
    
    void _FZOSWRAPPER::updateEvents(uint16_t dirtyFlags, uint16_t flags)
    {
        
    }
    
    
    void _FZOSWRAPPER::updateWindow(float sizeX, float sizeY, float rectOx, float rectOy, float rectSx, float rectSy)
    {
        
    }
    
    void _FZOSWRAPPER::loop()
    {
        while(isRunning_) {
            
            mgrDirector_->drawScene();
            SwapBuffers(m_hDeviceContext);
            
        }
    }
}


#endif
