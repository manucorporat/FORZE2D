

#include "TestBase.h"

using namespace FORZE;



class TestBase : public TestLayer
{
public:
    TestBase(const char *t1, const char *t2)
    : TestLayer(t1, t2)
    {
        Sprite *img = new Sprite("picture.png");
        img->setPosition(getContentSize() / 2);
        addChild(img);
        
        
        Sprite *grossini = new Sprite("grossini.png");
        grossini->setPosition(getContentSize() / 2);
        addChild(grossini);
        
        grossini->runAction(new RepeatForever(new RotateBy(1, 90)));
    }
};


class TestOriginal : public TestBase
{
public:
    TestOriginal()
    : TestBase("No filter", NULL)
    { }
};


class TestContrast : public TestBase
{
public:
    TestContrast()
    : TestBase("Filter Color", NULL)
    {
        FilterColor *color = new FilterColor();
        color->setBrightness(0.3f);
        color->setContrast(2.3f);
        setFilter(color);
    }
};


class TestGrayscale : public TestBase
{
public:
    TestGrayscale()
    : TestBase("Filter Grayscale", NULL)
    {
        FilterGrayscale *filter = new FilterGrayscale();
        setFilter(filter);
    }
};


class TestToon : public TestBase
{
public:
    TestToon()
    : TestBase("Filter Toon", NULL)
    {
        FilterToon *filter = new FilterToon();
        setFilter(filter);
    }
};


class TestInvert : public TestBase
{
public:
    TestInvert()
    : TestBase("Filter Invert", NULL)
    {
        FilterInvert *filter = new FilterInvert();
        setFilter(filter);
    }
};
