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

#include "FZMacros.h"
#include "FZDataStore.h"
#include "FZIO.h"
#include "FZData.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include <cstring>
#include <cstdlib>


#define XML_FILENAME "forze2d_datastore.xml"
#define XML_SIZE_TAG "size"
#define XML_ENTRY_TAG "e"
#define XML_KEY_ATTRIBUTE "k"
#define XML_TYPE_ATTRIBUTE "t"

using namespace rapidxml;

namespace FORZE {
    
    DataStore* DataStore::p_instance = NULL;
    
    DataStore& DataStore::Instance()
    {
        if (p_instance == NULL)
            p_instance = new DataStore();
        
        return *p_instance; 
    }
    
    
    DataStore::DataStore()
    : p_store(NULL)
    , p_path(NULL)
    , m_num(0)
    , m_capacity(0)
    , m_dirty(false)
    {
        {
            char buffer[1024];
            fzDevice_getPersistentPath(XML_FILENAME, buffer, 1024);
            strcat(buffer, XML_FILENAME);
            p_path = fzStrcpy(buffer);
        }
        
        // READ DATA
        try {
            readFromMemory();

        } catch (std::exception& error) {
            fzDevice_removePath(p_path);
            
        }
    }
    
    
    DataStore::~DataStore()
    {
        delete p_path;
    }

    
    void DataStore::reserveCapacity(fzUInt capacity)
    {
        if(capacity <= m_capacity)
            return;

      if(p_store == NULL) {
            capacity = (capacity < 10) ? 10 : capacity;
            p_store = static_cast<fzStoreEntry*>( malloc(capacity * sizeof(fzStoreEntry)) );
        }else{
            capacity += 10;
            p_store = static_cast<fzStoreEntry*>( realloc(p_store, capacity * sizeof(fzStoreEntry)) );
        }
        
        m_capacity = capacity;
    }
    
    
#pragma mark - IO management    
    
