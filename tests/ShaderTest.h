
#include "TestBase.h"

using namespace FORZE;

#define NUMBER 10
// Custom subclass of FORZE::Scene
// We add our own code in the constructor.
class ElectricField : public TestLayer {
    
    LayerColor *layerColor_;
    
public:
    ElectricField()
    : TestLayer("Electric field", NULL)
    {
        // TRACKING TAP EVENTS
        setTrackedEvents(kFZEventType_Tap);
        
        
        // CREATE A LAYER COLOR
        layerColor_ = new LayerColor(fzWHITE);

        
        // LOAD GLSL SHADER
        GLProgram *program = new GLProgram("simulation.vert", "simulation2.frag");
        program->addGenericAttributes();
        program->link();
        layerColor_->setGLProgram(program);
        
        
        // ADD CHILDS
        addChild(layerColor_);
        
        
        // SCHEDULE UPDATE()
        schedule();
    }
    
    
    void createCharge(const fzPoint& pos)
    {
        if(layerColor_->getChildren().size() < NUMBER) {
            
            //float chargeValue = (40.0f + FZ_RANDOM_0_1() * 50.0f) * 10.0f;
            float chargeValue = 700;
            if(FZ_RANDOM_0_1() < 0.5)
                chargeValue = -chargeValue;
            
            LayerColor *charge = new LayerColor(fzRED, fzSize(1, 1));
            charge->setPosition(pos);
            charge->setTag(chargeValue);
            layerColor_->addChild(charge);
        }
    }
    
    
    void update(fzFloat)
    {
        Node *node;
        GLfloat values[NUMBER*3];
        
        fzPoint rate = Director::Instance().getViewPort();
        rate = rate.compMult(1/getContentSize());

        GLint i = 0;
        FZ_LIST_FOREACH(layerColor_->getChildren(), node)
        {
            values[i*3 + 0] = node->getPosition().x * rate.x;
            values[i*3 + 1] = node->getPosition().y * rate.y;
            values[i*3 + 2] = node->getTag();
            ++i;
        }
        
        layerColor_->getGLProgram()->setUniform1i("u_number", i);
        layerColor_->getGLProgram()->setUniform3fv("u_data[0]", i, values);
    }
    
    
    bool event(Event& event)
    {
        switch (event.getState()) {
            case kFZEventState_Began:
            {
                Node *charge;
                FZ_LIST_FOREACH(layerColor_->getChildren(), charge) {
                    if(charge->getPosition().distance(event.getPoint()) < 80.0f) {
                        event.setUserData(charge);
                        return true;
                    }
                }
                createCharge(event.getPoint());
                return false;
            }
            default:
            {
                Light *light = static_cast<Light*>(event.getUserData());
                if(light)
                    light->setPosition(event.getPoint());
                
                return false;
            }
        }
    }
};
