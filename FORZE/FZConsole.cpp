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
#include "FZMacros.h"
#include "FZConsole.h"
#include "FZEventManager.h"
#include "FZDirector.h"
#include "FZDeviceConfig.h"
#include "FZTextureCache.h"
#include "FZSpriteFrameCache.h"
#include "FZFontCache.h"
#include "FZSpriteFrame.h"
#include "FZTexture2D.h"
#include "FZFont.h"
#include "FZScheduler.h"
#include "FZNode.h"
#include "FZResourcesManager.h"
#include "FZPerformManager.h"
#include "external/tinythread/tinythread.h"


#define CAPACITY 10
#define MAXLENGTH 1024



namespace FORZE {
        
#pragma mark - FUNCTIONS
    
    static bool __cmd_help(const char*,float*, int) {
        
        FZLog("Console:\n"
              " - clear[]\n"
              " - exit[]: exits the console.\n\n"
              
              " - pfps[]: Prints the framerate.\n"
              " - pinfo[]: Prints the system info.\n"
              " - pevents[]: Prints the active events.\n"
              " - pnodes[]: Prints the rendering tree.\n"
              " - pfonts[]: Prints the cached fonts.\n"
              " - ptextures[]: Prints the cached textures.\n\n"
              
              " - sfps[ framerate ]: Sets the framerate.\n"
              " - scanvassize[ width, height ]: Sets the canvas size.\n"
              " - swindowsize[ width, height ]: Sets the window size.\n"
              " - sresizemode[ mode ]: Sets the resize mode.\n"
              " - stimescale[ scale ]: Sets the time scale.\n"
              " - shud[ bool ]: Enables or disables the HUD.\n\n"
              
              " - event[identifier, type, state, x, y, z]: Creates a event.\n"
              " - resume[]: Resumes the director.\n"
              " - pause[]: Pauses the director.\n"
              " - startanimation[]: Starts the rendering thread.\n"
              " - stopanimation[]: Stops the rendering thread.\n"
              " - releasetextures[]: Purges the cache.\n"
              " - releasefonts[]: Purges the cache.\n"
              " - loadtexture[ filename ]: Caches if posible the specified texture filename.\n"
              " - loadfont[ filename ]: Caches if posible the specified font filename.\n"
              " - checkfile[ filename ]: Performs a complete check of the file availability.\n"
              " - pop[]: pops the current scene.\n");

        return true;
    }
    
    static bool __cmd_clear(const char*,float*, int) {
        for(int i = 0; i < 200; ++i)
            printf("\n");
        
        return true;
    }
    static bool __cmd_exit(const char*,float*, int) {
        return false;
    }
    
    static bool __cmd_pfps(const char*,float*, int) {
        FZLog("Console: %f\n", 1/Director::Instance().getDelta());
        return true;
    }
    static bool __cmd_pinfo(const char*,float*, int) {
        PerformManager::Instance().perform(&DeviceConfig::Instance(),
                                           SEL_VOID(DeviceConfig::logDeviceInfo), false);
        return true;
    }
    static bool __cmd_pevents(const char*,float*, int) {
        
        const list<Event>& events = EventManager::Instance().getEvent();
        list<Event>::const_iterator event(events.begin());
        for(; event != events.end(); ++event)
            event->log();
        
        return true;
    }
    
    static bool __cmd_ptextures(const char* text,float* values, int nuValues) {
        
        if(text == NULL) {
            const map<int32_t, Texture2D*>& textures = TextureCache::Instance().getTextures();
            map<int32_t, Texture2D*>::const_iterator it(textures.begin());
            for(; it != textures.end(); ++it) {
                if(nuValues == 0 || (nuValues > 0 && values[0] == it->second->getName()))
                    it->second->log();
            }
        }else{
            
            Texture2D *t = TextureCache::Instance().getTextureByName(text);
            if(t) t->log();
        }
        return true;
    }
    
    static bool __cmd_pfonts(const char* text,float* values, int nuValues) {
        
        if(text == NULL) {
            
            const map<int32_t, Font*>& fonts = FontCache::Instance().getFonts();
            map<int32_t, Font*>::const_iterator it(fonts.begin());
            for(; it != fonts.end(); ++it)
                it->second->log();
        }else{
            
            Font *f = FontCache::Instance().getFontByName(text);
            if(f) f->log();
        }
        
        return true;
    }
    
