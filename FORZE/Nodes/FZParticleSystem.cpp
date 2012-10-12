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
// IMPORTANT: Particle Designer is supported by cocos2d, but
// 'Radius Mode' in Particle Designer uses a fixed emit rate of 30 hz. Since that can't be guarateed in cocos2d,
//  cocos2d uses a another approach, but the results are almost identical. 
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
    : totalParticles_(number)
    , particleCount_(0)
    , particleIdx_(0)
    , isActive_(true)
    , autoRemoveOnFinish_(false)
    , blendFunc_()
    , texture_(NULL)
    , positionType_(kFZPositionTypeFree)
    , emitterMode_(kFZParticleModeGravity)
    {
        FZ_ASSERT(texture != NULL, "Texture cannot be NULL");
        setTexture(texture);

        particles_ = new tFZParticle[totalParticles_];
        schedule(); // schedule update()
    }
    
    
    ParticleSystem::~ParticleSystem()
    {
        delete [] particles_;
    }
    
    
    bool ParticleSystem::addParticle()
    {
        if( isFull() )
            return false;
        
        initParticle(particles_[particleCount_]);
        ++particleCount_;
        
        return true;
    }
    
    
    void ParticleSystem::initParticle(tFZParticle& particle)
    {
        // timeToLive
        particle.timeToLive = life_ + lifeVar_ * FZ_RANDOM_MINUS1_1();
        if(particle.timeToLive < 0) particle.timeToLive = 0;
        
        // position
        particle.pos = sourcePosition_ + posVar_ * FZ_RANDOM_MINUS1_1();
        
        // color
        fzColor4F start(startColor_.r + startColorVar_.r * FZ_RANDOM_MINUS1_1(),
                      startColor_.g + startColorVar_.g * FZ_RANDOM_MINUS1_1(),
                      startColor_.b + startColorVar_.b * FZ_RANDOM_MINUS1_1(),
                      startColor_.a + startColorVar_.a * FZ_RANDOM_MINUS1_1());
        
        fzColor4F end(endColor_.r + endColorVar_.r * FZ_RANDOM_MINUS1_1(),
                    endColor_.g + endColorVar_.g * FZ_RANDOM_MINUS1_1(),
                    endColor_.b + endColorVar_.b * FZ_RANDOM_MINUS1_1(),
                    endColor_.a + endColorVar_.a * FZ_RANDOM_MINUS1_1());
        
        particle.color = start;
        particle.deltaColor = (end - start) * (1 / particle.timeToLive);
        
        // size
        fzFloat startS = startSize_ + startSizeVar_ * FZ_RANDOM_MINUS1_1();
        if(startS < 0) startS = 0;
        particle.size = startS;
        
        if( endSize_ == kFZParticleStartSizeEqualToEndSize )
            particle.deltaSize = 0;
        
        else {
            fzFloat endS = endSize_ + endSizeVar_ * FZ_RANDOM_MINUS1_1();
            if(endS < 0) endS = 0;

            particle.deltaSize = (endS - startS) / particle.timeToLive;
        }
        
        // rotation
        fzFloat startA = startSpin_ + startSpinVar_ * FZ_RANDOM_MINUS1_1();
        fzFloat endA = endSpin_ + endSpinVar_ * FZ_RANDOM_MINUS1_1();
        particle.rotation = startA;
        particle.deltaRotation = (endA - startA) / particle.timeToLive;
        
        
        // position
        if( positionType_ == kFZPositionTypeFree )
            particle.startPos = convertToWorldSpace(FZPointZero);
        
        else if( positionType_ == kFZPositionTypeRelative )
            particle.startPos = m_position;
        
        
        // direction
        fzFloat a = FZ_DEGREES_TO_RADIANS( angle_ + angleVar_ * FZ_RANDOM_MINUS1_1() );	
        
        // Mode Gravity: A
        if( emitterMode_ == kFZParticleModeGravity ) {
            
            fzFloat s = mode.A.speed + mode.A.speedVar * FZ_RANDOM_MINUS1_1();
            
            // direction
            particle.mode.A.dirX = fzMath_sin(a) * s;
            particle.mode.A.dirY = fzMath_cos(a) * s;
            
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
        isActive_ = false;
        elapsed_ = duration_;
        emitCounter_ = 0; 
    }
    
    
    void ParticleSystem::resetSystem()
    {
        isActive_ = true;
        elapsed_ = 0;
        for(particleIdx_ = 0; particleIdx_ < particleCount_; ++particleIdx_)
            particles_[particleIdx_].timeToLive = 0;
    }
    
    
    bool ParticleSystem::isFull() const
    {
        return (particleCount_ == totalParticles_);
    }
    

    void ParticleSystem::update(fzFloat dt)
    {
        if( isActive_ && emissionRate_ ) {
            fzFloat rate = 1.0f / emissionRate_;
            emitCounter_ += dt;
            while( particleCount_ < totalParticles_ && emitCounter_ > rate ) {
                addParticle();
                emitCounter_ -= rate;
            }
            
            elapsed_ += dt;
            if(duration_ != -1 && duration_ < elapsed_)
                stopSystem();
        }
        
        
        fzPoint currentPosition;
        if( positionType_ == kFZPositionTypeFree )
            currentPosition = convertToWorldSpace(FZPointZero);
        
        else if( positionType_ == kFZPositionTypeRelative )
            currentPosition = m_position;
        
        particleIdx_ = 0;
        while( particleIdx_ < particleCount_ )
        {
            tFZParticle& p = particles_[particleIdx_];
            
            // life
            p.timeToLive -= dt;
            
            if( p.timeToLive > 0 ) {
                
                // Mode A
                if( emitterMode_ == kFZParticleModeGravity ) {
                    
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
                    tmp.x += mode.A.gravityX;
                    tmp.y += mode.A.gravityY;
                    tmp.x += p.mode.A.dirX;
                    tmp.y += p.mode.A.dirY;

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
                fzPoint	newPos = ( positionType_ == kFZPositionTypeFree || positionType_ == kFZPositionTypeRelative )
                ? p.pos - currentPosition - p.startPos
                : p.pos;
                
                // update quads
                updateQuadWithParticle(p, newPos);
                
                // update particle counter
                ++particleIdx_;
                
            } else {
                // life < 0
                --particleCount_;

                if( particleIdx_ != particleCount_ )
                    memmove(&particles_[particleIdx_], &particles_[particleCount_], sizeof(tFZParticle));
                
                if( particleCount_ == 0 && autoRemoveOnFinish_ ) {
                    unschedule();
                    removeFromParent(true);
                    return;
                }
            }
        }
        
#if FZ_VBO_STREAMING
        postStep();
#endif
    }
    
    
    void ParticleSystem::setIsActive(bool a)
    {
        isActive_ = a;
    }
    
    
    void ParticleSystem::setDuration(fzFloat d)
    {
        duration_ = d;
    }
    
    
    void ParticleSystem::setSourcePosition(const fzPoint& p)
    {
        sourcePosition_ = p;
    }
    
    
    void ParticleSystem::setPosVar(const fzPoint& p)
    {
        posVar_ = p;
    }
    
    
    void ParticleSystem::setLife(fzFloat l)
    {
        life_ = l;
    }
    
    
    void ParticleSystem::setLifeVar(fzFloat l)
    {
        lifeVar_ = l;
    }
    
    
    void ParticleSystem::setAngle(fzFloat a)
    {
        angle_ = a;
    }
    
    
    void ParticleSystem::setAngleVar(fzFloat a)
    {
        angleVar_ = a;
    }
    
    
    void ParticleSystem::setGravity(const fzPoint& g)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        mode.A.gravityX = g.x;
        mode.A.gravityY = g.y;
    }
    
    
    void ParticleSystem::setSpeed(fzFloat s)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        mode.A.speed = s;
    }
    
    
    void ParticleSystem::setSpeedVar(fzFloat s)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        mode.A.speedVar = s;
    }
    
    
    void ParticleSystem::setTangentialAccel(fzFloat t)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        mode.A.tangentialAccel = t;
    }
    
    
    void ParticleSystem::setTangentialAccelVar(fzFloat t)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        mode.A.tangentialAccelVar = t;
    }
    
    
    void ParticleSystem::setRadialAccel(fzFloat r)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        mode.A.radialAccel = r;
    }
    
    
    void ParticleSystem::setRadialAccelVar(fzFloat r)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        mode.A.radialAccelVar = r;
    }
    
    
    void ParticleSystem::setStartRadius(fzFloat r)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        mode.B.startRadius = r;
    }
    
    
    void ParticleSystem::setStartRadiusVar(fzFloat r)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        mode.B.startRadiusVar = r;
    }
    
    
    void ParticleSystem::setEndRadius(fzFloat r)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        mode.B.endRadius = r;
    }
    
    
    void ParticleSystem::setEndRadiusVar(fzFloat r)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        mode.B.endRadiusVar = r;
    }
    
    
    void ParticleSystem::setRotatePerSecond(fzFloat r)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        mode.B.rotatePerSecond = r;
    }
    
    
    void ParticleSystem::setRotatePerSecondVar(fzFloat r)
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        mode.B.rotatePerSecondVar = r;
    }
    
    
    void ParticleSystem::setStartSize(fzFloat s)
    {
        startSize_ = s;
    }
    
    
    void ParticleSystem::setStartSizeVar(fzFloat s)
    {
        startSizeVar_ = s;
    }
    
    
    void ParticleSystem::setEndSize(fzFloat s)
    {
        endSize_ = s;
    }
    
    
    void ParticleSystem::setEndSizeVar(fzFloat s)
    {
        endSizeVar_ = s;
    }
    
    
    void ParticleSystem::setStartColor(const fzColor4F& c)
    {
        startColor_ = c;
    }
    
    
    void ParticleSystem::setStartColorVar(const fzColor4F& c)
    {
        startColorVar_ = c;
    }
    
    
    void ParticleSystem::setEndColor(const fzColor4F& c)
    {
        endColor_ = c;
    }
    
    
    void ParticleSystem::setEndColorVar(const fzColor4F& c)
    {
        endColorVar_ = c;
    }
    
    
    void ParticleSystem::setStartSpin(fzFloat s)
    {
        startSpin_ = s;
    }
    
    
    void ParticleSystem::setStartSpinVar(fzFloat s)
    {
        startSpinVar_ = s;
    }
    
    
    void ParticleSystem::setEndSpin(fzFloat s)
    {
        endSpin_ = s;
    }
    
    
    void ParticleSystem::setEndSpinVar(fzFloat s)
    {
        endSpinVar_ = s;
    }
    
    
    void ParticleSystem::setEmissionRate(fzFloat r)
    {
        emissionRate_ = r;
    }
    
    
    void ParticleSystem::setTotalParticles(fzUInt t)
    {
        totalParticles_ = t;
    }
    
    
    void ParticleSystem::setTexture(Texture2D *texture)
    {
        FZRETAIN_TEMPLATE(texture, texture_);
    }
    
    
    void ParticleSystem::setBlendFunc(const fzBlendFunc& b)
    {
        blendFunc_ = b;
    }
    
    
    void ParticleSystem::setPositionType(tFZPositionType e)
    {
        positionType_ = e;
    }
    
    
    void ParticleSystem::setAutoRemoveOnFinish(bool a)
    {
        autoRemoveOnFinish_ = a;
    }
    
    
    void ParticleSystem::setEmitterMode(fzInt e)
    {
        emitterMode_ = e;
    }
    
    
    bool ParticleSystem::getIsActive() const
    {
        return isActive_;
    }
    
    
    bool ParticleSystem::getParticleCount() const
    {
        return particleCount_;
    }
    
    
    fzFloat ParticleSystem::getDuration() const
    {
        return duration_;
    }
    
    
    const fzPoint& ParticleSystem::getSourcePosition() const
    {
        return sourcePosition_;
    }
    
    
    const fzPoint& ParticleSystem::getPosVar() const
    {
        return posVar_;
    }
    
    
    fzFloat ParticleSystem::getLife() const
    {
        return life_;
    }
    
    
    fzFloat ParticleSystem::getLifeVar() const
    {
        return lifeVar_;
    }
    
    
    fzFloat ParticleSystem::getAngle() const
    {
        return angle_;
    }
    
    
    fzFloat ParticleSystem::getAngleVar() const
    {
        return angleVar_;
    }
    
    
    const fzPoint& ParticleSystem::getGravity() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        return FZPointZero;
        //return mode.A.gravity;
    }
    
    
    fzFloat ParticleSystem::getSpeed() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        return mode.A.speed;
    }
    
    
    fzFloat ParticleSystem::getSpeedVar() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        return mode.A.speedVar;
    }
    
    
    fzFloat ParticleSystem::getTangentialAccel() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        return mode.A.tangentialAccel;
    }
    
    
    fzFloat ParticleSystem::getTangentialAccelVar() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        return mode.A.tangentialAccelVar;
    }
    
    
    fzFloat ParticleSystem::getRadialAccel() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        return mode.A.radialAccel;
    }
    
    
    fzFloat ParticleSystem::getRadialAccelVar() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeGravity, "Particle Mode should be Gravity");
        return mode.A.radialAccelVar;
    }
    
    
    fzFloat ParticleSystem::getStartRadius() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        return mode.B.startRadius;
    }
    
    
    fzFloat ParticleSystem::getStartRadiusVar() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        return mode.B.startRadiusVar;
    }
    
    
    fzFloat ParticleSystem::getEndRadius() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        return mode.B.endRadius;
    }
    
    
    fzFloat ParticleSystem::getEndRadiusVar() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        return mode.B.endRadiusVar;
    }
    
    
    fzFloat ParticleSystem::getRotatePerSecond() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        return mode.B.rotatePerSecond;
    }
    
    
    fzFloat ParticleSystem::getRotatePerSecondVar() const
    {
        FZ_ASSERT( emitterMode_ == kFZParticleModeRadius, "Particle Mode should be Radius");
        return mode.B.rotatePerSecondVar;
    }
    
    
    fzFloat ParticleSystem::getStartSize() const
    {
        return startSize_;
    }
    
    
    fzFloat ParticleSystem::getStartSizeVar() const
    {
        return startSizeVar_;
    }
    
    
    fzFloat ParticleSystem::getEndSize() const
    {
        return endSize_;
    }
    
    
    fzFloat ParticleSystem::getEndSizeVar() const
    {
        return endSizeVar_;
    }
    
    
    const fzColor4F& ParticleSystem::getStartColor() const
    {
        return startColor_;
    }
    
    
    const fzColor4F& ParticleSystem::getStartColorVar() const
    {
        return startColorVar_;
    }
    
    
    const fzColor4F& ParticleSystem::getEndColor() const
    {
        return endColor_;
    }
    
    
    const fzColor4F& ParticleSystem::getEndColorVar() const
    {
        return endColorVar_;
    }
    
    
    fzFloat ParticleSystem::getStartSpin() const
    {
        return startSpin_;
    }
    
    
    fzFloat ParticleSystem::getStartSpinVar() const
    {
        return startSpinVar_;
    }
    
    
    fzFloat ParticleSystem::getEndSpin() const
    {
        return endSpin_;
    }
    
    
    fzFloat ParticleSystem::getEndSpinVar() const
    {
        return endSpinVar_;
    }
    
    
    fzFloat ParticleSystem::getEmissionRate() const
    {
        return emissionRate_;
    }
    
    
    fzUInt ParticleSystem::getTotalParticles() const
    {
        return totalParticles_;
    }
    
    
    Texture2D* ParticleSystem::getTexture() const
    {
        return texture_;
    }
    
    
    const fzBlendFunc& ParticleSystem::getBlendFunc() const
    {
        return blendFunc_;
    }
    
    
    tFZPositionType ParticleSystem::getPositionType() const
    {
        return positionType_;
    }
    
    
    bool ParticleSystem::getAutoRemoveOnFinish() const
    {
        return autoRemoveOnFinish_;
    }
    
    
    fzInt ParticleSystem::getEmitterMode() const
    {
        return emitterMode_;
    }
}
