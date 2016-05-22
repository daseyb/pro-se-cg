#include <ACGL/HardwareSupport/RiftSdk.hh>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

#include <iostream>

#include <ACGL/Utils/Log.hh>

#ifdef ACGL_USE_OCULUS_RIFT
#if ACGL_RIFT_SDK_VERSION >= 40

#ifdef ACGL_COMPILE_WITH_GLFW
	#define GLFW_EXPOSE_NATIVE_WIN32
	#define GLFW_EXPOSE_NATIVE_WGL
	#include <GLFW/glfw3.h>
	#include <GLFW/glfw3native.h>
#endif
#ifdef ACGL_COMPILE_WITH_QT
#include <QMainWindow>
#endif
#include <OVR_CAPI_GL.h>

using namespace std;
using namespace ACGL::Utils;

namespace ACGL{
namespace HardwareSupport{
namespace OVR{

// C API helpers:

static bool ACGL_RiftSDKInitialized = false;

bool initSDK()
{
	//cout << "initRiftSDK()" << endl;

	if (ACGL_RiftSDKInitialized) return true; // don't init twice

	ovrBool ok = ovr_Initialize();
	if (!ok) {
		error() << "could not initialize Oculus Rift library" << endl;
	}
	else {
		ACGL_RiftSDKInitialized = true;
	}
	return ACGL_RiftSDKInitialized;
}

void shutdownSDK()
{
	if (ACGL_RiftSDKInitialized) ovr_Shutdown();
	ACGL_RiftSDKInitialized = false;
}

// For more sophisticated use cases build your own Rift for your needs based on the Rift SDK instead of using this default Rift.
//
// _headTrackingIsRequired           = if false, the call will create a dummy device that won't generate any data in case no real Rift is connected
//                                     (for developing without an actual device).
// _headTranslationTrackingIsAllowed = if true the Tracking of DK2 will get supported, if false even a DK2 will behave like a DK1
ovrHmd createHMD(uint32_t _riftNumber)
{
	if (!ACGL_RiftSDKInitialized) {
		error() << "Rift SDK not initialized correctly - did you call/check initRiftSDK()?" << endl;
	}

	ovrHmd mHmd = ovrHmd_Create(_riftNumber);
	if (!mHmd) {
#if ACGL_RIFT_USE_DUMMY
		warning() << "could not connect to a real Oculus Rift HMD - generating sensorless dummy" << endl;
		mHmd = ovrHmd_CreateDebug(ovrHmd_DK1);
#else
		debug() << "could not connect to a real Oculus Rift HMD" << endl;
		mHmd = NULL;
#endif
		return mHmd;
	}


	// debug output:
	debug() << "Connected to: " << mHmd->ProductName << endl;

	// start the tracking:
	// what the application supports:
	unsigned int supportedCaps = ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position; // | ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction;

	// what the device must deliver as a bare minimum:
	unsigned int requiredCaps = 0;

	ovrBool ok = ovrHmd_ConfigureTracking(mHmd, supportedCaps, requiredCaps);
	if (!ok) {
		error() << "could not get connected to a Rift tracker - only rendering is supported" << endl;
	}

	return mHmd;
}

void destroyHMD(ovrHmd _hmd)
{
	if (_hmd) ovrHmd_Destroy(_hmd);
}


glm::uvec2 getOptimalRenderSizePerEye(ovrHmd _hmd)
{
	if (_hmd == NULL) return glm::uvec2(640, 800);

	ovrSizei optimalLeft = ovrHmd_GetFovTextureSize(_hmd, ovrEye_Left, _hmd->DefaultEyeFov[0], 1.0f);
	ovrSizei optimalRight = ovrHmd_GetFovTextureSize(_hmd, ovrEye_Right, _hmd->DefaultEyeFov[1], 1.0f);

	//debug() << "optimalLeft  " << optimalLeft.w << " " << optimalLeft.h << endl;
	//debug() << "optimalRight " << optimalRight.w << " " << optimalRight.h << endl;

	//debug() << "hmd: " << _hmd->ProductName << endl;
	//debug() << "hmd WindowsPos: " << _hmd->WindowsPos.x << " " << _hmd->WindowsPos.y << endl;

	return glm::uvec2(glm::max(optimalLeft.w, optimalRight.w), glm::max(optimalLeft.h, optimalRight.h));
}

void generateEyeTextureDescription(ACGL::OpenGL::ConstSharedTexture2D _leftTexture, ACGL::OpenGL::ConstSharedTexture2D _rightTexture, ovrGLTexture _eyeRenderDesc[2])
{
	// same for both eyes:
	for (int eyeIndex = 0; eyeIndex < 2; ++eyeIndex) {
		_eyeRenderDesc[eyeIndex].OGL.Header.API = ovrRenderAPI_OpenGL;
		_eyeRenderDesc[eyeIndex].OGL.Header.RenderViewport.Pos.x = 0;
		_eyeRenderDesc[eyeIndex].OGL.Header.RenderViewport.Pos.y = 0;
	}

	_eyeRenderDesc[0].OGL.Header.TextureSize.w = _leftTexture->getWidth();
	_eyeRenderDesc[0].OGL.Header.TextureSize.h = _leftTexture->getHeight();
	_eyeRenderDesc[1].OGL.Header.TextureSize.w = _rightTexture->getWidth();
	_eyeRenderDesc[1].OGL.Header.TextureSize.h = _rightTexture->getHeight();
	_eyeRenderDesc[0].OGL.Header.RenderViewport.Size.w = _leftTexture->getWidth();
	_eyeRenderDesc[0].OGL.Header.RenderViewport.Size.h = _leftTexture->getHeight();
	_eyeRenderDesc[1].OGL.Header.RenderViewport.Size.w = _rightTexture->getWidth();
	_eyeRenderDesc[1].OGL.Header.RenderViewport.Size.h = _rightTexture->getHeight();

	_eyeRenderDesc[0].OGL.TexId = _leftTexture->getObjectName();
	_eyeRenderDesc[1].OGL.TexId = _rightTexture->getObjectName();
}

void generateEyeTextureDescription(GLuint _leftTexture, GLuint _rightTexture, glm::uvec2 _size, ovrGLTexture _eyeRenderDesc[2])
{
	// same for both eyes:
	for (int eyeIndex = 0; eyeIndex < 2; ++eyeIndex) {
		_eyeRenderDesc[eyeIndex].OGL.Header.API = ovrRenderAPI_OpenGL;
		_eyeRenderDesc[eyeIndex].OGL.Header.RenderViewport.Pos.x  = 0;
		_eyeRenderDesc[eyeIndex].OGL.Header.RenderViewport.Pos.y  = 0;
		_eyeRenderDesc[eyeIndex].OGL.Header.RenderViewport.Size.w = _size.x;
		_eyeRenderDesc[eyeIndex].OGL.Header.RenderViewport.Size.h = _size.y;
		_eyeRenderDesc[eyeIndex].OGL.Header.TextureSize.w         = _size.x;
		_eyeRenderDesc[eyeIndex].OGL.Header.TextureSize.h         = _size.y;
	}
	_eyeRenderDesc[0].OGL.TexId = _leftTexture;
	_eyeRenderDesc[1].OGL.TexId = _rightTexture;
}


#ifdef ACGL_COMPILE_WITH_GLFW
bool configureRendering(ovrHmd _hmd, ovrEyeRenderDesc _eyeRenderDesc[2], glm::uvec2 _renderTagetSize, GLFWwindow *_window)
{
	ovrGLConfig cfg;
#ifdef OVR_OS_WIN32
	cfg.OGL.Window = glfwGetWin32Window(_window);
	cfg.OGL.DC = GetDC(cfg.OGL.Window);
#endif
#ifdef OVR_OS_MAC
	debug() << "configure Mac specifics: TODO" << std::endl;
#endif
#ifdef OVR_OS_LINUX
	debug() << "configure Linux specifics: TODO" << std::endl;
#endif
	return configureRendering(_hmd, _eyeRenderDesc, _renderTagetSize, cfg);
}
#endif

#ifdef ACGL_COMPILE_WITH_QT
bool configureRendering(ovrHmd _hmd, ovrEyeRenderDesc _eyeRenderDesc[2], glm::uvec2 _renderTagetSize, QWidget *_window)
{
	ovrGLConfig cfg;
#ifdef OVR_OS_WIN32
	cfg.OGL.Window = (HWND) _window->winId();
	cfg.OGL.DC = GetDC(cfg.OGL.Window);
#endif
#ifdef OVR_OS_MAC
	debug() << "configure Mac specifics: TODO" << std::endl;
#endif
#ifdef OVR_OS_LINUX
	debug() << "configure Linux specifics: TODO" << std::endl;
#endif
	return configureRendering(_hmd, _eyeRenderDesc, _renderTagetSize, cfg);
}
#endif


bool configureRendering(ovrHmd _hmd, ovrEyeRenderDesc _eyeRenderDesc[2], glm::uvec2 _renderTagetSize, ovrGLConfig _cfg)
{
	if (!_hmd) return false;

	// Update ovr Rendering configuration
	_cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
	_cfg.OGL.Header.RTSize.w = _renderTagetSize.x;
	_cfg.OGL.Header.RTSize.h = _renderTagetSize.y;
	_cfg.OGL.Header.Multisample = 0;
	// OS specifics are already set in _cfg!

	//debug() << "call ovrHmd_ConfigureRendering():" << std::endl;
	//printDistortionCaps(_hmd->DistortionCaps);
	unsigned int distortionCaps = _hmd->DistortionCaps ^ ovrDistortionCap_FlipInput; // don't flip the input
	//distortionCaps ^= ovrDistortionCap_NoRestore;
	distortionCaps ^= ovrDistortionCap_SRGB;

#ifdef OVR_OS_WIN32
	ovrHmd_AttachToWindow(_hmd, _cfg.OGL.Window, nullptr, nullptr);
#endif

	//printDistortionCaps(distortionCaps);
	ovrBool ok = ovrHmd_ConfigureRendering(_hmd, (ovrRenderAPIConfig*)&_cfg, distortionCaps, _hmd->DefaultEyeFov, _eyeRenderDesc);
	if (ok) {
		bool riftIsUsingExtendedDesktop = (bool)(_hmd->HmdCaps & ovrHmdCap_ExtendDesktop);
		//debug() << "OVR Rendering configured" << std::endl;
	}
	else {
		error() << "OVR Rendering failed to get configured" << std::endl;
	}

	return (bool)ok;
}


bool deactivateHealthWarning(ovrHmd _hmd)
{
	if (!_hmd) return false;

	ovrHSWDisplayState displayState;
	ovrHmd_GetHSWDisplayState(_hmd, &displayState);

	if (displayState.Displayed) {
		ovrHmd_DismissHSWDisplay(_hmd);
		return true;
	}

	return false;
}

void beginFrame(ovrHmd _hmd)
{
	glFrontFace(GL_CCW);  // not reset by the health warning :-(
	glDepthMask(GL_TRUE); // not reset by Rift SDK :-(
	ovrFrameTiming frameTiming = ovrHmd_BeginFrame(_hmd, 0);
}

// prints the distortion caps for debugging:
void printDistortionCaps(unsigned int _caps) {
	debug() << endl << "Distortion caps: " << _caps << " = ";
	if (_caps & ovrDistortionCap_Chromatic) debug() << "Chromatic ";
	if (_caps & ovrDistortionCap_TimeWarp) debug() << "TimeWarp ";
	if (_caps & ovrDistortionCap_Vignette) debug() << "Vignette ";
	if (_caps & ovrDistortionCap_NoRestore) debug() << "GLStateChangedBySDK ";
	if (_caps & ovrDistortionCap_FlipInput) debug() << "FlipInput ";
	if (_caps & ovrDistortionCap_SRGB) debug() << "sRGB_Input ";
	if (_caps & ovrDistortionCap_Overdrive) debug() << "reduceDK2Artefacts ";
	debug() << endl << endl;
}

}
}
}

#endif
#endif
