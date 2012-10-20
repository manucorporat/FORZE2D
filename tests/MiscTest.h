

#include "TestBase.h"

using namespace FORZE;


class SavePermanent : public TestLayer
{
public:
    SavePermanent()
    : TestLayer("Save in ROM", NULL)
    {
        setTrackedEvents(kFZEventType_Tap);
        
        {
            // DEFAULT
            fzSize middle = getContentSize()/2;
            DataStore::Instance().setDefaultFloat(middle.width, "positionX");
            DataStore::Instance().setDefaultFloat(middle.height, "positionY");
            
            fzBuffer data = ResourcesManager::Instance().loadResource("HUD.pvr");
            DataStore::Instance().setDefaultData(data, "image");
        }
        
        // LOAD IMAGE FROM CONTENT IN FILE
        fzBuffer buffer = DataStore::Instance().dataForKey("image");
        Texture2D *texture = new Texture2D();
        texture->loadPVRData(buffer.getPointer());
        
        Sprite *s = new Sprite(texture);
        
        float x = DataStore::Instance().floatForKey("positionX");
        float y = DataStore::Instance().floatForKey("positionY");
        
        s->setPosition(x, y);
        s->setName("grossini");
        addChild(s);
        
        // save new position each 2 seconds
        schedule(SEL_FLOAT(SavePermanent::savePosition), 2);
    }
    
    
    void savePosition() {
        
        FZLog("Saving position");
        Node *sprite = getChildByName("grossini");
        DataStore::Instance().setFloat(sprite->getPosition().x, "positionX");
        DataStore::Instance().setFloat(sprite->getPosition().y, "positionY");
    }
    
    
    bool event(Event& event)
    {
        Node *sprite = getChildByName("grossini");
        sprite->setPosition(event.getPoint());
        return true;
    }
};


class RenderToText : public TestLayer
{
    RenderTexture *render_;
public:
    RenderToText()
    : TestLayer("RenderTexture", NULL)
    {
        setTrackedEvents(kFZEventType_Tap);
        
        render_ = new RenderTexture(FZ_CANVAS_SIZE()/2, kFZTextureFormat_RGBA8888);
        render_->runAction(new RepeatForever(new Sequence(new MoveBy(3, FZ_CANVAS_SIZE()/2),
                                                          new MoveBy(3, FZ_CANVAS_SIZE()/-2), NULL)));
        addChild(render_);
        
        
        
        Sprite *sprite = new Sprite("grossini.png");
        sprite->setPosition(FZ_CANVAS_SIZE()/2);
        sprite->setName("grossini");
        
        addChild(sprite);
        
        schedule();
    }
    
    
    void update(fzFloat dt)
    {
        Node *node = getChildByName("grossini");
        render_->begin();
        
        node->visit();
        
        render_->end();
    }
    
    
    bool event(Event& event)
    {
        Node *node = getChildByName("grossini");
        node->setPosition(event.getPoint());
        return true;
    }
};



class MyNode : public Node {
    std::string text;
public:
    MyNode(const std::string& texta) :text(texta) {}
    ~MyNode() {
        FZLog("\"%s\" was dealloced.", text.c_str());
    }
};


class MemoryTest : public TestLayer {
    
    Node *node_;
    
public:
    MemoryTest()
    : TestLayer("Memory test", NULL)
    {
        FZLog("START");
        // This objects won't leak!! they will be autoreleased
        // At init the retain count is 1
        // and autorelease() is performed, this method releases one
        // when all tasks finish.
        // In this case, retain count becames 0 and "delete" is called.
        new MyNode("My stupid node 1");
        new MyNode("My stupid node 2");
        new MyNode("My stupid node 3");
        
        
        // Now we will retain our node to prevent it from removing
        node_ = new MyNode("To release in 3 seconds");
        node_->retain();
        schedule(SEL_FLOAT(MemoryTest::remove1), 3);
        
        
        // retain + autorelease -> retain 0 ( deferred release )
        Node *testing = new MyNode("Retain/Release/Autorelease");
        
        testing->retain(); // retain 1
        testing->retain(); // retain 2
        testing->release(); // retain 1
        testing->retain(); // retain 2
        //testing->autorelease(); // retain 1 (deferred release)
        //testing->autorelease(); // retain 0 THE OBJECT WILL BE DELETED (deferred release)
        
        
        
        // retain + autorelease -> retain 0 ( deferred release )
        Label *label = new Label("Hello World", "helvetica.fnt");
        label->setName("Label");
        label->setPosition( getContentSize()/2 );
        
        // retain 1, addChild() retain the label, so it won't be deleted
        addChild(label);
        schedule(SEL_FLOAT(MemoryTest::removeLabel), 5);
        
        FZLog("END");
    }
    
    
    ~MemoryTest()
    {
        if(node_) {
            node_->release();
            node_ = NULL;
        }
        // or
        // FZ_SAFE_RELEASE(node_);
    }
    
    
    void remove1(fzFloat)
    {
        FZ_SAFE_RELEASE(node_);
        // or
//        if(node_) {
//            node_->release();
//            node_ = NULL;
//        }
        unscheduleCurrent();
    }
    
    
    void removeLabel(fzFloat)
    {
        Node *label = getChildByName("Label");
        
        // addChild retained the child, now, removeChild will release it
        // if we have not retained it manually, the retain count will be 0
        // and the object will be deleted. (automatically)
        removeChild(label);
        
        unscheduleCurrent();
    }
    
};

class FullScreen : public TestLayer {
public:
    FullScreen()
    : TestLayer("Fullscreen", NULL)
    {
        MenuItemLabel *item = new MenuItemLabel(string(), "helvetica.fnt", this, SEL_PTR(FullScreen::toggleScreen));
        item->setPosition(getContentSize()/2);
        
        if(Director::Instance().isFullscreen())
            item->setString("Exit fullscreen");
        else
            item->setString("Enter fullscreen");
        
        addChild(item);
    }
    
    
    void toggleScreen(MenuItemLabel *sender)
    {
        if(Director::Instance().isFullscreen()) {
            Director::Instance().setWindowSize(FZ_CANVAS_SIZE());
            sender->setString("Enter fullscreen");
        }else{
            Director::Instance().setFullscreen();
            sender->setString("Exit fullscreen");
        }
    }
};



