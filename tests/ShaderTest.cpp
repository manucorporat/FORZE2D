
#include "ShaderTest.h"

using namespace FORZE;


#define NUMBER_OF_TESTS 1

TestLayer *allTest(fzUInt index)
{
    switch (index) {
        case 0: return new ElectricField();
        default:
            return NULL;
    }
}


// APPLICATION DELEGATE
// A instance of this object will receive the application events
// through the ApplicationProtocol protocol
class AppDelegate : public ApplicationProtocol {
public:
    AppDelegate() {}
    
    // this method is called when the application launched.
    void applicationLaunched(void *options)
    {
        Director::Instance().setDisplayFPS(true);
        
        // INITIALIZE SCENETEST
        TestScene *scene = new TestScene(allTest, NUMBER_OF_TESTS);
        
        // RUN SCENE
        Director::Instance().runWithScene(scene);
    }
};


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
