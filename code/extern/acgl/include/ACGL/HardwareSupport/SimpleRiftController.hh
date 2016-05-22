#pragma once

/**
 * IMPORTANT: DON'T EXPECT THIS CLASS TO HAVE A FINAL AND STABLE API!
 *
 * This class needs the LibOVR version 0.2.4 or higher to work.
 * Headers of this lib need to be placed in the search path.
 *
 * In addition ACGL_USE_OCULUS_RIFT has to be defined.
 *
 *
 */

#include <ACGL/ACGL.hh>
#ifdef ACGL_USE_OCULUS_RIFT
#if ACGL_RIFT_SDK_VERSION < 40

#include <ACGL/Math/Math.hh>
#include <glm/gtc/quaternion.hpp>
#include <ACGL/Scene/HMDCamera.hh>

#include <ACGL/OpenGL/Objects/Texture.hh>
#include <ACGL/OpenGL/Managers.hh>

#include <ACGL/HardwareSupport/OVRWrapper.hh>

namespace ACGL{
namespace HardwareSupport{

/**
 * This class provides access to the Oculus Rift. It can read out the orientation and control a HMDCamera
 * based on this.
 * Distorted rendering is provided in two ways:
 * * renderDistorted( texture ) if the input is one side-by-side rendered image
 * * renderDistorted( texture, texture ) if the input are two seperate textures for the both eyes
 *
 * Alternatively the application can implement the distortion on its own (e.g. to add other effects in the
 * same pass). For this the needed parameters are provided.
 *
 * Use the camera provided by this class (getCamera) or provide your own (attachCamera).
 * This class needs to use a HMDCamera which is derived from GenericCamera!
 */
class SimpleRiftController
{
public:
    /**
     * _riftnumber: which device to use in case multiple are attached - NOTE: the Rift SDK has problems supporting this yet!
     * _performAutomaticMagneticCalibration: try to calibrate the magetometer to reduce drift
     *                                       the user has to look into at least four very different directions
     *                                       for this to work.
     */
    SimpleRiftController( uint32_t _riftnumber = 0 );
    ~SimpleRiftController();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Camera and sensor handling:
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //! attach an external camera to manipulate (see updateCamera)
    //! per default the SimpleRiftController already has a camera which can be used as well
    //! only one camera can be attached at all times
    void attachCamera( ACGL::Scene::SharedHMDCamera _camera );

    ACGL::Scene::SharedHMDCamera getCamera() { return mCamera; }

    //! Query the orientation of the Rift and set it as the cameras orientation.
    //! This will do nothing if no Rift is attached (so the camera can get controlled
    //! e.g. by a mouse)!
    void updateCamera();

    //! returns the current orientation as a rotation matrix from the device.
    //! this can be used as an alternative to updateCamera if the attached camera should not be used.
    glm::mat3 getCurrentRotation();

    //! sets the amound of seconds to predict the headmovements into the future
    //! default is 0.03f, should be no more than the rendering latency!
    void setPrediction( float _seconds );

    void setFoVMultiplier( float _factor ) { mFoVMultiplier = _factor; updateCameraFoV(); }
    float getFoVMultiplier() { return mFoVMultiplier; }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Access to direct sensor data as an alternative to the use of the ACGL camera
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //! get the rotation as a quaternion, the native format of the Rift (with sensor fusion):
    glm::quat getFusedRotationQuaternion();

    //! get the rotation as a quaternion, the native format of the Rift (without sensor fusion):
    glm::quat getRotationQuaternion();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // RAW parameters for distortion rendering:
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //! Default is 1.0, larger values result in higher FoVs and larger areas of the Rift being used.
    //! Note that - depending on the lenses used - the user will not be able to see the whole screen.
    //! Often 1.75 is enough.
    //! Increase the offscreen rendering viewport accordingly to counter the decreased image quality.
    void setDistortionScaleFactor( float _f );
    float getDistortionScaleFactor() { return mDistortionScaleFactor; }

