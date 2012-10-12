

#import "FORZE.h"

using namespace FORZE;


class TestLayer : public Layer
{
protected:
    char* p_title;
    char* p_message;
    
public:
    TestLayer(const char *title, const char* message);
    ~TestLayer();
    
    const char* getTitle() const {
        return p_title;
    }
    
    const char* getMessage() const {
        return p_message;
    }
};


class TestScene : public Scene
{
protected:
    TestLayer* (*p_function)(fzUInt);

    Label *p_title;
    Label *p_message;

    
    fzInt m_nuLayers;
    fzInt m_currentLayer;
    
    virtual void updateLayout() override;
    
    // Menu callbacks
    void prevLayer(void*);
    void reloadLayer(void*);
    void nextLayer(void*);
    
public:
    TestScene(TestLayer*(*func)(fzUInt), fzUInt number);
    
    
};
