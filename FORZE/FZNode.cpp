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

#include "FZNode.h"
#include "FZFilter.h"
#include "FZCamera.h"
#include "FZMacros.h"
#include "FZDirector.h"
#include "FZActionManager.h"
#include "FZScheduler.h"
#include "FZShaderCache.h"
#include "FZMath.h"
#include "FZMS.h"
#include "FZGLState.h"
#include "FZGrabber.h"


namespace FORZE {
    
#pragma mark - FORZE BASIC ENTITY

    Node::Node()
    : m_children                 ()
    , m_dirtyFlags               (kFZDirty_all)
    , m_isRunning                (false)
    , m_isVisible                (true)
    , m_isRelativeAnchorPoint    (true)
    , m_rotation                 (0)
    , m_scaleX                   (1)
    , m_scaleY                   (1)
    , m_vertexZ                  (0)
    , m_opacity                  (1)
    , m_cachedOpacity            (1)
    , m_skewX                    (0)
    , m_skewY                    (0)
    , m_zOrder                   (0)
    , m_realZOrder               (0)
    , m_position                 (FZPointZero) // default value
    , m_anchorPointInPoints      (FZPointZero) // default value
    , m_anchorPoint              (FZPointZero) // default value
    , m_contentSize              (FZSizeZero) // default value
    , m_tag                      (kFZNodeTagInvalid)
    , p_grid                     (NULL)
    , p_camera                   (NULL)
    , p_userData                 (NULL)
    , p_parent                   (NULL)
    , p_FBO                      (NULL)
#if FZ_GL_SHADERS
    , p_glprogram                (NULL)
    , p_filter                   (NULL)
#endif
    { }
    
    
    Node::~Node()
    {
#if FZ_GL_SHADERS
        setFilter(NULL);
        setGLProgram(nullptr);
#endif
        if(p_camera)
            delete p_camera;
        
        Node *child;
        FZ_LIST_FOREACH_MUTABLE(m_children, child)
        {
            child->setParent(NULL);
            child->release();
        }
    }
    
    
    void Node::cleanup()
    {
        stopAllActions();
        unscheduleAllSelectors();

        Node *child;
        FZ_LIST_FOREACH(m_children, child) {
            child->cleanup();
        }
    }
    
    
#pragma mark - Setters
    
    void Node::setSkewX(fzFloat sx)
    {
        m_skewX = sx;
        makeDirty(kFZDirty_transform);
    }
    
    
    void Node::setSkewY(fzFloat sy)
    {
        m_skewY = sy;
        makeDirty(kFZDirty_transform);
    }

    
    void Node::setZOrder(fzInt z)
    {
        if(m_zOrder != z) {
            m_zOrder = z;
            if(p_parent)
                p_parent->reorderChild(this);
        }
    }
    
    
    void Node::setAnchorPoint(const fzPoint& p)
    {
        if( m_anchorPoint != p ) {
            m_anchorPoint = p;
            
            m_anchorPointInPoints.x = m_contentSize.width * m_anchorPoint.x;
            m_anchorPointInPoints.y = m_contentSize.height * m_anchorPoint.y;
            makeDirty(kFZDirty_transform);
        }
    }
    
    
    void Node::setAnchorPointInPoints(const fzPoint& p)
    {
        if( m_anchorPointInPoints != p ) {
            m_anchorPointInPoints = p;
            
            m_anchorPoint.x = m_anchorPointInPoints.x / m_contentSize.width;
            m_anchorPoint.y = m_anchorPointInPoints.y / m_contentSize.height;
            makeDirty(kFZDirty_transform);
        }
    }
    
    
    void Node::setContentSize(const fzSize& s)
    {
        if( m_contentSize != s ) {
            m_contentSize = s;
            m_anchorPointInPoints.x = m_contentSize.width * m_anchorPoint.x;
            m_anchorPointInPoints.y = m_contentSize.height * m_anchorPoint.y;
            makeDirty(kFZDirty_transform);
        }
    }
    
    
    void Node::setOpacity(fzFloat o)
    {
        FZ_ASSERT( o >= 0.0f && o <= 1.0f, "Opacity must be in [0, 1].");
        
        if(m_opacity != o) {
            m_opacity = o;
            makeDirty(kFZDirty_opacity);
        }
    }
    
    
    void Node::setName(const char* name)
    {
        FZ_ASSERT( name, "Name can not be NULL.");
        setTag(fzHash(name));
    }
    
    
    void Node::setGLProgram(GLProgram *program)
    {
#if FZ_GL_SHADERS
        FZRETAIN_TEMPLATE(program, p_glprogram);
#else
        FZ_ASSERT(false, "Shaders are not supported.");
#endif
    }
    
    
    void Node::setGLProgram(fzUInt programKey)
    {
#if FZ_GL_SHADERS
        setGLProgram(ShaderCache::Instance().getProgramByKey(programKey));
#else
        FZ_ASSERT(false, "Shaders are not supported.");
#endif
    }
    
    
#pragma mark Postprocessing effects
    
