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
#include "FZAnimation.h"
#include "FZSpriteFrame.h"
#include "FZSprite.h"


#define FZMAX_ACTION_BATCH 32

namespace FORZE {
    
#pragma mark - IntervalAction
    
    ActionInterval::ActionInterval(fzFloat d)
    : m_elapsed(0)
    , m_firstTick(true)
    {
        setDuration(d);
    }
    
    
    void ActionInterval::setElapsed(fzFloat e)
    {
        m_elapsed = e;
    }
    
    
    fzFloat ActionInterval::getElapsed() const
    {
        return m_elapsed;
    }
    
    
    bool ActionInterval::isDone() const
    {
        return (m_elapsed >= m_duration);
    }
    
    
    void ActionInterval::step(fzFloat dt)
    {
        if( m_firstTick ) {
            m_firstTick = false;
            m_elapsed = 0;
        } else
            m_elapsed += dt;
        
        dt = m_elapsed/m_duration;
        update(MIN(1, dt));
    }
    
    
    void ActionInterval::startWithTarget(void *t)
    {
        Action::startWithTarget(t);
        m_elapsed = 0.0f;
        m_firstTick = true;
    }
    
    
    ActionInterval* ActionInterval::reverse() const
    {
        Action::reverse();
        return NULL;
    }
    
    
    ActionInterval* ActionInterval::copy() const
    {
        Action::copy();
        return NULL;
    }
    
    
#pragma mark - Sequence
    
    Sequence::Sequence()
    : ActionInterval(0)
    , m_currentAction(0)
    , m_startedAction(false)
    , m_timeOffset(0)
    , m_numActions(0)
    , p_actions(NULL)
    { }
    
    Sequence::Sequence(FiniteTimeAction *action1, ...)
    : Sequence()
    {
        FZ_ASSERT(action1 != NULL, "Action cannot be NULL");

        FiniteTimeAction *buffer[FZMAX_ACTION_BATCH];
        fzFloat duration = 0;
        
        va_list params;
        va_start(params, action1);
        
        while(action1)
        {
            buffer[m_numActions] = action1;
            duration += action1->getDuration();
            ++m_numActions;
            
            action1 = va_arg(params, FiniteTimeAction*);
        }
        va_end(params);
        
        
        p_actions = new FiniteTimeAction*[m_numActions];
        for(fzUInt i = 0; i < m_numActions; ++i)
        {
            p_actions[i] = buffer[i];
            p_actions[i]->retain();
        }
        
        setDuration(duration);        
    }
    
    
    Sequence::Sequence(FiniteTimeAction **buffer, fzUInt nuActions)
    : Sequence()
    {
        m_numActions = nuActions;
        p_actions = new FiniteTimeAction*[m_numActions];
        
        fzFloat duration = 0;
        for(fzUInt i = 0; i < m_numActions; ++i)
        {
            p_actions[i] = buffer[i];
            p_actions[i]->retain();
            duration += p_actions[i]->getDuration();
        }
        
        setDuration(duration);
    }
    
    
    Sequence::~Sequence()
    {
        fzUInt i = 0;
        for(; i < m_numActions; ++i)
            p_actions[i]->release();
        
        delete [] p_actions;
    }
    
    
    void Sequence::startWithTarget(void *t)
    {
        m_currentAction = 0;
        m_timeOffset = 0;
        m_startedAction = false;

        
        ActionInterval::startWithTarget(t);
    }
    
    
    void Sequence::update(fzFloat t)
    {
        if(m_currentAction < m_numActions)
        {
            FiniteTimeAction *action = p_actions[m_currentAction];
            if(m_startedAction == false)
            {
                action->startWithTarget(p_target);
                m_startedAction = true;
            }
            fzFloat current = 1;
            fzFloat invRate = 0;
            if(action->getDuration() != 0)
            {
                fzFloat rate = getDuration() / action->getDuration();
                invRate = 1 / rate;
                current = (t-m_timeOffset) * rate;
                current = (current < 1.0f) ? current : 1.0f;
                action->update(current);
            }
            
            if(current == 1)
            {
                action->stop();
                m_timeOffset += invRate;
                m_startedAction = false;
                ++m_currentAction;
            }
        }
    }
    
    
    bool Sequence::isDone() const
    {
        return (m_currentAction >= m_numActions);
    }
    
    
    void Sequence::stop()
    {
        fzUInt i = 0;
        for(; i < m_numActions; i++)
            p_actions[i]->stop();
        
        Action::stop();
    }
    
    
    Sequence* Sequence::reverse() const
    {
        FiniteTimeAction *buffer[m_numActions];
        for(fzUInt i = 0; i < m_numActions; ++i)
            buffer[i] = p_actions[m_numActions-1-i]->reverse();
        
        return new Sequence(buffer, m_numActions);
    }
    
    
    Sequence* Sequence::copy() const
    {
        FiniteTimeAction *buffer[m_numActions];
        for(fzUInt i = 0; i < m_numActions; ++i)
            buffer[i] = p_actions[i]->copy();
        
        return new Sequence(buffer, m_numActions);
    }
    
    
#pragma mark - Spawn
    
