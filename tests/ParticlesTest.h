

#include "TestBase.h"

using namespace FORZE;


class ParticleTest : public TestLayer
{
public:
    ParticleTest()
    : TestLayer("Particles test", NULL)
    {
        setTrackedEvents(kFZEventType_Tap);
        
        ParticleFire *fire = new ParticleFire();
        fire->setName("fire");
        addChild(fire);
    }
    
    bool event(Event &event)
    {
        ((ParticleSystem*)getChildByName("fire"))->setSourcePosition(event.getPoint());
        return true;
    }
};
