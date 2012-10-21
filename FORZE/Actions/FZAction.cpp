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

#include "FZAction.h"
#include "FZActionInterval.h"
#include "FZNode.h"
#include "FZDirector.h"
#include "FZMacros.h"


namespace FORZE {
    
#pragma mark - Action
    
    Action::Action()
    : p_target(NULL)
    , m_tag(kFZActionTagInvalid)
    { }
    
    
    bool Action::isDone() const
    {
        return true;
    }
    
    
    void Action::startWithTarget(void *target)
    {
        FZ_ASSERT(target != NULL, "Argument target must be non-NULL");
        p_target = target;
    }
    
    
    void Action::stop()
    {
        p_target = NULL;
    }
    
    
    void Action::finish()
    {
        fzUInt iterations = 0;
        
        do
        {
            step(FLT_MAX);
            ++iterations;
            if(iterations > 200) {
                FZ_ASSERT(false, "Too many iterations, infinite actions cannot be finished.");
                break;
            }
        } while(!isDone());
    }
    
    
    Action* Action::reverse() const
    {
        FZLOGERROR("Action: Reversed action not implemented.");
        return NULL;
    }
    
    
    Action* Action::copy() const
    {
        FZLOGERROR("Action: Copied action not implemented.");
        return NULL;
    }
    
    
#pragma mark - FiniteTimeAction
    
    FiniteTimeAction::FiniteTimeAction()
    : m_duration(0)
    { }
    
    void FiniteTimeAction::setDuration(fzFloat duration)
    {
        FZ_ASSERT(duration >= 0, "Duration must be positive");
        m_duration = (duration < FLT_EPSILON) ? FLT_EPSILON : duration;
    }
    
    
    FiniteTimeAction* FiniteTimeAction::reverse() const
    {
        Action::reverse();
        return NULL;
    }
    
    
    fzFloat FiniteTimeAction::getElapsed() const
    {
        return 0;
    }
    
    
    FiniteTimeAction* FiniteTimeAction::copy() const
    {
        Action::reverse();
        return NULL;
    }
    
    
#pragma mark - RepeatForever
    
    RepeatForever::RepeatForever(FiniteTimeAction *action)
    : p_innerAction(action)
    {
        FZ_ASSERT(p_innerAction != NULL, "Action cannot be NULL");
        p_innerAction->retain();
    }
    
    
    RepeatForever::~RepeatForever()
    {
        setInnerAction(NULL);
    }
    
    
    void RepeatForever::setInnerAction(FiniteTimeAction *action)
    {
        FZRETAIN_TEMPLATE(action, p_innerAction);
    }
    
    
    void RepeatForever::startWithTarget(void *t)
    {
        Action::startWithTarget(t);
        p_innerAction->startWithTarget(t);
    }
    
    
    bool RepeatForever::isDone() const
    {
        return false;
    }
    
    
    void RepeatForever::step(fzFloat dt)
    {
        p_innerAction->step(dt);
        if( p_innerAction->isDone() ) {
            fzFloat diff = dt + p_innerAction->getDuration() - p_innerAction->getElapsed();
            p_innerAction->startWithTarget(p_target);
            p_innerAction->step(diff);
        }
    }
    
    
    RepeatForever* RepeatForever::reverse() const
    {
        return new RepeatForever(p_innerAction->reverse());
    }
    
    
    RepeatForever* RepeatForever::copy() const
    {
        return new RepeatForever(p_innerAction->copy());
    }

    
    
#pragma mark - Speed
    
    Speed::Speed(Action *a, fzFloat s)
    : p_innerAction(a)
    , m_speed(s)
    {
        FZ_ASSERT(p_innerAction != NULL, "ActionInterval cannot be NULL");
        p_innerAction->retain();
    }
    
    
    Speed::~Speed()
    {
        setInnerAction(NULL);
    }
    
    
    void Speed::setInnerAction(Action *action)
    {
        FZRETAIN_TEMPLATE(action, p_innerAction);
    }
    
    
    void Speed::setSpeed(fzFloat s)
    {
        m_speed = s;
    }
    
    
    Action* Speed::getInnerAction() const
    {
        return p_innerAction;
    }
    
    
    fzFloat Speed::getSpeed() const
    {
        return m_speed;
    }
    
    
    void Speed::startWithTarget(void *t)
    {
        Action::startWithTarget(t);
        p_innerAction->startWithTarget(t);
    }
    
    
    void Speed::stop()
    {
        p_innerAction->stop();
        Action::stop();
    }
    
    
    void Speed::step(fzFloat dt)
    {
        p_innerAction->step(dt * m_speed);
    }
    
    
    bool Speed::isDone() const
    {
        return p_innerAction->isDone();
    }
    
    
    Speed* Speed::reverse() const
    {
        return new Speed(p_innerAction->reverse(), m_speed);
    }

    
    Speed* Speed::copy() const
    {
        return new Speed(p_innerAction->copy(), m_speed);
    }
    
    
#pragma mark - Speed
    
