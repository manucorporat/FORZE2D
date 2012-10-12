
#include "FORZE.h"
#include "_fz_mat_aC4.shader.h"

using namespace FORZE;

#define NUMBER 10
// Custom subclass of FORZE::Scene
// We add our own code in the constructor.
class ElectricField : public Layer {
    
    LayerColor *layerColor_;
    LayerColor *layerColor2_;

    
public:
    ElectricField()
    {
        // TRACKING TAP EVENTS
        setTrackedEvents(kFZEventType_Tap);

        Label *potential = new Label(NULL, "helvetica.fnt");
        potential->setAnchorPoint(-0.2, -0.2);
        potential->setName("potencial");
        potential->setColor(fzWHITE * 0.7f);
        addChild(potential, 1000);
        
        // CREATE LABELS
        const char *text =
"Manuel Martinez-Almeida\n\
Universidad de Valladolid. Physics, Electric potential.\n\n\
White: equipotential lines\n\
Green: positive potential\n\
Red: negative potential\n\
Blue: potential close to zero";
        Label *textLa = new Label(text, "helvetica.fnt");
        textLa->setAnchorPoint(0, 1);
        textLa->setPosition(10, FZ_CANVAS_SIZE().height-20);
        textLa->setColor(fzWHITE * 0.9f);
        
        Sprite *sprite = new Sprite("logo_forze.png");
        sprite->getTexture()->setAliasTexParameters();
        sprite->setPosition(getContentSize()/2);        
        sprite->runAction(new Sequence(new DelayTime(1),
                                       new FadeOut(2),
                                       new CallFunc(sprite, SEL_VOID(Node::removeFromParent)), NULL));
        
        addChild(sprite, 100);
        
        
        // CREATE A LAYER COLOR
        layerColor_ = new LayerColor(fzWHITE);

        
        // LOAD GLSL SHADER
        GLProgram *program = new GLProgram("simulation.vert", "simulation2.frag");
        program->addGenericAttributes();
        program->link();
        layerColor_->setGLProgram(program);
        
        // ITEM
        MenuItemImage *item = new MenuItemImage("menu_reset.png", this, SEL_PTR(ElectricField::removeLastOne));
        item->setPosition(30, 20);
        item->setScale(0.5f);
        
        // ADD CHILDS
        addChild(layerColor_);
        addChild(textLa);
        addChild(item);
        
        // SCHEDULE UPDATE()
        schedule();
        
//        float radious = 200;
//        fzPoint center(400, 400);
//        for(int i = 0; i < NUMBER; ++i)
//        {
//            float angle = 2*M_PI * (i/(float)NUMBER);
//            fzPoint pos = center + fzPoint(cosf(angle) * radious, sinf(angle) * radious);
//            createCharge(pos);
//        }
    }
    
    void getPotencial(fzPoint point)
    {
        Label *potential = (Label*) getChildByName("potencial");
        potential->setPosition(point);
        
        fzPoint rate = Director::Instance().getViewPort();
        rate = rate.compMult(1/getContentSize());
        
        point = point.compMult(rate);

        double potencial = 0;
        _fzVec2<double>vectorFinal = FZPointZero;
        Node *node;
        FZ_LIST_FOREACH(layerColor_->getChildren(), node)
        {
            _fzVec2<double> vector = point - node->getPosition().compMult(rate);
            double dist = vector.length();
            
            double p = node->getTag() / dist;
            potencial += p;
            vectorFinal += vector * (p / (dist * dist));
        }
        
        potential->setString(FZT("V:%.3f\nE:%.3f",potencial, vectorFinal.length() ));

    }
    
    
    void removeLastOne(void*sender)
    {
        layerColor_->removeChild((Node*)layerColor_->getChildren().back());
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
#if 1
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
#endif
    }
    
    
    bool event(Event& event)
    {
        if(event.isType(kFZEventType_MouseMoved))
        {
            getPotencial(event.getPoint());
            return false;
            
        }else{
            
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
    }
};


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
        ElectricField *layer = new ElectricField();
        
        Scene *scene = new Scene();
        scene->addChild(layer);
        Director::Instance().pushScene(scene);
    }
};


// APPLICATION'S ENTRY POINT
int main(int argc, char *argv[])
{
    //Director::Instance().setWindowSize(kFZSize_720p);
    FORZE_INIT(new AppDelegate(), kFZSize_Auto, argc, argv);
    return EXIT_SUCCESS;
}

