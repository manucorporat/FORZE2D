/*
 * FORZE ENGINE: http://forzefield.com
 *
 * Copyright (c) 2011-2012 FORZEFIELD Studios S.L.
 * Copyright (c) 2012 Manuel Martínez-Almeida
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 @author Manuel Martínez-Almeida
 */

#include "FORZE.h"

using namespace FORZE;


// YOUR CUSTOM SUBCLASS OF FORZE::Layer
class HelloWorld : public Layer
{
public:
    HelloWorld()
    {
        // Creates a FORZE::Label, "Hello world" is the text
        // and "font_menu.fnt" is the font file.
        Label *label = new Label("Hello World", "font_menu.fnt");
        
        // you can change the color.
        label->setColor( fzORANGE ); // fzORANGE = fzColor3B(255,127,0);
        
        // Place the label at the center of the screen
        // getContentSize() is the size of the layer(this) where the label will be attached
        label->setPosition( getContentSize()/2 );
        //label->setPosition( getContentSize().width/2, getContentSize().height/2 );
        
        
        // add the label the layer (this)
        addChild(label);
    }
};



/*************************************************/
/************* APPLICATION DELEGATE **************/
/*************************************************/

// A instance of the class will receive the application events catched by FORZE.
class AppDelegate : public ApplicationProtocol
{
public:
    AppDelegate() {}
    
    //! This method is called by The Director, when the applicate launched.
    void applicationLaunched(void *options)
    {        
        // CREATE A VOID SCENE      
        Scene *scene = new Scene();
        
        // INIT LAYER AND ATTACH TO SCENE
        HelloWorld *layer = new HelloWorld();
        scene->addChild(layer);
        
        // RUN SCENE
        Director::Instance().runWithScene(scene);
    }
};


#pragma mark - main

int main(int argc, char *argv[])
{    
    FORZE_INIT(new AppDelegate(), kFZSize_iPhone, argc, argv);
    return EXIT_SUCCESS;
}
