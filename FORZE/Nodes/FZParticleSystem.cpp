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


// ideas taken from:
//	 . The ocean spray in your face [Jeff Lander]
//		http://www.double.co.nz/dust/col0798.pdf
//	 . Building an Advanced Particle System [John van der Burg]
//		http://www.gamasutra.com/features/20000623/vanderburg_01.htm
//   . LOVE game engine
//		http://love2d.org/
//
//
// Radius mode support, from 71 squared
//		http://particledesigner.71squared.com/
//
// IMPORTANT: Particle Designer is supported by FORZE2D, but
// 'Radius Mode' in Particle Designer uses a fixed emit rate of 30 hz. Since that can't be guarateed in FORZE2D,
//  FORZE2D uses a another approach, but the results are almost identical. 
//

#include "FZParticleSystem.h"
#include "FZMacros.h"
#include "FZTexture2D.h"
#include "FZTextureCache.h"
#include "FZMath.h"
#include "FZData.h"


//using namespace STD;

namespace FORZE {
    
    /*
     ParticleSystem::ParticleSystem(const string& plistFile)
     {
     NSString *path = [CCFileUtils fullPathFromRelativePath:plistFile];
     NSDictionary *dict = [NSDictionary dictionaryWithContentsOfFile:path];
     
     FZ_ASSERT( dict != nil, @"Particles: file not found");
     return [self initWithDictionary:dict];
     }
     
     ParticleSystem::ParticleSystem(const map& dictionary)
     {
     NSUInteger maxParticles = [[dictionary valueForKey:@"maxParticles"] intValue];
     // self, not super
     
     // angle
     angle = [[dictionary valueForKey:@"angle"] floatValue];
     angleVar = [[dictionary valueForKey:@"angleVariance"] floatValue];
     
     // duration
     duration = [[dictionary valueForKey:@"duration"] floatValue];
     
     // blend function 
     blendFunc_.src = [[dictionary valueForKey:@"blendFuncSource"] intValue];
     blendFunc_.dst = [[dictionary valueForKey:@"blendFuncDestination"] intValue];
     
     // color
     float r,g,b,a;
     
     r = [[dictionary valueForKey:@"startColorRed"] floatValue];
     g = [[dictionary valueForKey:@"startColorGreen"] floatValue];
     b = [[dictionary valueForKey:@"startColorBlue"] floatValue];
     a = [[dictionary valueForKey:@"startColorAlpha"] floatValue];
     startColor = (ccColor4F) {r,g,b,a};
     
     r = [[dictionary valueForKey:@"startColorVarianceRed"] floatValue];
     g = [[dictionary valueForKey:@"startColorVarianceGreen"] floatValue];
     b = [[dictionary valueForKey:@"startColorVarianceBlue"] floatValue];
     a = [[dictionary valueForKey:@"startColorVarianceAlpha"] floatValue];
     startColorVar = (ccColor4F) {r,g,b,a};
     
     r = [[dictionary valueForKey:@"finishColorRed"] floatValue];
     g = [[dictionary valueForKey:@"finishColorGreen"] floatValue];
     b = [[dictionary valueForKey:@"finishColorBlue"] floatValue];
     a = [[dictionary valueForKey:@"finishColorAlpha"] floatValue];
     endColor = (ccColor4F) {r,g,b,a};
     
     r = [[dictionary valueForKey:@"finishColorVarianceRed"] floatValue];
     g = [[dictionary valueForKey:@"finishColorVarianceGreen"] floatValue];
     b = [[dictionary valueForKey:@"finishColorVarianceBlue"] floatValue];
     a = [[dictionary valueForKey:@"finishColorVarianceAlpha"] floatValue];
     endColorVar = (ccColor4F) {r,g,b,a};
     
     // particle size
     startSize = [[dictionary valueForKey:@"startParticleSize"] floatValue];
     startSizeVar = [[dictionary valueForKey:@"startParticleSizeVariance"] floatValue];
     endSize = [[dictionary valueForKey:@"finishParticleSize"] floatValue];
     endSizeVar = [[dictionary valueForKey:@"finishParticleSizeVariance"] floatValue];
     
     
     // position
     float x = [[dictionary valueForKey:@"sourcePositionx"] floatValue];
     float y = [[dictionary valueForKey:@"sourcePositiony"] floatValue];
     self.position = ccp(x,y);
     posVar.x = [[dictionary valueForKey:@"sourcePositionVariancex"] floatValue];
     posVar.y = [[dictionary valueForKey:@"sourcePositionVariancey"] floatValue];
     
     
     // Spinning
     startSpin = [[dictionary valueForKey:@"rotationStart"] floatValue];
     startSpinVar = [[dictionary valueForKey:@"rotationStartVariance"] floatValue];
     endSpin = [[dictionary valueForKey:@"rotationEnd"] floatValue];
     endSpinVar = [[dictionary valueForKey:@"rotationEndVariance"] floatValue];
     
     emitterMode_ = [[dictionary valueForKey:@"emitterType"] intValue];
     
     // Mode A: Gravity + tangential accel + radial accel
     if( emitterMode_ == kCCParticleModeGravity ) {
     // gravity
     mode.A.gravity.x = [[dictionary valueForKey:@"gravityx"] floatValue];
     mode.A.gravity.y = [[dictionary valueForKey:@"gravityy"] floatValue];
     
     //
     // speed
     mode.A.speed = [[dictionary valueForKey:@"speed"] floatValue];
     mode.A.speedVar = [[dictionary valueForKey:@"speedVariance"] floatValue];
     
     // radial acceleration			
     NSString *tmp = [dictionary valueForKey:@"radialAcceleration"];
     mode.A.radialAccel = tmp ? [tmp floatValue] : 0;
     
     tmp = [dictionary valueForKey:@"radialAccelVariance"];
     mode.A.radialAccelVar = tmp ? [tmp floatValue] : 0;
     
     // tangential acceleration
     tmp = [dictionary valueForKey:@"tangentialAcceleration"];
     mode.A.tangentialAccel = tmp ? [tmp floatValue] : 0;
     
     tmp = [dictionary valueForKey:@"tangentialAccelVariance"];
     mode.A.tangentialAccelVar = tmp ? [tmp floatValue] : 0;
     }
     
     
     // or Mode B: radius movement
     else if( emitterMode_ == kCCParticleModeRadius ) {
     float maxRadius = [[dictionary valueForKey:@"maxRadius"] floatValue];
     float maxRadiusVar = [[dictionary valueForKey:@"maxRadiusVariance"] floatValue];
     float minRadius = [[dictionary valueForKey:@"minRadius"] floatValue];
     
     mode.B.startRadius = maxRadius;
     mode.B.startRadiusVar = maxRadiusVar;
     mode.B.endRadius = minRadius;
     mode.B.endRadiusVar = 0;
     mode.B.rotatePerSecond = [[dictionary valueForKey:@"rotatePerSecond"] floatValue];
     mode.B.rotatePerSecondVar = [[dictionary valueForKey:@"rotatePerSecondVariance"] floatValue];
     
     } else {
     FZ_ASSERT( NO, @"Invalid emitterType in config file");
     }
     
     // life span
     life = [[dictionary valueForKey:@"particleLifespan"] floatValue];
     lifeVar = [[dictionary valueForKey:@"particleLifespanVariance"] floatValue];				
     
     // emission Rate
     emissionRate = totalParticles/life;
     
     // texture		
     // Try to get the texture from the cache
     NSString *textureName = [dictionary valueForKey:@"textureFileName"];
     
     CCTexture2D *tex = [[CCTextureCache sharedTextureCache] addImage:textureName];
     
     if( tex )
     self.texture = tex;
     
     else {
     
     NSString *textureData = [dictionary valueForKey:@"textureImageData"];
     FZ_ASSERT( textureData, @"CCParticleSystem: Couldn't load texture");
     
     // if it fails, try to get it from the base64-gzipped data			
     unsigned char *buffer = NULL;
     int len = base64Decode((unsigned char*)[textureData UTF8String], (unsigned int)[textureData length], &buffer);
     FZ_ASSERT( buffer != NULL, @"CCParticleSystem: error decoding textureImageData");
     
     unsigned char *deflated = NULL;
     NSUInteger deflatedLen = ccInflateMemory(buffer, len, &deflated);
     free( buffer );
     
     FZ_ASSERT( deflated != NULL, @"CCParticleSystem: error ungzipping textureImageData");
     NSData *data = [[NSData alloc] initWithBytes:deflated length:deflatedLen];
     
     #ifdef __IPHONE_OS_VERSION_MAX_ALLOWED
     UIImage *image = [[UIImage alloc] initWithData:data];
     #elif defined(__MAC_OS_X_VERSION_MAX_ALLOWED)
     NSBitmapImageRep *image = [[NSBitmapImageRep alloc] initWithData:data];
     #endif
     
     free(deflated); deflated = NULL;
     
     self.texture = [[CCTextureCache sharedTextureCache] addCGImage:[image CGImage] forKey:textureName];
     [data release];
     [image release];
     }
     
     FZ_ASSERT( [self texture] != NULL, @"CCParticleSystem: error loading the texture");
     
     }
     */
    