    bool DataStore::save()
    {
        FZ_ASSERT(p_path, "Path cannot be NULL");

        if(!m_dirty)
            return true;
        
        xml_document<> doc;
        xml_node<>* node;
                
        // XML DECLARATION
        node = doc.allocate_node(node_declaration);
        node->append_attribute(doc.allocate_attribute("version", "1.0"));
        node->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
        doc.append_node(node);
        
        
        // SIZE TAG
        node = doc.allocate_node(node_element, XML_SIZE_TAG, doc.allocate_string(FZT("%d", m_num)));
        doc.append_node(node);

        fzUInt xmlSize = 60;

        // ENTRIES
        for(fzUInt i = 0; i < m_num; ++i) {
          
            char *content = NULL;
            fzStoreEntry& e = p_store[i];
                        
            switch (e.type) {
                case kFZData_string:
                case kFZData_data:
                {
                    fzBuffer enconded = fzData_B64Encode(e.data.getPointer(), e.data.getLength());
                    content = doc.allocate_string(enconded.getPointer());
                    enconded.free();
                    break;
                }
                case kFZData_float:
                    content = doc.allocate_string(FZT("%f", e.floatValue));
                    break;
                case kFZData_integer:
                    content = doc.allocate_string(FZT("%d", e.integerValue));
                    break;
                default:
                    FZLOGERROR("DataStore: Invalid data type.");
                    continue;
            }
            
            node = doc.allocate_node(node_element, XML_ENTRY_TAG, content);
            node->append_attribute(doc.allocate_attribute(XML_KEY_ATTRIBUTE, e.key));
            node->append_attribute(doc.allocate_attribute(XML_TYPE_ATTRIBUTE, doc.allocate_string(FZT("%d", e.type))));
            doc.append_node(node);
            
            xmlSize += 25 + strlen(content) + strlen(e.key);
        }
        
        
        // CREATE DIRECTORY (if needed)
        if(!fzDevice_createDirectory(p_path, false)) {
            FZLOGERROR("DataStore: Error creating directory.");
            return false;
        }
        
        
        // PRINTING
        char *buffer = new char[xmlSize];
        char *end = rapidxml::print(buffer, doc, 0);
        *end = '\0'; // NULL TERMINATION
        
        FZ_ASSERT(end < (buffer + xmlSize), "Memory overflow");
        
        bool success = fzIO_writeFile(buffer, p_path);
        delete [] buffer;
        
        m_dirty = !success;
        return success;
    }
    
    
    bool DataStore::readFromMemory()
    {
        FZ_ASSERT(p_path, "Path cannot be NULL");
        if(m_dirty)
            return true;
                
        fzBuffer buffer = fzIO_loadFile(p_path);
        
        if(!buffer.isEmpty()) {
            
            xml_document<> doc;
            doc.parse<parse_fastest | parse_validate_closing_tags>(buffer.getPointer());
            
            xml_node<> *node = doc.first_node();
            if(strncmp(node->name(), XML_SIZE_TAG, node->name_size()) != 0)
                FZ_RAISE("DataStore: XML is corrupted.");

            
            // RESERVE CAPACITY
            reserveCapacity(atoi(node->value())+1);
            
            
            // ITERATE XML
            m_num = 0;
            node = node->next_sibling();
            for(; node; node = node->next_sibling())
            {
                if(strncmp(node->name(), XML_ENTRY_TAG, node->name_size()) != 0) {
                    FZLOGERROR("DataStore: XML entry is corrupted.");
                    continue;
                }
                
                fzStoreEntry &entry = p_store[m_num];
                
                // KEY
                xml_attribute<> *attribute = node->first_attribute(XML_KEY_ATTRIBUTE);
                if(attribute == NULL) {
                    FZLOGERROR("DataStore: Key attribute is missing.");
                    continue;
                }
                entry.key = fzStrcpy(attribute->value(), attribute->value_size());
                entry.hash = fzHash(attribute->value(), attribute->value_size());
                
                
                // ENTRY TYPE
                attribute = node->last_attribute(XML_TYPE_ATTRIBUTE);
                if(attribute == NULL) {
                    FZLOGERROR("DataStore: Type attribute is missing.");
                    continue;
                }
                entry.type = atoi(attribute->value());
                
                
                // DATA
                const char *data = node->value();
                if(data) {
                    switch (entry.type) {
                        case kFZData_string:
                        case kFZData_data:
                        {
                            fzBuffer decoded = fzData_B64Decode(data, node->value_size());
                            entry.data = decoded;
                            break;
                        }
                        case kFZData_integer:
                            entry.integerValue = atoi(data);
                            break;
                        case kFZData_float:
                            entry.floatValue = atof(data);
                            break;
                        default:
                            FZ_RAISE("DataStore: Entry type is invalid.");
                            break;
                    }
                }
                ++m_num;
            }
            buffer.free();
            
            FZ_ASSERT(m_num <= m_capacity, "Memory overflow");
        }
    }
    
    
#pragma mark - Low level
    
    DataStore::fzStoreEntry* DataStore::entryForHash(int32_t keyhash) const
    {
        if(m_num == 0)
            return NULL;
        
        FZ_ASSERT(p_store, "Store was not allocated.");

        for(fzUInt i = 0; i < m_num; ++i) {
            if( keyhash == p_store[i].hash )
                return &p_store[i];
        }
        return NULL;
    }
    
    
    DataStore::fzStoreEntry* DataStore::entryForKey(const char *key) const
    {
        return entryForHash(fzHash(key));
    }
    
    
    void DataStore::setEntry(const fzStoreEntry& entry)
    {
        fzStoreEntry *current = entryForHash(entry.hash);
        removeEntry(current);
        
        reserveCapacity(m_num+1);
        p_store[m_num] = entry;
        ++m_num;
        m_dirty = true;
    }
    
    
    void DataStore::removeForKey(const char *key)
    {
        removeEntry(entryForKey(key));
    }
    
    
    void DataStore::removeEntry(fzStoreEntry *entry)
    {
        if(entry) {
            delete entry->key;
            
            if(entry->type == kFZData_string || entry->type == kFZData_data)
                entry->data.free();
            
            *entry = p_store[--m_num];
            m_dirty = true;
        }
    }
    
    
#pragma mark - Setters
    