    void Node::allocFBO()
    {
        // The FBO is used to create two kind of effects, Filter, Grid or both.
#if FZ_GL_SHADERS
        bool FBOisNeeded = (p_filter != NULL || p_grid != NULL);
#else
        bool FBOisNeeded = p_grid != NULL;
#endif
        
        if(FBOisNeeded) {
            
            if(p_FBO == NULL)
                p_FBO = new FBOTexture(Texture2D::screenTextureFormat(), Director::Instance().getCanvasSize());
            
        }else if(p_FBO != NULL) {
            delete p_FBO;
            p_FBO = NULL;
        }
    }
    
    
    void Node::setFilter(Filter *filter)
    {
        FZ_ASSERT(FZ_GL_SHADERS, "Shaders are not supported.");
        
#if FZ_GL_SHADERS
        if(filter == p_filter)
            return;
                
        FZRETAIN_TEMPLATE(filter, p_filter);
        allocFBO();
#endif
    }
    
    
#pragma mark - Getters
    
    fzFloat Node::getScale() const
    {
        FZ_ASSERT( m_scaleX == m_scaleY, "Node: ScaleX != ScaleY. Don't know which one to return.");
        return m_scaleX;
    }
    
    
    fzFloat Node::getSkewX() const
    {
        return m_skewX;
    }
    
    
    fzFloat Node::getSkewY() const
    {
        return m_skewY; 
    }
    
    
    Camera* Node::getCamera()
    {
        if(!p_camera )
            p_camera = new Camera(this);
        
        return p_camera;
    }

    
    fzRect Node::getBoundingBox()
    {
        fzRect rect(FZPointZero, m_contentSize);
      
        // m_transformMV is the absolute transform, after applying this transform the space is normalized
        // x(-1, 1) y(-1, 1), and the point (0,0) is the center of the screen.
        rect.applyTransform(m_transformMV);
      
        // unnormalize rect to natural reference system.
        return Director::Instance().unnormalizedRect(rect);
    }
    
    
    fzRect Node::getLocalBoundingBox()
    {
        fzRect rect(FZPointZero, m_contentSize);
        rect.applyTransform(getNodeToParentTransform());
        return rect;
    }
    
    
    GLProgram* Node::getGLProgram() const
    {
#if FZ_GL_SHADERS
        return p_glprogram;
#else
        return NULL;
#endif
    }

    
#pragma mark - Children management
    