    Spawn::Spawn()
    : ActionInterval(0)
    , m_numActions(0)
    , p_actions(NULL)
    { }
    
    
    Spawn::Spawn(FiniteTimeAction *action1, ...)
    : Spawn()
    {
        FZ_ASSERT(action1 != NULL, "Action cannot be NULL");
        
        FiniteTimeAction *buffer[FZMAX_ACTION_BATCH];
        fzFloat duration = 0;
        
        va_list params;
        va_start(params, action1);
        
        while(action1)
        {
            buffer[m_numActions] = action1;
            duration = MAX(duration, action1->getDuration());
            ++m_numActions;
            
            action1 = va_arg(params, FiniteTimeAction*);
        }
        va_end(params);
        
        
        p_actions = new FiniteTimeAction*[m_numActions];
        for(fzUInt i = 0; i < m_numActions; ++i)
        {
            p_actions[i] = buffer[i];
            p_actions[i]->retain();
        }
        
        setDuration(duration);
    }
    
    
    Spawn::Spawn(FiniteTimeAction **buffer, fzUInt nuActions)
    : Spawn()
    {
        m_numActions = nuActions;
        p_actions = new FiniteTimeAction*[m_numActions];
        
        fzFloat duration = 0;
        for(fzUInt i = 0; i < m_numActions; ++i)
        {
            p_actions[i] = buffer[i];
            p_actions[i]->retain();
            duration = MAX(duration, p_actions[i]->getDuration());
        }
        setDuration(duration);
    }
    
    
    Spawn::~Spawn()
    {
        for(fzUInt i = 0; i < m_numActions; ++i)
            p_actions[i]->release();
        
        delete [] p_actions;
    }
    
    
    void Spawn::startWithTarget(void *t)
    {
        fzUInt i = 0;
        for(; i < m_numActions; ++i)
            p_actions[i]->startWithTarget(t);
        
        Action::startWithTarget(t);
    }
    
    
    void Spawn::update(fzFloat t)
    {
        fzUInt i = 0;
        for (; i < m_numActions; ++i) {
            FiniteTimeAction *action = p_actions[i];
            if(!action->isDone())
            {
                fzFloat factor = getDuration() / action->getDuration();
                fzFloat newT = factor * t;
                newT = (newT > 1.0f) ? 1.0f : newT;
                action->update(newT);
            }            
        }
    }
    
    
    void Spawn::stop()
    {
        fzUInt i = 0;
        for(; i < m_numActions; ++i)
            p_actions[i]->stop();
        
        Action::stop();
    }
    
    
    Spawn* Spawn::reverse() const
    {
        FiniteTimeAction *buffer[m_numActions];
        for(fzUInt i = 0; i < m_numActions; ++i)
            buffer[i] = p_actions[i]->reverse();
        
        return new Spawn(p_actions, m_numActions);
    }
    
    
    Spawn* Spawn::copy() const
    {
        FiniteTimeAction *buffer[m_numActions];
        for(fzUInt i = 0; i < m_numActions; ++i)
            buffer[i] = p_actions[i]->copy();
        
        return new Spawn(p_actions, m_numActions);
    }
    
    
#pragma mark - Repeat
    
