

#import "ActionTest.h"

using namespace FORZE;


#define NUMBER_OF_TESTS 21

TestLayer *allTest(fzUInt index)
{
    switch (index) {
        case 0: return new ActionMove();
        case 1: return new ActionRotate();
        case 2: return new ActionScale();
        case 3: return new ActionSkew();
        case 4: return new ActionJump();
        case 5: return new ActionBezier();
        case 6: return new ActionBlink();
        case 7: return new ActionFade();
        case 8: return new ActionTint();
        case 9: return new ActionSequence();
        case 10: return new ActionSequence2();
        case 11: return new ActionRepeatForever();
        case 12: return new ActionRotateToRepeat();
        case 13: return new ActionRotateJerk();
        case 14: return new ActionReverse();
        case 15: return new ActionDelayTime();
        case 16: return new ActionReverseSequence();
        case 17: return new ActionReverseSequence2();
        case 18: return new ActionRepeat();
        case 19: return new ActionCallFunc();
        case 20: return new ActionCallFuncND();
        default:
            return NULL;
    }
}

#pragma mark -

ActionMove::ActionMove()
: ActionBase("Move", NULL)
{
	ActionInterval* actionTo = new MoveTo(2, fzPoint(getContentSize()-fzPoint(40, 40)));
	ActionInterval* actionBy = new MoveBy(2, fzPoint(80,80));
	ActionInterval* actionByBack = actionBy->reverse();
    
    tamara->runAction(actionTo);
    grossini->runAction(new Sequence(actionBy, actionByBack, NULL));
    kathia->runAction(new MoveTo(1, fzPoint(40, 40)));
}


#pragma mark -

ActionRotate::ActionRotate()
: ActionBase("Rotate", NULL)
{
    ActionInterval *actionTo = new RotateTo(2, 45);
    ActionInterval *actionTo2 = new RotateTo(2, -45);
    ActionInterval *actionTo0 = new RotateTo(2, 0);
    
    ActionInterval *actionBy = new RotateBy(2, 360);
    ActionInterval *actionByBack = actionBy->reverse();
    
    tamara->runAction(new Sequence(actionTo, actionTo0, NULL));
    grossini->runAction(new Sequence(actionBy, actionByBack, NULL));
    kathia->runAction(new Sequence(actionTo2, actionTo0->copy(), NULL));
}


#pragma mark -

ActionScale::ActionScale()
: ActionBase("Scale", NULL)
{
    ActionInterval *actionTo = new ScaleTo(2, 0.5f);
    ActionInterval *actionBy = new ScaleBy(2, 1, 10);
    ActionInterval *actionBy2 = new ScaleBy(2, 5, 1);
    
    tamara->runAction(new Sequence(actionBy, actionBy->reverse(), NULL));
    grossini->runAction(actionTo);
    kathia->runAction(new Sequence(actionBy2, actionBy2->reverse(), NULL));
}


#pragma mark -

ActionSkew::ActionSkew()
: ActionBase("Action Skew", NULL)
{
    ActionInterval *actionTo = new SkewTo(2, 37.2f, -37.2f);
    ActionInterval *actionToBack = new SkewTo(2, 0, 0);
    ActionInterval *actionBy = new SkewBy(2, 0, -90);
    ActionInterval *actionBy2 = new SkewBy(2, 45, 45);
    ActionInterval *actionByBack = actionBy->reverse();
    
    
    tamara->runAction(new Sequence(actionTo, actionToBack, NULL));
    grossini->runAction(new Sequence(actionBy, actionByBack, NULL));
    kathia->runAction(new Sequence(actionBy2, actionBy2->reverse(), NULL));
}


#pragma mark -

ActionJump::ActionJump()
: ActionBase("Jump", NULL)
{
    ActionInterval *actionTo = new JumpTo(2, fzPoint(300, 300), 50, 4);
    ActionInterval *actionBy = new JumpBy(2, fzPoint(300, 0), 50, 4);
    ActionInterval *actionUp = new JumpBy(2, fzPoint(0, 0), 80, 4);
    ActionInterval *actionByBack = actionBy->reverse();

    
    tamara->runAction(actionTo);
    grossini->runAction(new Sequence(actionBy, actionByBack, NULL));
    kathia->runAction(new RepeatForever(actionUp));
}


