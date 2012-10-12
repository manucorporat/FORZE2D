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

#include "FZLightSystem.h"
#include "FZSpriteBatch.h"
#include "FZTexture2D.h"
#include "FZPrimitives.h"
#include "FZGLState.h"
#include "FZDirector.h"
#include "FZShaderCache.h"
#include "FZTextureCache.h"
#include "FZMath.h"
#include "FZMacros.h"
#include "matrixStack.h"


using namespace STD;

namespace FORZE {
    
#pragma mark - Helper lighting methods
    
    
    inline fzPoint convertPoint(const fzPoint& current, const fzPoint& referencePoint)
    {
        fzPoint newPoint = Director::Instance().unnormalizedCoord(current);
        return newPoint - referencePoint;
    }
    
    
    inline fzInt previousVertex(fzInt current, fzInt total)
    {
        FZ_ASSERT(current >= 0 && total > 0, "Invalid indexes");
        FZ_ASSERT(current < total, "Index can not be bigger than total");
        return (current == 0) ? total-1 : current-1;
    }
    
    
    inline fzInt nextVertex(fzInt current, fzInt total)
    {
        FZ_ASSERT(current >= 0 && total > 0, "Invalid indexes");
        FZ_ASSERT(current < total, "Index can not be bigger than total");
        return (current == (total-1)) ? 0 : current+1;
    }
    
    
    fzFloat fastAngle(fzVec2 v1, fzVec2 v2)
    {
        fzFloat dot = v1.dot(v2); // dot product
        v1.x *= v1.x; v1.y *= v1.y;
        v2.x *= v2.x; v2.y *= v2.y;
        
        fzFloat numerator = copysignf(dot * dot, dot);
        fzFloat denominator = (v1.x+v1.y)*(v2.x+v2.y);
        return numerator / denominator;

        return dot;
    }
    
    
    void generateShape(const fzVec2 *input, fzVec2 *output, fzUInt nuVertices)
    {
        output[0] = input[0];
        const fzUInt iterations = nuVertices >> 1;
        
        for(fzUInt i = 1; i <= iterations; ++i) {
            const fzUInt idx = i << 1;
            output[idx-1] = input[i];
            output[idx] = input[nuVertices-i];
        }
    }
    
    
#pragma mark - Lighting system