    Repeat::Repeat(FiniteTimeAction *a, fzUInt t)
    : ActionInterval(0)
    , m_times(t)
    , p_innerAction(a)
    , m_total(0)
    {
        FZ_ASSERT(p_innerAction != NULL, "Action cannot be NULL");
        p_innerAction->retain();
        setDuration(p_innerAction->getDuration() * m_times);
    }
    
    
    Repeat::~Repeat()
    {
        setInnerAction(NULL);
    }
    
    
    void Repeat::setInnerAction(FiniteTimeAction *action)
    {
        FZRETAIN_TEMPLATE(action, p_innerAction)
    }
    
    
    FiniteTimeAction* Repeat::getInnerAction() const
    {
        return p_innerAction;
    }
    
    
    void Repeat::startWithTarget(void* t)
    {
        m_total = 0;
        ActionInterval::startWithTarget(t);
        p_innerAction->startWithTarget(t);
    }
    
    
    void Repeat::stop()
    {    
        p_innerAction->stop();
        Action::stop();
    }
    
    
    void Repeat::update(fzFloat dt)
    {
        fzFloat t = dt * m_times;
        if( t > m_total+1 ) {
            m_total++;
            p_innerAction->update(1.0f);
            p_innerAction->stop();
            p_innerAction->startWithTarget(p_target);
            
            if( m_total== m_times )
                p_innerAction->update(0);
            else
                p_innerAction->update(t-m_total);
            
        } else {
            
            fzFloat r = fmodf(t, 1.0f);
            
            // fix last repeat position
            // else it could be 0.
            if( dt== 1.0f) {
                r = 1.0f;
                m_total++; // this is the added line
            }
            p_innerAction->update(MIN(r, 1));
        }
    }
    
    
    bool Repeat::isDone() const
    {
        return ( m_total == m_times );
    }
    
    
    Repeat* Repeat::reverse() const
    {
        return new Repeat(p_innerAction->reverse(), m_times);
    }
    
    
    Repeat* Repeat::copy() const
    {
        return new Repeat(p_innerAction->copy(), m_times);
    }
    
    
#pragma mark - RotateBy
    
    RotateBy::RotateBy(fzFloat d, fzFloat a)
    : ActionInterval(d)
    , m_startAngle(0)
    , m_delta(a)
    { }
    
    
    void RotateBy::startWithTarget(void* t)
    {
        ActionInterval::startWithTarget(t);
        
        m_startAngle = ((Node*)p_target)->getRotation();
        
        m_startAngle = (m_startAngle > 0)
        ? fmodf(m_startAngle, 360.0f)
        : fmodf(m_startAngle, -360.0f);
    }
    
    
    void RotateBy::update(fzFloat t)
    {
        ((Node*)p_target)->setRotation( m_startAngle + m_delta * t );
    }
    
    
    RotateBy* RotateBy::reverse() const
    {
        return new RotateBy(m_duration, -m_delta);
    }
    
    
    RotateBy* RotateBy::copy() const
    {
        return new RotateBy(m_duration, m_delta);
    }
    
    
#pragma mark - RotateTo
    
    RotateTo::RotateTo(fzFloat d, fzFloat a)
    : RotateBy(d, a)
    , m_original(a)
    { }
    
    
    void RotateTo::startWithTarget(void* t)
    {
        RotateBy::startWithTarget(t);
        
        m_delta = m_original - m_startAngle;
        
        if (m_delta > 180)
            m_delta -= 360;
        
        if (m_delta < -180)
            m_delta += 360;    
    }
    
    
    RotateTo* RotateTo::copy() const
    {
        return new RotateTo(m_duration, m_original);
    }
    
    
    RotateTo* RotateTo::reverse() const
    {
        FZLOGERROR("RotateTo: Reverse action in not supported.");
        return NULL;
    }

    
    
#pragma mark - MoveBy
    