    static int __iterate(Node *node, int level) {
        
        Node *n;
        int count = 1;
        for(int i = 0; i < level; ++i)
            printf("\t");
        
        printf("-level %d, %p\n", level, node);
        FZ_LIST_FOREACH(node->getChildren(), n) {
            count += __iterate(n, level+1);
        }
        return count;
    }
    
    static bool __cmd_pnodes(const char*,float* values, int nuValues) {
        FZLog("Nodes:");
        Node *node = (Node*)Director::Instance().getRunningScene();
        int count = __iterate(node, 0);
        printf("%d nodes in total.\n\n", count);
        return true;
    }
    
    static bool __cmd_pframes(const char* text,float* values, int nuValues) {
        
        if(text == NULL) {
            const map<int32_t, fzSpriteFrame>& fonts = SpriteFrameCache::Instance().getFrames();
            map<int32_t, fzSpriteFrame>::const_iterator it(fonts.begin());
            for(; it != fonts.end(); ++it)
                it->second.log();
        }else{
            fzSpriteFrame f = SpriteFrameCache::Instance().getSpriteFrameByKey(text);
            if(f.isValid())
                f.log();
        }
        
        return true;
    }

    static bool __cmd_sfps(const char*,float* v, int) {
        Director::Instance().setAnimationInterval(1.0f/v[0]);
        return true;
    }
    static bool __cmd_shud(const char*,float* v, int) {
        Director::Instance().setDisplayFPS(v[0]!=0.0f);
        return true;
    }
    static bool __cmd_stimescale(const char*,float* v, int) {
        Scheduler::Instance().setTimeScale(v[0]);
        return true;
    }
    static bool __cmd_scanvassize(const char*,float* v, int) {
        Director::Instance().setCanvasSize(fzSize(v[0], v[1]));
        return true;
    }
    static bool __cmd_swindowsize(const char*,float* v, int) {
        Director::Instance().setWindowSize(fzSize(v[0], v[1]));
        return true;
    }
    static bool __cmd_sresizemode(const char*,float* v, int) {
        Director::Instance().setResizeMode((fzResizeMode)v[0]);
        return true;
    }
    static bool __cmd_event(const char*,float *values, int nuValues) {
        
        Event event(OSW::Instance(), (int)values[0],
                    (fzEventType)values[1],
                    (fzEventState)values[2],
                    values[3], values[4], values[5]);
        
        EventManager::Instance().catchEvent(event);
        return true;
    }
    static bool __cmd_resume(const char*,float*, int) {
        Director::Instance().resume();
        return true;
    }
    static bool __cmd_pause(const char*,float*, int) {
        Director::Instance().pause();
        return true;
    }
    static bool __cmd_startanimation(const char*,float*, int) {
        Director::Instance().startAnimation();
        return true;
    }
    static bool __cmd_stopanimation(const char*,float*, int) {
        Director::Instance().stopAnimation();
        return true;
    }
    static bool __cmd_pop(const char*,float*, int) {
        Director::Instance().popScene();
        return true;
    }
    static bool __cmd_releasetextures(const char*,float*, int) {
        PerformManager::Instance().perform(&TextureCache::Instance(),
                                           SEL_VOID(TextureCache::removeAllTextures), false);
        return true;
    }
    static bool __cmd_releasefonts(const char*,float*, int) {
        PerformManager::Instance().perform(&FontCache::Instance(),
                                           SEL_VOID(FontCache::removeAllFonts), false);
        return true;
    }
    static bool __cmd_loadtexture(const char* text,float*, int) {
        
        PerformManager::Instance().perform(&TextureCache::Instance(),
                                           SEL_PTR(TextureCache::addImage),
                                           (void*)text, false);
        
        return true;
    }
    static bool __cmd_loadfont(const char* text,float*, int) {
        
        PerformManager::Instance().perform(&FontCache::Instance(),
                                           SEL_PTR(FontCache::addFont),
                                           (void*)text, false);
        
        return true;
    }
    static bool __cmd_checkfile(const char* text, float*, int) {
        
        ResourcesManager::Instance().checkFile(text);
        return true;
    }


    
#pragma mark -
    
