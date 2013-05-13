cmake_minimum_required(VERSION 1.0)
project( FORZE )

add_definitions( -DFZ_OS=0 )
set(WARNING "")

set(CMAKE_C_FLAGS "-m32 -O2 ${WARNING}")
set(CMAKE_CXX_FLAGS "-std=c++0x -m32 -g -O2 -ftest-coverage -fprofile-arcs ${WARNING}")


set(LIBRARY_NAME forze2d)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(PATH "FORZE")
set(TEST_PATH "tests")


set(SRC_FILES
${PATH}/FZAction.cpp
${PATH}/FZActionCamera.cpp
${PATH}/FZActionEase.cpp
${PATH}/FZActionInstant.cpp
${PATH}/FZActionInterval.cpp
${PATH}/FZActionManager.cpp
${PATH}/FZAllocator.cpp
${PATH}/FZAnimation.cpp
${PATH}/FZAnimationCache.cpp
${PATH}/FZAutoList.cpp
${PATH}/FZBitOrder.cpp
${PATH}/FZCamera.cpp
${PATH}/FZCommon.cpp
${PATH}/FZConsole.cpp
${PATH}/FZData.cpp
${PATH}/FZDataStore.cpp
${PATH}/FZDeviceConfig.cpp
${PATH}/FZDirector.cpp
${PATH}/FZEvent.cpp
${PATH}/FZEventManager.cpp
${PATH}/FZFilter.cpp
${PATH}/FZFont.cpp
${PATH}/FZFontCache.cpp
${PATH}/FZGLConfig.cpp
${PATH}/FZGLProgram.cpp
${PATH}/FZGLState.cpp
${PATH}/FZGrabber.cpp
${PATH}/FZGrid.cpp
${PATH}/FZHUD.cpp
${PATH}/FZHash.cpp
${PATH}/FZIO.cpp
${PATH}/FZLabel.cpp
${PATH}/FZLayer.cpp
${PATH}/FZLightSystem.cpp
${PATH}/FZMS.cpp
${PATH}/FZMath.cpp
${PATH}/FZMenu.cpp
${PATH}/FZMenuItem.cpp
${PATH}/FZNode.cpp
${PATH}/FZParallaxNode.cpp
${PATH}/FZParticleExamples.cpp
${PATH}/FZParticleSystem.cpp
${PATH}/FZParticleSystemQuad.cpp
${PATH}/FZPerformManager.cpp
${PATH}/FZOSW.cpp
${PATH}/FZPrimitives.cpp
${PATH}/FZRenderTexture.cpp
${PATH}/FZResourcesManager.cpp
${PATH}/FZScene.cpp
${PATH}/FZScheduler.cpp
${PATH}/FZShaderCache.cpp
${PATH}/FZSprite.cpp
${PATH}/FZSpriteBatch.cpp
${PATH}/FZSpriteFrame.cpp
${PATH}/FZSpriteFrameCache.cpp
${PATH}/FZTMXLayer.cpp
${PATH}/FZTMXParser.cpp
${PATH}/FZTMXTiledMap.cpp
${PATH}/FZTexture2D.cpp
${PATH}/FZTextureAtlas.cpp
${PATH}/FZTextureCache.cpp
${PATH}/FZTransitions.cpp
${PATH}/FZTypes.cpp
${PATH}/Wrappers/iOSGL2_support.mm
${PATH}/Wrappers/iOS_support.mm
${PATH}/Wrappers/macosx_support.mm
${PATH}/Wrappers/model_support.cpp
${PATH}/external/tinythread/tinythread.cpp
${PATH}/external/libb64/base64.c
${PATH}/external/libpng/png.c
${PATH}/external/libpng/pngerror.c
${PATH}/external/libpng/pngget.c
${PATH}/external/libpng/pngmem.c
${PATH}/external/libpng/pngpread.c
${PATH}/external/libpng/pngread.c
${PATH}/external/libpng/pngrio.c
${PATH}/external/libpng/pngrtran.c
${PATH}/external/libpng/pngrutil.c
${PATH}/external/libpng/pngset.c
${PATH}/external/libpng/pngtrans.c
${PATH}/external/libpng/pngwio.c
${PATH}/external/libpng/pngwrite.c
${PATH}/external/libpng/pngwtran.c
${PATH}/external/libpng/pngwutil.c
)

