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


namespace FORZE {
    
#pragma mark - ActionCamera
    
    ActionCamera::ActionCamera(fzFloat d)
    : ActionInterval(d)
    { }
    
    void ActionCamera::startWithTarget(void *target)
    {
        ActionInterval::startWithTarget(target);
        
        //Camera *camera = ((Node*)target_)->getCamera();
        //camera->getCenter(centerXOrig_, centerYOrig_, centerZOrig_);
        //camera->getEye(eyeXOrig_, eyeYOrig_, eyeZOrig_);
        //camera->getUp(upXOrig_, upYOrig_, upZOrig_);
    }
    
    
#pragma mark - OrbitCamera
    
    OrbitCamera::OrbitCamera(fzFloat d, fzFloat r, fzFloat dr, fzFloat aZ, fzFloat daZ, fzFloat aX, fzFloat daX)
    : ActionCamera(d)
    , radius_(r)
    , deltaRadius_(dr)
    , angleZ_(aZ)
    , deltaAngleZ_(daZ)
    , angleX_(aX)
    , deltaAngleX_(daX)
    , radDeltaZ_(FZ_DEGREES_TO_RADIANS(daZ))
    , radDeltaX_(FZ_DEGREES_TO_RADIANS(daX))
    { }
    
    
    void OrbitCamera::startWithTarget(void *target)
    {
        ActionCamera::startWithTarget(target);
        
        // Get spherical radius
        fzFloat r, zenith, azimuth;
        sphericalRadius(r, zenith, azimuth);
        
        radZ_ = FZ_DEGREES_TO_RADIANS(angleZ_);
        radX_ = FZ_DEGREES_TO_RADIANS(angleX_);
    }
    
    
    void OrbitCamera::update(fzFloat dt)
    {
        fzFloat r = (radius_ + deltaRadius_ * dt) * Camera::defaultEyeZ;
        fzFloat za = radZ_ + radDeltaZ_ * dt;
        fzFloat xa = radX_ + radDeltaX_ * dt;
        
        fzFloat xaCos = fzMath_cos(xa);
        fzFloat zaSin = fzMath_sin(za);

        fzPoint3 eye(zaSin * xaCos * r + centerXOrig_,
                     zaSin * xaCos * r + centerYOrig_,
                     fzMath_cos(za) * r + centerZOrig_);

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
}