    Node* Node::getChildByTag(fzInt tag)
    {
        FZ_ASSERT(tag != kFZNodeTagInvalid, "Invalid tag.");
        
        Node *child;
        FZ_LIST_FOREACH(m_children, child) {
            if( child->getTag() == tag )
                return child;
        }
        return NULL;
    }
    
    
    Node* Node::getChildByName(const char* name)
    {
        FZ_ASSERT(name, "Name argument can not be NULL.");
        return getChildByTag(fzHash(name));
    }
    
  
    void Node::addChild(Node* node)
    {
        addChild(node, node->getZOrder());
    }
    
  
    void Node::addChild(Node *node, fzInt z)
    {
        FZ_ASSERT( node != NULL, "Argument must be non-NULL.");
        FZ_ASSERT( node->getParent() == NULL, "This node was already attached.");

        node->setZOrder(z);
      
        insertChild(node);
    }
    
    
    void Node::removeFromParent(bool clean)
    {
        if(p_parent)
            p_parent->removeChild(this, clean);
    }
    
    
    void Node::removeChild(Node *child, bool clean)
    {
        if(child == NULL)
            return;
        
        child->retain();
        if(detachChild(child, clean))
            m_children.remove(child);
        
        child->release();
    }
    
    
    void Node::removeChildByTag(fzInt tag, bool clean)
    {
        removeChild(getChildByTag(tag), clean);
    }
    
    
    void Node::removeChildByName(const char* name, bool clean)
    {
        removeChild(getChildByName(name), clean);
    }
    
    
    void Node::removeAllChildren(bool clean)
    {
        Node *child;
        FZ_LIST_FOREACH_MUTABLE(m_children, child) {
            detachChild(child, clean);
        }
      
        m_children.clear();
    }
    
    
    void Node::insertChild(Node* child)
    {
        // insert node at position giving a zOrder
        m_children.insert(indexForZOrder(child->getZOrder()), child);
        child->m_realZOrder = child->getZOrder();
        
        // set parent (after attach to list)
        child->setParent(this);

        // retain child
        child->retain();
        
        // onEnter() if this parent is running
        if(m_isRunning)
            child->onEnter();
        
        child->updateLayout();
        makeDirty(0);
    }
  
  
    bool Node::detachChild(Node *child, bool clean)
    {
        FZ_ASSERT(child, "Child argument can not be NULL.");
        
        if(child->getParent() != this) {
            FZ_ASSERT(false, "Trying to detach a non-attached node.");
            return false;
        }
        if (m_isRunning)
            child->onExit();
        
        if (clean)
            child->cleanup();
        
        child->setParent(NULL);
        child->release();
        makeDirty(0);
      
        return true;
    }
    
    
    void Node::reorderChild(Node* child)
    {
        FZ_ASSERT( child, "Child must be non-NULL.");
        FZ_ASSERT( child->getParent() == this, "This node isn't child of this node.");
        
        m_children.move(child, indexForZOrder(child->getZOrder()));
        child->m_realZOrder = child->getZOrder();
        makeDirty(0);
    }
    
    
    fzListItem* Node::indexForZOrder(fzInt z)
    {
        Node *back = static_cast<Node*>(m_children.back());
        
        // quick comparison to improve performance
        if (back == NULL || (back->m_realZOrder <= z))
            return NULL;
        
        else
        {
            Node *child = NULL;
            FZ_LIST_FOREACH(m_children, child)
            {
                if (child->m_realZOrder > z )
                    return child;
            }
        }
        return NULL;
    }
    
    
#pragma mark - Updating protocol
    
    void Node::visit()
    {
        // This method (visit()) will be only called from and external developer
        // so to ensure all looks great, we make the node dirty by default.
        makeDirty(kFZDirty_transform_absolute);
        internalVisit();
        makeDirty(kFZDirty_transform_absolute);
    }
    
    
    void Node::internalVisit()
    {
        // If the node is not visible, we stop the rendering here.
        if( !m_isVisible)
            return;
        
        // BEGIN FBO
        // When a Filter or grid effect is enabled, the node is rendered inside a FBO.
        if(p_FBO)
            p_FBO->beginWithClear();
        
        internalRender();
        
        
        if(p_FBO) {
            p_FBO->end();
            
#if FZ_GL_SHADERS
            if(p_filter) {
                p_filter->setTexture(p_FBO->getTexture());
                p_filter->draw();
            }else
#endif
            {
                FZ_ASSERT(false, "GRID SUPPORT IS NOT AVAILABLE.");
            }
        }
    }
    
    
    void Node::internalRender()
    {
        char dirtyFlags = m_dirtyFlags & kFZDirty_recursive;
        if(m_dirtyFlags != 0) {
            updateStuff();
            m_dirtyFlags = 0;
        }
        
        MS::pushMatrix(m_transformMV);
        {
            render(dirtyFlags);
        }
        MS::pop();
    }
    
    
    void Node::render(unsigned char dirtyFlags)
    {
        // ITERATE CHILDREN
        Node *child = static_cast<Node*>(m_children.front());
        for(; child; child = static_cast<Node*>(child->next())) {
            if ( child->m_realZOrder < 0 ) {
                child->m_dirtyFlags |= dirtyFlags;
                child->internalVisit();
            }else
                break;
        }
        
        draw();
        
        for(; child; child = static_cast<Node*>(child->next())) {
            child->m_dirtyFlags |= dirtyFlags;
            child->internalVisit();
        }
    }
    
    
    void Node::updateStuff()
    {
        // UPDATE TRANSFORM
        if( m_dirtyFlags & kFZDirty_transform_absolute )
            fzMath_mat4Multiply(MS::getMatrix(), getNodeToParentTransform().m, m_transformMV);
        
        
        // UPDATE OPACITY
        if(m_dirtyFlags & kFZDirty_opacity) {
            m_cachedOpacity = m_opacity * p_parent->getCachedOpacity();
            m_dirtyFlags |= kFZDirty_color;
        }
    }

    
    void Node::draw()
    {
        // override me
    }
    
    
#pragma mark - Scene Management
    
