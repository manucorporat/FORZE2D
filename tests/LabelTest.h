

#include "TestBase.h"

using namespace FORZE;


class LabelTest : public TestLayer
{
public:
    LabelTest()
    : TestLayer("FNT font", NULL)
    {
        // Direct way
        Label *label1 = new Label("Hello", "helvetica.fnt");
        addChild(label1);

        
        // String NULL way
        Label *label2 = new Label(NULL, "helvetica.fnt");
        label2->setString("Salut");
        addChild(label2);

        // Void label way
        Font *helvetica = FontCache::Instance().addFont("helvetica.fnt");
        Label *label3 = new Label();
        label3->setFont(helvetica);
        label3->setString("Hola");
        addChild(label3);
        
        // Change font
        Label *label4 = new Label("Hallo", "helvetica.fnt");
        label4->setName("label4");
        addChild(label4);
        
        alignChildrenVertically();
        

        schedule(SEL_FLOAT(LabelTest::changeFont), 2);
    }
    
    void changeFont(fzFloat)
    {
        Label *label4 = (Label*)getChildByName("label4");
        Font *helvetica = FontCache::Instance().addFont("helvetica.fnt");
        label4->setFont(helvetica);
        
        unscheduleCurrent();
    }
};


class LabelTest2 : public TestLayer
{
public:
    LabelTest2()
    : TestLayer("FNT kerning", NULL)
    {
        Label *label = new Label("AVA\n AMA\n MVM", "kerning_test.fnt");
        label->setAlignment(kFZLabelAlignment_center);
        label->setPosition(getContentSize()/2);
        addChild(label);        
    }
};


class LabelTest3 : public TestLayer
{
public:
    LabelTest3()
    : TestLayer("Multiline", NULL)
    {
        const char text[] =
"*********************************\n\
************************\n\
*************\n\
**";
        
        Label *label1 = new Label(text, "helvetica.fnt");
        label1->setPosition(getContentSize()/2 + fzPoint(0, 100));
        label1->setAlignment(kFZLabelAlignment_left);
        addChild(label1);
        
        Label *label2 = new Label(text, "helvetica.fnt");
        label2->setPosition(getContentSize()/2);
        label2->setAlignment(kFZLabelAlignment_center);
        addChild(label2);
        
        Label *label3 = new Label(text, "helvetica.fnt");
        label3->setPosition(getContentSize()/2+ fzPoint(0, -100));
        label3->setAlignment(kFZLabelAlignment_right);
        addChild(label3);
    }
};
