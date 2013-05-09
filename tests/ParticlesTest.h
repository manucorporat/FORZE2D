

#include "TestBase.h"

using namespace FORZE;


class ParticleTest : public TestLayer
{
public:
    ParticleTest()
    : TestLayer("Particles test", NULL)
    {
        setTrackedEvents(kFZEventType_Tap);
        
        ParticleFireQuad *fire = new ParticleFireQuad();
        //fire->setScale(0.1f);

        fire->setName("fire");
        addChild(fire);
    }
    
    bool event(Event &event)
    {
        ParticleSystemQuad *system = (ParticleSystemQuad*)getChildByName("fire");
        ((ParticleSystem*)system->getLogic())->setSourcePosition(event.getPoint());
        return true;
    }
};
