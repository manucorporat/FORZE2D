

#import "LabelTest.h"

using namespace FORZE;


#define NUMBER_OF_TESTS 3

TestLayer *allTest(fzUInt index)
{
    switch (index) {
        case 0: return new LabelTest();
        case 1: return new LabelTest2();
        case 2: return new LabelTest3();
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
    
    GLConfig fzGLConfig()
    {
        GLConfig config; // default config
        return config;
    }
};



#pragma mark - main

int main(int argc, char *argv[])
{    
    // INIT FORZE
    FORZE_INIT(new AppDelegate(), kFZSize_iPhone, argc, argv);
    return EXIT_SUCCESS;
}
