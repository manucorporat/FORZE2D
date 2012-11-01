// DO NOT MODIFY THE HEADERS IF FORZE IS ALREADY COMPILED AS A STATIC LIBRARY
#ifndef __FZACTIONCAMERA_H_INCLUDED__
#define __FZACTIONCAMERA_H_INCLUDED__
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

#include "FZActionInterval.h"


namespace FORZE {

    //! Base class for Camera actions.
    class ActionCamera : public ActionInterval
    {
    protected:
        fzFloat centerXOrig_;
        fzFloat centerYOrig_;
        fzFloat centerZOrig_;
        
        fzFloat eyeXOrig_;
        fzFloat eyeYOrig_;
        fzFloat eyeZOrig_;
        
        fzFloat upXOrig_;
        fzFloat upYOrig_;
        fzFloat upZOrig_;
        
    public:
        ActionCamera(fzFloat duration);
        
        // Redefined functions
        void startWithTarget(void *target);
    };
    
    
    /** OrbitCamera action
     Orbits the camera around the center of the screen using spherical coordinates
     */
    class OrbitCamera : public ActionCamera
    {
    protected:
        fzFloat radius_;
        fzFloat deltaRadius_;
        fzFloat angleZ_;
        fzFloat deltaAngleZ_;
        fzFloat angleX_;
        fzFloat deltaAngleX_;
        
        fzFloat radZ_;
        fzFloat radDeltaZ_;
        fzFloat radX_;
        fzFloat radDeltaX_;
        
    public:
        /** creates a CCOrbitCamera action with radius, delta-radius,  z, deltaZ, x, deltaX */
        OrbitCamera(fzFloat duration, fzFloat radius, fzFloat deltaRadius, fzFloat angleZ, fzFloat deltaAngleZ, fzFloat angleX, fzFloat deltaAngleX);
        
        /** positions the camera according to spherical coordinates */
        void sphericalRadius(fzFloat& newRadius, fzFloat& zenith, fzFloat& azimuth);
        
        // Redefined functions
        void startWithTarget(void *target);
        void update(fzFloat delta);
        
    };
}
#endif