#pragma mark -

ActionBezier::ActionBezier()
: ActionBase("Bezier", NULL)
{

	fzSize s = Director::Instance().getCanvasSize();
    
    
	// sprite 1
	fzBezierConfig bezier;
	bezier.controlPoint_1 = fzPoint(0, s.height/2);
	bezier.controlPoint_2 = fzPoint(300, -s.height/2);
	bezier.endPosition = fzPoint(300,100);
    
	ActionInterval *bezierForward = new BezierBy(3, bezier);
    ActionInterval *bezierBack = bezierForward->reverse();
	ActionInterval *seq = new Sequence(bezierForward, bezierBack, NULL);
    Action *rep = new RepeatForever(seq);
    
    
	// sprite 2
    tamara->setPosition(80, 160);
	fzBezierConfig bezier2;
	bezier2.controlPoint_1 = fzPoint(100, s.height/2);
	bezier2.controlPoint_2 = fzPoint(200, -s.height/2);
	bezier2.endPosition = fzPoint(240,160);
    ActionInterval *bezierTo1 = new BezierTo(2, bezier2);
    
    
	// sprite 3
	kathia->setPosition(400,160);
    ActionInterval *bezierTo2 = new BezierTo(2, bezier2);
    
    
    grossini->runAction(rep);
    tamara->runAction(bezierTo1);
    kathia->runAction(bezierTo2);
}


#pragma mark -

ActionBlink::ActionBlink()
: ActionBase("Blink", NULL)
{
    ActionInterval *action1 = new Blink(3, 10);
    ActionInterval *action2 = new Blink(3, 5);
    ActionInterval *action3 = new Blink(0.5f, 5);
    
    tamara->runAction(action1);
    grossini->runAction(action2);
    kathia->runAction(action3);
}


#pragma mark -

ActionFade::ActionFade()
: ActionBase("Fade", NULL)
{
    tamara->setOpacity(0);
    
    ActionInterval *action1 = new FadeIn(1);
    ActionInterval *action1Back = action1->reverse();
    ActionInterval *action2 = new FadeOut(1);
    ActionInterval *action2Back = action2->reverse();

    tamara->runAction(new Sequence(action1, action1Back, NULL));
    kathia->runAction(new Sequence(action2, action2Back, NULL));
}


#pragma mark -

ActionTint::ActionTint()
: ActionBase("Tint", NULL)
{
    ActionInterval *action1 = new TintTo(2, fzMAGENTA);
    ActionInterval *action2 = new TintBy(2, -127, -255, -127);
    ActionInterval *action2Back = action2->reverse();
    
    tamara->runAction(action1);
    kathia->runAction(new Sequence(action2, action2Back, NULL));
}


#pragma mark -

ActionSequence::ActionSequence()
: ActionBase("Sequence", NULL)
{
    ActionInterval *action = new Sequence(new MoveBy(2, fzPoint(240, 0)),
                                          new RotateBy(2, 540), NULL);
    
    grossini->runAction(action);
}


#pragma mark -

ActionSequence2::ActionSequence2()
: ActionBase("Sequence 2", NULL)
{
    grossini->setIsVisible(false);
    
    ActionInterval *action = new Sequence(new Place(fzPoint(200, 200)),
                                          new Show(),
                                          new MoveBy(1, fzPoint(100, 0)),
                                          new CallFunc(this, SEL_VOID(ActionSequence2::callfunc1)),
                                          new CallFuncN(this, SEL_PTR(ActionSequence2::callfunc2)),
                                          new CallFuncND(this, SEL_2PTR(ActionSequence2::callfunc3), (void*)0xbebabeba),
                                          NULL);

    grossini->runAction(action);
}

void ActionSequence2::callfunc1()
{
    FZLog("CALLFUNC1 called");
}

void ActionSequence2::callfunc2(void*)
{
    FZLog("CALLFUNC2 called");
}