    void Node::onEnter()
    {
        if(m_isRunning)
            return;
        
        Node *child;
        FZ_LIST_FOREACH_REVERSE(m_children, child)
        {
            child->onEnter();
        }
        
        resumeSchedulerAndActions();
        
        m_isRunning = true;
    }
    
    
    void Node::onExit()
    {
        if(!m_isRunning)
            return;
        
        pauseSchedulerAndActions();
        m_isRunning = false;	

        Node *child;
        FZ_LIST_FOREACH(m_children, child)
        {
            child->onExit();
        }
    }
    
    
    void Node::updateLayout()
    {
        makeDirty(kFZDirty_transform);
        Node *child;
        FZ_LIST_FOREACH(m_children, child)
        {
            child->updateLayout();
        }
    }
    
    
#pragma mark - Actions
    
    Action* Node::runAction(Action *action)
    {        
        ActionManager::Instance().addAction(action, this, !m_isRunning);
        return action;
    }
    
    
    void Node::stopAction(Action *action)
    {
        ActionManager::Instance().removeAction(action);
    }
    
    
    void Node::stopActionByTag(fzInt tag)
    {
        ActionManager::Instance().removeAction(tag, this);
    }
    
    
    void Node::stopAllActions()
    {
        ActionManager::Instance().removeAllActions(this);
    }
    
    
    Action* Node::getActionByTag(fzInt tag)
    {
        return ActionManager::Instance().getActionByTag(tag, this);
    }
    
    
    fzUInt Node::numberOfRunningActions()
    {
        return ActionManager::Instance().getNumberActions(this);
    }
    
    
#pragma mark - Scheduler
    
    void Node::update(fzFloat)
    {
        static bool p = true;
        if(p) {
            FZLOGERROR("Node: update() method should be redefined.");
            p = false;
        }
    }
    
    void Node::schedule()
    {
        schedule(SEL_FLOAT(Node::update), 0);
    }

    
    void Node::schedule(const SELECTOR_FLOAT selector, fzFloat interval)
    {        
        Scheduler::Instance().scheduleSelector(selector, this, interval, !m_isRunning);
    }
    
    
    void Node::unschedule()
    {
        unschedule(SEL_FLOAT(Node::update));
    }
    
    
    void Node::unschedule(const SELECTOR_FLOAT selector)
    {      
        Scheduler::Instance().unscheduleSelector(selector, this);
    }
    
    
    void Node::unscheduleCurrent()
    {
        Timer *timer = Scheduler::Instance().getCurrentTimer();
        if(timer == NULL)
            return;
        
        if(timer->getTarget() == this)
            unschedule(timer->getSelector());
    }
    
    
    void Node::unscheduleAllSelectors()
    {
        Scheduler::Instance().unscheduleAllSelectors(this);
    }
    
    
    void Node::resumeSchedulerAndActions()
    {
        Scheduler::Instance().resumeTarget(this);
        ActionManager::Instance().resumeTarget(this);
    }
    
    
    void Node::pauseSchedulerAndActions()
    {
        Scheduler::Instance().pauseTarget(this);
        ActionManager::Instance().pauseTarget(this);
    }
    

#pragma mark - Miscelaneous
    
    static bool allNodes(Node*) {
        return true;
    }

