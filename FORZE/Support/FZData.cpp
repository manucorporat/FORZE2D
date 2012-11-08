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

/*
 * Inflates either zlib or gzip deflated memory. The inflated memory is
 * expected to be freed by the caller.
 *
 * inflateMemory_ based on zlib example code
 *		http://www.zlib.net
 *
 * Some ideas were taken from:
 *		http://themanaworld.org/
 *		from the mapreader.cpp file 
 */

#include <zlib.h>
#include <new>

#include "FZData.h"
#include "FZMacros.h"
#include "FZAllocator.h"
#include "FZBitOrder.h"

#include "base64.h"


#define BUFFER_INC_FACTOR (2)

namespace FORZE {
    
    /** @struct CCZHeader */
    struct CCZHeader {
        uint8_t			sig[4];				// signature. Should be 'CCZ!' 4 bytes
        uint16_t		compression_type;	// should 0
        uint16_t		version;			// should be 2 (although version type==1 is also supported)
        uint32_t		reserved;			// Reserverd for users.
        uint32_t		len;				// size of the uncompressed file
    };
    
    enum {
        CCZ_COMPRESSION_ZLIB,				// zlib format.
        CCZ_COMPRESSION_BZIP2,				// bzip2 format (not supported yet)
        CCZ_COMPRESSION_GZIP,				// gzip format (not supported yet)
        CCZ_COMPRESSION_NONE,				// plain (not supported yet)
    };
    
    
    
    int inflateMemoryWithHint(unsigned char *input, unsigned int inLength, unsigned char **output, unsigned int *outLength, unsigned int outLenghtHint );
    