    //! x,y are the values for the left eye. z,w the values for the right eye
    glm::vec4 getLensCenter();

    //! x,y are the values for the left eye. z,w the values for the right eye
    glm::vec4 getScreenCenter();

    //! x,y are the values for both eyes, ignore z,w
    glm::vec4 getScale();

    //! x,y are the values for both eyes, ignore z,w
    glm::vec4 getScaleIn();

    //! the four distortion parameters are the same for both eyes
    glm::vec4 getHmdWarpParam();

    //! the four chromatic aberation parameters are the same for both eyes
    glm::vec4 getChromAbParam();

    //! the full physical screen resolution, offscreen rendering should get performed at a higher resolution!
    //! 'full' means it's the size used for both eyes!
    glm::uvec2 getPhysicalScreenResolution();

    //! returns the stereo projection from the stored camera adjusted for the rift
    //! returns nonsens in case no camera was set
    glm::mat4 getProjectionMatrixFromCamera();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Optional integrated distortion rendering:
    //
    // If it's activated and used, make sure the RiftDistort* shader files are located where the
    // ShaderProgramFileManager can find them.
    // They may set texture units 0..3 and render to Framebuffer 0 (which will get bound) using the viewport of the
    // physical dimensions of the rift!
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void renderDistorted( ACGL::OpenGL::ConstSharedTexture2D _sideBySideTexture );
    void renderDistorted( ACGL::OpenGL::ConstSharedTexture2D _leftTexture, ACGL::OpenGL::ConstSharedTexture2D _rightTexture );
    void renderDistortedP( ACGL::OpenGL::ConstSharedShaderProgram _program );

    bool getSuccessfulConnected() { return mSuccessfulConnected; }

    //! activate and deactivate the distortion, only works if the renderers above are used, does not change the raw
    //! distortion parameters!
    void setDistortion( bool _value ) { mUseDistortion = _value; }
    bool getDistortion() { return mUseDistortion; }

    //! activate and deactivate the chromatic aberation correction (true to correct the aberation), only works if the renderers
    //! above are used, does not change the raw distortion parameters!
    void setChromaticAberation( bool _value ) { mUseChromaticAberation = _value; }
    bool getChromaticAberation() { return mUseChromaticAberation; }

    //! Sets the size of the final rendering. This should be the size of the window to render into.
    void setOutputViewportSize( glm::uvec2 _size ) { mOutputViewport = _size; }

    //! Defines that the current HMD orientations should be defined as "no rotation"
    //! Can be used to "reset" the orientation.
    //! Note: if the user is just looking in the wrong direction, use setNeutralYaw(), if e.g. looking up should be
    //!       neutral (laying on the ground), this is the way to go.
    void setNeutralPosition();
    void resetNeutralRotation() {setNeutralPosition();}

    //! Will define the current view direction as the neutral direction but only takes yaw into account.
    //! Will also reset the neutral position.
    //! Basically works as if the Rift was started in the current orientation
    void setNeutralYaw();

private:

    // for rendering:
    ACGL::OpenGL::SharedShaderProgram mDistortShaderSideBySide;
    ACGL::OpenGL::SharedShaderProgram mDistortShaderTwoTextures;

    ACGL::OpenGL::SharedVertexArrayObject mVAO;
    GLint uLensCenter;
    GLint uScreenCenter;
    GLint uScale;
    GLint uScaleIn;
    GLint uHmdWarpParam;
    GLint uChromAbParam;
    GLint uDistort;
    GLint uCorrectChromaticAberation;
    bool mBuildInShader;

    bool mUseDistortion;
    bool mUseChromaticAberation;
    glm::uvec2 mOutputViewport; // if it's 0,0 -> use the Rifts screen dimensions!
private:
    glm::vec4 getShaderValue( int v );
    void updateCameraFoV();

    bool mSuccessfulConnected;
    ACGL::Scene::SharedHMDCamera mCamera;
    float mDistortionScaleFactor;