    MoveBy::MoveBy(fzFloat d, const fzPoint& p)
    : ActionInterval(d)
    , m_delta(p)
    , m_startPosition(FZPointZero)
    { }
    
    
    void MoveBy::startWithTarget(void* t)
    {        
        ActionInterval::startWithTarget(t);
        m_startPosition = ((Node*)p_target)->getPosition();
    }
    
    
    void MoveBy::update(fzFloat t)
    {
        ((Node*)p_target)->setPosition(m_startPosition + m_delta * t);
    }
    
    
    MoveBy* MoveBy::reverse() const
    {
        return new MoveBy(m_duration, -m_delta);
    }
    
    
    MoveBy* MoveBy::copy() const
    {
        return new MoveBy(m_duration, m_delta);
    }
    
    
#pragma mark - MoveTo
    
    MoveTo::MoveTo(fzFloat d, const fzPoint& p)
    : MoveBy(d, p)
    , m_original(p)
    { }
    
    
    void MoveTo::startWithTarget(void* t)
    {
        MoveBy::startWithTarget(t);
        m_delta = m_original - m_startPosition;
    }
    
    
    MoveTo* MoveTo::copy() const
    {
        return new MoveTo(m_duration, m_original);
    }
    
    
    MoveTo* MoveTo::reverse() const
    {
        FZLOGERROR("MoveTo: Reverse action in not supported.");
        return NULL;
    }
    
    
#pragma mark - SkewBy
    
    SkewBy::SkewBy(fzFloat d, fzFloat sX, fzFloat sY)
    : ActionInterval(d)
    , m_deltaX(sX)
    , m_deltaY(sY)
    { }
    
    
    void SkewBy::startWithTarget(void* t)
    {
        ActionInterval::startWithTarget(t);
        
        m_startSkewX = ((Node*)p_target)->getSkewX();
        m_startSkewX = (m_startSkewX > 0)
        ? fmodf(m_startSkewX, 180.0f)
        : fmodf(m_startSkewX, -180.0f);
        
        
        m_startSkewY = ((Node*)p_target)->getSkewY();
        m_startSkewY = (m_startSkewY > 0)
        ? fmodf(m_startSkewY, 360.0f)
        : fmodf(m_startSkewY, -360.0f);
    }
    
    
    void SkewBy::update(fzFloat dt)
    {
        ((Node*)p_target)->setSkewX(m_startSkewX + m_deltaX * dt );
        ((Node*)p_target)->setSkewY(m_startSkewY + m_deltaY * dt );
    }
    
    
    SkewBy* SkewBy::reverse() const
    {
        return new SkewBy(m_duration, -m_deltaX, -m_deltaY);
    }
    
    
    SkewBy* SkewBy::copy() const
    {
        return new SkewBy(m_duration, m_deltaX, m_deltaY);
    }
    
    
#pragma mark - SkewTo
    
    SkewTo::SkewTo(fzFloat d, fzFloat sX, fzFloat sY)
    : SkewBy(d, sX, sY)
    , m_originalX(sX)
    , m_originalY(sY)
    { }
    
    
    void SkewTo::startWithTarget(void* t)
    {
        SkewBy::startWithTarget(t);
        
        m_deltaX = m_originalX - m_startSkewX;
        
        if ( m_deltaX > 180 )
            m_deltaX -= 360;
        
        if ( m_deltaX < -180 )
            m_deltaX += 360;
        
        
        m_deltaY = m_originalY - m_startSkewY;
        
        if ( m_deltaY > 180 )
            m_deltaY -= 360;
        
        if ( m_deltaY < -180 )
            m_deltaY += 360;
    }
    
    
    SkewTo* SkewTo::copy() const
    {
        return new SkewTo(m_duration, m_originalX, m_originalY);
    }
    
    
    SkewTo* SkewTo::reverse() const
    {
        FZLOGERROR("SkewTo: Reverse action in not supported.");
        return NULL;
    }
    
    
#pragma mark - JumpBy
    
