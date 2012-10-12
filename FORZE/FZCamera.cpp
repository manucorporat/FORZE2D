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

#include "FZCamera.h"


namespace FORZE {
    
    const fzFloat Camera::defaultEyeZ = FLT_EPSILON;
    
    Camera::Camera()
    : m_eye(0, 0, defaultEyeZ)
    , m_center(FZPoint3Zero)
    , m_up(FZPoint3Zero)
    , m_isDirty(false)
    {
        fzMath_mat4Identity(m_lookupMatrix);
    }
    
    
    void Camera::setDirty(bool d)
    {
        m_isDirty = d;
    }
    
    
    bool Camera::getDirty() const
    {
        return m_isDirty;
    }
    
    
    void Camera::restore()
    {
        m_eye.x = m_eye.y = 0;
        m_eye.z = defaultEyeZ;
        
        m_center = FZPoint3Zero;
        m_up = FZPoint3Zero;
        
        fzMath_mat4Identity(m_lookupMatrix);
        
        m_isDirty = false;
    }
    
    
    void Camera::locate()
    {
        if( m_isDirty ) {
            fzMath_mat4LookAt(m_eye, m_center, m_up, m_lookupMatrix);
            m_isDirty = false;
        }
    }
    
    
    void Camera::setEye(const fzPoint3& eye)
    {
        m_eye = eye;
        m_isDirty = true;
    }
    
    
    void Camera::setCenter(const fzPoint3& center)
    {
        m_center = center;
        m_isDirty = true;
    }
    
    
    void Camera::setUp(const fzPoint3& up)
    {
        m_up = up;
        m_isDirty = true;
    }
    
    
    const fzPoint3& Camera::getEye() const
    {
        return m_eye;
    }
    
    
    const fzPoint3& Camera::getCenter() const
    {
        return m_center;
    }
    
    
    const fzPoint3& Camera::getUp() const
    {
        return m_up;
    }
}
