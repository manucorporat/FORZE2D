FORZE2D
=======
Innovator cross platform 2D engine based in cocos2d-iphone.

It's a framework to build 2D games easily without worry about the platform.

FORZE2D is:

* Fast
* Open source
* Cross platform
* Cocos2d like


How to start a project
-----------------------

1. Download the code from [Github].

2. Create a xcode project and include the folder called **FORZE/**.



How to compile
--------------
1. Change default compiler to LLVM.
2. FORZE need C++11. Change the "C++ Language Dialect" setting to C++11 (**-std=c++11**).
3. You must define FZ_OS in order to compile correctly fot each Operative System. A good practice is to define it in the .pch file, like this:
` c
#include "FZPlatformsHeaders.h"
#define FZ_OS kFZ_OS_MAC // compiling for mac
//#define FZ_OS kFZ_OS_IOS_GL2 // compiling for ios …
`