    void Node::alignVertically(fzFloat padding,
                               const fzPoint& center,
                               const fzRange& range,
                               bool (*func)(Node*))
    {
        if(range.size == 0)
            return;
        
        fzUInt index = 0;
        fzUInt endIndex = range.endIndex();
        fzFloat height = -padding;
        
        Node *child;
        FZ_LIST_FOREACH(m_children, child) {
            if(index >= range.origin && func(child))
                height += child->getContentSize().height * child->getScaleY() + padding;
            
            ++index;
            if(index >= endIndex) break;
        }
        
        index = 0;
        fzFloat y = center.y + height / 2;
        FZ_LIST_FOREACH(m_children, child) {
            if(index >= range.origin && func(child)) {
                fzFloat k = child->getContentSize().height * child->getScaleY();
                child->setPosition(center.x, y - k/2.0f );
                y -= k + padding;
            }
            
            ++index;
            if(index >= endIndex)
                break;
        }
    }
    
    
    void Node::alignHorizontally(fzFloat padding,
                                 const fzPoint& center,
                                 const fzRange& range,
                                 bool (*func)(Node*))
    {        
        if(range.size == 0)
            return;
        
        fzUInt index = 0;
        fzUInt endIndex = range.endIndex();
        fzFloat width = -padding;

        Node *child;
        FZ_LIST_FOREACH(m_children, child) {
            if(index >= range.origin && func(child))
                width += child->getContentSize().width * child->getScaleX() + padding;
            
            ++index;
            if(index >= endIndex)
                break;
        }
        
        index = 0;
        fzFloat x = center.x - width / 2;
        FZ_LIST_FOREACH(m_children, child) {
            if(index >= range.origin && func(child)) {
                const fzFloat k = child->getContentSize().width * child->getScaleX();
                child->setPosition(x + k / 2.0f, center.y);
                x += k + padding;
            }
            
            ++index;
            if(index >= endIndex)
                break;
        }
    }
    
    
    void Node::alignVertically(fzFloat padding, const fzPoint& center, const fzRange& range)
    {
        alignVertically(padding, center, range, allNodes);
    }
    
    
    void Node::alignHorizontally(fzFloat padding, const fzPoint& center, const fzRange& range)
    {
        alignHorizontally(padding, center, range, allNodes);
    }
    
    
    void Node::alignChildrenVertically(fzFloat padding)
    {
        alignVertically(padding, m_contentSize/2, fzRange(0, m_children.size()));
    }
    
    
    void Node::alignChildrenHorizontally(fzFloat padding)
    {
        alignHorizontally(padding, m_contentSize/2, fzRange(0, m_children.size()));
    }
    
    
#pragma mark - Transforms
    
    const fzAffineTransform& Node::getNodeToParentTransform()
    {
        if ( m_dirtyFlags & kFZDirty_transform_relative ) {
            
            // Relative position
            fzPoint translation(m_position);
            if( !m_isRelativeAnchorPoint )
                translation += m_anchorPointInPoints;
            
            
            // Rotation optimization
            float c = 1, s = 0;
            if( m_rotation ) {
                float radians = -FZ_DEGREES_TO_RADIANS(m_rotation);
                c = fzMath_cos(radians);
                s = fzMath_sin(radians);
            }

            
            // Updating the transform
            float data[] = {
                c * m_scaleX,  s * m_scaleX,
                -s * m_scaleY, c * m_scaleY,
                translation.x, translation.y, m_vertexZ
            };
            m_transform.assign(data);
            
            // Apply camera transform
            if(p_camera != NULL)
                m_transform.concat(p_camera->getLookupMatrix());

            
            // Apply anchor point
            if( m_anchorPointInPoints != FZPointZero )
                m_transform.translate(-m_anchorPointInPoints.x, -m_anchorPointInPoints.y, 0);
            
            m_dirtyFlags &= ~kFZDirty_transform_relative;
        }
        return m_transform;
    }
    
    
    fzAffineTransform Node::getParentToNodeTransform()
    {
        return m_transform.getInverse();
    }
    
    
    fzAffineTransform Node::getNodeToWorldTransform()
    {
        return getWorldToNodeTransform().getInverse();
    }
    
    
    fzAffineTransform Node::getWorldToNodeTransform()
    {
        return fzAffineTransform(m_transformMV);
    }
    
    
    fzPoint Node::convertToNodeSpace(fzPoint worldPoint)
    {
        return worldPoint.applyTransform(getWorldToNodeTransform());
    }
    
    
    fzPoint Node::convertToWorldSpace(fzPoint nodePoint)
    {
        return nodePoint.applyTransform(getNodeToWorldTransform());
    }
    
    
    fzPoint Node::convertToNodeSpaceAR(const fzPoint& worldPoint)
    {
        fzPoint point = convertToNodeSpace(worldPoint) - m_anchorPointInPoints;
        return point;
    }
    
    
    fzPoint Node::convertToWorldSpaceAR(const fzPoint& nodePoint)
    {
        fzPoint point = convertToWorldSpace(nodePoint + m_anchorPointInPoints);
        return point;
    }
}
