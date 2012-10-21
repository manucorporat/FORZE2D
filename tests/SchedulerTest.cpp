

#import "SchedulerTest.h"

using namespace FORZE;


#define NUMBER_OF_TESTS 3

TestLayer *allTest(fzUInt index)
{
    switch (index) {
        case 0: return new SchedulingTest();
        case 1: return new UnschedulingTest();
        case 2: return new ActionLoop1();

        default:
            return NULL;
    }
}



/*************************************************/
/************* APPLICATION DELEGATE **************/
/*************************************************/

class AppDelegate : public ApplicationProtocol
{
public:
    AppDelegate() {}
    
    void applicationLaunched(void *options)
    {
        Director::Instance().setDisplayFPS(true);
        
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
    
    // Set a resize mode. @see fzResizeMode
    Director::Instance().setResizeMode(kFZResizeMode_Fit);
    
    // INIT FORZE
    // kFZSize_iPad = fzSize(768, 1024)
    FORZE_INIT(new AppDelegate(), kFZSize_iPhone, argc, argv);
    return EXIT_SUCCESS;
}