    JumpBy::JumpBy(fzFloat d, const fzPoint& p, fzFloat h, fzUInt j)
    : ActionInterval(d)
    , m_delta(p)
    , m_height(h)
    , m_jumps(j)
    , m_startPosition(FZPointZero)
    { }
    
    
    void JumpBy::startWithTarget(void* t)
    {
        ActionInterval::startWithTarget(t);
        m_startPosition = ((Node*)p_target)->getPosition();
    }
    
    
    void JumpBy::update(fzFloat dt)
    {
        fzFloat frac = fmodf( dt * m_jumps, 1.0f );
        
        fzPoint xy(m_delta.x * dt, (m_height * 4 * frac * (1 - frac)));
        xy.y += m_delta.y * dt;
        
        ((Node*)p_target)->setPosition(m_startPosition + xy);
    }
    
    
    JumpBy* JumpBy::reverse() const
    {
        return new JumpBy(m_duration, -m_delta, m_height, m_jumps);
    }
    
    
    JumpBy* JumpBy::copy() const
    {
        return new JumpBy(m_duration, m_delta, m_height, m_jumps);
    }
    
    
#pragma mark - JumpTo
    
    JumpTo::JumpTo(fzFloat d, const fzPoint& p, fzFloat h, fzUInt j)
    : JumpBy(d, p, h, j)
    , m_original(p)
    { }
    
    
    void JumpTo::startWithTarget(void* t)
    {
        m_delta += m_startPosition;
        JumpBy::startWithTarget(t);
        m_delta -= m_startPosition;
    }
    
    
    JumpTo* JumpTo::copy() const
    {
        return new JumpTo(m_duration, m_original, m_height, m_jumps);
    }
    
    JumpTo* JumpTo::reverse() const
    {
        FZLOGERROR("JumpTo: Reverse action in not supported.");
        return NULL;
    }
    
    
#pragma mark - BezierBy
    
    BezierBy::BezierBy(fzFloat d, const fzBezierConfig& c)
    : ActionInterval(d)
    , m_config(c)
    , m_startPosition(FZPointZero)
    { }
    
    
    void BezierBy::startWithTarget(void* t)
    {
        ActionInterval::startWithTarget(t);
        m_startPosition = ((Node*)p_target)->getPosition();
    }
    
    
    inline fzFloat BezierBy::bezierat( fzFloat a, fzFloat b, fzFloat c, fzFloat d, fzFloat t )
    {
        const fzFloat tm = 1 - t; 
        return (tm * tm * tm * a + 
                3 * t * tm* tm * b + 
                3 * t * t * tm * c +
                t * t * t * d );
    }
    
    
    void BezierBy::update(fzFloat dt)
    {
        fzFloat& xb = m_config.controlPoint_1.x;
        fzFloat& xc = m_config.controlPoint_2.x;
        fzFloat& xd = m_config.endPosition.x;
        
        fzFloat& yb = m_config.controlPoint_1.y;
        fzFloat& yc = m_config.controlPoint_2.y;
        fzFloat& yd = m_config.endPosition.y;
        
        fzPoint advance(bezierat(0, xb, xc, xd, dt),
                        bezierat(0, yb, yc, yd, dt));
        
        advance += m_startPosition;
        
        ((Node*)p_target)->setPosition(advance);
    }
    
    
    BezierBy* BezierBy::reverse() const
    {
        fzBezierConfig r;
        r.endPosition	 = -m_config.endPosition;
        r.controlPoint_1 = m_config.controlPoint_2 - m_config.endPosition;
        r.controlPoint_2 = m_config.controlPoint_1 - m_config.endPosition;

        return new BezierBy(m_duration, r);
    }
    
    
    BezierBy* BezierBy::copy() const
    {
        return new BezierBy(m_duration, m_config);
    }
    
    
#pragma mark - BezierTo
    
