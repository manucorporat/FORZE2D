

#include "TestBase.h"

using namespace FORZE;


class ActionBase : public TestLayer
{
protected:
    Sprite *tamara;
    Sprite *grossini;
    Sprite *kathia;

public:
    ActionBase(const char* t1, const char* t2)
    : TestLayer(t1, t2)
    {
        grossini = new Sprite("grossini.png");
        tamara = new Sprite("grossinis_sister1.png");
        kathia = new Sprite("grossinis_sister2.png");
        
        addChild(grossini);
        addChild(tamara);
        addChild(kathia);
        
        alignChildrenHorizontally(40);
    }    
};


class ActionMove : public ActionBase
{
    LightSystem *system_;
    SpriteBatch *batch_;
    RenderTexture *render_;
public:
    ActionMove();
};

class ActionRotate : public ActionBase
{
public:
    ActionRotate();
};

class ActionScale : public ActionBase
{
public:
    ActionScale();
};

class ActionSkew : public ActionBase
{
public:
    ActionSkew();
};

class ActionJump : public ActionBase
{
public:
    ActionJump();
};

class ActionBezier : public ActionBase
{
public:
    ActionBezier();
};

class ActionBlink : public ActionBase
{
public:
    ActionBlink();
};

class ActionFade : public ActionBase
{
public:
    ActionFade();
};

class ActionTint : public ActionBase
{
public:
    ActionTint();
};

class ActionSequence : public ActionBase
{
public:
    ActionSequence();
};

class ActionSequence2 : public ActionBase
{
public:
    ActionSequence2();
    
    void callfunc1();
    void callfunc2(void*);
    void callfunc3(void*, void*);

};

class ActionRepeatForever : public ActionBase
{
public:
    ActionRepeatForever();
    
    void repeatForever(Node *node);
};


class ActionRotateToRepeat : public ActionBase
{
public:
    ActionRotateToRepeat();
};


class ActionRotateJerk : public ActionBase
{
public:
    ActionRotateJerk();
};

class ActionReverse : public ActionBase
{
public:
    ActionReverse();
};

class ActionDelayTime : public ActionBase
{
public:
    ActionDelayTime();
};

class ActionReverseSequence : public ActionBase
{
public:
    ActionReverseSequence();
};

class ActionReverseSequence2 : public ActionBase
{
public:
    ActionReverseSequence2();
};

class ActionRepeat : public ActionBase
{
public:
    ActionRepeat();
};

class ActionCallFunc : public ActionBase
{
public:
    ActionCallFunc();
    
    void callfunc1();
    void callfunc2(void*);
    void callfunc3(void*, void*);
};

class ActionCallFuncND : public ActionBase
{
public:
    ActionCallFuncND();
};
// */