void ActionSequence2::callfunc3(void*, void*)
{
    FZLog("CALLFUNC3 called");
}


#pragma mark -

ActionRepeatForever::ActionRepeatForever()
: ActionBase("RepeatForever", NULL)
{
    ActionInterval *action = new Sequence(new DelayTime(1),
                                          new CallFuncN(this, SEL_PTR(ActionRepeatForever::repeatForever)),
                                          NULL);
    
    grossini->runAction(action);
}

void ActionRepeatForever::repeatForever(Node *node)
{
    RepeatForever *repeat = new RepeatForever(new RotateBy(1, 360));
    node->runAction(repeat);
}


#pragma mark -

ActionRotateToRepeat::ActionRotateToRepeat()
: ActionBase("RotateToRepeat", NULL)
{
    ActionInterval *action1 = new RotateTo(1, 90);
    ActionInterval *action2 = new RotateTo(1, 0);
    ActionInterval *seq = new Sequence(action1, action2, NULL);
    
    
    Action *rep1 = new RepeatForever(seq->copy());   
    Action *rep2 = new Repeat(seq, 5);

    tamara->runAction(rep1);
    kathia->runAction(rep2);
}


#pragma mark -

ActionRotateJerk::ActionRotateJerk()
: ActionBase("RotateJerk", NULL)
{
    ActionInterval *seq = new Sequence(new RotateTo(0.5f, -20),
                                       new RotateTo(0.5f, 20), NULL);
    
    ActionInterval *rep1 = new Repeat(seq, 10);
    Action *rep2 = new RepeatForever(seq->copy());

    tamara->runAction(rep1);
    kathia->runAction(rep2);
}


#pragma mark -

ActionReverse::ActionReverse()
: ActionBase("Reverse", NULL)
{
    ActionInterval *jump = new JumpBy(2, fzPoint(300, 0), 50, 4);
    ActionInterval *action = new Sequence(jump, jump->reverse(), NULL);
    
    grossini->runAction(action);
}


#pragma mark -

ActionDelayTime::ActionDelayTime()
: ActionBase("Delay time", NULL)
{
    ActionInterval *move = new MoveBy(1, fzPoint(150, 0));
    ActionInterval *action = new Sequence(move,
                                          new DelayTime(2),
                                          move, NULL);

    grossini->runAction(action);
}


#pragma mark -

ActionReverseSequence::ActionReverseSequence()
: ActionBase("ReverseSeq", NULL)
{
    ActionInterval *move1 = new MoveBy(1, fzPoint(250, 0));
    ActionInterval *move2 = new MoveBy(1, fzPoint(0, 50));
    ActionInterval *seq = new Sequence(move1,
                                       move2, 
                                       move1->reverse(), NULL);
    
    ActionInterval *action = new Sequence(seq,
                                          seq->reverse(), NULL);

	grossini->runAction(action);
}


#pragma mark -

ActionReverseSequence2::ActionReverseSequence2()
: ActionBase("ReverseSeq 2", NULL)
{
	// Test:
	//   Sequence should work both with IntervalAction and InstantActions
    ActionInterval *move1 = new MoveBy(1, fzPoint(250, 0));
    ActionInterval *move2 = new MoveBy(1, fzPoint(0, 50));
    ActionInstant *tog1 = new ToggleVisibility();
    ActionInstant *tog2 = new ToggleVisibility();
    ActionInterval *seq = new Sequence(move1,
                                       tog1,
                                       move2,
                                       tog2,
                                       move1->reverse(), NULL);
    
    ActionInterval *action = new Repeat(new Sequence(seq, seq->reverse(), NULL), 3);

    
	// Test:
	//   Also test that the reverse of Hide is Show, and vice-versa
    kathia->runAction(action);
    
    ActionInterval *move_tamara = new MoveBy(1, fzPoint(100, 0));
    ActionInterval *move_tamara2 = new MoveBy(1, fzPoint(50, 0));
    ActionInstant *hide = new Hide();
    ActionInterval *seq_tamara = new Sequence(move_tamara,
                                              hide,
                                              move_tamara2, NULL);
    
    ActionInterval *action_tamara = new Sequence(seq_tamara, seq_tamara->reverse(), NULL);
    tamara->runAction(action_tamara);
}


