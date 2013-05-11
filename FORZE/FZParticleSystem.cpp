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
    
    ParticleSystem::ParticleSystem(fzUInt number)
    : m_totalParticles(number)
    , m_particleCount(0)
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
    , p_particles(NULL)
    , m_startColor(fzWHITE)
    , m_startColorVar(fzBLACK)
    , m_endColor(fzWHITE)
    , m_endColorVar(fzBLACK)
    , m_positionType(kFZPositionTypeFree)
    , m_emitterMode(kFZParticleModeGravity)
    {
        // reset union
        memset(&mode, 0, sizeof(mode));
        
        // allocate particles
        p_particles = new fztParticle[m_totalParticles];
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
    
    fzUInt ParticleSystem::getParticleCount() const {
        return m_particleCount;
    }
    
    
    fzUInt ParticleSystem::getTotalParticles() const {
        return m_totalParticles;
    }
    
    
    void ParticleSystem::initParticle(fztParticle& particle)
    {
        // time to live
        particle.timeToLive = m_life + m_lifeVar * FZ_RANDOM_MINUS1_1();
        if(particle.timeToLive < 0)
            particle.timeToLive = 0;
        
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
        if(startS < 0)
            startS = 0;
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
            
            if(mode.B.endRadius == kFZParticleStartRadiusEqualToEndRadius)
                particle.mode.B.deltaRadius = 0;
            else
                particle.mode.B.deltaRadius = (endRadius-startRadius) / particle.timeToLive;
            
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
        fzUInt i = 0;
        for(; i < m_particleCount; ++i)
            p_particles[i].timeToLive = 0;
    }
    
    
    bool ParticleSystem::isFull() const
    {
        return (m_particleCount == m_totalParticles);
    }
    
    
    void ParticleSystem::preUpdate(fzFloat dt)
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
    }
    
    
    void ParticleSystem::updateParticle(fzUInt index, fzFloat dt, fzParticle *particle)
    {
        fztParticle& p = p_particles[index];
        
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
            
        } else {
            // life < 0
            --m_particleCount;
            
            if( index != m_particleCount )
                memmove(&p_particles[index], &p_particles[m_particleCount], sizeof(fztParticle));

            if( m_particleCount == 0 && m_autoRemoveOnFinish ) {
                //unschedule();
                //removeFromParent(true);
                return;
            }
        }
        particle->pos = p.pos;
        particle->color = p.color;
        particle->size = fzSize(p.size, p.size);
        particle->rotation = p.rotation;
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

    
    const fzPoint& ParticleSystem::getGravity() const
    {
        FZ_ASSERT( m_emitterMode == kFZParticleModeGravity, "Particle Mode should be Gravity.");
        return mode.A.gravity;
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