    LightSystem::LightSystem(Texture2D *texture, Node *batch)
    : m_grabber()
    , p_batch(NULL)
    , p_texture(NULL)
    {
        FZ_ASSERT(batch != NULL, "Sprite batch cannot be NULL");
        FZ_ASSERT(texture != NULL, "Texture cannot be NULL");

        setTexture(texture);
        setBatch(batch);
        
#if FZ_GL_SHADERS
        // SHADER
        setGLProgram(kFZShader_mat_TEX);
#endif
    }
    
    
    LightSystem::LightSystem(const string& filename, Node *batch)
    : LightSystem(TextureCache::Instance().addImage(filename.c_str()), batch)
    { }
    
    
    LightSystem::~LightSystem()
    {
        setTexture(NULL);
        setBatch(NULL);
    }
    
    
    void LightSystem::insertChild(Node* node)
    {
        FZ_ASSERT( dynamic_cast<Light*>(node), "LightSystem's children must be Lights");

        Light *sprite = static_cast<Light*>(node);        

        Texture2D *texture = m_grabber.getTexture();
        if(texture) {
            sprite->setTexture(texture);
            sprite->setTextureRect(fzRect(FZPointZero, texture->getContentSize()));
        }
        
        Layer::insertChild(sprite);
    }
    
    
    void LightSystem::setTexture(Texture2D* texture)
    {
        FZRETAIN_TEMPLATE(texture, p_texture);
        
        if(texture)
            texture->setAliasTexParameters();

        makeDirty(kFZDirty_opengl);
    }
    
    
    Texture2D* LightSystem::getTexture() const
    {
        return p_texture;
    }
    
    
    void LightSystem::setBatch(Node *batch)
    {
        FZRETAIN_TEMPLATE(batch, p_batch);
    }

    
    void LightSystem::updateStuff()
    {
        Node::updateStuff();
        
        // UPDATE OPENGL STUFF
        if(m_dirtyFlags & kFZDirty_opengl) {
            
            const fzSize& size = p_texture->getContentSize();
            const fzSize& pixels = p_texture->getContentSizeInPixels();
            fzUInt wide = p_texture->getPixelsWide();
            fzUInt high = p_texture->getPixelsHigh();

            
            // CALCULATE VERTICES
            fzSize size_2 = size / 2;
            m_quad.bl.vertex = fzVec2(-size_2.width, -size_2.height);
            m_quad.br.vertex = fzVec2(size_2.width, -size_2.height);
            m_quad.tl.vertex = fzVec2(-size_2.width, size_2.height);
            m_quad.tr.vertex = fzVec2(size_2.width, size_2.height);
            
            m_quad.bl.texCoord = fzVec2(0, 0);
            m_quad.br.texCoord = fzVec2(pixels.width / wide, 0);
            m_quad.tl.texCoord = fzVec2(0, pixels.height / high);
            m_quad.tr.texCoord = fzVec2(pixels.width / wide, pixels.height / high);
            
            
            // CONFIG FBO GRABBER
            fzFloat quality = (fzFloat)p_texture->getFactor() / (fzFloat)Director::Instance().getResourcesFactor();
            m_grabber.config(kFZTextureFormat_RGBA8888, size, fzPoint(0.5f, 0.5f), quality);

            
            // UPDATE SPRITES
            Texture2D *texture = m_grabber.getTexture();
            Light *light;
            FZ_LIST_FOREACH(m_children, light) {
                light->setTexture(texture);
                light->setTextureRect(fzRect(FZPointZero, texture->getContentSize()));
            }
        }
    }

    
    void LightSystem::render(char dirtyFlags)
    {
        Light *light;
        Sprite *sprite;
        fzUInt shapeSize;
        fzUInt polySize = 1;
        
        register fzVec2 shape[10];
        register fzVec2 finalShape[8];
        float *shapeFloats = reinterpret_cast<float*>(shape);
        
        
        // ITERATE LIGHTS
        FZ_LIST_FOREACH(m_children, light)
        {  
            const fzPoint& reference = light->getPosition();

            fzFloat radius = light->getContentSize().width/2;
            fzFloat radius_2 = radius * radius;
            
            // FBO
            m_grabber.begin();
            
#if !FZ_GL_SHADERS
            glLoadMatrixf(fzMS_getMatrix());
#endif

            
            // DRAW LIGHT TEXTURE
            drawTexture();
            
            
            FZ_LIST_FOREACH(p_batch->getChildren(), sprite)
            {
                FZ_ASSERT(dynamic_cast<Sprite*>(sprite), "All children must be sprites");
                
                if(reference.distanceSquared(sprite->getPosition()) > radius_2)
                    continue;
                
                shapeSize = 4;
                sprite->getVertices(shapeFloats);
                for(fzUInt i = 0; i < shapeSize; ++i)
                    shape[i] = convertPoint(shape[i], reference);


                fzUInt i1 = 0, i2 = 1;
                fzPoint dv1;
                fzPoint dv2;
                
                
                // GET DETERMINANT VERTICES
                fzFloat maxAngle = 1000000;
                {
                    for(fzUInt i = 0; i < (shapeSize-1); ++i)
                    {
                        for(fzUInt w = i+1; w < shapeSize; ++w)
                        {
                            fzFloat angle = fastAngle(shape[i], shape[w]);
                            if(angle < maxAngle) {
                                maxAngle = angle;
                                dv1 = shape[i]; i1 = i;
                                dv2 = shape[w]; i2 = w;
                            }
                        }
                    }
                }
                
                
                // GET INTERSECCION POINTS
                { 
                    dv1.normalize();
                    dv2.normalize();

                    fzPoint mv(dv1);
                    mv += dv2;
                    mv.normalize();
                    mv *= radius;
                    
                    fzPoint mp = mv;
                    mv = mv.getPerp();
                    
                    fzFloat num = dv1.x*mp.y - dv1.y*mp.x;
                    fzFloat fi = num/(mv.x*dv1.y-mv.y*dv1.x);
                    fzPoint add = mv * fi;
                    finalShape[0] = fzVec2( mp - add );
                    finalShape[1] = fzVec2( mp + add );
                }
                polySize = 2;
                
                
                // GET FACE
                {
                    fzInt prev = previousVertex(i1, shapeSize);
                    fzInt next = nextVertex(i1, shapeSize);
                    
                    fzFloat anglePrev = fastAngle(dv1, shape[prev]-shape[i1]);
                    fzFloat angleNext = fastAngle(dv1, shape[next]-shape[i1]);
                    
                    if(angleNext < anglePrev) {
                        // NEXT IS THE FACE
                        for(fzUInt i = i1; i <= i2; ++i, ++polySize)
                            finalShape[polySize] = shape[i];
                    }
                    else{
                        // PREV IS THE FACE
                        fzUInt faceCount = shapeSize-i2+i1+1;
                        fzUInt current = i1;
                        for(fzUInt i = 0; i < faceCount; ++i, ++polySize) {
                            finalShape[polySize] = shape[current];
                            current = previousVertex(current, shapeSize);
                        }
                    }
                }
                
                
                // ENSAMBLING POLYGON
                generateShape(finalShape, shape, polySize);
                
                fzGLBlendFunc(light->getShadowBlendFunc());

                fzColor4F color(light->getShadowColor());
                color.a = 0;

                fzGLColor(color);
                fzDrawShape(shape, polySize);
            }
            m_grabber.end();

            
            light->makeDirty(dirtyFlags);
            light->internalVisit();
        }
    }
    
    
    void LightSystem::drawTexture()
    {    
        glDisable(GL_BLEND);
        
        // Bind texture
        fzGLSetMode(kFZGLMode_TextureNoColor);
        p_texture->bind();
        
#if FZ_GL_SHADERS
        p_glprogram->use();
        FZ_SAFE_APPLY_MATRIX(p_glprogram);
                
        glVertexAttribPointer(kFZAttribPosition, 2, GL_FLOAT, GL_FALSE, sizeof(_fzT2_V2), &m_quad.bl.vertex);
        glVertexAttribPointer(kFZAttribTexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(_fzT2_V2), &m_quad.bl.texCoord);
#else
        
        glVertexPointer(2, GL_FLOAT, sizeof(_fzT2_V2), &m_quad.bl.vertex);	
        glTexCoordPointer(2, GL_FLOAT, sizeof(_fzT2_V2), &m_quad.bl.texCoord);
#endif
        
        // Rendering
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        glEnable(GL_BLEND);
    }
    
    
#pragma mark - Light template
    
    Light::Light()
    : m_shadowColor(fzBLACK)
    , m_shadowBlend(GL_DST_COLOR, GL_ZERO)
    {
        // self rendering
        useSelfRender();
        
        // default blending
        setBlendFunc(fzBlendFunc(GL_SRC_ALPHA, GL_ONE));
    }
}