    int inflateMemoryWithHint(unsigned char *input, unsigned int inLength, unsigned char **output, unsigned int *outLength, unsigned int outLenghtHint )
    {        
        unsigned int bufferSize = outLenghtHint;
        
        // ALLOC OUTPUT BUFFER
        *output = new unsigned char[bufferSize];
        if (!*output) {
            FZLOGERROR("ZIP: Memory alloc failed.");
            return Z_MEM_ERROR;
        }
        
        // DESCOMPRESSION STREAM
        z_stream d_stream;
        d_stream.zalloc = (alloc_func)0;
        d_stream.zfree = (free_func)0;
        d_stream.opaque = (voidpf)0;
        d_stream.next_in  = input;
        d_stream.avail_in = inLength;
        d_stream.next_out = *output;
        d_stream.avail_out = bufferSize;
        
        // INIT STREAM
        int status = inflateInit2(&d_stream, 15 + 32);
        if( status != Z_OK )
            return status;
        
        
        // DESCOMPRESS INPUT DATA
        while (true) {
            status = inflate(&d_stream, Z_NO_FLUSH);
            
            if (status == Z_STREAM_END)
                break;
            
            switch (status) {
                case Z_NEED_DICT:
                    status = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    inflateEnd(&d_stream);
                    return status;
            }
            
            // REALLOC OUTPUT BUFFER
            if (status != Z_STREAM_END) {
                
                unsigned char *tmp = new(std::nothrow) unsigned char[bufferSize * BUFFER_INC_FACTOR];
                if (! tmp ) {
                    FZLOGERROR("ZIP: Memory realloc failed.");
                    inflateEnd(&d_stream);
                    return Z_MEM_ERROR;
                }
                memcpy(tmp, *output, bufferSize * BUFFER_INC_FACTOR);
                *output = tmp;
                
                d_stream.next_out = *output + bufferSize;
                d_stream.avail_out = bufferSize;
                bufferSize *= BUFFER_INC_FACTOR;                
            }
        }
        *outLength = bufferSize - d_stream.avail_out;
        
        // RELEASE UNUSED BUFFER MEMORY
        FZ_ASSERT(bufferSize >= *outLength, "Strange bug");
//        if(bufferSize != *outLength)
//            *output = (unsigned char*)realloc(*output, *outLength);
        
        status = inflateEnd(&d_stream);
        return status;
    }
    
    
    fzBuffer Data::inflateZIPWithHint(unsigned char *input, unsigned int inLength, unsigned int outLengthHint)
    {
        unsigned int outLength = 0;
        unsigned char* output;
        int err = inflateMemoryWithHint(input, inLength, &output, &outLength, outLengthHint );
        
        if (err != Z_OK || output == NULL) {
            if (err == Z_MEM_ERROR)
                FZLOGERROR("ZIP: Out of memory while decompressing map data.");
            
            else if (err == Z_VERSION_ERROR)
                FZLOGERROR("ZIP: Incompatible zlib version.");
            
            else if (err == Z_DATA_ERROR)
                FZLOGERROR("ZIP: Incorrect zlib compressed data.");
            
            else
                FZLOGERROR("ZIP: Unknown error while decompressing map data.");
            
            delete [] output;
            output = NULL;
            outLength = 0;
        }
        
        return fzBuffer((char*)output, outLength);
    }
    
    
    fzBuffer Data::inflateZIP(unsigned char *input, unsigned int inLength)
    {
        return Data::inflateZIPWithHint(input, inLength, 128 * 1024);        
    }
    
    
    fzBuffer Data::inflateCCZ(unsigned char *input, unsigned int inLength)
    {
        // load file into memory
        const CCZHeader *header = reinterpret_cast<const CCZHeader*>(input);
        
        
        // verify header
        if(!(header->sig[0] == 'C' &&
             header->sig[1] == 'C' &&
             header->sig[2] == 'Z' &&
             header->sig[3] == '!')) {
            FZLOGERROR("IO:CCZ: Invalid CCZ file.");
            return fzBuffer::empty();
        }
        
        
        // verify header version
        if( fzBitOrder_int16BigToHost(header->version) > 2 ) {
            FZLOGERROR("IO:CCZ: Unsupported version.");
            return fzBuffer::empty();
        }
        
        
        // verify compression format
        if( fzBitOrder_int16BigToHost(header->compression_type) != CCZ_COMPRESSION_ZLIB ) {
            FZLOGERROR("IO:CCZ: Unsupported compression method.");
            return fzBuffer::empty();
        }
        
        
        uLong fileLen = inLength-1;
        uint32_t expectedLen = fzBitOrder_int32BigToHost( header->len );
        uLongf realLen = expectedLen;
        
        char *contentData = new char[expectedLen];
        uLongf source = (uLongf)input + sizeof(CCZHeader);
        
        
        int ret = uncompress((Bytef*)contentData,
                             &realLen,
                             (Bytef*)source,
                             (uLong)(fileLen - sizeof(CCZHeader))
                             );
                
        if( ret != Z_OK ) {
            FZLOGERROR("IO:CCZ: Failed to uncompress data. Error code: %d.", ret);
            delete [] contentData;
            return fzBuffer::empty();
        }
        FZ_ASSERT(realLen <= expectedLen, "Corrupted .CCZ. The expected uncompressed data length is wrong. Buffer overflow occurred.");
        
        return fzBuffer(contentData, expectedLen);
    }
    
    
    fzBuffer Data::B64Encode(const char *input, fzUInt inLength)
    {
        FZ_ASSERT(input, "Input pointer cannot be NULL");
        FZ_ASSERT(inLength > 0, "Input data length cannot be 0.");        
        
        //should be enough to store 6-bit buffers in 8-bit buffers
        fzUInt outputSize = Base64encode_len(inLength);
        char *output = new(std::nothrow) char[outputSize];
        
        if( output ) {
            Base64encode(output, input, inLength);
            return fzBuffer(output, outputSize);
        
        }else{
            FZLOGERROR("Base64: error allocating memory.");
            return fzBuffer::empty();
        }
    }
    
    
    fzBuffer Data::B64Decode(const char *input, fzUInt inLength)
    {
        FZ_ASSERT(input, "Input pointer cannot be NULL");
        FZ_ASSERT(inLength > 0, "Input data length cannot be 0.");
        
        //should be enough to store 6-bit buffers in 8-bit buffers
        fzUInt outputSize = Base64decode_len(input);
        char *output = new(std::nothrow) char[outputSize];
        
        if( output ) {
            Base64decode(output, input);
            return fzBuffer(output, outputSize);
            
        }else{
            FZLOGERROR("Base64: error allocating memory.");
            return fzBuffer::empty();
        }
    }
}
