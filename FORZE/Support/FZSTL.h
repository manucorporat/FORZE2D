#ifndef __FZ_STD_H_INCLUDED__
#define __FZ_STD_H_INCLUDED__
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

#include "FZTypes.h"
#include "FZLifeCycle.h"
#include <cstdlib>


namespace FORZE {
    
#define FZ_FOREACH(__CONTAINER__, __ITEM__) \
__ITEM__ = __CONTAINER__.at(_i); \
for(fzUInt _i = 0; i < (__CONTAINER__).size();  __ITEM__ = (__CONTAINER__).at(_i))
    
    
    template <typename T> 
    class vector
    {
    protected:
        T *m_container;
        fzUInt m_count;
        fzUInt m_capacity;
        
        virtual void retain(const T&) {}
        virtual void release(const T&) {}
        
    public:        
        explicit vector(const fzUInt capacity)
        : m_container(NULL)
        , m_capacity(0)
        , m_count(0)
        {
            reserve(capacity);
        }
        
        
        vector()
        : vector(16)
        { }
        
        
        vector(const vector<T>& v)
        : vector(v.size())
        {
            assign(v);
        }
        
        
        ~vector() {
            clear();
            free(m_container);
        }
        
        
        void reserve(fzUInt capacity) {
            
            if(capacity > m_capacity) {
                if(m_container)
                    delete [] m_container;
                
                m_container = new T[capacity];
                m_capacity = capacity;
            }
        }
        
        
        void assign(const vector<T>& v)
        {
            clear();
            m_count = v.size();
            reserve(m_count);
            
            for(fzUInt i = 0; i < v.size(); ++i)
                retain(v.at(i));
            
            memcpy(m_container, v.getElements(), m_count * sizeof(T));
        }
        
        
        void insert(const T& item, fzUInt index)
        {
            if(m_count >= m_capacity)
                reserve(m_capacity * 2);
            
            if(index <= m_count) {
                fzInt remaining = m_count - index;
                if(remaining > 0)
                    memmove(&m_container[index+1], &m_container[index], remaining * sizeof(T));
                
                m_container[index] = item;
                retain(item);
                ++m_count;
            }
        }
        
        
        void remove(fzUInt index)
        {
            if(index < m_count) {
                fzInt remaining = m_count - index -1;
                release(m_container[index]);
                if(remaining > 0)
                    memmove(&m_container[index], &m_container[index+1], remaining);
                
                --m_count;
            }
        }
        
        
        void remove(const T& item)
        {
            fzInt index = index(item);
            remove(index);
        }
        
        
        void push_back(const T& item)
        {
            insert(item, m_count);
        }
        
        
        void push_front(const T& item)
        {
            insert(item, 0);
        }
        
        
        void pop_front()
        {
            remove(0);
        }
        
        
        void pop_back()
        {
            remove(m_count-1);
        }
        
        
        void clear()
        {
            for(fzUInt i = 0; i < m_count; ++i)
                retain(m_container[i]);
            
            m_count = 0;
            m_container[0] = NULL;
        }
        
        
        const T& at(fzUInt index) const {
            return m_container[index];
        }
        
        T& at(fzUInt index) {
            return m_container[index];
        }
        
        
        const T& operator[] (fzUInt index) const {
            return m_container[index];
        }
        
        
        T& operator[] (fzUInt index) {
            return m_container[index];
        }
        
        
        const T& front() const {
            return m_container[0];
        }
        
        
        const T& back() const {
            return m_container[m_count];
        }
        
        
        fzUInt size() const {
            return m_count;
        }
        
        bool empty() const {
            return m_count == 0;
        }
        
        
        fzUInt capacity() const {
            return m_capacity;
        }
        
        
        const T* getElements() const {
            return m_container;
        }
    };
    
    
    class fzVector : public vector<LifeCycle*>
    {
        void retain(LifeCycle*& item)
        {
            item->retain();
        }
        void release(LifeCycle*& item)
        {
            item->retain();
        }
          
        
    public:
        fzVector()
        : vector()
        { }
        
        
        fzVector(const fzUInt capacity)
        : vector(capacity)
        { }
        
