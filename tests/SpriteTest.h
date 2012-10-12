

#include "TestBase.h"

using namespace FORZE;

class SpriteTest1 : public TestLayer
{
public:
    SpriteTest1()
    : TestLayer("Add sprite", NULL)
    {
        setTrackedEvents(kFZEventType_Tap);
    }
    
    void addSpriteAt(const fzPoint& pos)
    {        
        int idx = FZ_RANDOM_0_1() * 1400 / 100;
        int x = (idx%5) * 85;
        int y = (idx/5) * 121;
        
        Sprite *sprite = new Sprite("grossini_dance_atlas.png", fzRect(x, y, 85,121));
        sprite->setPosition(pos);
        addChild(sprite);
        
        
        ActionInterval *action;
        fzFloat rand = FZ_RANDOM_0_1();
        
        if( rand < 0.2f )
            action = new ScaleBy(3, 2);
        else if(rand < 0.4f)
            action = new RotateBy(3, 360);
        else if( rand < 0.6f)
            action = new Blink(1, 3, 0.5f);
        else if( rand < 0.8f )
            action = new TintBy(2, 0, -255, -255);
        else
            action = new FadeOut(2);
        
        ActionInterval *action_back = action->reverse();
        Sequence *seq = new Sequence(action, action_back, NULL);
        sprite->runAction(new RepeatForever(seq));        
    }
    
    
    bool event(Event &event)
    {
        if(event.getState() == kFZEventState_Began) {
            addSpriteAt(event.getPoint());
        }
        return false;
    }
};


class SpriteTest2 : public TestLayer
{
public:
    SpriteTest2()
    : TestLayer("Add batch sprite", NULL)
    {
        setTrackedEvents(kFZEventType_Tap);
        
        SpriteBatch *batch = new SpriteBatch("grossini_dance_atlas.png");
        batch->setName("batch");
        addChild(batch);
    }
    
    void addSpriteAt(const fzPoint& pos)
    {
        int idx = FZ_RANDOM_0_1() * 1400 / 100;
        int x = (idx%5) * 85;
        int y = (idx/5) * 121;
        
        SpriteBatch *batch = (SpriteBatch*) getChildByName("batch");
        Sprite *sprite = new Sprite(fzRect(x, y, 85,121));
        sprite->setPosition(pos);
        batch->addChild(sprite);
        
        
        ActionInterval *action;
        fzFloat rand = FZ_RANDOM_0_1();
        
        if( rand < 0.2f )
            action = new ScaleBy(3, 2);
        else if(rand < 0.4f)
            action = new RotateBy(3, 360);
        else if( rand < 0.6f)
            action = new Blink(1, 3, 0.5f);
        else if( rand < 0.8f )
            action = new TintBy(2, 0, -255, -255);
        else
            action = new FadeOut(2);
        
        ActionInterval *action_back = action->reverse();
        Sequence *seq = new Sequence(action, action_back, NULL);
        sprite->runAction(new RepeatForever(seq));
    }
    
    
    bool event(Event &event)
    {
        if(event.getState() == kFZEventState_Began) {
            addSpriteAt(event.getPoint());
        }
        return false;
    }
};


