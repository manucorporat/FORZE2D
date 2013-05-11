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

#include "FZFilter.h"

#if FZ_GL_SHADERS

#include "FZTexture2D.h"
#include "FZNode.h"
#include "FZDirector.h"
#include "FZGLState.h"
#include "FZMacros.h"
#include "FZResourcesManager.h"
#include "FZMath.h"
#include "FZMS.h"

// Shaders
#include "Shaders/_fz_mat_TEX.shader.h"
#include "Shaders/Effects/effect_color.shader.h"
#include "Shaders/Effects/effect_grayscale.shader.h"
#include "Shaders/Effects/effect_invert.shader.h"
#include "Shaders/Effects/effect_toon.shader.h"
#include "Shaders/Effects/effect_xray.shader.h"


using namespace STD;

namespace FORZE {
    
#pragma mark - Base filter
    
    Filter::Filter(fzFloat quality)
    : p_texture(NULL)
    , p_glprogram(NULL)
    {   
        FZ_ASSERT(quality > 0 && quality <= 1, "Quality is out of bounds (0, 1].");
    }
    
    
    Filter::Filter(const GLShader& fragmentShader, fzFloat quality)
    : Filter(quality)
    {
        // EXECUTABLE PROGRAM
        GLProgram *newprogram = new GLProgram(GLShader(__fz_vert_mat_TEX, GL_VERTEX_SHADER), fragmentShader);
        newprogram->addGenericAttributes();        
        newprogram->link();
        
        setGLProgram(newprogram);
    }
    
    
    Filter::Filter(const string& sFragmentFile, fzFloat quality)
    : Filter(quality)
    {
        // FRAGMENT SHADER
        fzBuffer source = ResourcesManager::Instance().loadResource(sFragmentFile.c_str());
        GLShader fragmentShader((const char*)source.getPointer(), GL_FRAGMENT_SHADER);
        source.free();
        
        // EXECUTABLE PROGRAM
        GLProgram *newprogram = new GLProgram(GLShader(__fz_vert_mat_TEX, GL_VERTEX_SHADER), fragmentShader);
        newprogram->addGenericAttributes();        
        newprogram->link();

        setGLProgram(newprogram);
    }
    
    
    Texture2D* Filter::getTexture() const
    {
        return p_texture;
    }
    
    
    void Filter::setTexture(Texture2D *texture)
    {
        if(p_texture == texture)
            return;
                
        if(texture) {
            
            fzUInt POTWide = texture->getPixelsWide();
            fzUInt POTHigh = texture->getPixelsHigh();
            fzSize contentSize = texture->getContentSize();
            fzSize textureSize = texture->getContentSizeInPixels();
            
            
            // CALCULATE VERTICES
            m_quad.bl.vertex = fzVec2(0, 0);
            m_quad.br.vertex = fzVec2(contentSize.width, 0);
            m_quad.tl.vertex = fzVec2(0, contentSize.height);
            m_quad.tr.vertex = fzVec2(contentSize.width, contentSize.height);
            
            
            m_quad.tl.texCoord = fzVec2(0, 0);
            m_quad.tr.texCoord = fzVec2(textureSize.width / POTWide, 0);
            m_quad.bl.texCoord = fzVec2(0, textureSize.height / POTHigh);
            m_quad.br.texCoord = fzVec2(textureSize.width / POTWide, textureSize.height / POTHigh);
        }

        p_texture = texture;
    }
    
    
    void Filter::setGLProgram(GLProgram *program)
    {
#if FZ_GL_SHADERS
        FZRETAIN_TEMPLATE(program, p_glprogram);
#else
        FZ_ASSERT(false, "Shaders are not supported.");
#endif
    }
    
    
    void Filter::draw()
    {        
        // Bind texture
        fzGLSetMode(kFZGLMode_TextureNoColor);
        p_texture->bind();        
        
        glDisable(GL_BLEND);
        
        p_glprogram->use();
        FZ_PROGRAM_APPLY_TRANSFORM(p_glprogram);
        
        glVertexAttribPointer(kFZAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(_fzT2_V2), &m_quad.bl.vertex);
        glVertexAttribPointer(kFZAttribTexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(_fzT2_V2), &m_quad.bl.texCoord);
        
        // Rendering
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	
        
        glEnable(GL_BLEND);
    }
    
    
#pragma mark - Filter Color ( brightness/contrast )
    