    Step::Step(Action *a, fzFloat s)
    : p_innerAction(a)
    , m_step(s)
    , m_elapsed(0)
    {
        FZ_ASSERT(p_innerAction != NULL, "ActionInterval cannot be NULL");
        p_innerAction->retain();
    }
    
    
    Step::~Step()
    {
        setInnerAction(NULL);
    }
    
    
    void Step::setInnerAction(Action *action)
    {
        FZRETAIN_TEMPLATE(action, p_innerAction);
    }
    
    
    void Step::setStep(fzFloat s)
    {
        m_step = s;
    }
    
    
    Action* Step::getInnerAction() const
    {
        return p_innerAction;
    }
    
    
    fzFloat Step::getStep() const
    {
        return m_step;
    }
    
    
    void Step::startWithTarget(void *t)
    {
        Action::startWithTarget(t);
        p_innerAction->startWithTarget(t);
    }
    
    
    void Step::stop()
    {
        p_innerAction->stop();
        Action::stop();
    }
    
    
    void Step::step(fzFloat dt)
    {
        m_elapsed += dt;
        if(m_elapsed >= m_step) {
            p_innerAction->step(m_step);
            m_elapsed -= m_step;
        }
    }
    
    
    bool Step::isDone() const
    {
        return p_innerAction->isDone();
    }
    
    
    Step* Step::reverse() const
    {
        return new Step(p_innerAction->reverse(), m_step);
    }
    
    
    Step* Step::copy() const
    {
        return new Step(p_innerAction->copy(), m_step);
    }
    
    
#pragma mark - Follow
    
    Follow::Follow(Node *n)
    : followedNode_(n)
    , boundarySet_(false)
    , boundaryFullyCovered_(false)
    , fullScreenSize_(Director::Instance().getCanvasSize())
    , halfScreenSize_(fullScreenSize_ * 0.5f)
    , leftBoundary_ (0)
    , rightBoundary_(0)
    , topBoundary_(0)
    , bottomBoundary_(0)
    { }
    
    
    Follow::Follow(Node *n, const fzRect& rect)
    : followedNode_(n)
    , boundarySet_(true)
    , boundaryFullyCovered_(false)
    , fullScreenSize_(Director::Instance().getCanvasSize())
    , halfScreenSize_(fullScreenSize_ * 0.5f)
    , leftBoundary_ ( -rect.origin.x - rect.size.width - fullScreenSize_.x )
    , rightBoundary_( -rect.origin.x )
    , topBoundary_( -rect.origin.y)
    , bottomBoundary_( -rect.origin.y - rect.size.height - fullScreenSize_.y )
    {        
        // screen width is larger than world's boundary width
        // set both in the middle of the world
        if(rightBoundary_ < leftBoundary_)
            rightBoundary_ = leftBoundary_ = (leftBoundary_ + rightBoundary_) / 2;
        
        // screen width is larger than world's boundary width
        // set both in the middle of the world
        if(topBoundary_ < bottomBoundary_)
            topBoundary_ = bottomBoundary_ = (topBoundary_ + bottomBoundary_) / 2;
        
        if( (topBoundary_ == bottomBoundary_) && (leftBoundary_ == rightBoundary_) )
            boundaryFullyCovered_ = true;
    }
    
    
    void Follow::setBoundarySet(bool b)
    {
        boundarySet_ = b;
    }
    
    
    bool Follow::getBoundarySet() const
    {
        return boundarySet_;
    }
    
    
    void Follow::step(fzFloat delta)
    {
#define CLAMP(x,y,z) MIN(MAX(x,y),z)
        
        fzPoint tempPos = halfScreenSize_ - followedNode_->getPosition();
        if(boundarySet_)
        {
            // whole map fits inside a single screen, no need to modify the position - unless map boundaries are increased
            if(boundaryFullyCovered_)
                return;
            
            tempPos.x = CLAMP(tempPos.x, leftBoundary_, rightBoundary_);
            tempPos.y = CLAMP(tempPos.y, bottomBoundary_, topBoundary_);
        }
        ((Node*)p_target)->setPosition(tempPos);
        
#undef CLAMP
    }
    
    
    bool Follow::isDone() const
    {
        return !followedNode_->isRunning();
    }
}
