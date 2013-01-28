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

#include "FZActionEase.h"
#include "FZMacros.h"
#include "FZMath.h"


namespace FORZE {
    
#ifndef M_PI_X_2
#define M_PI_X_2 (float)M_PI * 2.0f
#endif
    
#pragma mark - EaseAction
    
    ActionEase::ActionEase(ActionInterval *action)
    : ActionInterval(action->getDuration())
    , p_innerAction(action)
    {
        FZ_ASSERT( action != NULL, "Ease: arguments must be non-NULL.");
        p_innerAction->retain();
    }
    
    
    ActionEase::~ActionEase()
    {
        FZ_SAFE_RELEASE(p_innerAction);
    }
    
    
    void ActionEase::startWithTarget(void *t)
    {
        FZ_ASSERT(p_innerAction, "Inner action cannot be NULL.");
        Action::startWithTarget(t);
        p_innerAction->startWithTarget(t);
    }
    
    
    void ActionEase::stop()
    {
        p_innerAction->stop();
        Action::stop();
    }
    
    
    void ActionEase::update(fzFloat t)
    {
        p_innerAction->update(t);
    }
    
    
#pragma mark - EaseRate
    
    EaseRateAction::EaseRateAction(ActionInterval *action, fzFloat rate)
    : ActionEase(action)
    , m_rate(rate)
    { }
    
    
    void EaseRateAction::setRate(fzFloat r)
    {
        m_rate = r;
    }
    
    
    fzFloat EaseRateAction::getRate() const
    {
        return m_rate;
    }
    
    
    void EaseRateAction::update(fzFloat t)
    {
        p_innerAction->update(powf(t, m_rate));
    }
    
    
    ActionInterval* EaseRateAction::reverse() const
    {
        return new EaseRateAction(p_innerAction->reverse(), 1/m_rate);
    }
    
    
    ActionInterval* EaseRateAction::copy() const
    {
        return new EaseRateAction(p_innerAction->copy(), m_rate);
    }
    
    
#pragma mark EaseInOut
    
    void EaseInOut::update(fzFloat t)
    {
        int sign = 1;
        int r = (int) m_rate;
        if (r % 2 == 0)
            sign = -1;
        
        t *= 2;
        
        t = (t < 1)
        ? (0.5f * powf (t, m_rate))
        : (0.5f * sign * (powf (t-2, m_rate) + sign * 2));
        
        p_innerAction->update(t);
    }
    
    
    ActionInterval* EaseInOut::reverse() const
    {
        return new EaseInOut(p_innerAction->reverse(), 1/m_rate);
    }
    
    
    ActionInterval* EaseInOut::copy() const
    {
        return new EaseInOut(p_innerAction->copy(), m_rate);
    }
    
    
#pragma mark - EaseExponential actions
    
#pragma mark EaseExponentialIn
    