    // handles to the rift:
    OVR::Ptr<OVR::DeviceManager> mORManager;
    OVR::Ptr<OVR::HMDDevice>     mORDevice;
    OVR::Ptr<OVR::SensorDevice>  mORSensor;
    OVR::SensorFusion            *mORSensorFusion;
    OVR::HMDInfo                 *mORHMDInfo;

    // all rotations are relative to the one the Rift started with:
    OVR::Quatf mInverseNeutralRotation; // as quaternion

    float mPredictionTime;
    float mFoVMultiplier;
};

ACGL_SMARTPOINTER_TYPEDEFS(SimpleRiftController)

}
}

#else

#include <ACGL/Math/Math.hh>
#include <glm/gtc/quaternion.hpp>
#include <ACGL/Scene/OculusRiftCamera.hh>
#include <ACGL/Scene/GenericCamera.hh>

#include <ACGL/OpenGL/Objects/Texture.hh>

#include <ACGL/HardwareSupport/OVRWrapper.hh>
#include <ACGL/HardwareSupport/RiftSdk.hh>

namespace ACGL{
	namespace HardwareSupport{

		class SimpleRiftController
		{
		public:
			SimpleRiftController(uint32_t _riftnumber = 0);
			~SimpleRiftController();

			// only used for compatibility with the old SimpleRiftController:
			ACGL::Scene::SharedGenericCamera getCamera() {
				if (mHMD) { return mCamera; }
				else { return mNormalCamera; }
			}

			ACGL::Scene::SharedOculusRiftCamera getOVRCamera() { return mCamera; }

			// set the textures to render into to this size or smaller:
			glm::uvec2 getOptimalRenderSizePerEye() { return OVR::getOptimalRenderSizePerEye(mHMD); }

			// update the camera to catch the position for the eye 0 or 1, don't assume that 0 is left!
			// call updateCameraEye(0) and updateCameraEye(1) once per frame!
			ACGL::Scene::GenericCamera::Eye updateCameraEye(int _eyeNumber);
			
			//! the full physical screen resolution, offscreen rendering should get performed at a higher resolution!
			//! 'full' means it's the size used for both eyes!
			glm::uvec2 getPhysicalScreenResolution();

			// if _outputViewportSize is 0, use mOutputViewportSize (set previously by setOutputViewportSize() );
			void configureRendering(ACGL::OpenGL::ConstSharedTexture2D _leftTexture, ACGL::OpenGL::ConstSharedTexture2D _rightTexture );
			void configureRendering(GLuint _leftTexture, GLuint _rightTexture, glm::uvec2 _size );
			void renderDistorted();

#ifdef ACGL_COMPILE_WITH_GLFW
			void setGLFWWindow(GLFWwindow *_window) { mGLFWWindow = _window; }
#endif
#ifdef ACGL_COMPILE_WITH_QT
			void setQWidget( QWidget *_qwidget ) { mQWidget = _qwidget; }
#endif

			bool getSuccessfulConnected() { return (mHMD != NULL); }

			// recenter translation and yaw:
			void recenterPose() { ovrHmd_RecenterPose(mHMD); }

		private:
			void configureRendering();
			ACGL::Scene::SharedOculusRiftCamera mCamera;
			ACGL::Scene::SharedGenericCamera mNormalCamera;
			ovrHmd mHMD;

			bool mRenderingConfigured;
			ovrGLTexture     mGLTexture[2];

#ifdef ACGL_COMPILE_WITH_GLFW
			GLFWwindow *mGLFWWindow;
#endif
#ifdef ACGL_COMPILE_WITH_QT
			QWidget *mQWidget;
#endif
			int mNextExpectedEyeNumber;
		};

		ACGL_SMARTPOINTER_TYPEDEFS(SimpleRiftController)

	}
}

#endif // RIFT_VERSION
#endif // ACGL_USE_OCULUS_RIFT