    ParticleSystem::ParticleSystem(fzUInt number, Texture2D *texture)
    : m_totalParticles(number)
    , m_particleCount(0)
    , m_particleIdx(0)
    , m_duration(0)
    , m_elapsed(0)
    , m_angle(0)
    , m_angleVar(0)
    , m_startSize(0)
    , m_startSizeVar(0)
    , m_endSize(kFZParticleStartSizeEqualToEndSize)
    , m_endSizeVar(0)
    , m_startSpin(0)
    , m_startSpinVar(0)
    , m_endSpin(0)
    , m_endSpinVar(0)
    , m_life(1)
    , m_lifeVar(0)
    , m_isActive(true)
    , m_autoRemoveOnFinish(false)
    , m_blendFunc()
    , p_texture(NULL)
    , p_particles(NULL)
    , m_startColor(fzWHITE)
    , m_startColorVar(fzBLACK)
    , m_endColor(fzWHITE)
    , m_endColorVar(fzBLACK)
    , m_positionType(kFZPositionTypeFree)
    , m_emitterMode(kFZParticleModeGravity)
    {
        memset(&mode, 0, sizeof(mode));
        
        //FZ_ASSERT(texture != NULL, "Texture cannot be NULL");
        setTexture(texture);

        p_particles = new fzParticle[m_totalParticles];
        schedule();
    }
    
    
    ParticleSystem::~ParticleSystem()
    {
        delete [] p_particles;
    }
    
    
    bool ParticleSystem::addParticle()
    {
        if( isFull() )
            return false;
        
        initParticle(p_particles[m_particleCount]);
        ++m_particleCount;
        
        return true;
    }
    
    
    void ParticleSystem::initParticle(fzParticle& particle)
    {
        // time to live
        particle.timeToLive = m_life + m_lifeVar * FZ_RANDOM_MINUS1_1();
        if(particle.timeToLive < 0) particle.timeToLive = 0;
        
        // position
        particle.pos = m_sourcePosition + m_posVar * FZ_RANDOM_MINUS1_1();
        
        // color
        fzColor4F start(m_startColor.r + m_startColorVar.r * FZ_RANDOM_MINUS1_1(),
                      m_startColor.g + m_startColorVar.g * FZ_RANDOM_MINUS1_1(),
                      m_startColor.b + m_startColorVar.b * FZ_RANDOM_MINUS1_1(),
                      m_startColor.a + m_startColorVar.a * FZ_RANDOM_MINUS1_1());
        
        fzColor4F end(m_endColor.r + m_endColorVar.r * FZ_RANDOM_MINUS1_1(),
                    m_endColor.g + m_endColorVar.g * FZ_RANDOM_MINUS1_1(),
                    m_endColor.b + m_endColorVar.b * FZ_RANDOM_MINUS1_1(),
                    m_endColor.a + m_endColorVar.a * FZ_RANDOM_MINUS1_1());
        
        particle.color = start;
        particle.deltaColor = (end - start) * (1 / particle.timeToLive);
        
        // size
        fzFloat startS = m_startSize + m_startSizeVar * FZ_RANDOM_MINUS1_1();
        if(startS < 0) startS = 0;
        particle.size = startS;
        
        if( m_endSize == kFZParticleStartSizeEqualToEndSize )
            particle.deltaSize = 0;
        
        else {
            fzFloat endS = m_endSize + m_endSizeVar * FZ_RANDOM_MINUS1_1();
            if(endS < 0) endS = 0;

            particle.deltaSize = (endS - startS) / particle.timeToLive;
        }
        
        // rotation
        fzFloat startA = m_startSpin + m_startSpinVar * FZ_RANDOM_MINUS1_1();
        fzFloat endA = m_endSpin + m_endSpinVar * FZ_RANDOM_MINUS1_1();
        particle.rotation = startA;
        particle.deltaRotation = (endA - startA) / particle.timeToLive;
        
        
        // direction
        fzFloat a = FZ_DEGREES_TO_RADIANS( m_angle + m_angleVar * FZ_RANDOM_MINUS1_1() );	
        
        // Mode Gravity: A
        if( m_emitterMode == kFZParticleModeGravity ) {
            
            fzFloat s = mode.A.speed + mode.A.speedVar * FZ_RANDOM_MINUS1_1();
            
            // direction
            particle.mode.A.dir.x = fzMath_cos(a) * s;
            particle.mode.A.dir.y = fzMath_sin(a) * s;
            
            // radial accel
            particle.mode.A.radialAccel = mode.A.radialAccel + mode.A.radialAccelVar * FZ_RANDOM_MINUS1_1();
            
            // tangential accel
            particle.mode.A.tangentialAccel = mode.A.tangentialAccel + mode.A.tangentialAccelVar * FZ_RANDOM_MINUS1_1();
        }
        
        // Mode Radius: B
        else {
            // Set the default diameter of the particle from the source position
            fzFloat startRadius = mode.B.startRadius + mode.B.startRadiusVar * FZ_RANDOM_MINUS1_1();
            fzFloat endRadius = mode.B.endRadius + mode.B.endRadiusVar * FZ_RANDOM_MINUS1_1();
            
            particle.mode.B.radius = startRadius;
            
            particle.mode.B.deltaRadius = (mode.B.endRadius == kFZParticleStartRadiusEqualToEndRadius) ?
            0 : ((endRadius- startRadius) / particle.timeToLive);
            
            
            particle.mode.B.angle = a;
            particle.mode.B.degreesPerSecond = FZ_DEGREES_TO_RADIANS(mode.B.rotatePerSecond + mode.B.rotatePerSecondVar * FZ_RANDOM_MINUS1_1());
        }	
    }
    
    
    void ParticleSystem::stopSystem()
    {
        m_isActive = false;
        m_elapsed = m_duration;
        m_emitCounter = 0; 
    }
    
    
    void ParticleSystem::resetSystem()
    {
        m_isActive = true;
        m_elapsed = 0;
        for(m_particleIdx = 0; m_particleIdx < m_particleCount; ++m_particleIdx)
            p_particles[m_particleIdx].timeToLive = 0;
    }
    
    
    bool ParticleSystem::isFull() const
    {
        return (m_particleCount == m_totalParticles);
    }
    

