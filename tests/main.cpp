
#import "FORZE.h"
#import "SpriteTest.h"

using namespace FORZE;
class AppDelegate;
#pragma mark - Application delegate

/*************************************************/
/************* APPLICATION DELEGATE **************/
/*************************************************/

class AppDelegate : public ApplicationProtocol
{
public:
    AppDelegate() {}
    
    void fzApplicationLaunched(void *options)
    {
        Director::Instance().setDisplayFPS(false);
        
        // INITIALIZE SCENETEST
        TestLayer *newScene = new TestLayer();
        
        Scene *scene = new Scene();
        scene->addChild(newScene);
        
        // RUN SCENE
        Director::Instance().runWithScene(scene);
    }
};



#pragma mark - main

int main(int argc, char *argv[])
{    
    FORZE_INIT(new AppDelegate(), kFZSize_iPad, argc, argv);
    return EXIT_SUCCESS;
}
