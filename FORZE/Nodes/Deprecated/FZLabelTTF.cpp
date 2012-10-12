/*
 * cocos2d for iPhone: http://www.cocos2d-iphone.org
 *
 * Copyright (c) 2008-2010 Ricardo Quesada
 * Copyright (c) 2011 Zynga Inc.
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
 *
 */



#import "FZLabelTTF.h"
#import "FZMacros.h"
#import "FZShaderCache.h"
#import "FZGLProgram.h"
#import "FZBufferManager.h"
#import "FZFontManager.h"


#if CC_USE_LA88_LABELS
#define SHADER_PROGRAM kCCShader_PositionTextureColor
#else
#define SHADER_PROGRAM kCCShader_PositionTextureA8Color
#endif

LabelTTF::init(const std::string& str, const FZSize& d, CCTextAlignment a, const std::string& fn, float fs)
{
    // shader program
    self.shaderProgram = [[CCShaderCache sharedShaderCache] programForKey:SHADER_PROGRAM];
    
    dimensions_ = d;
    alignment_ = a;
    fontName_ = fn;
    fontSize_ = fs;
    
    setString(str);
}


LabelTTF::LabelTTF(const std::string& s, const FZSize& d, CCTextAlignment a, const std::string& fn, float fs)
{
    init(s, d, a, fn, fs);
}


LabelTTF::LabelTTF(const std::string& s, const std::string& fn, float fs)
{
    init(s, FZSizeZero, 0, fn, fs);
}


void LabelTTF::setString(const std::string& str)
{
    string_ = str;
    
	Texture2D *tex;
	if( dimensions_ == FZSizeZero )
    {
        // Render label
        fontOutData outData = FontManager::Instance().generateLabelBitmap(fontName_, string_, fontSize_);
        

        // Create Texture2D
        tex = new Texture2D(outData.buffer.getPointer(),
                                      kFZTexture2DPixelFormat_AI88,
                                      outData.width,
                                      outData.height,
                                      outData.size
                                      );
        
        
        // Close buffer
        outData.buffer.close();
        
    }else{
        FZLog("FZLabelTTF dimensions != Zero not implemented");
        exit(-1);
    }
    
    
	FZRect rect;
	rect.size = outData.size;
    
    // Set texture
    setTexture(tex);
    
    // Set texture rect
    setTextureRect(rect);
}

const std::string& LabelTTF::getString() const
{
    return string_;
}


@implementation CCLabelTTF

- (id) init
{
	FZ_ASSERT(NO, "CCLabelTTF: Init not supported. Use initWithString");
	[self release];
	return nil;
}

+ (id) labelWithString:(NSString*)string dimensions:(CGSize)dimensions alignment:(CCTextAlignment)alignment lineBreakMode:(CCLineBreakMode)lineBreakMode fontName:(NSString*)name fontSize:(CGFloat)size;
{
	return [[[self alloc] initWithString: string dimensions:dimensions alignment:alignment lineBreakMode:lineBreakMode fontName:name fontSize:size]autorelease];
}

+ (id) labelWithString:(NSString*)string dimensions:(CGSize)dimensions alignment:(CCTextAlignment)alignment fontName:(NSString*)name fontSize:(CGFloat)size
{
	return [[[self alloc] initWithString: string dimensions:dimensions alignment:alignment fontName:name fontSize:size]autorelease];
}

+ (id) labelWithString:(NSString*)string fontName:(NSString*)name fontSize:(CGFloat)size
{
	return [[[self alloc] initWithString: string fontName:name fontSize:size]autorelease];
}


- (id) initWithString:(NSString*)str dimensions:(CGSize)dimensions alignment:(CCTextAlignment)alignment lineBreakMode:(CCLineBreakMode)lineBreakMode fontName:(NSString*)name fontSize:(CGFloat)size
{
	if( (self=[super init]) ) {

		// shader program
		self.shaderProgram = [[CCShaderCache sharedShaderCache] programForKey:SHADER_PROGRAM];
		
		dimensions_ = CGSizeMake( dimensions.width, dimensions.height );
		alignment_ = alignment;
		fontName_ = [name retain];
		fontSize_ = size;
		lineBreakMode_ = lineBreakMode;
		
		[self setString:str];
	}
	return self;
}

- (id) initWithString:(NSString*)str dimensions:(CGSize)dimensions alignment:(CCTextAlignment)alignment fontName:(NSString*)name fontSize:(CGFloat)size
{
	return [self initWithString:str dimensions:dimensions alignment:alignment lineBreakMode:CCLineBreakModeWordWrap fontName:name fontSize:size];
}

- (id) initWithString:(NSString*)str fontName:(NSString*)name fontSize:(CGFloat)size
{
	if( (self=[super init]) ) {
		
		// shader program
		self.shaderProgram = [[CCShaderCache sharedShaderCache] programForKey:SHADER_PROGRAM];
		
		dimensions_ = CGSizeZero;
		fontName_ = [name retain];
		fontSize_ = size;
		
		[self setString:str];
	}
	return self;
}