set( INCLUDE_FILES
${PATH}/FORZE.h
${PATH}/FZAction.h
${PATH}/FZActionCamera.h
${PATH}/FZActionEase.h
${PATH}/FZActionInstant.h
${PATH}/FZActionInterval.h
${PATH}/FZActionManager.h
${PATH}/FZAllocator.h
${PATH}/FZAnimation.h
${PATH}/FZAnimationCache.h
${PATH}/FZAutoList.h
${PATH}/FZBitOrder.h
${PATH}/FZCamera.h
${PATH}/FZCommon.h
${PATH}/FZConfig.h
${PATH}/FZConsole.h
${PATH}/FZData.h
${PATH}/FZDataStore.h
${PATH}/FZDefaultOpenGLAPI.h
${PATH}/FZDeviceConfig.h
${PATH}/FZDirector.h
${PATH}/FZEvent.h
${PATH}/FZEventManager.h
${PATH}/FZFilter.h
${PATH}/FZFont.h
${PATH}/FZFontCache.h
${PATH}/FZGLConfig.h
${PATH}/FZGLProgram.h
${PATH}/FZGLState.h
${PATH}/FZGrabber.h
${PATH}/FZGrid.h
${PATH}/FZHUD.h
${PATH}/FZHash.h
${PATH}/FZIO.h
${PATH}/FZLabel.h
${PATH}/FZLayer.h
${PATH}/FZLifeCycle.h
${PATH}/FZLightSystem.h
${PATH}/FZMS.h
${PATH}/FZMacros.h
${PATH}/FZMath.h
${PATH}/FZMenu.h
${PATH}/FZMenuItem.h
${PATH}/FZNode.h
${PATH}/FZParallaxNode.h
${PATH}/FZParticleExamples.h
${PATH}/FZParticleSystem.h
${PATH}/FZParticleSystemQuad.h
${PATH}/FZPerformManager.h
${PATH}/FZOSW.h
${PATH}/FZOSW_header.h
${PATH}/FZPrimitives.h
${PATH}/FZProtocols.h
${PATH}/FZRenderTexture.h
${PATH}/FZResourcesManager.h
${PATH}/FZScene.h
${PATH}/FZScheduler.h
${PATH}/FZSelectors.h
${PATH}/FZShaderCache.h
${PATH}/FZSprite.h
${PATH}/FZSpriteBatch.h
${PATH}/FZSpriteFrame.h
${PATH}/FZSpriteFrameCache.h
${PATH}/FZTMXLayer.h
${PATH}/FZTMXParser.h
${PATH}/FZTMXTiledMap.h
${PATH}/FZTexture2D.h
${PATH}/FZTextureAtlas.h
${PATH}/FZTextureCache.h
${PATH}/FZTransitions.h
${PATH}/FZTypes.h
${PATH}/Shaders/_fz_aC4_TEX.shader.h
${PATH}/Shaders/_fz_mat_TEX.shader.h
${PATH}/Shaders/_fz_mat_aC4.shader.h
${PATH}/Shaders/_fz_mat_uC4.shader.h
${PATH}/Shaders/_fz_mat_uC4_TEX.shader.h
${PATH}/Shaders/Effects/effect_color.shader.h
${PATH}/Shaders/Effects/effect_grayscale.shader.h
${PATH}/Shaders/Effects/effect_invert.shader.h
${PATH}/Shaders/Effects/effect_toon.shader.h
${PATH}/Shaders/Effects/effect_xray.shader.h
${PATH}/Wrappers/iOSGL2_support.h
${PATH}/Wrappers/iOS_support.h
${PATH}/Wrappers/macosx_support.h
${PATH}/Wrappers/model_support.h
${PATH}/Wrappers/GLAPI/gl.h
${PATH}/Wrappers/GLAPI/glext.h
${PATH}/external/tinythread/tinythread.h
${PATH}/external/libb64/base64.h
${PATH}/external/libpng/png.h
${PATH}/external/libpng/pngconf.h
${PATH}/external/libpng/pngdebug.h
${PATH}/external/libpng/pnginfo.h
${PATH}/external/libpng/pnglibconf.h
${PATH}/external/libpng/pngpriv.h
${PATH}/external/libpng/pngstruct.h
${PATH}/external/libpng/contrib/visupng/cexcept.h
)


find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIR})


add_definitions(-DFORZE_DEBUG=2)

add_library(
    ${LIBRARY_NAME}
    STATIC
    ${SRC_FILES}
    ${INCLUDE_FILES}
)
target_link_libraries(${LIBRARY_NAME} ${ZLIB_LIBRARY})

# Make sure the compiler can find include files from our Hello library. 
include_directories (${FORZE_SOURCE_DIR}/${PATH}) 

# Make sure the linker can find the Hello library once it is built. 
link_directories (${FORZE_BINARY_DIR}/${PATH}) 

add_executable (helloworld 	${TEST_PATH}/HelloWorld.cpp) 
add_executable (miscTest 	${TEST_PATH}/MiscTest.cpp
				${TEST_PATH}/TestBase.cpp) 


# Link the executable to the Hello library. 
target_link_libraries (helloworld ${LIBRARY_NAME}) 
target_link_libraries (miscTest ${LIBRARY_NAME}) 