class SpriteOpacityColorTest : public TestLayer
{
public:
    SpriteOpacityColorTest()
    : TestLayer("Sprite test 3", NULL)
    {        
        Sprite *sprite1 = new Sprite("grossini_dance_atlas.png", fzRect(85*0, 121*1, 85, 121));
        Sprite *sprite2 = new Sprite("grossini_dance_atlas.png", fzRect(85*1, 121*1, 85, 121));
        Sprite *sprite3 = new Sprite("grossini_dance_atlas.png", fzRect(85*2, 121*1, 85, 121));
        Sprite *sprite4 = new Sprite("grossini_dance_atlas.png", fzRect(85*3, 121*1, 85, 121));
        
        Sprite *sprite5 = new Sprite("grossini_dance_atlas.png", fzRect(85*1, 121*1, 85, 121));
        sprite5->setName("Sprite5");
        Sprite *sprite6 = new Sprite("grossini_dance_atlas.png", fzRect(85*2, 121*1, 85, 121));
        Sprite *sprite7 = new Sprite("grossini_dance_atlas.png", fzRect(85*3, 121*1, 85, 121));
        Sprite *sprite8 = new Sprite("grossini_dance_atlas.png", fzRect(85*4, 121*1, 85, 121));
        
        
        addChild(sprite1);
        addChild(sprite2);
        addChild(sprite3);
        addChild(sprite4);
        addChild(sprite5);
        addChild(sprite6);
        addChild(sprite7);
        addChild(sprite8);
        
        // ALIGN HORIZONTALLY
        alignHorizontally(0, getContentSize()/2 + fzPoint(0, 80), fzRange(0, 4));
        alignHorizontally(0, getContentSize()/2 + fzPoint(0, -80), fzRange(4, 4));
        
        
        // ACTIONS
        ActionInterval *action = new FadeIn(2);
        ActionInterval *action_back = action->reverse();
        Action *fade = new RepeatForever(new Sequence(action, action_back, NULL));
        
        ActionInterval *tintred = new TintBy(2, 0, -255, -255);
        ActionInterval *tintred_back = tintred->reverse();
        Action *red = new RepeatForever(new Sequence(tintred, tintred_back, NULL));
        
        ActionInterval *tintgreen = new TintBy(2, -255, -255, 0);
        ActionInterval *tintgreen_back = tintgreen->reverse();
        Action *green = new RepeatForever(new Sequence(tintgreen, tintgreen_back, NULL));
        
        ActionInterval *tintblue = new TintBy(2, -255, -255, 0);
        ActionInterval *tintblue_back = tintblue->reverse();
        Action *blue = new RepeatForever(new Sequence(tintblue, tintblue_back, NULL));

        sprite5->runAction(red);
        sprite6->runAction(green);
        sprite7->runAction(blue);
        sprite8->runAction(fade);
        
        
        // SCHEDULE
        schedule(SEL_FLOAT(SpriteOpacityColorTest::removeAndAdd), 2);
    }
    
    
    void removeAndAdd(fzFloat)
    {
        Node *sprite = getChildByName("Sprite5");
        sprite->retain();
        
        removeChild(sprite, false);
        addChild(sprite);
        
        sprite->release();
    }
};


class SpriteBatchOpacityColorTest : public TestLayer
{
    SpriteBatch *batch_;
public:
    SpriteBatchOpacityColorTest()
    : TestLayer("Sprite test 4", NULL)
    {        
        batch_ = new SpriteBatch("grossini_dance_atlas.png");
        addChild(batch_);
        
        Sprite *sprite1 = new Sprite(fzRect(85*0, 121*1, 85, 121));
        Sprite *sprite2 = new Sprite(fzRect(85*1, 121*1, 85, 121));
        Sprite *sprite3 = new Sprite(fzRect(85*2, 121*1, 85, 121));
        Sprite *sprite4 = new Sprite(fzRect(85*3, 121*1, 85, 121));
        
        Sprite *sprite5 = new Sprite(fzRect(85*1, 121*1, 85, 121));
        sprite5->setName("Sprite5");
        Sprite *sprite6 = new Sprite(fzRect(85*2, 121*1, 85, 121));
        Sprite *sprite7 = new Sprite(fzRect(85*3, 121*1, 85, 121));
        Sprite *sprite8 = new Sprite(fzRect(85*4, 121*1, 85, 121));
        
        
        batch_->addChild(sprite1);
        batch_->addChild(sprite2);
        batch_->addChild(sprite3);
        batch_->addChild(sprite4);
        batch_->addChild(sprite5);
        batch_->addChild(sprite6);
        batch_->addChild(sprite7);
        batch_->addChild(sprite8);
        
        // ALIGN HORIZONTALLY
        batch_->alignHorizontally(0, getContentSize()/2 + fzPoint(0, 80), fzRange(0, 4));
        batch_->alignHorizontally(0, getContentSize()/2 + fzPoint(0, -80), fzRange(4, 4));
        
        
        // ACTIONS
        ActionInterval *action = new FadeIn(2);
        ActionInterval *action_back = action->reverse();
        Action *fade = new RepeatForever(new Sequence(action, action_back, NULL));
        
        ActionInterval *tintred = new TintBy(2, 0, -255, -255);
        ActionInterval *tintred_back = tintred->reverse();
        Action *red = new RepeatForever(new Sequence(tintred, tintred_back, NULL));
        
        ActionInterval *tintgreen = new TintBy(2, -255, -255, 0);
        ActionInterval *tintgreen_back = tintgreen->reverse();
        Action *green = new RepeatForever(new Sequence(tintgreen, tintgreen_back, NULL));
        
        ActionInterval *tintblue = new TintBy(2, -255, -255, 0);
        ActionInterval *tintblue_back = tintblue->reverse();
        Action *blue = new RepeatForever(new Sequence(tintblue, tintblue_back, NULL));
        
        sprite5->runAction(red);
        sprite6->runAction(green);
        sprite7->runAction(blue);
        sprite8->runAction(fade);
        
        
        // SCHEDULE
        schedule(SEL_FLOAT(SpriteBatchOpacityColorTest::removeAndAdd), 2);
    }
    
    
    void removeAndAdd(fzFloat)
    {
        Node *sprite = batch_->getChildByName("Sprite5");
        sprite->retain();
        
        batch_->removeChild(sprite, false);
        batch_->addChild(sprite);
        
        sprite->release();
    }
};

