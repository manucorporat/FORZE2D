

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
        const char text[] =
        "MVA\n"
        "AMV\n"
        "AVA";
        
        Label *label = new Label(text, "kerning_test.fnt");
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
        "*********************************\n"
        "************************\n"
        "*************\n"
        "**";
        
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

class LabelTest4 : public TestLayer
{
public:
    LabelTest4()
    : TestLayer("Spacing", NULL)
    {
        const char text[] =
        "Hello, my name is\n"
        "FORZE";

        Label *label1 = new Label(text, "helvetica.fnt");
        label1->setPosition(getContentSize()/2 + fzPoint(0, 120));
        label1->setAlignment(kFZLabelAlignment_center);
        addChild(label1);
        
        Label *label2 = new Label(text, "helvetica.fnt");
        label2->setVerticalPadding(8);
        label2->setPosition(getContentSize()/2 + fzPoint(0, 50));
        label2->setAlignment(kFZLabelAlignment_center);
        addChild(label2);
        
        Label *label3 = new Label(text, "helvetica.fnt");
        label3->setLetterSpacing(6);
        label3->setPosition(getContentSize()/2 + fzPoint(0, -20));
        label3->setAlignment(kFZLabelAlignment_center);
        addChild(label3);
        
        Label *label4 = new Label(text, "helvetica.fnt");
        label4->setLetterSpacing(5);
        label4->setVerticalPadding(10);
        label4->setPosition(getContentSize()/2 + fzPoint(0, -90));
        label4->setAlignment(kFZLabelAlignment_center);
        addChild(label4);
    }
};