        fzVector(const fzVector& v)
        : vector(v)
        { }
    };
    
    
    
    class string : public vector<char>
    {
    public:
        string()
        : vector()
        { }
        
        
        string(const char* str, fzUInt length)
        : vector(length+1)
        {
            memmove(m_container, str, length);
            m_container[length] = '\0';
            m_count = length;
            
            assert(length <= m_capacity);
        }
        
        
        string(const char* str)
        : string(str, strlen(str))
        { }
        
        
        string(const string& str)
        : string(str.c_str(), str.size())        
        { }
        

        const char* c_str() const {
            return m_container;
        }
        
        
        string& append(const char* str, fzUInt length) {
            fzUInt newSize = m_count + length + 1;
            if(newSize > m_capacity)
                reserve(m_capacity * 2);
            
            memcpy(&m_container[m_count], str, length);
            m_count = newSize;
            
            return *this;
        }
        
        string& append(const char* str) {
            return append(str, strlen(str));
        }
        
        string& append(const string& str, fzUInt pos, fzUInt length) {
            return append(&str.c_str()[pos], length);
        }
        
        string& append(const string& str, fzUInt pos = 0) {
            return append(str, pos, str.size());
        }
        
        int compare(const char* str, fzUInt length) const {
            return strncmp(m_container, str, length);
        }
        
        int compare(const char* str) const {
            return strcmp(m_container, str);
        }
        
        int compare(const string& str, fzUInt pos, fzUInt length) const {
            return strncmp(&m_container[pos], str.c_str(), length);
        }
        
        int compare(const string& str) const {
            return strcmp(m_container, str.c_str());
        }
        
        
        fzInt find(const char* str, fzUInt pos, fzUInt length) const {
            const char *ptr = strnstr(&m_container[pos], str, length);
            return (ptr) ? (ptr-m_container) : -1;
        }
        
        fzInt find(const char* str, fzUInt pos = 0) const {
            const char *ptr = strstr(&m_container[pos], str);
            return (ptr) ? (ptr-m_container) : -1;
        }
        
        fzInt find(const string& str, fzUInt pos = 0) const {
            return find(str.c_str(), pos);
        }
        
        fzInt find(char c, fzUInt pos = 0) const {
            const char *ptr = strchr(m_container, c);
            return (ptr) ? (ptr-m_container) : -1;
        }
        
        
        fzInt rfind(const char* str, fzUInt pos, fzUInt length) const {
            return -1;
        }
        
        fzInt rfind(const char* str, fzUInt pos = 0) const {
            return -1;

        }
        
        fzInt rfind(const string& str, fzUInt pos = 0) const {
            return -1;
        }
        
        fzInt rfind(char c, fzUInt pos = 0) const {
            return -1;
        }
        
        
        fzInt find_first_of(const char* str, fzUInt pos = 0) const {
            const char *ptr = strpbrk(&m_container[pos], str);
            return (ptr) ? (ptr-m_container) : -1;
        }
        
        fzInt find_first_of(const string& str, fzUInt pos = 0) const {
            return find(str.c_str(), pos);
        }
        
        fzInt find_first_of(char c, fzUInt pos = 0) const {
            return find(c, pos);
        }
        
        
        void copy(char *str, fzUInt length, fzUInt pos = 0) const {
            assert((length+pos) <= (m_count+1));
            memcpy(str, &m_container[pos], length);
        }
        
        string substr(fzUInt pos, fzUInt length) const {
            assert((length+pos) <= m_count);
            return string(&m_container[pos], length);
        }
        
        

        
        
        string& operator += (const char *str) {
            return append(str);
        }
        
        string& operator += (const string& str) {
            return append(str);
        }
        
        
        bool operator == (const char *str) const {
            return compare(str) == 0;
        }
        
        bool operator == (const string& str) const {
            return compare(str) == 0;
        }
        
        
        bool operator != (const char *str) const {
            return compare(str) != 0;
        }
        
        bool operator != (const string& str) const {
            return compare(str) != 0;
        }
    };
    
    
    template <typename K, typename T>
    struct pair 
    {
        int32_t m_hash;
        
    };
}
#endif
