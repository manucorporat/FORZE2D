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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FZResourcesManager.h"
#include "FZDeviceConfig.h"
#include "FZDirector.h"
#include "FZIO.h"
#include "FZMacros.h"


namespace FORZE {
    
    ResourcesManager* ResourcesManager::p_instance = NULL;
    
    ResourcesManager& ResourcesManager::Instance()
    {
        if (p_instance == NULL)
            p_instance = new ResourcesManager();
        
        return *p_instance; 
    }
    
    
    ResourcesManager::ResourcesManager()
    : m_nuRules(0)
    {
        // GET RESOURCES PATH
        char tmp[512];
        if(!fzDevice_getResourcesPath(tmp, 512))
            FZ_RAISE_STOP("ResourcesManager: Buffer too small, impossible to get the resources path.");
        
        p_resourcesPath = fzStrcpy(tmp);
        
        
        setupDefaultRules();
    }
    
    
    void ResourcesManager::setupDefaultRules()
    {
        // mac OS X default rules
        addRule("iMac*", "mac", 1); // mac
        addRule("Mac*", "mac", 1); // mac

        // iOS default rules
        addRule("iPad1*", "ipad", 1); // iPad 1
        addRule("iPad2*", "ipad", 1); // iPad 2 (Wi-Fi)
        addRule("iPad3*", "ipadhd", 1); // iPad 3 (Wi-Fi)
        addRule("iPad3*", "ipad", 2); // iPad 3 (Wi-Fi)
    }
    
    
    void ResourcesManager::addRule(const char *deviceCode, const char *flag, fzUInt factor)
    {
        FZ_ASSERT(deviceCode, "Device code can not be NULL.");
        FZ_ASSERT(flag, "Flag can not be NULL.");
        FZ_ASSERT(factor >= 1, "Factor can not be less than 1.");
        
        
        const char *device = DeviceConfig::Instance().getDeviceCode();
        fzUInt deviceCodeLength = strlen(deviceCode);
        
        int cmp = 1;
        if(deviceCode[deviceCodeLength-1] == '*')
            cmp = strncmp(deviceCode, device, deviceCodeLength-1);
        else
            cmp = strcmp(deviceCode, device);
        
        
        if(cmp == 0) {
            
            if(m_nuRules >= FZRULE_NU) {
                FZ_ASSERT(false, "No enough memory to store new rule.");
                return;
            }

            fzUInt flagLength = strlen(flag);
            if(flagLength > FZRULE_MAXSIZE-1) {
                FZ_ASSERT(false, "Too long prefix. 7 characters.");
                flagLength = FZRULE_MAXSIZE-1;
            }
            memcpy(m_rules[m_nuRules].flag, flag, flagLength+1);
            m_rules[m_nuRules].factor = factor;
            
            FZLOGINFO("ResourcesManager: New rule:\"%c%s\" Factor:%d.", FZ_IO_SUBFIX_CHAR, m_rules[m_nuRules].flag, m_rules[m_nuRules].factor);
            ++m_nuRules;
        }
    }
    
    
    void ResourcesManager::_generateAbsolutePath(const char *filename, const char *suffix, char *absolutePath) const
    {
        FZ_ASSERT(filename != NULL, "Filename can not be NULL.");
        FZ_ASSERT(absolutePath != NULL, "AbsolutePath must be a valid pointer.");

        int u;
        if(suffix == NULL) 
            u = sprintf(absolutePath, "%s/%s", p_resourcesPath, filename);
        
        else {
            // GET EXTENSION
            char *extension = strchr(filename, '.');
            
            // GET NAME
            size_t nameLength = extension - filename;
            char *name = new char[nameLength+1];
            memcpy(name, filename, nameLength);
            name[nameLength] = '\0';

            // BUILD 
            u = sprintf(absolutePath, "%s/%s%c%s%s", p_resourcesPath, name, FZ_IO_SUBFIX_CHAR, suffix, extension);
            
            delete [] name;
        }
        FZ_ASSERT(u > 0, "Error generating absolute path. sprintf().");
    }
    
    
    void ResourcesManager::generateAbsolutePath(const char *filename, fzUInt factor, char *absolutePath) const
    {
        FZ_ASSERT(factor <= 99, "Factor is out of bounds [0, 99]");
        
        if(factor == 1 || factor == 0)
            _generateAbsolutePath(filename, nullptr, absolutePath);
        
        else {
            char suffix[4];
            sprintf(suffix, "x%d", factor);
            
            _generateAbsolutePath(filename, suffix, absolutePath);
        }
    }

    
    void ResourcesManager::getPath(const char *filename, fzUInt priority, char *absolutePath, fzUInt *factor) const
    {
        *factor = 0;
        if(priority < m_nuRules)
        {
            // RULES
            _generateAbsolutePath(filename, m_rules[priority].flag, absolutePath);
            *factor = m_rules[priority].factor;
            
        }else
        {
            // FACTOR
            fzUInt preferedFactor = Director::Instance().getResourcesFactor();
            fzInt tmpFactor = m_nuRules-priority + preferedFactor;
            if(tmpFactor > 0) {
                generateAbsolutePath(filename, tmpFactor, absolutePath);
                *factor = tmpFactor;
            }
        }
    }
    
    
    fzBuffer ResourcesManager::loadResource(const char *filename, fzUInt *outFactor) const
    {
        FZ_ASSERT(filename != NULL, "Filename can not be NULL.");
        FZ_ASSERT(outFactor != NULL, "outFactor can not be NULL.");

        // REMOVING FORCED FLAGS
        char *filenameCpy = fzStrcpy(filename);
        IO::removeFileSuffix(filenameCpy);
        
        // LOOK FOR TEXTURE
        *outFactor = 0;
        char absolutePath[512];
        fzUInt factor, priority = 0;
        
        while (true) {
            
            // Get texture path for priority
            getPath(filenameCpy, priority, absolutePath, &factor);
            
            if(factor == 0) {
                FZLOGERROR("IO: \"%s\" not found.", filename);
                delete filenameCpy;
                return fzBuffer::empty();
            }

            fzBuffer buffer = IO::loadFile(absolutePath);
            if(!buffer.isEmpty()) {
                *outFactor = factor;
                delete filenameCpy;
                return buffer;
            }
            ++priority;
        }
    }
    
    
    fzBuffer ResourcesManager::loadResource(const char *filename) const
    {
        FZ_ASSERT(filename, "Filename cannot be NULL");
        
        char absolutePath[512];
        generateAbsolutePath(filename, 1, absolutePath);
        
        fzBuffer buffer = IO::loadFile(absolutePath);
        if(buffer.isEmpty())
            FZLOGERROR("IO: \"%s\" not found.", filename);
        
        return buffer;
    }
    
    
    void ResourcesManager::checkFile(const char* filename) const
    {
        FZ_ASSERT(filename != NULL, "Filename can not be NULL.");
        
        // REMOVING FORCED FLAGS
        char *filenameCpy = fzStrcpy(filename);
        IO::removeFileSuffix(filenameCpy);
        
        // LOOK FOR TEXTURE
        char absolutePath[512];
        fzUInt factor, priority = 0;
        
        FZLog("ResourcesManager:");
        while (true) {
            
            // Get texture path for priority
            getPath(filenameCpy, priority, absolutePath, &factor);
            
            if(factor == 0)
                return;
            
            char *file = strrchr(absolutePath, '/')+1;
            if(IO::checkFile(absolutePath))
                printf(" - NOT FOUND: %s\n", file);
            else
                printf(" - FOUND: %s\n", file);

            ++priority;
        }
        
        delete filenameCpy;
    }
}
