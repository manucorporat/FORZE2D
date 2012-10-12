

#include "TestBase.h"

using namespace FORZE;


class SchedulingTest : public TestLayer
{
    fzFloat delta;
    
public:
    SchedulingTest()
    : TestLayer("Scheduling test", NULL)
    , delta(0)
    {
        Label *labelUpdate = new Label("", "helvetica.fnt");
        labelUpdate->setTag(0);
        addChild(labelUpdate);
        Label *label1 = new Label("", "helvetica.fnt");
        label1->setTag(1);
        addChild(label1);
        Label *label2 = new Label("", "helvetica.fnt");
        label2->setTag(2);
        addChild(label2);
        
        alignChildrenVertically();
        
        
        //schedule();
        schedule(SEL_FLOAT(SchedulingTest::perSecond), 1);
        schedule(SEL_FLOAT(SchedulingTest::per3), FZ_RANDOM_0_1() * 4.0f);

        
        {
            // test possible bugs
            schedule();
            schedule();
            schedule();
            schedule(SEL_FLOAT(SchedulingTest::perSecond), 2);
            schedule(SEL_FLOAT(SchedulingTest::perSecond), 1);
        }
    }
    
    void update(fzFloat dt)
    {
        Label *label = (Label*) getChildByTag(0);
        label->setString(FZT("%f", delta));
        delta += dt;
    }
    
    void perSecond(fzFloat dt)
    {
        Label *label = (Label*) getChildByTag(1);
        label->setString(FZT("%f", delta));
    }
    
    void per3(fzFloat dt)
    {
        schedule(SEL_FLOAT(SchedulingTest::per3), FZ_RANDOM_0_1() * 4.0f);

        Label *label = (Label*) getChildByTag(2);
        label->setString(FZT("%f", delta));
    }
};


class UnschedulingTest : public TestLayer
{
    fzFloat delta;
    
public:
    UnschedulingTest()
    : TestLayer("Unscheduling", "Rate should be random.")
    , delta(0)
    {
        Label *labelUpdate = new Label("", "helvetica.fnt");
        labelUpdate->setTag(0);
        addChild(labelUpdate);
        Label *label1 = new Label("", "helvetica.fnt");
        label1->setTag(1);
        addChild(label1);
        Label *label2 = new Label("", "helvetica.fnt");
        label2->setTag(2);
        addChild(label2);
        
        alignChildrenVertically();
        
        
        schedule();
    }
    
    void update(fzFloat dt)
    {
        unscheduleCurrent(); //unschedule();
        schedule(SEL_FLOAT(UnschedulingTest::call), 1);
        
        Label *label = new Label("CALLED UPDATE()", "font_menu.fnt");
        label->setScale(0.5f);
        label->setPosition(getContentSize()/2);
        ActionInterval *action = new Sequence(new Spawn(new MoveBy(2, fzPoint(0, 300)), new FadeOut(2), NULL),
                                              new CallFunc(label, SEL_VOID(Node::removeFromParent)), NULL);
        
        label->runAction(action);
        addChild(label);
    }
    
    void call(fzFloat dt)
    {
        schedule(SEL_FLOAT(Node::update), FZ_RANDOM_0_1() * 3.0f);
        unscheduleCurrent(); // unschedule(SEL_FLOAT(UnschedulingTest::call));
        
        Label *label = new Label("CALLED CALL()", "font_menu.fnt");
        label->setPosition(getContentSize()/2);
        label->setScale(0.5f);
        ActionInterval *action = new Sequence(new Spawn(new MoveBy(2, fzPoint(0, 300)), new FadeOut(2), NULL),
                                              new CallFunc(label, SEL_VOID(Node::removeFromParent)), NULL);
        
        label->runAction(action);
        addChild(label);
    }

};


