/*
 * FORZE ENGINE: http://forzefield.com
 *
 * Copyright (c) 2011-2012 FORZEFIELD Studios S.L.
 * Copyright (c) 2012 Manuel Martínez-Almeida
 * Copyright (c) 2008-2010 Ricardo Quesada
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

/* Idea of decoupling Window from Director taken from OC3D project: http://code.google.com/p/oc3d/
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "FZDirector.h"
#include "FZActionManager.h"
#include "FZTextureCache.h"
#include "FZFontCache.h"
#include "FZAnimationCache.h"
#include "FZEventManager.h"
#include "FZSpriteFrameCache.h"
#include "FZResourcesManager.h"
#include "FZScheduler.h"
#include "FZDeviceConfig.h"
#include "FZScene.h"
#include "FZGLState.h"
#include "FZGLProgram.h"
#include "FZMacros.h"
#include "FZTexture2D.h"
#include "FZMath.h"
#include "FZMS.h"
#include "FZDataStore.h"
#include "FZHUD.h"
#include "FZTransitions.h"
#include "FZPerformManager.h"


using namespace STD;

namespace FORZE {
    
#define kFZ_FPS_DEFAULT	60.0f
    
    Director* Director::p_instance = NULL;
    
    Director& Director::Instance()
    {
        if (p_instance == NULL)
            p_instance = new Director();
        
        return *p_instance;
    }
    
    
    Director::Director()
    : p_runningScene        (NULL)
    , p_appdelegate         (NULL)
    , p_nextScene           (NULL)
    , p_notificationNode    (NULL)
    , p_hud                 (NULL)
    , m_resourcesFactor     (1)
    , m_glConfig            ()
    , m_animationInterval   ( 1.0f / kFZ_FPS_DEFAULT )
    , m_projection          (kFZProjectionDefault)
    , m_resizeMode          (kFZResizeMode_Fit)
    , m_orientation         (kFZOrientation_Auto)
    , m_autoOrientation     (kFZOrientation_All)
    , m_displayFPS          (false)
    , m_frames              (0)
    , m_isInitialized       (false)
    , m_isRunning           (false)
    , m_isPaused            (false)
    , m_dirtyFlags          (true)
    , m_clearColor          (0, 0, 0.09f)
    , m_screenFactor        (1)
    , m_screenSize          (FZSizeZero)
    , m_canvasSize          (FZSizeZero)
    , m_windowSize          (FZSizeZero)
    , m_renderingRect       (FZRectZero)
    {
        FZLog(FORZE_VERSION);
		logDebugMode();
        
        // RANDOM STUFF
        unsigned int t = (unsigned int)time(NULL);
        srand(t);
        srandom(t);
        
        // INITIALIZE MATRIX STACK
        MS::initialize();
		
        // GET SCREEN SIZE
        fzDevice_getScreenSize(&m_screenSize, &m_screenFactor);
        
#ifndef FZ_OS_DESKTOP
        // if the OS is not a desktop OS, then the default window size
        // is equal to the screen size. -> we call setFullscreen()
        setFullscreen();
#else
        setOrientation(kFZOrientation_Portrait);
#endif        
    }
    
    
    void Director::logDebugMode() const
    {
#if !defined(NDEBUG) || (defined(FORZE_DEBUG) && FORZE_DEBUG)
        char debugMode[60];
        strcpy(debugMode, "DEBUG MODE: ");
        
        // ASSETION
#ifndef NDEBUG
        strcat(debugMode, "-assertions ");
#endif
        
#if defined(FORZE_DEBUG) && FORZE_DEBUG
        switch (FORZE_DEBUG) {
            case 1:
                strcat(debugMode, "-errors ");
                break;
            default:
                strcat(debugMode, "-errors -info");
                break;
                
                break;
        }
#endif
        
        FZLog("%s", debugMode);
        
#else
        FZLog("RELEASE MODE");
#endif
    }
    
    
#pragma mark - Setters
    
    void Director::setDelegate(AppDelegateProtocol *delegate)
    {
        FZ_ASSERT(delegate, "You cannot set a NULL app delegate.");
        p_appdelegate = delegate;
    }
    
    
    void Director::setCanvasSize(const fzSize& size)
    {
        if(m_originalCanvasSize == size)
            return;
                
        m_originalCanvasSize = size;
        m_dirtyFlags = kFZDDirty_viewPort;
    }
    
    
    void Director::setWindowSize(const fzSize& windowSize)
    {
        if(m_windowSize == windowSize)
            return;
        
        m_windowSize = windowSize;
        m_dirtyFlags = kFZDDirty_viewPort;
    }
    
    
    void Director::setFullscreen()
    {
        setWindowSize(m_screenSize/m_screenFactor);
    }
    
    
    void Director::setResizeMode(fzResizeMode resizeMode)
    {
        if(m_resizeMode == resizeMode)
            return;
        
        m_resizeMode = resizeMode;
        m_dirtyFlags = kFZDDirty_viewPort;
    }
    
    
    void Director::setProjection(fzProjection projection)
    {
        if(m_projection == projection)
            return;
        
        m_projection = projection;
        m_dirtyFlags = kFZDDirty_projection;
    }
    
    
    void Director::setOrientation(fzOrientation orientation)
    {
        if(m_orientation == orientation)
            return;
        
        m_orientation = orientation;
        m_dirtyFlags = kFZDDirty_viewPort;
    }
    
    
    void Director::setDisplayFPS(bool toDisplay)
    {
        FZ_ASSERT( OSW::Instance(), "Too soon, Director was not initialized.");
        
        if(toDisplay && p_hud == NULL) {
            p_hud = new HUD();
            p_hud->retain();
        }
        
        m_displayFPS = toDisplay;
    }
    
    
    void Director::setAnimationInterval(fzFloat interval)
    {
        FZ_ASSERT(interval >= 0, "Animation interval must be positive.");
        if(m_animationInterval == interval)
            return;
        
        m_animationInterval =  interval;
        if(isRunning()) {
            stopAnimation();
            startAnimation();
        }
    }
    
    
    void Director::setClearColor(const fzColor4F& color)
    {
        FZ_ASSERT(color.r >= 0 && color.r <= 1.0f, "Red component is out of bounds [0, 1].");
        FZ_ASSERT(color.g >= 0 && color.g <= 1.0f, "Green component is out of bounds [0, 1].");
        FZ_ASSERT(color.b >= 0 && color.b <= 1.0f, "Blue component is out of bounds [0, 1].");
        FZ_ASSERT(color.a >= 0 && color.a <= 1.0f, "Alpha component is out of bounds [0, 1].");

        m_clearColor = color;
    }
    
    
    void Director::setDepthTest(bool depthTest)
    {
        if (depthTest) {
            fzGLClearDepth(1.0f);
            
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);
        } else
            glDisable( GL_DEPTH_TEST );        
    }
    
    
    void Director::setDefaultGLValues()
    {
        // This method SHOULD be called only after openGLView_ was initialized
        FZ_ASSERT( OSW::Instance(), "The OSWrapper must be initialized.");
        setDepthTest(m_glConfig.depthFormat);
        
        // set other opengl default values
        glEnable(GL_BLEND);
        glActiveTexture(GL_TEXTURE0);
        
        //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
        
#if !FZ_GL_SHADERS
        glHint(GL_FOG_HINT, GL_FASTEST);
        glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
        glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
        glEnableClientState(GL_VERTEX_ARRAY);
#else
        glEnableVertexAttribArray(kFZAttribPosition);
#endif
    }
    
    
#pragma mark - Getters
    
    AppDelegateProtocol* Director::getDelegate() const
    {
        return p_appdelegate;
    }
    
    
    const fzSize& Director::getCanvasSize() const
    {
        return m_canvasSize;
    }
    
    
    const fzSize& Director::getWindowSize() const
    {
        return m_windowSize;
    }
    
    
    const fzRect& Director::getRenderingRect() const
    {
        return m_renderingRect;
    }
    
    
    fzOrientation Director::getOrientation() const
    {
        if(m_orientation == kFZOrientation_Auto) {
            
            const fzSize &windowSize = getWindowSize();
            fzSize canvasSize = (m_originalCanvasSize == kFZSize_Auto) ? windowSize : m_originalCanvasSize;
            
            fzFloat windowRate = windowSize.width/windowSize.height;
            fzFloat canvasRate = canvasSize.width/canvasSize.height;
            
            if(canvasRate == 1.0f || windowRate == 1.0f)
                return kFZOrientation_Portrait;
            
            if((canvasRate < 1.0f) != (windowRate < 1.0f))
                return kFZOrientation_LandscapeLeft;
            else
                return kFZOrientation_Portrait;
        }
        return m_orientation;
    }
    
    
    fzProjection Director::getProjection() const
    {
        return m_projection;
    }
    
    
    const fzSize& Director::getScreenSize() const
    {
        return m_screenSize;
    }
    
    
    fzSize Director::getViewPort() const
    {
        return m_renderingRect.size * getContentScaleFactor();
    }
    
    
    fzUInt Director::getResourcesFactor() const
    {
        return m_resourcesFactor;
    }
    
    
    fzFloat Director::getContentScaleFactor() const
    {
        return m_screenFactor * m_glConfig.quality;
    }
    
    
    Scene* Director::getRunningScene() const
    {
        return p_runningScene;
    }
    
    
    fzFloat Director::getAnimationInteval() const
    {
        return m_animationInterval;
    }
    

    fzFloat Director::getDelta() const
    {
        return m_dt;
    }
    
    
    fzFloat Director::getZEye() const
    {
        return m_screenSize.height / 1.1566f;
    }
    
    
    bool Director::isFullscreen() const
    {
        return (getWindowSize() * m_screenFactor == m_screenSize);
    }
    
    
    const fzColor4F& Director::getClearColor() const
    {
        return m_clearColor;
    }
    
    
    const GLConfig& Director::getGLConfig() const
    {
        return m_glConfig;
    }
    
    
    GLConfig& Director::getGLConfig()
    {
        return m_glConfig;
    }
    
    
    bool Director::isRunning() const
    {
        return m_isRunning;
    }
    
    
    bool Director::isPaused() const
    {
        return m_isPaused;
    }
    
    
    Texture2D* Director::getScreenshot(int format)
    {
#if 0
        fzSize viewPort = getViewPort();
        fzPixelInfo formatInfo = Texture2D::getPixelInfo((fzPixelFormat)format);
        fzTextureInfo textureInfo = Texture2D::getTextureInfo(formatInfo.textureFormat);
        
        fzUInt bufferSize = (formatInfo.bbp * viewPort.width * viewPort.height)/8;
        unsigned char *pixels = new unsigned char[bufferSize];
        
        glReadPixels(0, 0, viewPort.width, viewPort.height, textureInfo.openGLFormat, formatInfo.dataType, pixels);
        
        
        Texture2D *texture;
        try {
            texture = new Texture2D(pixels, (fzTextureFormat)format,
                                    fzMath_nextPOT(viewPort.width), fzMath_nextPOT(viewPort.height), viewPort);
            
        } catch (std::exception& error) {
            FZLOGERROR("%s", error.what());
            delete [] pixels;
            return NULL;
        }
        return texture;
#endif
        return NULL;
    }
    
    
#pragma mark - Updating
    
    void Director::updateViewRect()
    {
        if(!(m_dirtyFlags & kFZDDirty_viewPort))
            return;
        
        if(m_windowSize == FZSizeZero && m_originalCanvasSize == FZSizeZero)
            setFullscreen();
        else if(m_windowSize == FZSizeZero)
            setWindowSize(m_originalCanvasSize);
        
        fzSize windowSize = getWindowSize();
        fzSize canvasSize = (m_originalCanvasSize == FZSizeZero) ? windowSize : m_originalCanvasSize;
        fzOrientation orientation = getOrientation();
        
        if(orientation == kFZOrientation_LandscapeLeft ||
           orientation == kFZOrientation_LandscapeRight)
        {
            FZ_SWAP(canvasSize.width, canvasSize.height);
        }
                
        fzFloat windowRate = windowSize.width/windowSize.height;
        fzFloat canvasRate = canvasSize.width/canvasSize.height;
        fzSize newCanvasSize = canvasSize; // could be the same

        if(windowRate == canvasRate) {
            
            // No resizing because the canvas and window rate is the same.
            m_renderingRect = fzRect(FZPointZero, windowSize);
        
        }else{
            
            // The window and the canvas rate is different, so we have to apply
            // the proper resizing algorythm
            switch (m_resizeMode) {
                    
                case kFZResizeMode_None:
                {
                    m_renderingRect.size = canvasSize;
                    m_renderingRect.origin = (windowSize - canvasSize)/2;
                    
                    break;
                }
                case kFZResizeMode_Expand:
                {
                    m_renderingRect = fzRect(FZPointZero, windowSize);
                    
                    break;
                }
                case kFZResizeMode_Fit:
                {
                    if(canvasRate > windowRate)
                    {
                        m_renderingRect.size.width = windowSize.width;
                        m_renderingRect.size.height = canvasSize.height * windowSize.width/canvasSize.width;
                        m_renderingRect.origin = fzPoint(0, (windowSize.height-m_renderingRect.size.height)/2);
                    }else{
                        m_renderingRect.size.height = windowSize.height;
                        m_renderingRect.size.width = canvasSize.width * windowSize.height/canvasSize.height;
                        m_renderingRect.origin = fzPoint((windowSize.width-m_renderingRect.size.width)/2, 0);
                    }
                    break;
                }
                case kFZResizeMode_FitFill:
                {
                    if(canvasRate > windowRate)
                        newCanvasSize.height = canvasSize.width / windowRate;
                    else
                        newCanvasSize.width = canvasSize.height * windowRate;
                                        
                    m_renderingRect = fzRect(FZPointZero, windowSize);
                    break;
                }
                case kFZResizeMode_IntFit:
                {                    
                    fzFloat factorX = windowSize.width / canvasSize.width;
                    fzFloat factorY = windowSize.height / canvasSize.height;
                    fzFloat factor = fzMin(factorX, factorY);
                    factor = (factor >= 1.0f) ? static_cast<fzInt>(factor) : factor;
                    
                    m_renderingRect.size = canvasSize * factor;
                    m_renderingRect.origin = (windowSize - m_renderingRect.size)/2;
                    
                    break;
                }
                case kFZResizeMode_IntFitFill:
                {
                    fzFloat factorX = windowSize.width / canvasSize.width;
                    fzFloat factorY = windowSize.height / canvasSize.height;
                    fzFloat factor = fzMax(factorX, factorY);
                    factor = (factor >= 1.0f) ? static_cast<fzInt>(factor) : factor;

                    newCanvasSize = windowSize / factor;
                    
                    m_renderingRect = fzRect(FZPointZero, windowSize);
                    
                    break;
                }
                    
                default:
                    break;
            }
        }
        m_canvasSize = newCanvasSize;

        if(orientation == kFZOrientation_LandscapeLeft ||
           orientation == kFZOrientation_LandscapeRight)
        {
            FZ_SWAP(m_canvasSize.width, m_canvasSize.height);
        }
        
        // FACTORS
        fzSize viewPort = getViewPort();
        fzFloat factorX = viewPort.width / newCanvasSize.width;
        fzFloat factorY = viewPort.height / newCanvasSize.height;
        
        
        // COMPUTE FINAL FACTOR
        fzFloat newFactor = roundf(fzMax(fzMax(factorX, factorY), 1.0f));
        if(newFactor > DeviceConfig::Instance().getMaxFactor())
            newFactor = DeviceConfig::Instance().getMaxFactor();
        
        if(newFactor > m_resourcesFactor)
        {
            TextureCache::Instance().removeAllTextures();
            FontCache::Instance().removeAllFonts();
        }
        m_resourcesFactor = newFactor;
        
        m_dirtyFlags &= ~kFZDDirty_viewPort;
        m_dirtyFlags |= kFZDDirty_projection;
        
        
        // Notify changes to the OS Wrapper
        OSW::setOrientation(orientation);
        OSW::updateWindow();
    }
    
    
    void Director::updateProjection()
    {
        FZ_ASSERT(OSW::Instance(), "OS Wrapper is not defined.");

        updateViewRect();

        if(!(m_dirtyFlags & kFZDDirty_projection))
            return;
        
        // VIEW PORT
        // The view port must be the display size in pixels.
        // Display size is not equal to the screen size, an application could not use the whole screen.
        fzSize viewPort = getViewPort();
        glViewport(0, 0, (GLsizei)viewPort.width, (GLsizei)viewPort.height);
        
        
        // PROJECTION
        // The projection must be calculated using the canvas size. No pixels here.
        fzSize canvasSize = getCanvasSize();
        fzOrientation orientation = getOrientation();
        if(orientation == kFZOrientation_LandscapeLeft || orientation == kFZOrientation_LandscapeRight)
            FZ_SWAP(canvasSize.width, canvasSize.height);
        
        
        switch (m_projection) {
            case kFZProjection2D:
            {
                fzMath_mat4OrthoProjection(0, canvasSize.width, 0, canvasSize.height, -1024, 1024, m_transformMV);
                break;
            }
            default:
                FZ_RAISE("Director: Unrecognized projection.");
        }
        
        m_orientationTransform = FZAffineTransformIdentity;
        if(orientation == kFZOrientation_LandscapeLeft) {

            m_orientationTransform.translate(canvasSize.width, 0);
            m_orientationTransform.rotate(FZ_DEGREES_TO_RADIANS(90));
            
            fzMat4 mat;
            fzMath_mat4Multiply(m_transformMV, m_orientationTransform, mat);
            fzMath_mat4Copy(mat, m_transformMV);
            
        }else if(orientation == kFZOrientation_LandscapeRight) {
            m_orientationTransform.translate(0, canvasSize.height);
            m_orientationTransform.rotate(FZ_DEGREES_TO_RADIANS(-90));
            
            fzMat4 mat;
            fzMath_mat4Multiply(m_transformMV, m_orientationTransform, mat);
            fzMath_mat4Copy(mat, m_transformMV);
        }
        m_orientationTransform = m_orientationTransform.getInverse();
        
        m_dirtyFlags &= ~kFZDDirty_projection;

        if(p_runningScene) {
            p_runningScene->updateLayout();
            if(p_hud)
                p_hud->updateLayout();
        }
    }
    
    
#pragma mark - Scene management
    
    void Director::runWithScene(Scene *scene)
    {
        FZ_ASSERT( scene != NULL, "Scene must be non-NULL.");
        FZ_ASSERT( p_runningScene == NULL, "You can't run an scene if another Scene is running. Use replaceScene or pushScene instead.");

        pushScene(scene);
    }
    
    
    void Director::pushScene(Scene *scene)
    {
        FZ_ASSERT( scene != NULL, "Argument must be non-NULL.");
        
        m_sendCleanupToScene = false;
        
        m_scenesStack.push_back(scene);
        scene->retain();
        p_nextScene = scene;
    }
    
    
    void Director::replaceScene(Scene *scene)
    {
        FZ_ASSERT( scene != NULL, "Scene must be non-NULL.");
        
        m_sendCleanupToScene = true;

        m_scenesStack.back()->release();
        m_scenesStack.pop_back();
        
        m_scenesStack.push_back(scene);
        scene->retain();
        
        p_nextScene = scene;
    }
    
    
    void Director::popScene()
    {
        FZ_ASSERT( p_runningScene != NULL, "A running Scene is needed.");
        
        if( !m_scenesStack.empty() ) {
            m_scenesStack.back()->release();
            m_scenesStack.pop_back();
            
            m_sendCleanupToScene = true;
            p_nextScene = m_scenesStack.back();
        }
    }
    
    
    void Director::setNextScene()
    {
        bool runningIsTransition = (p_runningScene != NULL) ? dynamic_cast<Transition*>(p_runningScene) : false;
        bool newIsTransition = (p_nextScene != NULL) ? dynamic_cast<Transition*>(p_nextScene) : false;

        // If it is not a transition, call onExit/cleanup
        if(p_runningScene) {
            if( ! newIsTransition  ) {
                p_runningScene->onExit();
                
                if(m_sendCleanupToScene)
                    p_runningScene->cleanup();
                
            }
            p_runningScene->release();
        }
        p_runningScene = p_nextScene;
        p_nextScene = NULL;

        if(p_runningScene) {
            p_runningScene->retain();
            
            if( ! runningIsTransition ) {
                p_runningScene->onEnter();
                
                //p_runningScene->onEnterTransitionDidFinish();
            }
        }
#if FZ_RENDER_ON_DEMAND
        m_sceneIsDirty = true;
#endif
    }
    
    
#pragma mark - Drawing loop
    
    void Director::calculateDeltaTime()
    {
        struct timeval now;
        
        if( gettimeofday( &now, NULL) != 0 ) {
            m_dt = 0;
            return;
        }
        
        // new delta time
        if( m_nextDeltaTimeZero ) {
            m_dt = 0;
            m_nextDeltaTimeZero = false;
        } else {
            fzFloat newDt = (now.tv_sec - m_lastUpdate.tv_sec) + (now.tv_usec - m_lastUpdate.tv_usec) / 1000000.0f;
            newDt = fzMax(0.0f, newDt);
#if 0
            if(newDt > (getAnimationInteval()*3.0f))
                newDt = getAnimationInteval();
#endif
#if 0
            m_dt = m_dt * 0.5f + newDt * 0.5f;
#else
            m_dt = newDt;
#endif
        }
    
        m_lastUpdate = now;	
    }
    

    bool Director::drawScene()
    {
        // CALCULATE DELTA TIME
        calculateDeltaTime();

        // DISPATCH EVENTS
        EventManager::Instance().dispatchEvents();
        
        // SCHEDULE
        if(!m_isPaused) {
            Scheduler::Instance().tick( m_dt );
        }
        
        if(m_dirtyFlags)
            updateProjection();

        if(p_nextScene)
            setNextScene();
        

        // RENDERING
#if FZ_RENDER_ON_DEMAND
        bool newContent = m_sceneIsDirty;
        if(m_sceneIsDirty) {
#endif
            
#if !FZ_GL_SHADERS
            glLoadIdentity();
#endif
            MS::loadBaseMatrix(m_transformMV);
            
            // CLEAR OPENGL BUFFERS
            fzGLClearColor(m_clearColor);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            if(p_runningScene) {
                p_runningScene->internalVisit();
                CHECK_GL_ERROR_DEBUG();
            }
            
            FZ_ASSERT(MS::getLevel() == 1, "A matrix hasn't been popped. Review the push/pop code.");
        
            // SHOW FPS
            if( m_displayFPS )
                showFPS();
            
#if FZ_RENDER_ON_DEMAND
            m_sceneIsDirty = false;
        }
#endif
        PerformManager::Instance().clean();
        
#if FZ_RENDER_ON_DEMAND
        return newContent;
#else
        return true;
#endif
    }
    
    
    void Director::showFPS()
    {
        ++m_frames;
        m_accumDt += m_dt;
        
        if ( m_accumDt > FZ_DIRECTOR_FPS_INTERVAL)  {
            m_frameRate = m_frames / m_accumDt;
            m_frames = 0;
            m_accumDt = 0;
            
            p_hud->setFrame(m_frameRate);
        }
        p_hud->visit();
    }
    
    
    void Director::pause()
    {
        if( m_isPaused )
            return;
        
        m_cachedAnimationInterval = m_animationInterval;
        
        // when paused, reduce framerate drastically.
        setAnimationInterval(1/4.0f);
        m_isPaused = true;
    }
    
    
    void Director::resume()
    {
        if( !m_isPaused )
            return;
        
        setAnimationInterval(m_cachedAnimationInterval);
        
        m_nextDeltaTimeZero = true;        
        m_isPaused = false;
        m_dt = 0;
    }
    
    
#pragma mark - Threading management
    
    void Director::startAnimation()
    {        
        if(!isRunning()) {
            m_isRunning = true;
            m_nextDeltaTimeZero = true;
            OSW::startRendering(m_animationInterval);
        }        
    }
    
    
    void Director::stopAnimation()
    {        
        if(isRunning()) {
            m_isRunning = false;
            OSW::stopRendering();
        }
    }
    
    
#pragma mark - Spatial tranformations
    
    fzPoint Director::convertToGL(fzPoint point, bool isFlippedY) const
    {
        fzSize canvasSize = getCanvasSize();
        if(getOrientation() == kFZOrientation_LandscapeLeft)
            FZ_SWAP(canvasSize.width, canvasSize.height);
        
        fzAffineTransform transform;

        if(isFlippedY) {
            transform.scale(1, -1);
            transform.translate(0, -canvasSize.height);
        }
        transform.scale(canvasSize.width / m_renderingRect.size.width, canvasSize.height / m_renderingRect.size.height);
        transform.concat(m_orientationTransform);
        
        return point.applyTransform(transform);
    }
    
    
    fzPoint Director::normalizedCoord(fzPoint point) const
    {
        return point.applyTransform(fzAffineTransform(m_transformMV));
    }
    
    
    fzPoint Director::unnormalizedCoord(fzPoint point) const
    {
        return point.applyTransform(fzAffineTransform(m_transformMV).getInverse());
    }
    
    
    fzRect Director::unnormalizedRect(fzRect rect) const
    {
        return rect.applyTransform(fzAffineTransform(m_transformMV).getInverse());
    }
    
    
#pragma mark - Operative system events
    
    void Director::applicationLaunching(void *OSWrapper)
    {
        FZ_ASSERT(p_appdelegate, "FORZE was not initialized properly. App delegate is missing.");
        FZ_ASSERT(OSW::p_oswrapper == NULL, "OS Wrapper already initialized.");
        FZ_ASSERT(OSWrapper, "OS Wrapper can not be NULL");
        
        FZLOGINFO("Director: Application launching.");
        OSW::setInstance(OSWrapper);
        
        // GET OPENGL CONFIG
        m_glConfig = p_appdelegate->fzGLConfig();
        
        // GLConfig::validate() will throw an exception if the configuration is invalid.
        m_glConfig.validate();
    }
    
    
    void Director::applicationLaunched(void *options)
    {
        FZ_ASSERT(m_isInitialized == false, "FORZE was already launched.")
        FZ_ASSERT(p_appdelegate, "FORZE was not initialized properly. App delegate is missing.");
        FZ_ASSERT(OSW::Instance(), "OS wrapper should call Director::applicationLaunching() before.");

        if(!m_isInitialized) {
            
            m_isInitialized = true;

            // initialize singletons
            DataStore::Instance();
            ResourcesManager::Instance();
            DeviceConfig::Instance().logDeviceInfo();
            
            // set default GL values
            setDefaultGLValues();
            
            FZLOGINFO("Director: Application launched.");
            p_appdelegate->applicationLaunched(options);
            
            if(m_scenesStack.size() == 0)
            {
                FZLOGERROR("Director: A running scene is expected before applicationLaunched() is called. \n"
                           " - You should call Director::Instance().runWithScene( <YOUR FORZE::Scene> ).\n");
            }
            startAnimation();
        }
    }
    
//    
//    bool Director::applicationShouldRotate(fzOrientation toOrientation)
//    {
//        fzOrientation orientation = getOrientation();
//        if(orientation & kFZOrientation_Landscape && toOrientation & kFZOrientation_Landscape)
//            return true;
//        
//        if(orientation & kFZOrientation_Portrait && toOrientation & kFZOrientation_Portrait)
//            return true;
//    }
//    
//    
//    void Director::applicationRotated(fzOrientation toOrientation)
//    {
//        setOrientation(toOrientation);
//    }
//    
    void Director::applicationPaused()
    {
        FZ_ASSERT(p_appdelegate, "FORZE was not initialized properly. App delegate is missing.");

        pause();
        p_appdelegate->applicationPaused();
        EventManager::Instance().cancelAllEvents();
        DataStore::Instance().save();

        FZLOGINFO("Director: Application paused.");
    }
    
    
    void Director::applicationResumed()
    {
        FZ_ASSERT(p_appdelegate, "FORZE was not initialized properly. App delegate is missing.");
        FZLOGINFO("Director: Application resumed.");

        resume();
        p_appdelegate->applicationResumed();
    }
    
    
    void Director::applicationTerminate()
    {
        FZ_ASSERT(p_appdelegate, "FORZE was not initialized properly. App delegate is missing.");

        pause();
        p_appdelegate->applicationTerminate();
        DataStore::Instance().save();
        
        FZLOGINFO("Director: Application terminared.");
    }
    
    
    void Director::applicationDidReceiveMemoryWarning()
    {
        FZ_ASSERT(p_appdelegate, "FORZE was not initialized properly. App delegate is missing.");

        TextureCache::Instance().removeUnusedTextures();
        FontCache::Instance().removeUnusedFonts();
        FZLOGINFO("Director: Memory warning.");
    }
    
    
    void Director::applicationSignificantTimeChange()
    {
        m_nextDeltaTimeZero = true;
    }
}
