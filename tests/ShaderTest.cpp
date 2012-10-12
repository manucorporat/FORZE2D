
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


// APPLICATION'S ENTRY POINT
int main(int argc, char *argv[])
{
    //Director::Instance().setWindowSize(kFZSize_720p);
    FORZE_INIT(new AppDelegate(), kFZSize_Auto, argc, argv);
    return EXIT_SUCCESS;
}