class SpriteZOrderTest : public TestLayer
{
    int dir;
public:
    SpriteZOrderTest()
    : TestLayer("Sprite test 4", NULL)
    {
		dir = 1;
        fzSize s = FZ_CANVAS_SIZE();
		float step = s.width/11;
		for(int i = 0; i<5; i++) {
            Sprite *sprite = new Sprite("grossini_dance_atlas.png", fzRect(85*0, 121*1, 85, 121));
            sprite->setPosition((i+1)*step, s.height/2);
            sprite->setTag(i);
            addChild(sprite, i);
		}
        
		for(int i=5; i<10; i++) {
            Sprite *sprite = new Sprite("grossini_dance_atlas.png", fzRect(85*0, 121*0, 85, 121));
            sprite->setPosition((i+1)*step, s.height/2);
            sprite->setTag(i);
            addChild(sprite, 14-i);
		}
        
        Sprite *sprite = new Sprite("grossini_dance_atlas.png", fzRect(85*3, 121*0, 85, 121));
        sprite->setPosition(s/2 + fzPoint(0, -20));
        sprite->setColor(fzRED);
        sprite->setScale(6);
        sprite->setName("mainSprite");
        addChild(sprite, -1);
        
        schedule(SEL_FLOAT(SpriteZOrderTest::reorderSprite), 1);
    }
    
    
    void reorderSprite(fzFloat)
    {
        Node *sprite = getChildByName("mainSprite");
        fzInt z = sprite->getZOrder();
        
        if( z < -1 )
            dir = 1;
        
        if( z > 10 )
            dir = -1;
        
        z += dir * 3;
        
        
        sprite->setZOrder(z);
        
        FZLog("Reorder %d", z);
        Node *node;
        FZ_LIST_FOREACH(getChildren(), node) {
            FZLog("%d %p", node->getTag(), static_cast<fzListItem*>(node));
        }
    }
};


class SpriteFrameTest : public TestLayer
{
public:
    SpriteFrameTest()
    : TestLayer("Sprite frame", NULL)
    {
        SpriteFrameCache::Instance().addSpriteFrames("enemiesSheet.xml");
        
        char textos[][12] = { "EnBlue", "EnRed", "EnTriangle", "EnPink", "EnOrange", "EnPurple", "EnGreen" };
        for(int i = 0; i < 7; ++i) {
            Sprite *sprite = new Sprite();
            sprite->setDisplayFrame(textos[i]);
            addChild(sprite);
        }
        alignChildrenVertically(-15);
    }
};


class SpriteFrameBatchTest : public TestLayer
{
public:
    SpriteFrameBatchTest()
    : TestLayer("Sprite frame batch", NULL)
    {
        SpriteFrameCache::Instance().addSpriteFrames("enemiesSheet.xml");
        
        char textos[][12] = { "EnBlue", "EnRed", "EnTriangle", "EnPink", "EnOrange", "EnPurple", "EnGreen" };
        SpriteBatch *batch = new SpriteBatch("enemiesSheet.pvr");
        addChild(batch);
        for(int i = 0; i < 7; ++i) {
            Sprite *sprite = new Sprite();
            sprite->setDisplayFrame(textos[i]);
            batch->addChild(sprite);
        }
        batch->alignChildrenVertically(-15);
    }
};
