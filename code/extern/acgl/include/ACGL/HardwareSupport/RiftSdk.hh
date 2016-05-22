#pragma once

/**
 * IMPORTANT: DON'T EXPECT THIS TO HAVE A FINAL AND STABLE API!
 *
 * This needs the LibOVR version 0.4 (or higher, unless the API breaks) to work.
 * Headers of this lib need to be placed in the search path.
 *
 * In addition ACGL_USE_OCULUS_RIFT has to be defined.
 *
 * IMPORTANT:
 * The order of SDK and window initialisation is important:
 * 1. call initSDK();
 * 2. call createHMD
 * 3. create a window and a OpenGL context
 * 4. ACGL::init();
 * 5. build textures to render into (one for each eye), the size can be getOptimalRenderSizePerEye() or smaller
 * 6. build a desription of those textures for the Rift SDK using generateEyeTextureDescription ( <EyeTexture> )
 * 7. configureRendering - store the ovrEyeRenderDesc values for later, _renderTagetSize is the size of your Rift window
 * 
 * At runtime:
 * * call deactivateHealthWarning when the user presses a button
 *
 * Each frame:
 * 1. beginFrame()
 * 2. TODO: camera handling -> generates <poseOfEyes>
 * 3. ovrHmd_EndFrame( <HMD> , <poseOfEyes> , (ovrTexture*) <EyeTexture> );
 *    IMPORTANT: if ovrHmd_EndFrame gets called, don't call any swapBuffers more on your own!
 *
 * clean up:
 * 1. destroyHMD
 * 2. shutdownSDK
 *
 */
#ifdef ACGL_USE_OCULUS_RIFT

#if ACGL_RIFT_SDK_VERSION >= 40
#include <OVR_Version.h>

#if ((OVR_MAJOR_VERSION > 0) || (OVR_MINOR_VERSION >= 4))

#include <ACGL/ACGL.hh>
#include <glm/glm.hpp>
#include <OVR_CAPI.h>

#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Objects/Texture.hh>

#ifdef ACGL_COMPILE_WITH_GLFW
#include <GLFW/glfw3.h>
#endif
#ifdef ACGL_COMPILE_WITH_QT
#include <qwidget.h>
#endif
#include <OVR_CAPI_GL.h>

namespace ACGL{
namespace HardwareSupport{
namespace OVR{

	// init the SDK once before calling any of the following functions:
	bool initSDK();

	// shut it down at the end:
	void shutdownSDK();

	// create a standard Rift, can be replaced with own, more specialized code:
	ovrHmd createHMD(uint32_t _riftNumber = 0);
	void destroyHMD(ovrHmd _hmd);

	// SDK gives different sizes per eye, return the max to make things easier:
	glm::uvec2 getOptimalRenderSizePerEye(ovrHmd _hmd);

	// returns a ovrEyeRenderDesc[2] used by the SDK in _eyeRenderDesc. The returned struct stores the size of the 
	// textures, so update this if the textures change in size!
	// _eyeRenderDesc has to be an array with at least two elements!
	void generateEyeTextureDescription(ACGL::OpenGL::ConstSharedTexture2D _leftTexture,
		ACGL::OpenGL::ConstSharedTexture2D _rightTexture,
		ovrGLTexture _eyeRenderDesc[2]);

	// alternative with two (size) identical gl textures:
	void generateEyeTextureDescription(GLuint _leftTexture,
		GLuint _rightTexture, glm::uvec2 _size,
		ovrGLTexture _eyeRenderDesc[2]);

	// the Rift SDK needs to know our window, this function hides the window system/OS specifics:
	// it's limited to OpenGL / no multisampling back buffers, if more flexibility is needed,
	// write your own similar function.
	// _eyeRenderDesc will be set by this function!
#ifdef ACGL_COMPILE_WITH_GLFW
	bool configureRendering(ovrHmd _hmd, ovrEyeRenderDesc _eyeRenderDesc[2], glm::uvec2 _renderTagetSize, GLFWwindow *_window);
#endif
#ifdef ACGL_COMPILE_WITH_QT
	bool configureRendering(ovrHmd _hmd, ovrEyeRenderDesc _eyeRenderDesc[2], glm::uvec2 _renderTagetSize, QWidget *_window);
#endif

	// assumed that _cfg has already set the OS specific fields!
	bool configureRendering(ovrHmd _hmd, ovrEyeRenderDesc _eyeRenderDesc[2], glm::uvec2 _renderTagetSize, ovrGLConfig _cfg);
	
	// after calling this the warning might still be presented for a few more seconds:
	// returns true if the warning is still displayed. Can be called multiple times.
	bool deactivateHealthWarning(ovrHmd _hmd);

	// call at the beginning of a frame:
	void beginFrame(ovrHmd _hmd);

	// used only for debugging:
	void printDistortionCaps(unsigned int _caps);
}
}
}

#endif // RIFT_VERSION
#endif // ACGL_USE_OCULUS_RIFT
#endif