    BezierTo::BezierTo(fzFloat d, const fzBezierConfig& c)
    : BezierBy(d, c)
    , m_original(c)
    { }
    
    
    void BezierTo::startWithTarget(void* t)
    {
        BezierBy::startWithTarget(t);
        m_config.controlPoint_1 = m_original.controlPoint_1 - m_startPosition;
        m_config.controlPoint_2 = m_original.controlPoint_2 -  m_startPosition;
        m_config.endPosition = m_original.endPosition -  m_startPosition;
    }
    
    
    BezierTo* BezierTo::copy() const
    {
        return new BezierTo(m_duration, m_original);
    }
    
    BezierTo* BezierTo::reverse() const
    {
        FZLOGERROR("BezierTo: Reverse action in not supported.");
        return NULL;
    }

    
    
#pragma mark - ScaleBy
    
    ScaleBy::ScaleBy(fzFloat d, fzFloat sX, fzFloat sY)
    : ActionInterval(d)
    , m_deltaX(sX)
    , m_deltaY(sY)
    , m_originalX(sX)
    , m_originalY(sY)
    , m_startScaleX(0)
    , m_startScaleY(0)
    { }
    
    ScaleBy::ScaleBy(fzFloat d, fzFloat s)
    : ScaleBy(d, s, s)
    { }
    
    
    void ScaleBy::startWithTarget(void* t)
    {
        ActionInterval::startWithTarget(t);
        m_startScaleX = ((Node*)p_target)->getScaleX();
        m_startScaleY = ((Node*)p_target)->getScaleY();
        m_deltaX = m_startScaleX * m_originalX - m_startScaleX;
        m_deltaY = m_startScaleY * m_originalY - m_startScaleY;
    }
    
    
    void ScaleBy::update(fzFloat t)
    {
        ((Node*) p_target)->setScaleX( m_startScaleX + m_deltaX * t);
        ((Node*) p_target)->setScaleY( m_startScaleY + m_deltaY * t);
    }
    
    
    ScaleBy* ScaleBy::reverse() const
    {
        return new ScaleBy(m_duration, 1/m_deltaX, 1/m_deltaY);
    }
    
    
    ScaleBy* ScaleBy::copy() const
    {
        return new ScaleBy(m_duration, m_deltaX, m_deltaY);
    }
    
    
#pragma mark - ScaleTo
    
    ScaleTo::ScaleTo(fzFloat d, fzFloat sX, fzFloat sY)
    : ScaleBy(d, sX, sY)
    { }
    
    
    ScaleTo::ScaleTo(fzFloat d, fzFloat s)
    : ScaleBy(d, s)
    { }
    
    
    void ScaleTo::startWithTarget(void* t)
    {
        ActionInterval::startWithTarget(t);
        m_startScaleX = ((Node*)p_target)->getScaleX();
        m_startScaleY = ((Node*)p_target)->getScaleY();
        m_deltaX = m_originalX - m_startScaleX;
        m_deltaY = m_originalY - m_startScaleY;
    }

    
    ScaleTo* ScaleTo::copy() const
    {
        return new ScaleTo(m_duration, m_originalX, m_originalY);
    }
    
    
    ScaleTo* ScaleTo::reverse() const
    {
        FZLOGERROR("ScaleTo: Reverse action in not supported.");
        return NULL;
    }
    
    
#pragma mark - FadeTo
    
    FadeTo::FadeTo(fzFloat d, GLubyte o)
    : ActionInterval(d)
    , m_original(o)
    { }
    
    
    void FadeTo::startWithTarget(void* t)
    {
        ActionInterval::startWithTarget(t);
        m_startOpacity = ((Node*) p_target)->getOpacity();
        m_delta = m_original - (fzFloat)m_startOpacity;
    }
    
    
    void FadeTo::update(fzFloat t)
    {
        t *= m_delta;
        t += m_startOpacity;
        ((Node*) p_target)->setOpacity( t );
    }
    
    
    FadeTo* FadeTo::copy() const
    {
        return new FadeTo(m_duration, m_original);
    }
    
    
    FadeTo* FadeTo::reverse() const
    {
        FZLOGERROR("FadeTo: Reverse action in not supported.");
        return NULL;
    }
    
    
#pragma mark - FadeIn
    