    const static struct {
        int32_t hash;
        bool (*func)(const char*,float*,int);
        int values;
    } funcList[] =
    {
        // GENERAL COMMANDS
        {fzHashConst("help"), __cmd_help, 0},
        {fzHashConst("exit"), __cmd_exit, 0},
        {fzHashConst("clear"), __cmd_clear, 0},
        
        // PRINT COMMANDS
        {fzHashConst("pfps"), __cmd_pfps, 0},
        {fzHashConst("pinfo"), __cmd_pinfo, 0},
        {fzHashConst("pevents"), __cmd_pevents, 0},
        {fzHashConst("ptextures"), __cmd_ptextures, 0},
        {fzHashConst("pfonts"), __cmd_pfonts, 0},
        {fzHashConst("pnodes"), __cmd_pnodes, 0},
        {fzHashConst("pframes"), __cmd_pframes, 0},


        // SET COMMANDS
        {fzHashConst("sfps"), __cmd_sfps, 1},
        {fzHashConst("scanvassize"), __cmd_scanvassize, 2},
        {fzHashConst("swindowsize"), __cmd_swindowsize, 2},
        {fzHashConst("sresizemode"), __cmd_sresizemode, 1},
        {fzHashConst("stimescale"), __cmd_stimescale, 1},
        {fzHashConst("shud"), __cmd_shud, 1},

        // MISCELANEOUS
        {fzHashConst("event"), __cmd_event, 3},
        {fzHashConst("resume"), __cmd_resume, 0},
        {fzHashConst("pause"), __cmd_pause, 0},
        {fzHashConst("startanimation"), __cmd_startanimation, 0},
        {fzHashConst("stopanimation"), __cmd_stopanimation, 0},
        {fzHashConst("pop"), __cmd_pop, 0},
        {fzHashConst("releasetextures"), __cmd_releasetextures, 0},
        {fzHashConst("releasefonts"), __cmd_releasefonts, 0},
        {fzHashConst("loadtexture"), __cmd_loadtexture, 0},
        {fzHashConst("loadfont"), __cmd_loadfont, 0},
        {fzHashConst("checkfile"), __cmd_checkfile, 0},
    };
    
    
    static void consoleLoop(void* t)
    {
        Console *c = (Console*)t;
        while(Console::peekMessage(c->p_stream));
        
        c->p_thread->detach();
        delete c->p_thread;
        c->p_thread = NULL;
    }
    
    
    Console::Console()
    : Console(stdin)
    { }
    
    Console::Console(FILE *stream)
    {
        p_stream = stream;
        p_thread = new thread(consoleLoop, this);
    }
    
    Console::~Console()
    {
        delete p_thread;
    }
    
    void Console::sendMessage(const char *message)
    {
        fprintf(p_stream, "\n%s\n", message);
    }
    
    void Console::join()
    {
        p_thread->join();
    }
    
    bool Console::peekMessage(FILE *stream)
    {
        static char line[MAXLENGTH];
        static char command[MAXLENGTH];
        char *arguments;
        
        int nuValues = 0;
        float values[CAPACITY];
        
        // GET LINE
        fgets(line, MAXLENGTH, stream);
        
        // GET ARGUMMENTS
        arguments = strchr(line, '[');
        if(arguments != NULL) {
            *arguments = '\0';
            ++arguments;
            nuValues = sscanf(arguments, "%f , %f , %f , %f , %f , %f , %f , %f , %f , %f",
                              &values[0],&values[1],&values[2],&values[3],&values[4],
                              &values[5],&values[6],&values[7],&values[8],&values[9]);
            
            char *end = strrchr(arguments, ']');
            if(end) *end = '\0';
            
            if(*arguments == '\0')
                arguments = NULL;
        }
        
        // NORMALIZE COMMAND's TEXT
        memset(command, 0, MAXLENGTH);
        sscanf(line, "%[^ \n]s", command);
        if(strlen(command) == 0)
            return true;
        
        // GET HASH
        int32_t hash = fzHashConst(command);
        
        // FIND COMMAND
        for(unsigned int i = 0; i < (sizeof(funcList)/sizeof(funcList[0])); ++i){
            if(hash == funcList[i].hash) {
                if(nuValues < funcList[i].values) {
                    FZLog("Console: Missing params: \"%s\" needs %d args.", command, funcList[i].values);
                    return true;
                }
                return funcList[i].func(arguments, values, nuValues);
            }
        }
        
        FZLog("Console: Unknown script. %s", line);
        return true;
    }
}
