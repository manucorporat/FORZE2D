

#include "TestBase.h"

using namespace FORZE;


class TexturePNG : public TestLayer
{
public:
    TexturePNG()
    : TestLayer("TexturePNG", NULL)
    {
        Sprite *img = new Sprite("test_image.png");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};

class TextureMipMap : public TestLayer
{
public:
    TextureMipMap()
    : TestLayer("TextureMipMap", "mipmap  -  no mipmap")
    {
        fzPoint center = getContentSize() / 2;
        
        Sprite *img0 = new Sprite("grossini_dance_atlas.png", fzRect(85, 121, 85, 121));
        img0->setPosition(center + fzPoint(-150, 0));
        addChild(img0);
        
        Texture2D *texture0 = img0->getTexture();
        texture0->generateMipmap();
        fzTexParams texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture0->setTexParameters(texParams);
        
        
        
        Sprite *img1 = new Sprite("grossini_dance_atlas_nomipmap.png", fzRect(85, 121, 85, 121));
        img1->setPosition(center + fzPoint(150, 0));
        addChild(img1);
        
        
        ActionInterval *scale1 = new EaseOut(new ScaleBy(4, 0.01f), 3);
        ActionInterval *scale2 = scale1->copy();

        Action *action1 = new RepeatForever(new Sequence(scale1, scale1->reverse(), NULL));
        Action *action2 = new RepeatForever(new Sequence(scale2, scale2->reverse(), NULL));
        
        img0->runAction(action1);
        img1->runAction(action2);
    }
};

class TexturePVR : public TestLayer
{
public:
    TexturePVR()
    : TestLayer("TexturePVR", NULL)
    {
#if FZ_GL_PVRTC
        Sprite *img = new Sprite("test_image.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
#endif
    }
};

class TexturePVR2BPP : public TestLayer
{
public:
    TexturePVR2BPP()
    : TestLayer("TexturePVR2BPP", NULL)
    {
#if FZ_GL_PVRTC
        Sprite *img = new Sprite("test_image_pvrtc2bpp.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
#endif
    }
};


class TexturePVR4BPP : public TestLayer
{
public:
    TexturePVR4BPP()
    : TestLayer("TexturePVR4BPP", NULL)
    {
#if FZ_GL_PVRTC
        Sprite *img = new Sprite("test_image_pvrtc4bpp.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
#endif
    }
};


class TexturePVRRGBA8888 : public TestLayer
{
public:
    TexturePVRRGBA8888()
    : TestLayer("TexturePVRRGBA8888", NULL)
    {
        Sprite *img = new Sprite("test_image_rgba8888.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};

class TexturePVRBGRA8888 : public TestLayer
{
public:
    TexturePVRBGRA8888()
    : TestLayer("TexturePVRBGRA8888", NULL)
    {
        Sprite *img = new Sprite("test_image_bgra8888.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};

class TexturePVRRGBA4444 : public TestLayer
{
public:
    TexturePVRRGBA4444()
    : TestLayer("TexturePVRRGBA4444", NULL)
    {
        Sprite *img = new Sprite("test_image_rgba4444.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};

class TexturePVRRGBA4444GZ : public TestLayer
{
public:
    TexturePVRRGBA4444GZ()
    : TestLayer("TexturePVRRGBA4444GZ", NULL)
    {
        //Sprite *img = new Sprite("test_image_rgba4444.pvr.gz");
        //img->setPosition(getContentSize() / 2);
        //addChild(img);
    }
};

class TexturePVRRGBA4444CCZ : public TestLayer
{
public:
    TexturePVRRGBA4444CCZ()
    : TestLayer("TexturePVRRGBA4444CCZ", NULL)
    {
        Sprite *img = new Sprite("test_image_rgba4444.pvr.ccz");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};

class TexturePVRRGBA5551 : public TestLayer
{
public:
    TexturePVRRGBA5551()
    : TestLayer("TexturePVRRGBA5551", NULL)
    {
        Sprite *img = new Sprite("test_image_rgba5551.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};

class TexturePVRRGB565 : public TestLayer
{
public:
    TexturePVRRGB565()
    : TestLayer("TexturePVRRGB565", NULL)
    {
        Sprite *img = new Sprite("test_image_rgb565.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};

class TexturePVRRGB888 : public TestLayer
{
public:
    TexturePVRRGB888()
    : TestLayer("TexturePVRRGB888", NULL)
    {
        //Sprite *img = new Sprite("texture_test.png");
        //img->setPosition(getContentSize() / 2);
        //addChild(img);
    }
};

class TexturePVRA8 : public TestLayer
{
public:
    TexturePVRA8()
    : TestLayer("TexturePVRA8", NULL)
    {
        Sprite *img = new Sprite("test_image_a8.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};

class TexturePVRI8 : public TestLayer
{
public:
    TexturePVRI8()
    : TestLayer("TexturePVRI8", NULL)
    {
        Sprite *img = new Sprite("test_image_i8.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};


class TexturePVRLA88 : public TestLayer
{
public:
    TexturePVRLA88()
    : TestLayer("TexturePVRLA88", NULL)
    {
        Sprite *img = new Sprite("test_image_ai88.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};


class TexturePVRBadEncoding : public TestLayer
{
public:
    TexturePVRBadEncoding()
    : TestLayer("TexturePVRBadEncoding", NULL)
    {
        Sprite *img = new Sprite("test_image-bad_encoding.pvr");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};


class TexturePVRMipMap1 : public TestLayer
{
public:
    TexturePVRMipMap1()
    : TestLayer("TexturePVRMipMap1", "mipmap  -  no mipmap")
    {
        fzPoint center = getContentSize() / 2;
        
        Sprite *img0 = new Sprite("test_image_rgba4444_mipmap.pvr");
        img0->setPosition(center + fzPoint(-100, 0));
        addChild(img0);
        
        Texture2D *texture0 = img0->getTexture();
        texture0->generateMipmap();
        fzTexParams texParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
        texture0->setTexParameters(texParams);
        
        
        
        Sprite *img1 = new Sprite("test_image.png");
        img1->setPosition(center + fzPoint(100, 0));
        addChild(img1);
        
        
        ActionInterval *scale1 = new EaseOut(new ScaleBy(4, 0.01f), 3);
        ActionInterval *scale2 = scale1->copy();
        
        Action *action1 = new RepeatForever(new Sequence(scale1, scale1->reverse(), NULL));
        Action *action2 = new RepeatForever(new Sequence(scale2, scale2->reverse(), NULL));
        
        img0->runAction(action1);
        img1->runAction(action2);
    }
};

class TexturePVRNonSquare : public TestLayer
{
public:
    TexturePVRNonSquare()
    : TestLayer("TexturePVRNonSquare", NULL)
    {
        Sprite *img = new Sprite("texture_test.png");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};

class TexturePVRNPOT4444 : public TestLayer
{
public:
    TexturePVRNPOT4444()
    : TestLayer("TexturePVRNPOT4444", NULL)
    {
        Sprite *img = new Sprite("texture_test.png");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};


class TexturePVRNPOT8888 : public TestLayer
{
public:
    TexturePVRNPOT8888()
    : TestLayer("TexturePVRNPOT8888", NULL)
    {
        Sprite *img = new Sprite("texture_test.png");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};

class TexturePVRRaw : public TestLayer
{
public:
    TexturePVRRaw()
    : TestLayer("TexturePVRRaw", NULL)
    {
        Sprite *img = new Sprite("texture_test.png");
        img->setPosition(getContentSize() / 2);
        addChild(img);
    }
};
