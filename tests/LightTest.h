

#include "TestBase.h"

using namespace FORZE;

#define LIGHT_TAG 2

class LightBasic : public TestLayer
{
    Light *light;

public:
    LightBasic()
    : TestLayer("LightBasic", NULL)
    {
        SpriteBatch *batch = new SpriteBatch("blocks.png");
        addChild(batch);
        
        for(fzInt i = 0; i < 60; ++i) {
            Sprite *sprite = new Sprite(fzRect(0, 0, 16, 16));
            sprite->setPosition(getContentSize().width * FZ_RANDOM_0_1(),
                                getContentSize().height * FZ_RANDOM_0_1());
            
            batch->addChild(sprite);
        }
        
        LightSystem *system = new LightSystem("light.pvr", batch);
        addChild(system);
        
        light = new Light();
        light->setTag(LIGHT_TAG);
        system->addChild(light);
        
        nextStep();
    }
    
    
    void nextStep()
    {
        fzPoint nextPosition(getContentSize().width * FZ_RANDOM_0_1(),
                             getContentSize().height * FZ_RANDOM_0_1());
        
      light->runAction(new Sequence(new FORZE::MoveTo(2, nextPosition),
                                      new CallFunc(this, SEL_VOID(LightBasic::nextStep)), NULL));
    }
};


class LightTMX : public TestLayer
{   
    LightSystem *system;
public:
    LightTMX()
    : TestLayer("LightBasic", NULL)
    {
        setTrackedEvents(kFZEventType_Tap);
        
        TMXTiledMap *map = new TMXTiledMap("orthogonal-test6.tmx");
        addChild(map);

        system = new LightSystem("light.pvr", map->getChildByName("Walls"));
        addChild(system);
        
        for(fzInt i = 0; i < 5; ++i) {
            Light *light = new Light();
            light->setAlpha(100);
            system->addChild(light);
        }
    }
    
    
    bool event(Event& event)
    {
        // YOU ONLY WILL RECEIVE TOUCH EVENTS
        
        switch (event.getState()) {
            case kFZEventState_Began:
            {
                Light *light;
                FZ_LIST_FOREACH(system->getChildren(), light) {
                    if(light->getBoundingBox().contains(event.getPoint())) {
                        event.setUserData(light);
                        return true;
                    }
                }
                return false;
            }
            default:
            {
                Light *light = static_cast<Light*>(event.getUserData());
                if(light)
                    light->setPosition(event.getPoint());
                
                return false;
            }
        }
    }
};
