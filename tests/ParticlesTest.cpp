

#import "ParticlesTest.h"

using namespace FORZE;


#define NUMBER_OF_TESTS 1

static TestLayer *allTest(fzUInt index)
{
    switch (index) {
        case 0: return new ParticleTest();

        default:
            return NULL;
    }
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
        Director::Instance().setDisplayFPS(true);
        
        // INITIALIZE SCENETEST        
        TestScene *scene = new TestScene(allTest, NUMBER_OF_TESTS);
        
        // RUN SCENE
        Director::Instance().runWithScene(scene);
    }
    
    GLConfig fzGLConfig()
    {
        GLConfig config; // default config
        return config;
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
