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

#include "FZTexture2D.h"
#include "FZMacros.h"
#include "FZAllocator.h"
#include "FZGLState.h"
#include "FZDeviceConfig.h"
#include "FZResourcesManager.h"
#include "FZShaderCache.h"
#include "FZDirector.h"
#include "FZIO.h"
#include "FZMath.h"
#include "FZBitOrder.h"
#include "FZData.h"

#include "png.h"
#include "pnginfo.h"


using namespace STD;

namespace FORZE {

    static const char gPVRTexIdentifier[] = "PVR!";
    
#define PVR_TEXTURE_FLAG_TYPE_MASK	0xff

    enum {
        kPVRTextureFlagMipmap		= (1<<8),		// has mip map levels
        kPVRTextureFlagTwiddle		= (1<<9),		// is twiddled
        kPVRTextureFlagBumpmap		= (1<<10),		// has normals encoded for a bump map
        kPVRTextureFlagTiling		= (1<<11),		// is bordered for tiled pvr
        kPVRTextureFlagCubemap		= (1<<12),		// is a cubemap/skybox
        kPVRTextureFlagFalseMipCol	= (1<<13),		// are there false coloured MIP levels
        kPVRTextureFlagVolume		= (1<<14),		// is this a volume texture
        kPVRTextureFlagAlpha		= (1<<15),		// v2.1 is there transparency info in the texture
        kPVRTextureFlagVerticalFlip	= (1<<16),		// v2.1 is the texture vertically flipped
    };
    
    
    // PVR HEADER
    struct PVRTexHeader
    {
        uint32_t headerLength;
        uint32_t height;
        uint32_t width;
        uint32_t numMipmaps;
        uint32_t flags;
        uint32_t dataLength;
        uint32_t bpp;
        uint32_t bitmaskRed;
        uint32_t bitmaskGreen;
        uint32_t bitmaskBlue;
        uint32_t bitmaskAlpha;
        uint32_t pvrTag;
        uint32_t numSurfs;
    };
    
    
    enum
    {
        kPVRTexturePixelTypeRGBA_4444= 0x10,
        kPVRTexturePixelTypeRGBA_5551,
        kPVRTexturePixelTypeRGBA_8888,
        kPVRTexturePixelTypeRGB_565,
        kPVRTexturePixelTypeRGB_555,				// unsupported
        kPVRTexturePixelTypeRGB_888,
        kPVRTexturePixelTypeI_8,
        kPVRTexturePixelTypeAI_88,
        kPVRTexturePixelTypePVRTC_2,
        kPVRTexturePixelTypePVRTC_4,	
        kPVRTexturePixelTypeBGRA_8888,
        kPVRTexturePixelTypeA_8,
    };
    
    
    static const uint32_t pvrTableFormats[][2] = 
    {
        { kPVRTexturePixelTypeRGBA_4444,  kFZPixelFormat_RGBA4444},
        { kPVRTexturePixelTypeRGBA_5551,  kFZPixelFormat_RGB5A1},
        { kPVRTexturePixelTypeRGBA_8888,  kFZPixelFormat_RGBA8888},
        { kPVRTexturePixelTypeRGB_565,    kFZPixelFormat_RGB565},
        { kPVRTexturePixelTypeRGB_555,    0}, // unsupported
        { kPVRTexturePixelTypeRGB_888,    kFZPixelFormat_RGB888},
        { kPVRTexturePixelTypeI_8,        kFZPixelFormat_L8},
        { kPVRTexturePixelTypeAI_88,      kFZPixelFormat_LA88},
        { kPVRTexturePixelTypePVRTC_2,    kFZPixelFormat_PVRTC2},
        { kPVRTexturePixelTypePVRTC_4,    kFZPixelFormat_PVRTC4},
        { kPVRTexturePixelTypeBGRA_8888,  kFZPixelFormat_BGRA8888},
        { kPVRTexturePixelTypeA_8,        kFZPixelFormat_A8}
    };
    
    
    static const fzPixelInfo _pixelFormat_hash[] =
    {
        { kFZTextureFormat_RGBA8888,    GL_RGBA,            FZ_P_8888,  32,  false },
        { kFZTextureFormat_BGRA8888,    GL_BGRA,            FZ_P_8888,  32,  false },
        { kFZTextureFormat_RGB888,      GL_RGB,             FZ_P_888,   24,  false },
        { kFZTextureFormat_RGBA4444,    GL_RGBA,            FZ_P_4444,  16,  false },
        { kFZTextureFormat_RGB5A1,      GL_RGBA,            FZ_P_5551,  16,  false },
        { kFZTextureFormat_RGB565,      GL_RGB,             FZ_P_565,	16,  false },
        { kFZTextureFormat_LA88,        GL_LUMINANCE_ALPHA, FZ_P_88,    16,  false },
        { kFZTextureFormat_A8,          GL_ALPHA,           FZ_P_8,     8,	 false },
        { kFZTextureFormat_L8,          GL_LUMINANCE,       FZ_P_8,     8,	 false }
#if FZ_GL_PVRTC
        , { kFZTextureFormat_PVRTC4, 0, 0, 4, true}
        , { kFZTextureFormat_PVRTC2, 0, 0, 2, true}
#endif
    };
    
