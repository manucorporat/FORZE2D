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
#include "FZDataStore.h"
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
    
    static bool __cmd_help(const char*,float*, int)
    {
        FZLog("Console:\n"
              " - clear[]\n"
              " - exit[]: exits the console.\n\n"
              
              " - pfps[]: Prints the framerate.\n"
              " - pinfo[]: Prints the system info.\n"
              " - pevents[]: Prints the active events.\n"
              " - pnodes[]: Prints the rendering tree.\n"
              " - pfonts[]: Prints the cached fonts.\n"
              " - ptextures[]: Prints the cached textures.\n"
              " - pcanvassize[]: Prints the canvas' size.\n"
              " - pwindowsize[]: Prints the window's size.\n"
              " - pviewport[]: Prints the view port.\n\n"
              
              " - sfps[ framerate ]: Sets the framerate.\n"
              " - scanvassize[ width, height ]: Sets the canvas size.\n"
              " - swindowsize[ width, height ]: Sets the window size.\n"
              " - sresizemode[ mode ]: Sets the resize mode.\n"
              " - stimescale[ scale ]: Sets the time scale.\n"
              " - shud[ bool ]: Enables or disables the HUD.\n\n"
              
              " - event[identifier, type, state, x, y, z]: Creates a event.\n"
              " - save[]: DataStore saves data in permanent memory.\n"
              " - resume[]: Resumes the director.\n"
              " - pause[]: Pauses the director.\n"
              " - startanimation[]: Starts the rendering thread.\n"
              " - stopanimation[]: Stops the rendering thread.\n"
              " - releasetextures[]: Purges the texture cache.\n"
              " - releasefonts[]: Purges the font cache.\n"
              " - loadtexture[ filename ]: Caches if posible the specified texture.\n"
              " - loadfont[ filename ]: Caches if posible the specified font.\n"
              " - checkfile[ filename ]: Performs a complete check of the file availability.\n"
              " - pop[]: Pops the current scene.\n");

        return true;
    }
    static bool __cmd_clear(const char*,float*, int)
    {
        for(int i = 0; i < 200; ++i)
            printf("\n");
        
        return true;
    }
    static bool __cmd_exit(const char*,float*, int)
    {
        return false;
    }
    static bool __cmd_pfps(const char*,float*, int)
    {
        FZLog("Console: %f\n", 1/Director::Instance().getDelta());
        return true;
    }
    static bool __cmd_pcanvassize(const char*,float*, int)
    {
        fzSize size = Director::Instance().getCanvasSize();
        FZLog("Canvas size: {%f, %f}\n", size.width, size.height);
        return true;
    }
    static bool __cmd_pwindowsize(const char*,float*, int)
    {
        fzSize size = Director::Instance().getWindowSize();
        FZLog("Window size: {%f, %f}\n", size.width, size.height);
        return true;
    }
    static bool __cmd_pviewport(const char*,float*, int)
    {
        fzSize size = Director::Instance().getViewPort();
        FZLog("View port: {%f, %f}\n", size.width, size.height);
        return true;
    }
    static bool __cmd_pinfo(const char*,float*, int)
    {
        PerformManager::Instance().perform(&DeviceConfig::Instance(),
                                           SEL_VOID(DeviceConfig::logDeviceInfo), false);
        return true;
    }
    static bool __cmd_pevents(const char*,float*, int)
    {
        const list<Event>& events = EventManager::Instance().getEvent();
        list<Event>::const_iterator event(events.begin());
        for(; event != events.end(); ++event)
            event->log();
        
        return true;
    }
    static bool __cmd_ptextures(const char* text,float* values, int nuValues)
    {
        if(text == NULL) {
            const auto textures = TextureCache::Instance().getTextures();
            decltype(textures)::const_iterator it(textures.begin());
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
    static bool __cmd_pfonts(const char* text,float* values, int nuValues)
    {
        if(text == NULL) {
            
            const auto fonts = FontCache::Instance().getFonts();
            decltype(fonts)::const_iterator it(fonts.begin());
            for(; it != fonts.end(); ++it)
                it->second->log();
        }else{
            
            Font *f = FontCache::Instance().getFontByName(text);
            if(f) f->log();
        }
        
        return true;
    }
    static int __iterate(Node *node, int level)
    {
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
    static bool __cmd_pnodes(const char*,float* values, int nuValues)
    {
        FZLog("Nodes:");
        Node *node = (Node*)Director::Instance().getRunningScene();
        int count = __iterate(node, 0);
        printf("%d nodes in total.\n\n", count);
        return true;
    }
    static bool __cmd_pframes(const char* text,float* values, int nuValues)
    {
        if(text == NULL) {
            const auto fonts = SpriteFrameCache::Instance().getFrames();
            decltype(fonts)::const_iterator it(fonts.begin());
            for(; it != fonts.end(); ++it)
                it->second.log();
        }else{
            fzSpriteFrame f = SpriteFrameCache::Instance().getSpriteFrameByKey(text);
            if(f.isValid())
                f.log();
        }
        
        return true;
    }
    static bool __cmd_sfps(const char*,float* v, int)
    {
        Director::Instance().setAnimationInterval(1.0f/v[0]);
        return true;
    }
    static bool __cmd_shud(const char*,float* v, int)
    {
        Director::Instance().setDisplayFPS(v[0]!=0.0f);
        return true;
    }
    static bool __cmd_stimescale(const char*,float* v, int)
    {
        Scheduler::Instance().setTimeScale(v[0]);
        return true;
    }
    static bool __cmd_scanvassize(const char*,float* v, int)
    {
        Director::Instance().setCanvasSize(fzSize(v[0], v[1]));
        return true;
    }
    static bool __cmd_swindowsize(const char*,float* v, int)
    {
        Director::Instance().setWindowSize(fzSize(v[0], v[1]));
        return true;
    }
    static bool __cmd_sresizemode(const char*,float* v, int)
    {
        Director::Instance().setResizeMode((fzResizeMode)v[0]);
        return true;
    }
    static bool __cmd_event(const char*,float *values, int nuValues)
    {
        Event event(OSW::Instance(), (int)values[0],
                    (fzEventType)values[1],
                    (fzEventState)values[2],
                    values[3], values[4], values[5]);
        
        EventManager::Instance().catchEvent(event);
        return true;
    }
    static bool __cmd_save(const char*,float *, int)
    {
        DataStore::Instance().save();
        return true;
    }
    static bool __cmd_resume(const char*,float*, int)
    {
        Director::Instance().resume();
        return true;
    }
    static bool __cmd_pause(const char*,float*, int)
    {
        Director::Instance().pause();
        return true;
    }
    static bool __cmd_startanimation(const char*,float*, int)
    {
        Director::Instance().startAnimation();
        return true;
    }
    static bool __cmd_stopanimation(const char*,float*, int)
    {
        Director::Instance().stopAnimation();
        return true;
    }
    static bool __cmd_pop(const char*,float*, int)
    {
        Director::Instance().popScene();
        return true;
    }
    static bool __cmd_releasetextures(const char*,float*, int)
    {
        PerformManager::Instance().perform(&TextureCache::Instance(),
                                           SEL_VOID(TextureCache::removeAllTextures), false);
        return true;
    }
    static bool __cmd_releasefonts(const char*,float*, int)
    {
        PerformManager::Instance().perform(&FontCache::Instance(),
                                           SEL_VOID(FontCache::removeAllFonts), false);
        return true;
    }
    static bool __cmd_loadtexture(const char* text,float*, int)
    {
        PerformManager::Instance().perform(&TextureCache::Instance(),
                                           SEL_PTR(TextureCache::addImage),
                                           (void*)text, false);
        return true;
    }
    static bool __cmd_loadfont(const char* text,float*, int)
    {
        PerformManager::Instance().perform(&FontCache::Instance(),
                                           SEL_PTR(FontCache::addFont),
                                           (void*)text, false);
        return true;
    }
    static bool __cmd_checkfile(const char* text, float*, int)
    {
        ResourcesManager::Instance().checkFile(text);
        return true;
    }

    
#pragma mark -
    
    const static struct {
        uint32_t hash;
        bool (*func)(const char*, float*, int);
        int values;
    } funcList[] =
    {
        // GENERAL COMMANDS
        {"help"_hash, __cmd_help, 0},
        {"exit"_hash, __cmd_exit, 0},
        {"clear"_hash, __cmd_clear, 0},
        
        // PRINT COMMANDS
        {"pfps"_hash, __cmd_pfps, 0},
        {"pinfo"_hash, __cmd_pinfo, 0},
        {"pevents"_hash, __cmd_pevents, 0},
        {"ptextures"_hash, __cmd_ptextures, 0},
        {"pfonts"_hash, __cmd_pfonts, 0},
        {"pnodes"_hash, __cmd_pnodes, 0},
        {"pframes"_hash, __cmd_pframes, 0},
        {"pcanvassize"_hash, __cmd_pcanvassize, 0},
        {"pwindowsize"_hash, __cmd_pwindowsize, 0},
        {"pviewport"_hash, __cmd_pviewport, 0},


        // SET COMMANDS
        {"sfps"_hash, __cmd_sfps, 1},
        {"scanvassize"_hash, __cmd_scanvassize, 2},
        {"swindowsize"_hash, __cmd_swindowsize, 2},
        {"sresizemode"_hash, __cmd_sresizemode, 1},
        {"stimescale"_hash, __cmd_stimescale, 1},
        {"shud"_hash, __cmd_shud, 1},

        // MISCELANEOUS
        {"event"_hash, __cmd_event, 3},
        {"save"_hash, __cmd_save, 0},
        {"resume"_hash, __cmd_resume, 0},
        {"pause"_hash, __cmd_pause, 0},
        {"startanimation"_hash, __cmd_startanimation, 0},
        {"stopanimation"_hash, __cmd_stopanimation, 0},
        {"pop"_hash, __cmd_pop, 0},
        {"releasetextures"_hash, __cmd_releasetextures, 0},
        {"releasefonts"_hash, __cmd_releasefonts, 0},
        {"loadtexture"_hash, __cmd_loadtexture, 0},
        {"loadfont"_hash, __cmd_loadfont, 0},
        {"checkfile"_hash, __cmd_checkfile, 0},
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
        FZLog("Console: warning: Remember to disable the console before distributing.");
        
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
        float values[CAPACITY];
        char *arguments;
        int nuValues = 0;
        
        
        // CLEAN VALUES
        memset(command, 0, MAXLENGTH);
        //memset(values, 0, MAXLENGTH * sizeof(float));
        
        // GET LINE
        fgets(line, MAXLENGTH, stream);
        
        // GET ARGUMMENTS
        arguments = strchr(line, '[');
        if(arguments != NULL) {
            *arguments = '\0';
            ++arguments;
            nuValues = sscanf(arguments, "%f , %f , %f , %f , %f , %f , %f , %f , %f , %f",
                              &values[0], &values[1], &values[2], &values[3], &values[4],
                              &values[5], &values[6], &values[7], &values[8], &values[9]);
            
            char *end = strrchr(arguments, ']');
            if(end) *end = '\0';
            
            if(*arguments == '\0')
                arguments = NULL;
        }
        
        // NORMALIZE COMMAND's TEXT
        sscanf(line, "%[^ \n]s", command);
        if(command[0] == '\0')
            return true;
        
        // GET HASH
        uint32_t hash = fzHash(command);
        
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
