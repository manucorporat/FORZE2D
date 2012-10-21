

#import "SceneTest.h"

using namespace FORZE;


void SceneTest3::replaceScene2() {
    Director::Instance().replaceScene(new SceneTest2());
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
        SceneTest1 *scene = new SceneTest1();
        
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
    FORZE_INIT(new AppDelegate(), kFZSize_iPhone, argc, argv);
    return EXIT_SUCCESS;
}