    static const fzTextureInfo _textureFormat_hash[] =
    {
        { FZ_T_RGBA8888,    32,  false },
        { FZ_T_BGBA8888,    32,  false },
        { FZ_T_RGB888,      24,  false },
        { FZ_T_RGBA4444,    16,  false },
        { FZ_T_RGBA5551,    16,  false },
        { FZ_T_RGB565,      16,  false },
        { FZ_T_LA88,        16,  false },
        { FZ_T_A8,          8,	 false },
        { FZ_T_L8,          8,	 false }
#if FZ_GL_PVRTC
        , { GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, 4, true}
        , { GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, 2, true}
#endif
    };
    
#define MAX_PVR_FORMATS (sizeof(pvrTableFormats) / sizeof(pvrTableFormats[0]))
#define MAX_PIXEL_FORMATS (sizeof(_pixelFormat_hash) / sizeof(_pixelFormat_hash[0]))
#define MAX_TEXTURE_FORMATS (sizeof(_textureFormat_hash) / sizeof(_textureFormat_hash[0]))
    
    
    
#pragma mark - Texture2D implementation
    
    fzTextureFormat Texture2D::_defaultPixelFormat = kFZTextureFormat_auto;
    
    void Texture2D::allocTexture()
    {
        if(m_textureID != 0)
            return;
        
        glGenTextures(1, &m_textureID);        
        setAntiAliasTexParameters();
    }
    
    
    unsigned char* Texture2D::expand(fzPixelFormat format, fzUInt fromX, fzUInt fromY, fzUInt toX, fzUInt toY, const void *ptr)
    {
        FZ_ASSERT(format >=0 && format < MAX_TEXTURE_FORMATS, "Texture format is not supported.");
        FZ_ASSERT(_pixelFormat_hash[format].isCompressed == false, "Compressed textures cannot be expanded.");
        FZ_ASSERT(toX >= fromX && toY >= fromY, "You cannot reduce a texture");
        FZ_ASSERT(toX != fromX || toY != fromY, "Not to resize  TO == FROM");
        
        uint8_t bytesPerPixel = _pixelFormat_hash[format].dataBBP >> 3;
        FZ_ASSERT(bytesPerPixel > 0, "Strange bug: memory corruption?");
        fzUInt rowSize = fromX * bytesPerPixel;
        fzUInt rowPOTSize = toX * bytesPerPixel;
        
        const uint8_t *src = static_cast<const uint8_t*>(ptr);
        uint8_t *dest = new uint8_t[rowPOTSize * toY];
            
        if(toY == fromY)
            memcpy(dest, ptr, rowSize * fromY);
        else for(fzUInt i = 0; i < fromY; ++i)
            memcpy(&dest[rowPOTSize * i], &src[rowSize * i], rowSize);
        
        return dest;
    }
    
    
    void Texture2D::upload(fzPixelFormat format, GLint level, GLsizei width, GLsizei height, GLsizei packetSize, const void *ptr)
    {
        FZ_ASSERT(level >= 0, "Level must me positive.");
        FZ_ASSERT(format >= 0, "Format must me positive.");
        FZ_ASSERT(m_format != kFZTextureFormat_invalid, "Texture format not initialized.")
        
        if(format >= MAX_TEXTURE_FORMATS)
            FZ_RAISE("Texture2D: Uploading error. Format is invalid.");
        
        allocTexture();
        bind();
        
        fzPixelInfo dataInfo = _pixelFormat_hash[format];
        fzTextureInfo textureInfo = _textureFormat_hash[m_format];

        if(!dataInfo.isCompressed)
        {
            glTexImage2D(GL_TEXTURE_2D, level,
                         textureInfo.openGLFormat,
                         width, height, 0,
                         dataInfo.dataFormat,
                         dataInfo.dataType,
                         ptr);
            

        }else if(DeviceConfig::Instance().isPVRTCSupported())
        {
            // COMPRESSED TEXTURE
            glCompressedTexImage2D(GL_TEXTURE_2D, level,
                                   textureInfo.openGLFormat,
                                   width, height, 0, packetSize, ptr);
        }
        CHECK_GL_ERROR_DEBUG();
    }
    
    
    Texture2D::Texture2D()
    : m_textureID(0)
    , m_factor(1)
    , m_texParams()
    , m_format(kFZTextureFormat_invalid)
    { }
    
    
    Texture2D::Texture2D(const void* ptr, fzPixelFormat pixelFormat, fzTextureFormat textureFormat, GLsizei width, GLsizei height, const fzSize &size)
    : Texture2D()
    {
        m_width = width;
        m_height = height;
        m_size = size;
        
        if(!DeviceConfig::Instance().isNPOTSupported()) {
          if(fzMath_isPOT(m_width) && fzMath_isPOT(m_height))
              FZ_RAISE("Texture2D: NPOT textures are not supported.");
        }
        
        setPixelFormat(pixelFormat, textureFormat);
        upload(pixelFormat, 0, width, height, 0, ptr);
    }
    
    
    Texture2D::Texture2D(fzTextureFormat format, const fzSize& size)
    : Texture2D(NULL, kFZPixelFormat_RGBA8888, format, fzMath_nextPOT(size.width), fzMath_nextPOT(size.height), size)
    { }
    
    
    Texture2D::Texture2D(const char* filename)
    : Texture2D()
    {
        FZ_ASSERT(filename != NULL, "Filename cannot be empty.");
        
        const char *extension = fzIO_getExtension(filename);
        if(extension == NULL)
            FZ_RAISE_STOP("Texture2D: File extension is missing.");

        
        if(strcasecmp( extension, "png") == 0 )
            loadPNGFile(filename);
        
        else if(strcasecmp( extension, "pvr") == 0 )
            loadPVRFile(filename);
        
        else if(strcasecmp( extension, "pvr.ccz") == 0 )
            loadPVRCCZFile(filename);
        
        else
            FZ_RAISE_STOP("Texture2D: Invalid file extension.");
    }
    
    
    Texture2D::~Texture2D()
    {
        if(m_textureID)
            fzGLDeleteTexture(m_textureID);
    }
    
    
    void Texture2D::setPixelFormat(fzPixelFormat pixelFormat, fzTextureFormat textureFormat)
    {
#if 0
        fzPixelInfo pixelInfo = _pixelFormat_hash[pixelFormat];
        
        if(pixelInfo.isCompressed)
            m_format = pixelInfo.textureFormat;
        
        else
        {
            switch (textureFormat) {
                case kFZTextureFormat_quality:
                case kFZTextureFormat_performance:
                    if(pixelFormat == kFZPixelFormat_A8 || pixelFormat == kFZPixelFormat_LA88)
                        m_format = kFZTextureFormat_LA88;
                    else
                        m_format = kFZTextureFormat_RGBA8888;
                    
                    break;
                case kFZTextureFormat_auto:
                    m_format = pixelInfo.textureFormat;
                    break;
                default:
                    m_format = textureFormat;
                    break;
            }
        }
#else
        fzPixelInfo pixelInfo = _pixelFormat_hash[pixelFormat];
        m_format = pixelInfo.textureFormat;
#endif
    }
    
    
    void Texture2D::loadPNGFile(const char *filename)
    {
        // LOAD CORRECT TEXTURE FILE
        FILE *file = NULL;
        {
            char absolutePath[512];
            fzUInt step = 0, factor = 0;
            while (true) {
                
                ResourcesManager::Instance().getPath(filename, step, absolutePath, &factor);
                if(factor == 0)
                    FZ_RAISE("Texture2D:PVR:IO: File not found.");
                
                file = fopen(absolutePath, "rb");
                if(file) {
                    m_factor = factor;
                    break;
                }
                ++step;
            }
        }
        
        
        // check png sign
        png_byte header[8];
        fread(header, 1, 8, file);
        if (::png_sig_cmp(header, 0, 8)) {
            fclose(file);
            FZ_RAISE_STOP("Texture2D:PNG: Invalid PNG tag.");
        }
        
        // initialize stuff
        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if(!png_ptr) {
            fclose(file);
            FZ_RAISE_STOP("Texture2D:PNG: \"png_create_read_struct\" failed, memory issue.");
        }

        
        png_infop info_ptr = png_create_info_struct(png_ptr);
        if(!info_ptr) {
            png_destroy_read_struct(&png_ptr, NULL, NULL);
            fclose(file);
            FZ_RAISE_STOP("Texture2D:PNG: \"png_create_info_struct\" failed, memory issue.");
        }
        
        
        if (setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(file);
            FZ_RAISE_STOP("Texture2D:PNG: Error during init_io.");
        }


        ::png_init_io(png_ptr, file);
        ::png_set_sig_bytes(png_ptr, 8);
        
        // get image info
        ::png_read_info(png_ptr, info_ptr);
   
        // Fix image format
        png_set_expand(png_ptr);

        // get image info
        int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        if (bit_depth == 16)
            png_set_strip_16(png_ptr);
        
        
        // Get updated info
        int color_type;
        png_uint_32 sizeWidth;
        png_uint_32 sizeHeight;
        
        png_get_IHDR( png_ptr, info_ptr,
                     &sizeWidth,
                     &sizeHeight,
                     &bit_depth,
                     &color_type,
                     NULL, NULL, NULL );
                
        fzPixelFormat pixelFormat;
        switch( color_type )
        {
            case PNG_COLOR_TYPE_GRAY: pixelFormat = kFZPixelFormat_A8; break;
            case PNG_COLOR_TYPE_GRAY_ALPHA: pixelFormat = kFZPixelFormat_LA88; break;
            case PNG_COLOR_TYPE_RGB: pixelFormat = kFZPixelFormat_RGB888; break;
            case PNG_COLOR_TYPE_RGBA: pixelFormat = kFZPixelFormat_RGBA8888; break;
                
            default:
                png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                fclose(file);
                FZ_RAISE_STOP("Texture2D:PNG: Invalid internal format.");
        }
        
        uint32_t width = fzMath_nextPOT(sizeWidth);
        uint32_t height = fzMath_nextPOT(sizeHeight);
        uint8_t bytes = _pixelFormat_hash[pixelFormat].dataBBP / 8;
        
        // get raw pixels
        fzUInt texLength = width * height * bytes;
        fzUInt structLength = sizeHeight * sizeof(png_bytep**);
        
        
        /* read file */
        if (setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(file);
            FZ_RAISE_STOP("Texture2D:PNG: libpng exception.");
        }
        
              png_byte *buffer;
        try{
            
            buffer = new png_byte[texLength + structLength];
            
        }catch(std::bad_alloc& error)
        {
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_info**)NULL);
            fclose(file);
            throw error;
        }

        png_byte *pixels = buffer;
        png_byte** row_ptrs = (png_byte**) (pixels + texLength);
        
        for (fzUInt i = 0; i < sizeHeight; ++i)
            row_ptrs[i] = pixels + (i * width * bytes);
        

        png_read_image(png_ptr, row_ptrs);
        png_read_end(png_ptr, NULL);
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_info**)NULL);

        fclose(file);

        
        // UPLOADING TEXTURE DATA TO GPU
        setPixelFormat(pixelFormat, getDefaultTextureFormat());
        upload(pixelFormat,
               0,
               width,
               height,
               0,
               pixels
               );
        
        delete [] buffer;
        
        
        m_width = width;
        m_height = height;
        m_size = fzSize(sizeWidth, sizeHeight);        
    }
    
    
    void Texture2D::loadPVRFile(const char *filename)
    {
        fzUInt factor;
        fzBuffer buffer = ResourcesManager::Instance().loadResource(filename, &factor);
        if(buffer.isEmpty())
            FZ_RAISE("Texture2D:IO: Error reading file.");
        
        m_factor = factor;
        
        // Unpack PVR Data
        try {
            loadPVRData(buffer.getPointer());
            buffer.free();

        } catch(std::runtime_error &error) {
            buffer.free();
            throw error;
        }
    }
    
    
    void Texture2D::loadPVRCCZFile(const char *filename)
    {
        fzUInt factor;
        fzBuffer buffer = ResourcesManager::Instance().loadResource(filename, &factor);
        
        if(buffer.isEmpty())
            FZ_RAISE("Texture2D:IO: Error reading file.");
        
        fzBuffer buffer2 = fzData_inflateCCZ((unsigned char*)buffer.getPointer(), buffer.getLength());
        buffer.free();
        
        if(buffer2.isEmpty())
            FZ_RAISE_STOP("Texture2D:IO: Error descompressing data.");
        
        m_factor = factor;
        
        
        // Unpack PVR Data
        try {
            loadPVRData(buffer2.getPointer());
            buffer2.free();
        } catch(std::runtime_error &error) {
            buffer2.free();
            throw error;
        }
    }
    
    
    void Texture2D::loadPVRData(const char* data)
    {
        if(data == NULL)
            FZ_RAISE("Texture2D:PVR: Imposible to load data. Pointer is NULL.");
        
        // CLEAN OPENGL ERROR
        glGetError();

        
        // GET HEADER RELEVANT INFO
        const PVRTexHeader *header = (const PVRTexHeader *)data;
        uint32_t pvrTag     = fzBitOrder_int32LittleToHost(header->pvrTag);
        uint32_t flags      = fzBitOrder_int32LittleToHost(header->flags);
        uint32_t width      = fzBitOrder_int32LittleToHost(header->width);
        uint32_t height     = fzBitOrder_int32LittleToHost(header->height);
        uint32_t dataLength = fzBitOrder_int32LittleToHost(header->dataLength);
        uint32_t pvrFormat  = flags & PVR_TEXTURE_FLAG_TYPE_MASK;
        uint32_t widthPOT   = fzMath_nextPOT(width);
        uint32_t heightPOT  = fzMath_nextPOT(height);
        bool isPOT          = (width == widthPOT && height == heightPOT);
        
        
        // VALIDATE DATA INTEGRITY
        if ((uint32_t)gPVRTexIdentifier[0] != ((pvrTag >>  0) & 0xff) ||
            (uint32_t)gPVRTexIdentifier[1] != ((pvrTag >>  8) & 0xff) ||
            (uint32_t)gPVRTexIdentifier[2] != ((pvrTag >> 16) & 0xff) ||
            (uint32_t)gPVRTexIdentifier[3] != ((pvrTag >> 24) & 0xff))
        {
            FZ_RAISE_STOP("Texture2D:PVR: Invalid PVR Tag.");
        }
        
        if(flags & kPVRTextureFlagVerticalFlip)
            FZLOGERROR("Texture2D:PVR:WARNING: Image is flipped. Regenerate it using PVRTexTool.");
        
        
        fzPixelFormat pixelFormat;
        fzUInt tableIndex = 0;
        for(; tableIndex < MAX_PVR_FORMATS; ++tableIndex) {
            if(pvrTableFormats[tableIndex][0] == pvrFormat) {
                pixelFormat = static_cast<fzPixelFormat>(pvrTableFormats[tableIndex][1]);
                break;
            }
        }
        if(tableIndex == MAX_PVR_FORMATS)
            FZ_RAISE_STOP("Texture2D:PVR: Unsupported format. Re-encode it with a OpenGL pixel format variant.");
        
        // GET TEXTURE METADATA
        setPixelFormat(pixelFormat, getDefaultTextureFormat());
        m_size      = fzSize(width, height);
        
        
        // GET TEXTURE DATA POINTER
        const char *textureData = data + sizeof(PVRTexHeader);

        
        // TEXTURE UPLOADING
        if( !isPOT ) {
            
            if(_pixelFormat_hash[pixelFormat].isCompressed != false)
                FZ_RAISE_STOP("Texture2D:PVR: Compressed textures cannot be NPOT.");

            if(DeviceConfig::Instance().isNPOTSupported()) {
                upload(pixelFormat, 0, width, height, 0, textureData);
                m_width  = width;
                m_height = height;
            
            }else{
                // If NPOT is not supported, the texture will be expanded.
                unsigned char *expanded = expand(pixelFormat, width, height, widthPOT, heightPOT, textureData);
                upload(pixelFormat, 0, widthPOT, heightPOT, 0, expanded);
                m_width  = widthPOT;
                m_height = heightPOT;
                delete [] expanded;
            }
            
        }else{
            m_width  = widthPOT;
            m_height = heightPOT;
            
            // MIPMAP TEXTURE UPLOADING
            uint32_t blockSize, widthBlocks, heightBlocks;
            uint32_t level = 0, dataOffset = 0, packetLength = 0;
            uint32_t bpp = _pixelFormat_hash[m_format].dataBBP;
            uint32_t dataSize = 0;
            const char *packetData;
            
            while (dataOffset < dataLength)
            {
                switch (pvrFormat) {
                    case kPVRTexturePixelTypePVRTC_2:
                        blockSize     = 8 * 4;
                        widthBlocks   = widthPOT / 8;
                        heightBlocks  = heightPOT / 4;
                        break;
                        
                    case kPVRTexturePixelTypePVRTC_4:
                        blockSize     = 4 * 4;
                        widthBlocks   = widthPOT / 4;
                        heightBlocks  = heightPOT / 4;
                        break;
                        
                    case kPVRTexturePixelTypeBGRA_8888:
                        FZ_ASSERT(DeviceConfig::Instance().isBGRA8888Supported(), "BGRA8888 not supported on this device");
                        
                    default:
                        blockSize     = 1;
                        widthBlocks   = widthPOT;
                        heightBlocks  = heightPOT;
                        break;
                }
                if(widthBlocks < 2) widthBlocks = 2;
                if(heightBlocks < 2) heightBlocks = 2;
                
                // CALCULATE PACKET
                dataSize        = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
                packetLength    = dataLength - dataOffset;
                packetLength    = (packetLength > dataSize) ? dataSize : packetLength;
                packetData      = textureData + dataOffset;
                
                
                // UPLOAD PACKET TO GPU
                upload(pixelFormat, level, widthPOT, heightPOT, packetLength, packetData);
                
                widthPOT >>= 1; // dividing by 2
                heightPOT >>= 1; // dividing by 2
                
                dataOffset += packetLength;
                ++level;
            }
        }
        return true;
    }
    
    
    fzSize Texture2D::getContentSize() const
    {
        return m_size / m_factor;
    }
    
    
    void Texture2D::setAntiAliasTexParameters()
    {
        fzTexParams params( GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
        setTexParameters(params);
    }
    
    
    void Texture2D::setAliasTexParameters()
    {
        fzTexParams params( GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
        setTexParameters(params);
    }
    
    
    void Texture2D::setTexParameters(const fzTexParams& params)
    {
        bind();
        if(m_texParams.minFilter != params.minFilter) {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minFilter);
            m_texParams.minFilter = params.minFilter;
        }
        if(m_texParams.magFilter != params.magFilter) {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magFilter );
            m_texParams.magFilter = params.magFilter;
        }
        if(m_texParams.wrapS != params.wrapS) {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrapS );
            m_texParams.wrapS = params.wrapS;
        }
        if(m_texParams.wrapT != params.wrapT) {
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrapT );
            m_texParams.wrapT = params.wrapT;
        }
        CHECK_GL_ERROR_DEBUG();
    }
    
    
    void Texture2D::generateMipmap() const
    {
        bind();
        fzGLGenerateMipmap(GL_TEXTURE_2D);
    }
    
    
    void Texture2D::bind() const
    {
        fzGLBindTexture2D(m_textureID);
    }
    
    
    void Texture2D::draw(fzFloat sX, fzFloat sY) const
    {
        float vertices[4*2] = {
            0, 0,
            sX * m_size.width / m_factor, 0,
            0, sY * m_size.height / m_factor,
            sX * m_size.width / m_factor, sY * m_size.height / m_factor
        };
        
        float texCoords[4*2] = {
            0, 0,
            m_size.width / m_width, 0,
            0, m_size.height / m_height,
            m_size.width / m_width, m_size.height / m_height
        };
        
        bind();

#if FZ_GL_SHADERS
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, vertices);
        glVertexAttribPointer(kFZAttribTexCoords, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
        
#else
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
        
#endif      
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        CHECK_GL_ERROR_DEBUG();
    }
    
    
#pragma mark - Static methods
    
    void Texture2D::setDefaultTextureFormat(fzTextureFormat format)
    {
        _defaultPixelFormat = format;
    }
    
    
    fzTextureFormat Texture2D::getDefaultTextureFormat()
    {
        return _defaultPixelFormat;
    }
    
    
    fzTextureFormat Texture2D::screenTextureFormat()
    {
        switch (Director::Instance().getGLConfig().colorFormat) {
            case kFZGLConfigColorRGB565: return kFZTextureFormat_RGB565;
            default: return kFZTextureFormat_RGBA8888;
        }
    }
    
    
    fzPixelInfo Texture2D::getPixelInfo(fzPixelFormat format)
    {
        FZ_ASSERT(format >= 0 && format < MAX_PIXEL_FORMATS, "Invalid pixel format");
        return _pixelFormat_hash[format];
    }
    
    
    fzTextureInfo Texture2D::getTextureInfo(fzTextureFormat format)
    {
        FZ_ASSERT(format >= 0 && format < (fzTextureFormat)MAX_TEXTURE_FORMATS, "Invalid texture format");
        return _textureFormat_hash[format];
    }
    
}