    FadeIn::FadeIn(fzFloat d)
    : ActionInterval(d)
    { }
    
    
    void FadeIn::update(fzFloat dt)
    {
        ((Node*) p_target)->setOpacity( dt );
    }
    
    
    ActionInterval* FadeIn::reverse() const
    {
        return new FadeOut(m_duration);
    }
    
    
    ActionInterval* FadeIn::copy() const
    {
        return new FadeIn(m_duration);
    }
    
    
#pragma mark - FadeOut
    
    FadeOut::FadeOut(fzFloat d)
    : ActionInterval(d)
    { }
    
    
    void FadeOut::update(fzFloat dt)
    {
        ((Node*) p_target)->setOpacity((1-dt));
    }
    
    
    ActionInterval* FadeOut::reverse() const
    {
        return new FadeIn(m_duration);
    }
    
    
    ActionInterval* FadeOut::copy() const
    {
        return new FadeOut(m_duration);
    }
    
    
#pragma mark - Blink
    
    Blink::Blink(fzFloat d, fzUInt b, fzFloat p)
    : ActionInterval(d)
    , m_slice(1.0f / b)
    , m_percentVisible(p)
    { }
    
    
    void Blink::update(fzFloat dt)
    {
        fzFloat m = fmodf(dt, m_slice);
        bool isVisible = (m < (m_slice * m_percentVisible));
        ((Node*) p_target)->setIsVisible(isVisible);
    }
    
    
    Blink* Blink::copy() const
    {
        return new Blink(m_duration, 1.0f/m_slice, m_percentVisible);
    }
    
    
    Blink* Blink::reverse() const
    {
        return copy();
    }
    
    
#pragma mark - TintBy
    
    TintBy::TintBy(fzFloat duration, fzFloat red, fzFloat green, fzFloat blue)
    : ActionInterval(duration)
    , m_deltaR(red)
    , m_deltaG(green)
    , m_deltaB(blue)
    { }
    
    
    void TintBy::startWithTarget(void* t)
    {
        ActionInterval::startWithTarget(t);
        Sprite *sprite = static_cast<Sprite*>(p_target);
        m_startColor = sprite->getColor();
    }
    
    
    void TintBy::update(fzFloat dt)
    {
        fzColor3B newColor(m_startColor.r + m_deltaR * dt,
                           m_startColor.g + m_deltaG * dt,
                           m_startColor.b + m_deltaB * dt);
        
        ((Sprite*) p_target)->setColor(newColor);
    }
    
    
    TintBy* TintBy::reverse() const
    {
        return new TintBy(m_duration, -m_deltaR, -m_deltaG, -m_deltaB);
    }
    
    
    TintBy* TintBy::copy() const
    {
        return new TintBy(m_duration, m_deltaR, m_deltaG, m_deltaB);
    }
    
    
    
#pragma mark - TintTo
    
    TintTo::TintTo(fzFloat duration, fzFloat red, fzFloat green, fzFloat blue)
    : TintBy(duration, red, green, blue)
    , m_originalR(red)
    , m_originalG(green)
    , m_originalB(blue)
    { }
    
    
    TintTo::TintTo(fzFloat d, const fzColor3B& c)
    : TintTo(d, c.r, c.g, c.b)
    { }
    
    
    void TintTo::startWithTarget(void* t)
    {
        TintBy::startWithTarget(t);
        
        m_deltaR = m_originalR - m_startColor.r;
        m_deltaG = m_originalG - m_startColor.g;
        m_deltaB = m_originalB - m_startColor.b;
    }
    
    
    TintTo* TintTo::copy() const
    {
        return new TintTo(m_duration, m_originalR, m_originalG, m_originalB);
    }
    
    
    TintTo* TintTo::reverse() const
    {
        FZLOGERROR("TintTo: Reverse action in not supported.");
        return NULL;
    }
    
    
#pragma mark - DelayTime
    
