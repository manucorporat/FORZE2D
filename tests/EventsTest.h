

#include "TestBase.h"

using namespace FORZE;


#define GROSSINI_TAG 2
class TrackingFingers : public TestLayer
{
public:
    TrackingFingers()
    : TestLayer("Tracking fingers", NULL)
    {
        setTrackedEvents( kFZEventType_Tap );
    }
    
    
    bool event(Event& event)
    {
        fzPoint point = event.getPoint();
           
        switch (event.getState()) {
            case kFZEventState_Began:
            {
                LayerColor *box = new LayerColor(fzWHITE, fzSize(50, 50));
                box->setAnchorPoint(0.5f, 0.5f);
                box->setPosition(point);
                event.setUserData(box);
                addChild(box);
                
                return true;
            }
            case kFZEventState_Updated:
            {
                LayerColor *box = (LayerColor*)event.getUserData();
                box->setPosition(point);
                break;
            }
            case kFZEventState_Cancelled:
            case kFZEventState_Ended:
            {
                LayerColor *box = (LayerColor*)event.getUserData();
                box->removeFromParent();
                break;
            }
            default: break;
        }
        return false;
    }
};


class MovingSprite : public TestLayer
{
public:
    MovingSprite()
    : TestLayer("Moving sprite", NULL)
    {
        setTrackedEvents( kFZEventType_Tap );
        
        Sprite *sprite = new Sprite("grossini.png");
        sprite->setPosition(getContentSize()/2);
        sprite->setTag(GROSSINI_TAG);
        addChild(sprite);
    }
    
    
    bool event(Event& event)
    {
        // YOU ONLY WILL RECEIVE TOUCH EVENTS
        
        switch (event.getState()) {
            case kFZEventState_Began:
            {
                Node *grossini = getChildByTag(GROSSINI_TAG);
                if(grossini) {
                    if(grossini->getBoundingBox().contains(event.getPoint()))
                        return true;
                }
                return false;
            }
            case kFZEventState_Updated:
            case kFZEventState_Cancelled:
            case kFZEventState_Ended:
            {
                Node *grossini = getChildByTag(GROSSINI_TAG);
                if(grossini)
                    grossini->setPosition(event.getPoint());
                break;
            }
            default:
                break;
        }
        return false;
    }
};


class Keyboard : public TestLayer
{
public:
    Keyboard()
    : TestLayer("Keyboard", NULL)
    {
        setTrackedEvents( kFZEventType_Keyboard );
    }
    
    void updateLayout()
    {
        setContentSize(FZ_CANVAS_SIZE());
        Node::updateLayout();
    }
    
    
    bool event(Event& event)
    {
        if(event.getState() == kFZEventState_Updated)
            return false;
        
        fzPoint startPos = fzPoint(0.5f, 0.2f).compMult(getContentSize());
        fzPoint endPos   = fzPoint(0.5f, 1.0f).compMult(getContentSize());
        
        
        std::string text;
        switch(event.getState()) {
            case kFZEventState_Began: text = "Down"; break;
            case kFZEventState_Ended: text = "Up"; break;
            case kFZEventState_Cancelled: text = "Cancelled"; break;
            default: break;
        }
                
        // YOU ONLY WILL RECEIVE TOUCH EVENTS
        Label *label = new Label(FZT("%d %s", event.getIdentifier(), text.c_str()), "font_menu.fnt");
        label->setPosition( startPos );
        ActionInterval *action = new Sequence(new Spawn(new MoveTo(2, endPos), new FadeOut(2), NULL),
                                              new CallFunc(label, SEL_VOID(Node::removeFromParent)), NULL);
        
        label->runAction(action);
        addChild(label);
        
        return true;
    }
};

class AccelGyro : public TestLayer
{
public:
    AccelGyro()
    : TestLayer("Accel + Gyro", NULL)
    {
        setTrackedEvents( kFZEventType_Accelerometer | kFZEventType_Gyro );
        
        Layer *accelX = new LayerColor(fzRED, fzSize(5, 20));
        accelX->setAnchorPoint(0, 0.5f);
        accelX->setPosition(50, FZ_CANVAS_SIZE().height/2+65+30);
        accelX->setName("accelX");
        
        Layer *accelY = new LayerColor(fzRED, fzSize(5, 20));
        accelY->setAnchorPoint(0, 0.5f);
        accelY->setPosition(50, FZ_CANVAS_SIZE().height/2+65);
        accelY->setName("accelY");
        
        Layer *accelZ = new LayerColor(fzRED, fzSize(5, 20));
        accelZ->setAnchorPoint(0, 0.5f);
        accelZ->setPosition(50, FZ_CANVAS_SIZE().height/2+65-30);
        accelZ->setName("accelZ");
        
        Layer *gyroX = new LayerColor(fzGREEN, fzSize(5, 20));
        gyroX->setAnchorPoint(0, 0.5f);
        gyroX->setPosition(50, FZ_CANVAS_SIZE().height/2-65+30);
        gyroX->setName("gyroX");
        
        Layer *gyroY = new LayerColor(fzGREEN, fzSize(5, 20));
        gyroY->setAnchorPoint(0, 0.5f);
        gyroY->setPosition(50, FZ_CANVAS_SIZE().height/2-65);
        gyroY->setName("gyroY");
        
        Layer *gyroZ = new LayerColor(fzGREEN, fzSize(5, 20));
        gyroZ->setAnchorPoint(0, 0.5f);
        gyroZ->setPosition(50, FZ_CANVAS_SIZE().height/2-65-30);
        gyroZ->setName("gyroZ");
        
        
        addChild(accelX);
        addChild(accelY);
        addChild(accelZ);

        addChild(gyroX);
        addChild(gyroY);
        addChild(gyroZ);
    }
    
    
    bool event(Event& event)
    {
        Node *X;
        Node *Y;
        Node *Z;
        if(event.isType(kFZEventType_Accelerometer))
        {
            // TYPE: ACCELEROMETER
            X = getChildByName("accelX");
            Y = getChildByName("accelY");
            Z = getChildByName("accelZ");

        }else{
            // TYPE: GYRO
            X = getChildByName("gyroX");
            Y = getChildByName("gyroY");
            Z = getChildByName("gyroZ");
        }
        
        fzPoint3Double data = event.getVectorDouble();
        X->setContentSize(fabsf(data.x * 50), 20);
        Y->setContentSize(fabsf(data.y * 50), 20);
        Z->setContentSize(fabsf(data.z * 50), 20);

        return false;
    }
};