#pragma mark -

ActionRepeat::ActionRepeat()
: ActionBase("Repeat", NULL)
{    
	ActionInterval *a1 = new MoveBy(1, fzPoint(150, 0));
    ActionInterval *action1 = new Repeat(new Sequence(new Place(fzPoint(60, 60)), a1, NULL), 3);
    Action *action2 = new RepeatForever(new Sequence(a1->copy(), a1->reverse(), NULL));

    kathia->runAction(action1);
    tamara->runAction(action2);
}


#pragma mark -

ActionCallFunc::ActionCallFunc()
: ActionBase("CallFunc", NULL)
{
    
    ActionInterval *action1 = new Sequence(new MoveBy(2, fzPoint(200, 0)),
                                           new CallFunc(this, SEL_VOID(ActionCallFunc::callfunc1)), NULL);
    
    ActionInterval *action2 = new Sequence(new ScaleBy(2, 2),
                                           new FadeOut(2),
                                           new CallFuncN(this, SEL_PTR(ActionCallFunc::callfunc2)), NULL);
    
    ActionInterval *action3 = new Sequence(new RotateBy(2, 360),
                                           new FadeOut(2),
                                           new CallFuncND(this, SEL_2PTR(ActionCallFunc::callfunc3), (void*)0xbebabeba), NULL);
    

	grossini->runAction(action1);
    tamara->runAction(action2);
	kathia->runAction(action3);

}


void ActionCallFunc::callfunc1()
{
    FZLog("callback 1 called");

    fzPoint center = getContentSize() / 2;
    Label *label = new Label("callback 1 called", "helvetica.fnt");
    label->setPosition(center + fzPoint(0, 20));
    addChild(label);
}


void ActionCallFunc::callfunc2(void *sender)
{
   	FZLog("callback 2 called from:%p", sender);
    
    fzPoint center = getContentSize() / 2;    
    Label *label = new Label("callback 2 called", "helvetica.fnt");
    label->setPosition(center + fzPoint(0, 0));
    addChild(label);    
}


void ActionCallFunc::callfunc3(void *sender, void *data)
{
    FZLog("callback 3 called from:%p with data:%x",sender,(unsigned int)data);
    
    fzPoint center = getContentSize() / 2;
    Label *label = new Label("callback 3 called", "helvetica.fnt");
    label->setPosition(center + fzPoint(0, -20));
    addChild(label);    
}


#pragma mark -

ActionCallFuncND::ActionCallFuncND()
: ActionBase("CallFuncND", NULL)
{
	ActionInterval *action = new Sequence(new MoveBy(2, fzPoint(200, 0)),
                                          new CallFuncND(grossini, SEL_2PTR(Node::removeFromParent), (void*)true), NULL);
    
    grossini->runAction(action);
}




/*************************************************/
/************* APPLICATION DELEGATE **************/
/*************************************************/

class AppDelegate : public AppDelegateProtocol
{
public:
    AppDelegate() {}
    
    void applicationLaunched(void *options)
    {
        //Director::Instance().setDisplayFPS(true);
        
        // INITIALIZE SCENETEST        
        TestScene *scene = new TestScene(allTest, NUMBER_OF_TESTS);

        // RUN SCENE
        Director::Instance().runWithScene(scene);
    }
};



#pragma mark - main

int main(int argc, char *argv[])
{
#ifdef FZ_OS_DESKTOP
    // If you are running on a desktop OS (windows, linux, mac)
    // FORZE is able to simulate a different screen resolution in order to see how your game would look in another devices. In this case iPad (768x1024)
    Director::Instance().setWindowSize( kFZSize_iPhone );
#endif
    
    // INIT FORZE
    // kFZSize_iPad = fzSize(768, 1024)
    FORZE_INIT(new AppDelegate(), kFZSize_Auto, argc, argv);
    return EXIT_SUCCESS;
}