    bool DataStore::contains(const char *key)
    {
        return entryForKey(key) != NULL;
    }
    

    void DataStore::setDefaultFloat(fzFloat value, const char *key)
    {
        if(contains(key))
            return;
        
        setFloat(value, key);
    }
    
    
    void DataStore::setDefaultInteger(fzInt value, const char *key)
    {
        if(contains(key))
            return;
        
        setInteger(value, key);
    }
    
    
    void DataStore::setDefaultString(const char* value, const char *key)
    {
        if(contains(key))
            return;
        
        setString(value, key);
    }
    
    void DataStore::setDefaultData(const fzBuffer& data, const char *key)
    {
        if(contains(key))
            return;
        
        setData(data, key);
    }

    
    void DataStore::setFloat(fzFloat value, const char *key)
    {
        fzStoreEntry entry;
        entry.key = fzStrcpy(key);
        entry.hash = fzHash(key);
        entry.type = kFZData_float;
        
        entry.floatValue = value;
        
        setEntry(entry);
    }
    
    
    void DataStore::setInteger(fzInt value, const char *key)
    {
        fzStoreEntry entry;
        entry.key = fzStrcpy(key);
        entry.hash = fzHash(key);
        entry.type = kFZData_integer;
        
        entry.integerValue = value;
        
        setEntry(entry);
    }
    
    
    void DataStore::setString(const char* value, const char *key)
    {
        fzStoreEntry entry;
        entry.key  = fzStrcpy(key);
        entry.hash = fzHash(key);
        entry.type = kFZData_string;
        
        entry.data = fzBuffer(fzStrcpy(value), strlen(value)+1);
        
        setEntry(entry);
    }
    
    
    void DataStore::setData(const fzBuffer& data, const char *key)
    {
        fzStoreEntry entry;
        entry.key  = fzStrcpy(key);
        entry.hash = fzHash(key);
        entry.type = kFZData_data;
        
        entry.data = data;
        
        setEntry(entry);
    }
    
    
#pragma mark - Getters
    
    fzInt DataStore::integerForKey(const char *key) const
    {
        fzStoreEntry *entry = entryForKey(key);
        
        if(entry) {
            switch (entry->type) {
                case kFZData_integer: return entry->integerValue;
                case kFZData_float: return static_cast<fzInt>(entry->floatValue);
                default:
                    FZ_RAISE_STOP("DataStore: Imposible to convert type.");
                    return 0;
            }
        }
        return 0;
    }
    
    
    fzFloat DataStore::floatForKey(const char *key) const
    {
        fzStoreEntry *entry = entryForKey(key);

        if(entry) {
            switch (entry->type) {
                case kFZData_float: return entry->floatValue;
                case kFZData_integer: return static_cast<fzFloat>(entry->integerValue);
                default:
                    FZ_RAISE_STOP("DataStore: Imposible to convert type.");
                    return 0.f;
            }
        }
        return 0.f;
    }
    
    
    const char* DataStore::stringForKey(const char *key) const
    {
        fzStoreEntry *entry = entryForKey(key);
        
        if(entry) {
            switch (entry->type) {
                case kFZData_string:
                    return entry->data.getPointer();
                    
                default:
                    FZ_RAISE_STOP("DataStore: Imposible to convert type.");
                    return NULL;
            }
        }
        return NULL;
    }
    
    
    const fzBuffer DataStore::dataForKey(const char *key) const
    {
        fzStoreEntry *entry = entryForKey(key);
        
        if(entry) {
            switch (entry->type) {
                case kFZData_string:
                case kFZData_data:
                    return entry->data;
                    
                default:
                    FZ_RAISE_STOP("DataStore: Imposible to convert type.");
                    return fzBuffer::empty();
            }
        }
        return fzBuffer::empty();
    }
    
    
    void DataStore::removeAllData()
    {
        FZLog("NOT IMPLEMENTED");
    }

}