    DelayTime::DelayTime(fzFloat d)
    : ActionInterval(d)
    { }
    
    
    void DelayTime::update(fzFloat dt)
    {
        return;
    }
    
    
    DelayTime* DelayTime::copy() const
    {
        return new DelayTime(m_duration);
    }
    
    
    DelayTime* DelayTime::reverse() const
    {
        return copy();
    }
    
    
#pragma mark - Animate
    
    Animate::Animate(Animation *animation)
    : ActionInterval(0)
    , p_animation(animation)
    , m_nextFrame(0)
    , m_executedLoops(0)
    {
        FZ_ASSERT(animation != NULL, "Argument Animation must be non-nil");
        animation->retain();
        
        fzFloat singleDuration = animation->getDuration();
        m_duration = singleDuration * animation->getLoops();

            
        p_splitTimes = new fzFloat[animation->getFrames().size()];
        
        fzUInt i = 0;
        fzFloat accumUnitsOfTime = 0;
        fzFloat newUnitOfTimeValue = singleDuration / animation->getTotalDelayUnit();
            
        vector<fzAnimationFrame>::const_iterator it(animation->getFrames().begin());
        for(; it != animation->getFrames().end(); ++it, ++i) {
            p_splitTimes[i] = (accumUnitsOfTime * newUnitOfTimeValue) / singleDuration;
            accumUnitsOfTime += it->delay;
        }
    }
    
    
    Animate::~Animate()
    {
        // if pointer is non-NULL, we release it.
        FZ_SAFE_RELEASE(p_animation);
    }
    
    
    Animation* Animate::getAnimation() const
    {
        return p_animation;
    }
    
    
    void Animate::startWithTarget(void* target)
    {
        //FZ_ASSERT(dynamic_cast<Sprite*>(target), "Target is not a Sprite");
        ActionInterval::startWithTarget(target);
        Sprite *sprite = static_cast<Sprite*>(target);
        
        if( p_animation->restoreOriginalFrame() )
            m_origFrame = sprite->getDisplayFrame();
        
        m_nextFrame = 0;
        m_executedLoops = 0;  
    }
    
    
    void Animate::stop()
    {
        if( p_animation->restoreOriginalFrame() ) {
            Sprite *sprite = static_cast<Sprite*>(p_target);
            sprite->setDisplayFrame(m_origFrame);
        }
        
        ActionInterval::stop();
    }
    
    
    void Animate::update(fzFloat t)
    {
        // if t==1, ignore. Animation should finish with t==1
        if( t < 1.0f ) {
            t *= p_animation->getLoops();
            
            if( static_cast<fzUInt>(t) > m_executedLoops) {
                m_nextFrame = 0;
                ++m_executedLoops;
            }
            
            // new t for animations
            t = fmodf(t, 1.0f);
        }
        
        auto& frames = p_animation->getFrames();
        
        fzUInt numberOfFrames = frames.size();
        
        for( fzUInt i = m_nextFrame; i < numberOfFrames; ++i ) {
            fzFloat splitTime = p_splitTimes[i];
            
            if( splitTime <= t ) {
                const fzAnimationFrame& anim = frames.at(i);
                const fzSpriteFrame& frame = anim.frame;
                Sprite *sprite = static_cast<Sprite*>(p_target);
                sprite->setDisplayFrame(frame);
                
                m_nextFrame = i+1;
                break;
            }
        }
    }
    
    
    Animate* Animate::copy() const
    {
        
    }

    Animate* Animate::reverse() const
    {
        /*
        NSArray *oldArray = [animation_ frames];
        NSMutableArray *newArray = [NSMutableArray arrayWithCapacity:[oldArray count]];
        NSEnumerator *enumerator = [oldArray reverseObjectEnumerator];
        for (id element in enumerator)
            [newArray addObject:[[element copy] autorelease]];
        
        CCAnimation *newAnim = [CCAnimation animationWithAnimationFrames:newArray delayPerUnit:animation_.delayPerUnit loops:animation_.loops];
        newAnim.restoreOriginalFrame = animation_.restoreOriginalFrame;
        return [[self class] actionWithAnimation:newAnim];
        */
        return NULL;
    }

}
