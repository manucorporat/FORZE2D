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

#include "FZBitOrder.h"


namespace FORZE {
    
    inline bool fzBitOrder_isLittleEndian()
    {
        int i = 1;
        char *pc = (char *)&i;
        return *pc == 1;
    }
    
    
    uint16_t fzBitOrder_int16Swap (uint16_t n)
    {
        return ((n>>8)&0xff) | ((n<<8)&0xff00);   
    }
    
    
    uint32_t fzBitOrder_int32Swap (uint32_t n)
    {
        return ((n>>24)&0xff) | ((n>>8)&0xff00) | 
        ((n<<8)&0xff0000) | ((n << 24)&0xff000000);
    }
    
    
    uint64_t fzBitOrder_int64Swap (uint64_t n)
    {
        return (uint64_t) fzBitOrder_int32Swap( n&0x00000000ffffffff) << 32 | 
        (uint64_t) fzBitOrder_int32Swap( (n>>32)&0x00000000ffffffff);
    }
    
    
    uint16_t fzBitOrder_int16LittleToHost(uint16_t little)
    {
        if(!fzBitOrder_isLittleEndian())
            return fzBitOrder_int16Swap(little);
        
        return little;
    }
    
    
    uint32_t fzBitOrder_int32LittleToHost(uint32_t little)
    {
        if(!fzBitOrder_isLittleEndian())
            return fzBitOrder_int32Swap(little);
        
        return little;
    }
    
    
    uint64_t fzBitOrder_int64LittleToHost(uint64_t little)
    {
        if(!fzBitOrder_isLittleEndian())
            return fzBitOrder_int64Swap(little);
        
        return little;
    }
    
    
    uint16_t fzBitOrder_int16BigToHost(uint16_t big)
    {
        if(fzBitOrder_isLittleEndian())
            return fzBitOrder_int16Swap(big);
        
        return big;
    }
    
    
    uint32_t fzBitOrder_int32BigToHost(uint32_t big)
    {
        if(fzBitOrder_isLittleEndian())
            return fzBitOrder_int32Swap(big);
        
        return big;
    }
    
    
    uint64_t fzBitOrder_int64BigToHost(uint64_t big)
    {
        if(fzBitOrder_isLittleEndian())
            return fzBitOrder_int64Swap(big);
        
        return big;
    }

}
