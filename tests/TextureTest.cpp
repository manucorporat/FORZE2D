

#import "TextureTest.h"

using namespace FORZE;


#define NUMBER_OF_TESTS 23

TestLayer *allTest2(fzUInt index)
{
    switch (index) {
        case 0: return new TexturePNG();
        case 1: return new TextureMipMap();
        case 2: return new TexturePVRMipMap1();
        case 3: return new TexturePVR();
        case 4: return new TexturePVR2BPP();
        case 5: return new TexturePVR4BPP();
        case 6: return new TexturePVRRGBA8888();
        case 7: return new TexturePVRBGRA8888();
        case 8: return new TexturePVRRGBA4444();
        case 9: return new TexturePVRRGBA4444GZ();
        case 10: return new TexturePVRRGBA4444CCZ();
        case 11: return new TexturePVRRGBA5551();
        case 12: return new TexturePVRRGB565();
        case 13: return new TexturePVRRGB888();
        case 14: return new TexturePVRA8();
        case 15: return new TexturePVRI8();
        case 16: return new TexturePVRLA88();
        case 17: return new TexturePVRNonSquare();
        case 18: return new TexturePVRNPOT4444();
        case 19: return new TexturePVRNPOT8888();
        case 20: return new TexturePVRRaw();
        case 21: return new TexturePVRBadEncoding();
            
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
        TestScene *scene = new TestScene(allTest2, NUMBER_OF_TESTS);
        
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