    FilterColor::FilterColor(fzFloat quality)
    : Filter(GLShader(effect_color_frag, GL_FRAGMENT_SHADER), quality)
    , m_brightness(-1)
    , m_contrast(-1)
    {
        setBrightness(0.0f);
        setContrast(1.0f);
    }
    
    
    void FilterColor::setBrightness(fzFloat brightness)
    {
        FZ_ASSERT(brightness >= -1.0f && brightness <= 1.0f, "Brightness is out of bounds [-1, 1].");
        if(m_brightness != brightness) {
            p_glprogram->setUniform1f("u_brightness"_hash, brightness + 0.5f);
            m_brightness = brightness;
        }
    }
    
    
    void FilterColor::setContrast(fzFloat contrast)
    {
        FZ_ASSERT(contrast >= 0 && contrast <= 5.0f, "Brightness is out of bounds [0, 5].");

        if(m_contrast != contrast) {
            p_glprogram->setUniform1f("u_contrast"_hash, contrast);
            m_contrast = contrast;
        }
    }
    
    
    fzFloat FilterColor::getBrightness() const
    {
        return m_brightness;
    }
    
    
    fzFloat FilterColor::getContrast() const
    {
        return m_contrast;
    }
    
    
#pragma mark - Filter Grayscale
    
    FilterGrayscale::FilterGrayscale(fzFloat quality)
    : Filter(GLShader(effect_grayscale_frag, GL_FRAGMENT_SHADER), quality)
    , m_intensity()
    {
        setIntensity(fzPoint3(0.33f, 0.33f, 0.33f));
    }
    
    
    void FilterGrayscale::setIntensity(const fzPoint3& intensity)
    {
        FZ_ASSERT(intensity.x >= 0 && intensity.x <= 1.0, "X (red) component is out of bounds [0, 1].");
        FZ_ASSERT(intensity.y >= 0 && intensity.y <= 1.0, "Y (green) component is out of bounds [0, 1].");
        FZ_ASSERT(intensity.z >= 0 && intensity.z <= 1.0, "Z (blue) component is out of bounds [0, 1].");
        FZ_ASSERT((intensity.x+intensity.y+intensity.z) <= 1, "Summation of intensity components must be less of equal to 1.0.");
        
        //if(m_intensity != intensity)
        {
            p_glprogram->setUniform3f("u_intensity"_hash, intensity.x, intensity.y, intensity.z);
            m_intensity = intensity;
        }
    }
    
    
    const fzPoint3& FilterGrayscale::getIntensity() const
    {
        return m_intensity;
    }
  
    
#pragma mark - Filter XRAy ( brightness/contrast )
    
    FilterXRay::FilterXRay(fzFloat quality)
    : Filter(GLShader(effect_xray_frag, GL_FRAGMENT_SHADER), quality)
    , m_intensity()
    {        
        setIntensity(fzPoint3(0.30f, 0.59f, 0.11f));
    }
    
    
    void FilterXRay::setIntensity(const fzPoint3& intensity)
    {
        FZ_ASSERT(intensity.x >= 0 && intensity.x <= 1.0, "X (red) component is out of bounds [0, 1].");
        FZ_ASSERT(intensity.y >= 0 && intensity.y <= 1.0, "Y (green) component is out of bounds [0, 1].");
        FZ_ASSERT(intensity.z >= 0 && intensity.z <= 1.0, "Z (blue) component is out of bounds [0, 1].");
        FZ_ASSERT((intensity.x+intensity.y+intensity.z) <= 1, "Summation of intensity components must be less of equal to 1.0.");
        
        //if(m_intensity != intensity)
        {
            p_glprogram->setUniform3f("u_intensity"_hash, intensity.x, intensity.y, intensity.z);
            m_intensity = intensity;
        }
    }
    
    
    const fzPoint3& FilterXRay::getIntensity() const
    {
        return m_intensity;
    }
    
    
    
#pragma mark - Filter Color ( brightness/contrast )
    
    FilterToon::FilterToon(fzFloat quality)
    : Filter(GLShader(effect_toon_frag, GL_FRAGMENT_SHADER), quality)
    , m_factor(-1)
    {        
        setFactor(4);
    }
    
    
    void FilterToon::setFactor(fzFloat factor)
    {
        FZ_ASSERT(factor >= 0 && factor <= 255, "Factor is out of bounds [0, 255].");
        if(m_factor != factor) {
            
            p_glprogram->setUniform1f("u_factor"_hash, factor);
            m_factor = factor;
        }
    }
    
    
    fzFloat FilterToon::getFactor() const
    {
        return m_factor;
    }
    

    
#pragma mark - Filter Invert color

    FilterInvert::FilterInvert(fzFloat quality)
    : Filter(GLShader(effect_invert_frag, GL_FRAGMENT_SHADER), quality)
    { }
}
#endif

