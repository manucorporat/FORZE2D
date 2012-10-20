

#include "TestBase.h"

using namespace FORZE;

class LayerTest : public TestLayer
{
public:
    LayerTest()
    : TestLayer("Layer color", NULL)
    {
        LayerColor *background = new LayerColor(fzGRAY);
        background->setIsRelativeAnchorPoint(true);
        background->setPosition(getContentSize()/2);
        addChild(background, -1);
        
        LayerColor *layer1 = new LayerColor(fzGREEN, getContentSize()/2);
        layer1->setPosition(0, 0);
        LayerColor *layer2 = new LayerColor(fzRED, getContentSize()/2);
        layer2->setPosition(getContentSize().width/2, 0);
        LayerColor *layer3 = new LayerColor(fzYELLOW, getContentSize()/2);
        layer3->setPosition(0, getContentSize().height/2);
        LayerColor *layer4 = new LayerColor(fzBLUE, getContentSize()/2);
        layer4->setPosition(getContentSize()/2);
        
        
        background->addChild(layer1);
        background->addChild(layer2);
        background->addChild(layer3);
        background->addChild(layer4);
        
        Action *rotate = new RepeatForever(new Sequence(new RotateBy(2, 180),
                                                        new DelayTime(1), NULL));
        
        background->runAction(rotate);
        layer1->runAction(rotate->copy());
        layer2->runAction(rotate->copy());
        layer3->runAction(rotate->copy());
        layer4->runAction(rotate->copy());
    }
};

class LayerTest2 : public TestLayer
{
    float angle_;
public:
    LayerTest2()
    : TestLayer("Layer color", NULL)
    {
        LayerGradient *background = new LayerGradient(fzBLACK, fzGRAY);
        ActionInterval *action = new TintBy(1, 255, 255, 255);
        background->runAction(new RepeatForever(new Sequence(action, action->reverse(), NULL)));
        addChild(background, -1);
        
        
        // COMPRESSED INTERPOLATION IS DISABLED
        LayerGradient *layer = new LayerGradient(fzBLUE, fzGREEN, getContentSize()/2);
        layer->setIsRelativeAnchorPoint(true);
        layer->setAnchorPoint(0, 0.5f);
        layer->setPosition(0, getContentSize().height/2);
        layer->setCompressedInterpolation(false);
        layer->setName("layer");
        addChild(layer);
        
        
        // COMPRESSED INTERPOLATION IS ENABLED
        LayerGradient *layer2 = new LayerGradient(fzBLUE, fzGREEN, getContentSize()/2);
        layer2->setIsRelativeAnchorPoint(true);
        layer2->setAnchorPoint(0, 0.5f);
        layer2->setPosition(getContentSize()/2);
        layer2->setCompressedInterpolation(true);
        layer2->setName("layer2");
        addChild(layer2);
        
        schedule();
    }
    
    
    void update(fzFloat dt)
    {
        fzPoint vector(cosf(angle_), sinf(angle_));
        LayerGradient *layer = (LayerGradient*)getChildByName("layer");
        layer->setVector(vector);

        LayerGradient *layer2 = (LayerGradient*)getChildByName("layer2");
        layer2->setVector(vector);

        angle_ += FZ_DEGREES_TO_RADIANS(90) * dt;
    }
};

