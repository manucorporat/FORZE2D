/*
 * FORZE ENGINE: http://forzefield.com
 *
 * Copyright (c) 2011-2012 Manuel Martinez-Almeida
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
 @author Manuel Martinez-Almeida
 */

#include "FZActionCamera.h"
#include "FZCamera.h"
#include "FZMacros.h"
#include "FZNode.h"
#include "FZMath.h"
#include "FZCamera.h"


namespace FORZE {
    
#pragma mark - ActionCamera
    
    ActionCamera::ActionCamera(fzFloat d)
    : ActionInterval(d)
    , m_centerOrig(FZPoint3Zero)
    , m_eyeOrig(FZPoint3Zero)
    , m_upOrig(FZPoint3Zero)
    { }
    
    void ActionCamera::startWithTarget(void *target)
    {
        ActionInterval::startWithTarget(target);

        Camera *camera = ((Node*)target)->getCamera();
        m_centerOrig = camera->getCenter();
        m_eyeOrig = camera->getEye();
        m_upOrig = camera->getUp();
    }
    
    ActionInterval* ActionCamera::reverse() const
    {
        return new ReverseTime(this->copy());
    }
    
    ActionCamera* ActionCamera::copy() const
    {
        FZLOGERROR("NEEDs implementation.");
        return NULL;
    }
    
    
#pragma mark - OrbitCamera
    
    OrbitCamera::OrbitCamera(fzFloat duration, fzFloat radius, fzFloat deltaRadius, fzFloat angleZ, fzFloat deltaAngleZ, fzFloat angleX, fzFloat deltaAngleX)
    : ActionCamera(duration)
    , m_radius(radius)
    , m_deltaRadius(deltaRadius)
    , m_angleZ(angleZ)
    , m_deltaAngleZ(deltaAngleZ)
    , m_angleX(angleX)
    , m_deltaAngleX(deltaAngleX)
    , m_radDeltaZ(FZ_DEGREES_TO_RADIANS(deltaAngleZ))
    , m_radDeltaX(FZ_DEGREES_TO_RADIANS(deltaAngleX))
    { }
    
    
    void OrbitCamera::startWithTarget(void *target)
    {
        ActionCamera::startWithTarget(target);
        
        // Get spherical radius
        //fzFloat r, zenith, azimuth;
        //sphericalRadius(r, zenith, azimuth);
        
        m_radZ = FZ_DEGREES_TO_RADIANS(m_angleZ);
        m_radX = FZ_DEGREES_TO_RADIANS(m_angleX);
    }
    
    
    void OrbitCamera::update(fzFloat dt)
    {
        fzFloat r = (m_radius + m_deltaRadius * dt) * Camera::defaultEyeZ;
        fzFloat za = m_radZ + m_radDeltaZ * dt;
        fzFloat xa = m_radX + m_radDeltaX * dt;
        
        fzFloat zaSin = fzMath_sin(za);
        fzPoint3 eye(zaSin * fzMath_cos(xa) * r + m_centerOrig.x,
                     zaSin * fzMath_sin(xa) * r + m_centerOrig.y,
                     fzMath_cos(za) * r + m_centerOrig.z);

        // Update camera values
        ((Node*)p_target)->getCamera()->setEye(eye);
    }
    
    
    void OrbitCamera::sphericalRadius(fzFloat& newRadius, fzFloat& zenith, fzFloat& azimuth)
    {
        Camera *camera = ((Node*)p_target)->getCamera();

        // Camera values
        const fzPoint3& eye = camera->getEye();
        const fzPoint3& center = camera->getCenter();

        fzFloat x = eye.x-center.x;
        fzFloat y = eye.y-center.y;
        fzFloat z = eye.z-center.z;
        
        fzFloat s = x*x + y*y;
        fzFloat r = s + z*z;
        
        if(s == 0.0f)
            s = FLT_EPSILON;
        if(r == 0.0f)
            r = FLT_EPSILON;
        
        // Set values
        s = fzMath_sqrt(s);
        r = fzMath_sqrt(r);
        zenith = acosf(z/r);
        azimuth = (x < 0) ? (float)M_PI - asinf(y/s) : asinf(y/s);
        newRadius = r / Camera::defaultEyeZ;
    }
    
    
    OrbitCamera* OrbitCamera::copy() const
    {
        return new OrbitCamera(getDuration(),
                               m_radius,
                               m_deltaRadius,
                               m_angleZ,
                               m_deltaAngleZ,
                               m_angleX,
                               m_deltaAngleX);
    }

}