    void EaseExponentialIn::update(fzFloat t)
    {
        t = (t==0) ? 0 : powf(2, 10 * (t/1 - 1)) - 1 * 0.001f;
        p_innerAction->update(t);
    }
    
    
    ActionInterval* EaseExponentialIn::reverse() const
    {
        return new EaseExponentialOut(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseExponentialIn::copy() const
    {
        return new EaseExponentialIn(p_innerAction->copy());
    }
    
    
#pragma mark EaseExponentialIn
    
    void EaseExponentialOut::update(fzFloat t)
    {
        t = (t==1) ? 1 : (-powf(2, -10 * t/1) + 1);
        p_innerAction->update(t);
    }
    
    
    ActionInterval* EaseExponentialOut::reverse() const
    {
        return new EaseExponentialIn(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseExponentialOut::copy() const
    {
        return new EaseExponentialOut(p_innerAction->copy());
    }
    
    
#pragma mark EaseExponentialInOut
    
    void EaseExponentialInOut::update(fzFloat t)
    {
        t /= 0.5f;
        
        t = (t < 1)
        ? 0.5f * powf(2, 10 * (t - 1))
        : 0.5f * (-powf(2, -10 * (t-1)) + 2);
        
        p_innerAction->update(t);
    }
    
    
    ActionInterval* EaseExponentialInOut::reverse() const
    {
        return new EaseExponentialInOut(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseExponentialInOut::copy() const
    {
        return new EaseExponentialInOut(p_innerAction->copy());
    }
    
    
#pragma mark - EaseSin actions
    
#pragma mark EaseSineIn

    void EaseSineIn::update(fzFloat t)
    {
        t = -fzMath_cos(t * (fzFloat)M_PI_2) + 1;
        p_innerAction->update(t);
    }
    
    
    ActionInterval* EaseSineIn::reverse() const
    {
        return new EaseSineOut(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseSineIn::copy() const
    {
        return new EaseSineIn(p_innerAction->copy());
    }
    
    
#pragma mark EaseSineOut
    
    void EaseSineOut::update(fzFloat t)
    {
        t = fzMath_sin(t * (fzFloat)M_PI_2);
        p_innerAction->update(t);
    }
    
    
    ActionInterval* EaseSineOut::reverse() const
    {
        return new EaseSineIn(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseSineOut::copy() const
    {
        return new EaseSineOut(p_innerAction->copy());
    }
    
    
#pragma mark EaseSineInOut
    
    void EaseSineInOut::update(fzFloat t)
    {
        t = -0.5f * (fzMath_cos( (float)M_PI*t) - 1);
        p_innerAction->update(t);
    }
    
    ActionInterval* EaseSineInOut::reverse() const
    {
        return new EaseSineInOut(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseSineInOut::copy() const
    {
        return new EaseSineInOut(p_innerAction->copy());
    }
    
    
#pragma mark - EaseElastic
    
    EaseElastic::EaseElastic(ActionInterval *a, fzFloat p)
    : ActionEase(a)
    , m_period(p)
    { }
    
    
    void EaseElastic::setPeriod(fzFloat p)
    {
        m_period = p;
    }
    
    
    fzFloat EaseElastic::getPeriod() const
    {
        return m_period;
    }
    
    
#pragma mark EaseElasticIn
    
    void EaseElasticIn::update(fzFloat t)
    {
        fzFloat newT;
        if (t == 0 || t == 1)
            newT = t;
        
        else {
            fzFloat s = m_period / 4;
            t -= 1;
            newT = -powf(2, 10 * t) * fzMath_sin( (t-s) * M_PI_X_2 / m_period);
        }
        
        p_innerAction->update(newT);
    }
    
    
    ActionInterval* EaseElasticIn::reverse() const
    {
        return new EaseElasticOut(p_innerAction->reverse(), m_period);
    }
    
    
    ActionInterval* EaseElasticIn::copy() const
    {
        return new EaseElasticIn(p_innerAction->copy(), m_period);
    }
    
    
#pragma mark EaseElasticOut
    
    void EaseElasticOut::update(fzFloat t)
    {
        fzFloat newT = 0;
        if (t == 0 || t == 1)
            newT = t;
            
        else {
            fzFloat s = m_period / 4;
            newT = powf(2, -10 * t) * fzMath_sin( (t-s) *M_PI_X_2 / m_period) + 1;
        }
        
        p_innerAction->update(newT);
    }
    
    
    ActionInterval* EaseElasticOut::reverse() const
    {
        return new EaseElasticIn(p_innerAction->reverse(), m_period);
    }
    
    
    ActionInterval* EaseElasticOut::copy() const
    {
        return new EaseElasticOut(p_innerAction->copy(), m_period);
    }
    
    
#pragma mark EaseElasticInOut
    
    void EaseElasticInOut::update(fzFloat t)
    {
        fzFloat newT;
        
        if( t == 0 || t == 1 )
            newT = t;
        else {
            t = t * 2;
            if(! m_period )
                m_period = 0.3f * 1.5f;
            
            fzFloat s = m_period / 4;
            
            t = t -1;
            if( t < 0 )
                newT = -0.5f * powf(2, 10 * t) * fzMath_sin((t - s) * M_PI_X_2 / m_period);
            else
                newT = powf(2, -10 * t) * fzMath_sin((t - s) * M_PI_X_2 / m_period) * 0.5f + 1;
        }
        
        p_innerAction->update(newT);
    }
    
    
    ActionInterval* EaseElasticInOut::reverse() const
    {
        return new EaseElasticInOut(p_innerAction->reverse(), m_period);
    }
    
    
    ActionInterval* EaseElasticInOut::copy() const
    {
        return new EaseElasticInOut(p_innerAction->copy(), m_period);
    }
    
    
#pragma mark - EaseBounce actions
    
    static fzFloat bounceTime(fzFloat t)
    {
        if (t < 1 / 2.75f)
            return 7.5625f * t * t;
        
        else if (t < 2 / 2.75f) {
            t -= 1.5f / 2.75f;
            return 7.5625f * t * t + 0.75f;
        }
        else if (t < 2.5f / 2.75f) {
            t -= 2.25f / 2.75f;
            return 7.5625f * t * t + 0.9375f;
        }
        
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
    
    
#pragma mark EaseBounceIn
    
    void EaseBounceIn::update(fzFloat t)
    {
        t = 1 - bounceTime(1-t);	
        p_innerAction->update(t);
    }
    
    ActionInterval* EaseBounceIn::reverse() const
    {
        return new EaseBounceOut(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseBounceIn::copy() const
    {
        return new EaseBounceIn(p_innerAction->copy());
    }
    
    
#pragma mark EaseBounceOut
    
    void EaseBounceOut::update(fzFloat t)
    {
        t = bounceTime(t);	
        p_innerAction->update(t);
    }
    
    ActionInterval* EaseBounceOut::reverse() const
    {
        return new EaseBounceIn(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseBounceOut::copy() const
    {
        return new EaseBounceOut(p_innerAction->copy());
    }
    
    
#pragma mark EaseBounceInOut
    
    void EaseBounceInOut::update(fzFloat t)
    {
        fzFloat newT;
        if (t < 0.5f) {
            t *= 2;
            newT = (1 - bounceTime(1 - t) ) * 0.5f;
        } else
            newT = bounceTime(t * 2 - 1) * 0.5f + 0.5f;
        
        p_innerAction->update(newT);
    }
    
    
    ActionInterval* EaseBounceInOut::reverse() const
    {
        return new EaseBounceInOut(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseBounceInOut::copy() const
    {
        return new EaseBounceInOut(p_innerAction->copy());
    }
    
    
#pragma mark - EaseBack actions
    
#pragma mark EaseBackIn

    void EaseBackIn::update(fzFloat t)
    {
        const fzFloat overshoot = 1.70158f;
        
        t = t * t * (t * (overshoot + 1) - overshoot);
        p_innerAction->update(t);
    }
    
    ActionInterval* EaseBackIn::reverse() const
    {
        return new EaseBackOut(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseBackIn::copy() const
    {
        return new EaseBackIn(p_innerAction->copy());
    }
    
    
#pragma mark EaseBackOut
    
    void EaseBackOut::update(fzFloat t)
    {
        const fzFloat overshoot = 1.70158f;
        
        t = t - 1;
        t = t * t * (t * (overshoot + 1) + overshoot) + 1;
        p_innerAction->update(t);
    }
    
    
    ActionInterval* EaseBackOut::reverse() const
    {
        return new EaseBackIn(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseBackOut::copy() const
    {
        return new EaseBackOut(p_innerAction->copy());
    }
    
    
#pragma mark EaseBackInOut
    
    void EaseBackInOut::update(fzFloat t)
    {
        const fzFloat overshoot = 1.70158f * 1.525f;
        
        fzFloat newT;
        t *= 2;
        if (t < 1)
            newT = (t * t * (t * (overshoot + 1) - overshoot)) / 2;
        else {
            t -= 2;
            newT = (t * t * (t * (overshoot + 1) + overshoot)) / 2 + 1;
        }
        
        p_innerAction->update(newT);
    }
    
    
    ActionInterval* EaseBackInOut::reverse() const
    {
        return new EaseBackInOut(p_innerAction->reverse());
    }
    
    
    ActionInterval* EaseBackInOut::copy() const
    {
        return new EaseBackInOut(p_innerAction->copy());
    }
}
