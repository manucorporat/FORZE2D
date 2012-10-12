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


#import "FZTexture2D.h"
#import "FZSprite.h"

namespace FORZE {
    
    
    /** CCLabel is a subclass of CCTextureNode that knows how to render text labels
     *
     * All features from CCTextureNode are valid in CCLabel
     *
     * CCLabel objects are slow. Consider using CCLabelAtlas or CCBitmapFontAtlas instead.
     */
    
    class LabelTTF : public Sprite, public LabelProtocol
    {
    protected:
        FZSize dimensions_;
        float fontSize_;
        std::string fontName_;
        std::string string_;
        
        bool init(const char* str, const FZSize& d, const char* fn, float fs);
    public:
        /** initializes the CCLabel with a font name, alignment, dimension in points, line brea mode and font size in points.
         */
        LabelTTF(const char* str, const FZSize& dimensions, const char* fontName, float fontSize);
        
        
        /** initializes the CCLabel with a font name and font size in points */
        LabelTTF(const char* str, const char* fontName, float fontSize);
        
        /** changes the string to render
         * @warning Changing the string is as expensive as creating a new CCLabel. To obtain better performance use CCLabelAtlas
         */
        void setString(const char* str);
        const char* getString() const;
        
    };
}
