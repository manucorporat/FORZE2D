// DO NOT MODIFY THE HEADERS IF FORZE IS ALREADY COMPILED AS A STATIC LIBRARY
#ifndef __FZDATACACHE_H_INCLUDED__
#define __FZDATACACHE_H_INCLUDED__
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

#include "FZTypes.h"


namespace FORZE {
    
    class DataStore
    {
    private:
        enum {
            kFZData_string,
            kFZData_integer,
            kFZData_float,
            kFZData_data,
        };
        
        struct fzStoreEntry
        {
            unsigned char m_type;
            int32_t m_hash;
            char *p_key;
            
            union {
                char *p_ptr;
                fzFloat m_float;
                fzInt m_integer;
            };
        };
        
        //! Instance
        static DataStore* p_instance;
        
        //! array with all entries
        fzStoreEntry *p_store;
        
        //! number of entries
        fzUInt m_num;
        
        //! store capacity
        fzUInt m_capacity;
        
        //! is data dirty
        bool m_dirty;
        
        //! data base absolute path.
        const char *p_path;
        
        //! Used intenally to read the xml data base.
        bool readFromMemory();
        
        //! Reserve capacity.
        void reserveCapacity(fzUInt capacity);
        
        //! Used internally to set or update an entry.
        void setEntry(const fzStoreEntry& entry);
        
        //! Used internally to get a entry given the hash value
        fzStoreEntry *entryForHash(int32_t hash) const;
        
        //! Used internally to get a entry given the key
        fzStoreEntry *entryForKey(const char *key) const;
        
        
    protected:
        DataStore();
        DataStore(const DataStore& );
        DataStore &operator = (const DataStore& );
        
        
    public:
        //! Gets and allocates the singleton.
        static DataStore& Instance();
        
        //! Sets a float value for the key specified.
        //! @param value is a float.
        //! @param key is a NULL-terminated char string.
        bool contains(const char *key);
        
        
        //! Sets the default float value for the key specified.
        //! @param value is a float.
        //! @param key is a NULL-terminated char string.
        //! @code if(!contain(key)) setFloat(value, key)
        void setDefaultFloat(fzFloat value, const char *key);
        
        
        //! Sets the default integer value for the key specified.
        //! @param value is an integer.
        //! @param key is a NULL-terminated char string.
        //! @code if(!contain(key)) setInteger(value, key)
        void setDefaultInteger(fzInt value, const char *key);
        
        
        //! Sets the default string value for the key specified.
        //! @param value is a NULL-terminated char string.
        //! @param key is a NULL-terminated char string.
        //! @code if(!contain(key)) setString(value, key)
        void setDefaultString(const char* value, const char *key);
        
        
        //! Sets a float value for the key specified.
        //! @param value is a float.
        //! @param key is a NULL-terminated char string.
        void setFloat(fzFloat value, const char *key);
        
        
        //! Sets an integer value for the key specified.
        //! @param value is an integer.
        //! @param key is a NULL-terminated char string.
        void setInteger(fzInt value, const char *key);
        
        
        //! Sets a string value for the key specified.
        //! @param value is a NULL-terminated char string.
        //! @param key is a NULL-terminated char string.
        void setString(const char* value, const char *key);
        
        
        //! Sets a string value for the key specified.
        //! @param value is a NULL-terminated char string.
        //! @param key is a NULL-terminated char string.
        void setData(const char* data, fzUInt length, const char *key);
        
        
        //! Returns the integer paired with the given key.
        //! @param key is a NULL-terminated char string.
        //! @return if no value found for key, 0 is returned.
        fzInt integerForKey(const char *key) const;
        
        
        //! Returns the float value paired with the given key.
        //! @param key is a NULL-terminated char string.
        //! @return if no value found for key, 0.0 is returned.
        fzFloat floatForKey(const char *key) const;
        
        
        //! Returns the string paired with the given key.
        //! @param key is a NULL-terminated char string.
        //! @return If no value found for key, NULL is returned.
        const char* stringForKey(const char *key) const;
        
        
        //! Removes an entry giving the key
        //! @param key is a NULL-terminated char array
        void removeForKey(const char *key);
        
        
        //! Removes all entries.
        void removeAllData();
        

        //! Saves the changes in the persistent memory.
        //! You don't have to call this method unless you want to force the saving,
        //! FORZE saves the data automatically.
        //! @return If data could not be saved, false is returned. Change to debug mode.
        bool save();
    };
}
#endif