    void ParticleSystem::update(fzFloat dt)
    {
        if( m_isActive && m_emissionRate ) {
            fzFloat rate = 1.0f / m_emissionRate;
            m_emitCounter += dt;
            while( m_particleCount < m_totalParticles && m_emitCounter > rate ) {
                addParticle();
                m_emitCounter -= rate;
            }
            
            m_elapsed += dt;
            if(m_duration != -1 && m_duration < m_elapsed)
                stopSystem();
        }
        
        
        m_particleIdx = 0;
        
        while( m_particleIdx < m_particleCount )
        {
            fzParticle& p = p_particles[m_particleIdx];
            
            // life
            p.timeToLive -= dt;
            
            if( p.timeToLive > 0 ) {
                
                // Mode A
                if( m_emitterMode == kFZParticleModeGravity ) {
                    
                    fzPoint tmp(p.pos);
                    if(tmp != FZPointZero)
                    {
                        // calculate tangential
                        fzPoint tangential(tmp.getPerp());
                        tangential.normalize();
                        tangential *= p.mode.A.tangentialAccel;
                        
                        // radial acceleration
                        tmp *= p.mode.A.radialAccel;

                        // radial + tangential
                        tmp += tangential;
                    }
                    
                    // (gravity + dir + radial + tangential) * dt
                    tmp += mode.A.gravity;
                    tmp += p.mode.A.dir;

                    p.pos += tmp * dt;
                }
                
                // Mode B
                else {				
                    // Update the angle and radius of the particle.
                    p.mode.B.angle += p.mode.B.degreesPerSecond * dt;
                    p.mode.B.radius += p.mode.B.deltaRadius * dt;
                    
                    p.pos.x = -fzMath_cos(p.mode.B.angle) * p.mode.B.radius;
                    p.pos.y = -fzMath_sin(p.mode.B.angle) * p.mode.B.radius;
                }
                
                // color
                p.color += p.deltaColor * dt;
                
                // size
                p.size += p.deltaSize * dt;
                p.size = p.size < 0 ? 0 : p.size;
                
                // angle
                p.rotation += p.deltaRotation * dt;
                
                
                // update values in quad
                updateQuadWithParticle(p);
                
                // update particle counter
                ++m_particleIdx;
                
            } else {
                // life < 0
                --m_particleCount;

                if( m_particleIdx != m_particleCount )
                    memmove(&p_particles[m_particleIdx], &p_particles[m_particleCount], sizeof(fzParticle));
                
                if( m_particleCount == 0 && m_autoRemoveOnFinish ) {
                    unschedule();
                    removeFromParent(true);
                    return;
                }
            }
        }
        
        if(m_particleCount)
        makeDirty(0);
        
#if FZ_VBO_STREAMING
        postStep();
#endif
    }
    
    
    void ParticleSystem::setGravity(const fzPoint& g)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        mode.A.gravity = g;
    }
    
    
    void ParticleSystem::setSpeed(fzFloat s)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        mode.A.speed = s;
    }
    
    
    void ParticleSystem::setSpeedVar(fzFloat s)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        mode.A.speedVar = s;
    }
    
    
    void ParticleSystem::setTangentialAccel(fzFloat t)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity");
        mode.A.tangentialAccel = t;
    }
    
    
    void ParticleSystem::setTangentialAccelVar(fzFloat t)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        mode.A.tangentialAccelVar = t;
    }
    
    
    void ParticleSystem::setRadialAccel(fzFloat r)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        mode.A.radialAccel = r;
    }
    
    
    void ParticleSystem::setRadialAccelVar(fzFloat r)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        mode.A.radialAccelVar = r;
    }
    
    
    void ParticleSystem::setStartRadius(fzFloat r)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        mode.B.startRadius = r;
    }
    
    
    void ParticleSystem::setStartRadiusVar(fzFloat r)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        mode.B.startRadiusVar = r;
    }
    
    
    void ParticleSystem::setEndRadius(fzFloat r)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        mode.B.endRadius = r;
    }
    
    
    void ParticleSystem::setEndRadiusVar(fzFloat r)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        mode.B.endRadiusVar = r;
    }
    
    
    void ParticleSystem::setRotatePerSecond(fzFloat r)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        mode.B.rotatePerSecond = r;
    }
    
    
    void ParticleSystem::setRotatePerSecondVar(fzFloat r)
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        mode.B.rotatePerSecondVar = r;
    }
       
    
    void ParticleSystem::setTexture(Texture2D *texture)
    {
        FZRETAIN_TEMPLATE(texture, p_texture);
    }
    
    Texture2D* ParticleSystem::getTexture() const
    {
        return p_texture;
    }
    
    
    void ParticleSystem::setBlendFunc(const fzBlendFunc& b)
    {
        m_blendFunc = b;
    }
    
    const fzBlendFunc& ParticleSystem::getBlendFunc() const
    {
        return m_blendFunc;
    }
    
    
    const fzPoint& ParticleSystem::getGravity() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        return FZPointZero;
        //return mode.A.gravity;
    }
    
    
    fzFloat ParticleSystem::getSpeed() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        return mode.A.speed;
    }
    
    
    fzFloat ParticleSystem::getSpeedVar() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity");
        return mode.A.speedVar;
    }
    
    
    fzFloat ParticleSystem::getTangentialAccel() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        return mode.A.tangentialAccel;
    }
    
    
    fzFloat ParticleSystem::getTangentialAccelVar() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        return mode.A.tangentialAccelVar;
    }
    
    
    fzFloat ParticleSystem::getRadialAccel() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity");
        return mode.A.radialAccel;
    }
    
    
    fzFloat ParticleSystem::getRadialAccelVar() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        return mode.A.radialAccelVar;
    }
    
    
    fzFloat ParticleSystem::getStartRadius() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        return mode.B.startRadius;
    }
    
    
    fzFloat ParticleSystem::getStartRadiusVar() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        return mode.B.startRadiusVar;
    }
    
    
    fzFloat ParticleSystem::getEndRadius() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        return mode.B.endRadius;
    }
    
    
    fzFloat ParticleSystem::getEndRadiusVar() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        return mode.B.endRadiusVar;
    }
    
    
    fzFloat ParticleSystem::getRotatePerSecond() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        return mode.B.rotatePerSecond;
    }
    
    
    fzFloat ParticleSystem::getRotatePerSecondVar() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeRadius, "Particle Mode should be Radius.");
        return mode.B.rotatePerSecondVar;
    }
}
