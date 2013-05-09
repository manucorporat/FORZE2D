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

#include "FZParticleExamples.h"
#include "FZTextureCache.h"
#include "FZDirector.h"


namespace FORZE {

    class ParticleFire : public ParticleSystem
    {
    public:
        ParticleFire()
        : ParticleSystem(1000)
        {
            // duration
            setDuration(kFZParticleDurationInfinity);
            
            // Gravity Mode
            setEmitterMode(kFZParticleModeGravity);
            
            // Gravity Mode: gravity
            setGravity(fzPoint(0, 0)); // fzPoint(0, 0) = FZPointZero
            
            // Gravity Mode: radial acceleration
            setRadialAccel(0);
            setRadialAccelVar(0);
            
            // Gravity Mode: speed of particles
            setSpeed(410);
            setSpeedVar(26);
            
            // starting angle
            setAngle(90);
            setAngleVar(12);
            
            // emitter position
            fzSize winSize = Director::Instance().getCanvasSize();
            setSourcePosition(winSize/2);
            setPosVar(fzPoint(22, 0));
            
            // life of particles
            setLife(1);
            setLifeVar(0.25f);
            
            // size, in pixels
            setStartSize(65);
            setStartSizeVar(16);
            setEndSize(kFZParticleStartSizeEqualToEndSize);
            
            // emits per frame
            setEmissionRate(getTotalParticles() / getLife());
            
            // color of particles
            setStartColor(fzColor4F(0.76f, 0.25f, 0.12f, 1));
            setStartColorVar(fzColor4F(0, 0, 0, 0));
            
            setEndColor(fzColor4F(0.76f, 0.25f, 0.12f, 0));
            setEndColorVar(fzColor4F(0, 0, 0, 0));
        }
    };
    
    
    ParticleFireQuad::ParticleFireQuad()
    : ParticleSystemQuad(new ParticleFire())
    {
        Texture2D *texture = TextureCache::Instance().addImage("fire_rgba8888.pvr");
        setTexture(texture);
        setBlendFunc(fzBlendFunc(GL_SRC_ALPHA, GL_ONE ));
    };
}
