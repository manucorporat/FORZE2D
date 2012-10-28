

#include "FORZE.h"

using namespace FORZE;

class SceneTest3 : public Scene
{
public:
    SceneTest3()
    {
        MenuItemLabel *item = new MenuItemLabel("Replace with scene 2", "helvetica.fnt",
                                                this, SEL_PTR(SceneTest3::replaceScene));
        
        MenuItemLabel *item2 = new MenuItemLabel("Pop scene", "helvetica.fnt",
                                                 this, SEL_PTR(SceneTest3::pop));
        
        Menu *menu = new Menu(item, item2, NULL);
        menu->alignChildrenVertically();
        addChild(menu);
        
        FZLog("SCENE TEST 3 ALLOCATED");
    }
    ~SceneTest3()
    {
        FZLog("SCENE TEST 3 DEALLOCATED");
    }
    
    void replaceScene(void*) {
        runAction(new CallFunc(this, SEL_VOID(SceneTest3::replaceScene2)));
    }
    void replaceScene2();
    
    
    void pop(void*) {
        runAction(new CallFunc(this, SEL_VOID(SceneTest3::pop2)));
    }
    void pop2() {
        Director::Instance().popScene();
    }
};


class SceneTest2 : public Scene
{
public:
    SceneTest2()
    {
        MenuItemLabel *item1 = new MenuItemLabel("Replace with scene 3", "helvetica.fnt",
                                                 this, SEL_PTR(SceneTest2::replaceScene));
        
        MenuItemLabel *item2 = new MenuItemLabel("Pop scene", "helvetica.fnt",
                                                 this, SEL_PTR(SceneTest2::pop));
        
        Menu *menu = new Menu(item1, item2, NULL);
        menu->alignChildrenVertically();
        addChild(menu);
        
        FZLog("SCENE TEST 2 ALLOCATED");
    }
    
    ~SceneTest2()
    {
        FZLog("SCENE TEST 2 DEALLOCATED");
    }
    
    
    
    void replaceScene(void*) {
        schedule(SEL_FLOAT(SceneTest2::replaceScene2), 0);
    }
    void replaceScene2(void*) {
        Director::Instance().replaceScene(new SceneTest3());
        unscheduleCurrent();
    }
    
    
    void pop(void*) {
        schedule(SEL_FLOAT(SceneTest2::pop2), 0);
    }
    void pop2(void*) {
        Director::Instance().popScene();
        unscheduleCurrent();
    }
};


class SceneTest1 : public Scene
{
public:
    SceneTest1()
    {
        MenuItemLabel *item1 = new MenuItemLabel("Push scene2", "helvetica.fnt",
                                                 this, SEL_PTR(SceneTest1::pushScene));
        
        addChild(item1);
        
        FZLog("SCENE TEST 1 ALLOCATED");
    }
    
    ~SceneTest1()
    {
        FZLog("SCENE TEST 1 DEALLOCATED");
    }
    
    
    void pushScene(void*) {
        Director::Instance().pushScene(new SceneTest2());
    }
};
