#include <ACGL/HardwareSupport/SimpleRiftController.hh>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

#ifdef ACGL_USE_OCULUS_RIFT
#if ACGL_RIFT_SDK_VERSION < 40

using namespace OVR;
using namespace ACGL;
using namespace ACGL::Utils;
using namespace ACGL::Scene;
using namespace ACGL::HardwareSupport;
using namespace ACGL::OpenGL;
using namespace std;


SimpleRiftController::SimpleRiftController( uint32_t _riftnumber )
{
    mSuccessfulConnected = false;
    mDistortionScaleFactor = 1.0f;
    mUseDistortion = true;
    mUseChromaticAberation = true;
    mPredictionTime = -1.0f;
    mFoVMultiplier  =  1.0f;
    mORSensorFusion = NULL;

    if (_riftnumber != 0) {
        error() << "opening any other Rift than ID 0 is not supported yet! Trying to open Rift nr 0" << endl;
        _riftnumber = 0;
    }

#ifdef ACGL_DEBUG
    LogMaskConstants mask = LogMask_All;
#else
    LogMaskConstants mask = LogMask_None;
#endif
    System::Init( Log::ConfigureDefaultLog( mask) );

    mORManager = *DeviceManager::Create();
    if (!mORManager) {
        error() << "could not create a Rift Device Manager" << endl;
        mORDevice = NULL;
    } else {
        DeviceEnumerator<HMDDevice> hmdEnumerator = mORManager->EnumerateDevices<HMDDevice>();

        bool riftOK = true;
        for (uint32_t i = 0; i < _riftnumber; ++i) {
            riftOK = hmdEnumerator.Next();
        }

        if (riftOK) {
            mORDevice = *hmdEnumerator.CreateDevice();
        } else {
            error() << "Rift number " << _riftnumber << " not found" << endl;
            mORDevice = NULL;
        }
    }

    mORHMDInfo = new OVR::HMDInfo();
    const char *profileName = NULL;
    OVR::Ptr<OVR::Profile> profile = NULL;
    if (mORDevice) {
        // get profile of this device:
        profile     = mORDevice->GetProfile();
        profileName = mORDevice->GetProfileName();

        // get device info:
        if (!mORDevice->GetDeviceInfo( mORHMDInfo )) {
            error() << "could not get HMD device info" << endl;
            return;
        }

        mORSensor = *mORDevice->GetSensor();
        if (!mORSensor) {
            error() << "could not get sensor of HMD (is the USB pluged in?)" << endl;
            #ifdef __linux
            error() << "do you have read/write permissions of /dev/hidraw* ?" << endl;
            #endif
        } else {
            // configure sensor
            mORSensorFusion = new OVR::SensorFusion();
            mORSensorFusion->AttachToSensor( mORSensor );

            mSuccessfulConnected = true;
        }
    } else {
        // get the default profile:
        OVR::Ptr<OVR::ProfileManager> pm = *OVR::ProfileManager::Create();
        profileName = pm->GetDefaultProfileName(OVR::Profile_RiftDK1);
        profile     = pm->LoadProfile(OVR::Profile_RiftDK1, profileName);

        // default device info from the first devkit:
        mORHMDInfo->HResolution = 1280;
        mORHMDInfo->VResolution =  800;
        mORHMDInfo->HScreenSize = 0.14976f;
        mORHMDInfo->VScreenSize = 0.0935f;
        mORHMDInfo->VScreenCenter = mORHMDInfo->VScreenSize*0.5f;
        mORHMDInfo->DistortionK[0]        = 1.0f;
        mORHMDInfo->DistortionK[1]        = 0.22f;
        mORHMDInfo->DistortionK[2]        = 0.24f;
        mORHMDInfo->EyeToScreenDistance   = 0.041f;
        mORHMDInfo->ChromaAbCorrection[0] = 0.996f;
        mORHMDInfo->ChromaAbCorrection[1] = -0.004f;
        mORHMDInfo->ChromaAbCorrection[2] = 1.014f;
        mORHMDInfo->ChromaAbCorrection[3] = 0.0f;
        mORHMDInfo->LensSeparationDistance = 0.0635f;
        mORHMDInfo->InterpupillaryDistance = 0.064f;
        mORHMDInfo->DisplayDeviceName[0] = 0;
    }

    mCamera = SharedHMDCamera(); // set to NULL
    attachCamera( SharedHMDCamera( new HMDCamera() ) ); // attach a blank camera
    // set a good default viewport:
    glm::uvec2 viewport = getPhysicalScreenResolution();
    mCamera->resize( viewport.x/2, viewport.y );
    updateCameraFoV(); // indirectly based on the viewport

    if (profile) {
        mCamera->setInterpupillaryDistance( profile->GetIPD() );
        mCamera->setEyeHeight( profile->GetEyeHeight() );
    }

    if (mSuccessfulConnected) {
        bool magneticYawCorrection = (mORSensorFusion->IsYawCorrectionEnabled() && mORSensorFusion->HasMagCalibration());

        if (magneticYawCorrection && profile) {
            debug() << "Connected to Rift (SDK: "<<OVR_VERSION_STRING<<"). Using local user settings of profile " << profileName << ". Yaw correction active." << endl;
        } else if (!magneticYawCorrection && profile) {
            debug() << "Connected to Rift (SDK: "<<OVR_VERSION_STRING<<"). Using local user settings of profile " << profileName << ". Yaw correction inactive." << endl;
        } else if (magneticYawCorrection && !profile) {
            debug() << "Connected to Rift (SDK: "<<OVR_VERSION_STRING<<"). Yaw correction active. No local user profile found." << endl;
        } else {
            debug() << "Connected to Rift (SDK: "<<OVR_VERSION_STRING<<"). Yaw correction inactive. No local user profile found." << endl;
        }
    } else {
        error() << "Could not create a Rift device - using default values (SDK: "<<OVR_VERSION_STRING<<")." << endl;
        error() << "In some cases this happens if the Rift is NOT set to the native resolution (1280*800 in case of the first/old DevKit)." << endl;
    }
}

SimpleRiftController::~SimpleRiftController()
{
    // setting the reference counted pointers to NULL will call the object destructors:
    debug() << "disconnecting from Oculus Rift..." << endl;

    mORSensor  = NULL;
    mORDevice  = NULL;
    mORManager = NULL;
    delete mORHMDInfo;
    delete mORSensorFusion;
    System::Destroy(); // Oculus Rift
}

void SimpleRiftController::setPrediction( float _seconds )
{
    if(getSuccessfulConnected()) {
        if ( _seconds >= 0.0f ) {
            mORSensorFusion->SetPrediction( _seconds );
        } else {
            mORSensorFusion->SetPrediction( 0.0f, false );
        }
        mPredictionTime = _seconds;
    }
}

glm::quat riftQuatToGLM( const Quatf &_q )
{
    glm::quat glmQuat;

    glmQuat.x = _q.x;
    glmQuat.y = _q.y;
    glmQuat.z = _q.z;
    glmQuat.w = _q.w;

    return glmQuat;
}

glm::quat SimpleRiftController::getFusedRotationQuaternion()
{
    return riftQuatToGLM( mORSensorFusion->GetPredictedOrientation() );
}

glm::quat SimpleRiftController::getRotationQuaternion()
{
    return riftQuatToGLM( mORSensorFusion->GetOrientation() );
}

glm::mat4 SimpleRiftController::getProjectionMatrixFromCamera()
{
    if (!mCamera) return glm::mat4();
    return mCamera->getProjectionMatrix();
}

glm::mat3 riftMatrixToGLM( const Matrix4f &_mat4 )
{
    glm::mat3 glmMat3;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glmMat3[i][j] = _mat4.M[i][j];
        }
    }

    return glmMat3;
}

void SimpleRiftController::attachCamera( ACGL::Scene::SharedHMDCamera _camera )
{
    mCamera = _camera;
    mCamera->setInterpupillaryDistance( mORHMDInfo->InterpupillaryDistance );
    updateCameraFoV();

    float viewCenterH = mORHMDInfo->HScreenSize * 0.25f;
    float eyeProjectionShift = viewCenterH - mORHMDInfo->LensSeparationDistance * 0.5f;
    float offsetX = 4.0f * eyeProjectionShift / mORHMDInfo->HScreenSize;
    mCamera->setProjectionCenterOffset( glm::vec2(offsetX, 0.0f));

    mCamera->setNeckToEyeVerticalDistance(   0.12f );
    mCamera->setNeckToEyeHorizontalDistance( 0.08f );
}

glm::mat3 SimpleRiftController::getCurrentRotation()
{
    // update orientation:
    Quatf q;
    if (mPredictionTime > 0.0f) {
        q = mORSensorFusion->GetPredictedOrientation();
    } else {
        q = mORSensorFusion->GetOrientation();
    }

    q.Normalize();

    //debug() << "Rift orientation: " << q.x << " " << q.y << " " << q.z << " " << q.w << endl;

    q = mInverseNeutralRotation * q;

    if (mORSensorFusion->IsYawCorrectionEnabled()) {
        //debug() << "yaw corrected" << endl;
    }

    Matrix4f orientation( q );
    return riftMatrixToGLM( orientation );
}

void SimpleRiftController::setNeutralPosition()
{
    mInverseNeutralRotation = mORSensorFusion->GetOrientation();
    mInverseNeutralRotation.Normalize();
    mInverseNeutralRotation.x *= -1.0;
    mInverseNeutralRotation.y *= -1.0;
    mInverseNeutralRotation.z *= -1.0;
}

void SimpleRiftController::setNeutralYaw()
{
    mInverseNeutralRotation = OVR::Quatf();
    mORSensorFusion->Reset();
}

void SimpleRiftController::updateCamera()
{
    if (!mSuccessfulConnected) return;

    mCamera->setHMDRotation( getCurrentRotation() );
}

void SimpleRiftController::updateCameraFoV()
{
    float percievedHalfRTDist = (mORHMDInfo->VScreenSize / 2) * mDistortionScaleFactor;
    float vfov = 2.0f * atan( percievedHalfRTDist/mORHMDInfo->EyeToScreenDistance );
    vfov = ACGL::Math::Functions::calcRadToDeg( vfov );

    vfov *= mFoVMultiplier;
    if (vfov <    5.0f) vfov =   5.0f;
    if (vfov >= 175.0f) vfov = 175.0f;

    mCamera->setVerticalFieldOfView( vfov );
    //debug() << "update VFoV: " << vfov << endl;
}

glm::uvec2 SimpleRiftController::getPhysicalScreenResolution()
{
    return glm::uvec2( mORHMDInfo->HResolution, mORHMDInfo->VResolution );
}

void SimpleRiftController::setDistortionScaleFactor( float _f ) {
    if (mDistortionScaleFactor == _f) return;
    ACGL::Utils::debug() << "set distortion scale " << _f << std::endl;
    mDistortionScaleFactor = _f;
    updateCameraFoV();
}

glm::vec4 SimpleRiftController::getShaderValue( int v )
{
    //
    // note that this isn't the virtul camera viewport (the size the offscreen rendering is performed in)
    // but the physical size of the Rifts screen:
    glm::vec2 windowSize = glm::vec2( getPhysicalScreenResolution() );

    glm::vec2 viewport = windowSize;
    viewport.x /= 2;

    glm::vec2 viewportPosL = glm::vec2(          0, 0 );
    glm::vec2 viewportPosR = glm::vec2( viewport.x, 0 );
    glm::vec2 viewportSize = glm::vec2( viewport.x, viewport.y ); // viewport of one eye

    float w  = float(viewportSize.x) / float(windowSize.x);
    float h  = float(viewportSize.y) / float(windowSize.y);
    float xl = float(viewportPosL.x) / float(windowSize.x);
    float yl = float(viewportPosL.y) / float(windowSize.y);

    float xr = float(viewportPosR.x) / float(windowSize.x);
    float yr = float(viewportPosR.y) / float(windowSize.y);

    // both eyes have the same aspect ratio: hals the windowsize as the image was rendered side by side
    float aspectRatio =  (0.5 * windowSize.x) / windowSize.y;

    float lensOffset        = mORHMDInfo->LensSeparationDistance * 0.5f;
    float lensShift         = mORHMDInfo->HScreenSize * 0.25f - lensOffset;
    float lensViewportShift = 4.0f * lensShift / mORHMDInfo->HScreenSize;
    float lensViewportShiftL =  lensViewportShift;
    float lensViewportShiftR = -lensViewportShift;

    glm::vec4 lensCenter;
    lensCenter.x = xl + (w + lensViewportShiftL * 0.5f)*0.5f;
    lensCenter.y = yl + h*0.5f;
    lensCenter.z = xr + (w + lensViewportShiftR * 0.5f)*0.5f;
    lensCenter.w = yr + h*0.5f;

    glm::vec4 screenCenter;
    screenCenter.x = xl + w*0.5f;
    screenCenter.y = yl + h*0.5f;
    screenCenter.z = xr + w*0.5f;
    screenCenter.w = yr + h*0.5f;

    glm::vec4 scale;
    scale.x = (w/2);
    scale.y = (h/2) * aspectRatio;
    scale /= mDistortionScaleFactor;

    glm::vec4 scaleIn;
    scaleIn.x = (2/w);
    scaleIn.y = (2/h) / aspectRatio;

    if (v == 0) return lensCenter;
    if (v == 1) return screenCenter;
    if (v == 2) return scale;
    return scaleIn;
}

glm::vec4 SimpleRiftController::getLensCenter()
{
    return getShaderValue(0);
}

glm::vec4 SimpleRiftController::getScreenCenter()
{
    return getShaderValue(1);
}

glm::vec4 SimpleRiftController::getScale()
{
    return getShaderValue(2);
}

glm::vec4 SimpleRiftController::getScaleIn()
{
    return getShaderValue(3);
}

glm::vec4 SimpleRiftController::getHmdWarpParam()
{
    glm::vec4 distortionK;
    distortionK.x = mORHMDInfo->DistortionK[0];
    distortionK.y = mORHMDInfo->DistortionK[1];
    distortionK.z = mORHMDInfo->DistortionK[2];
    distortionK.w = mORHMDInfo->DistortionK[3];
    return distortionK;
}

glm::vec4 SimpleRiftController::getChromAbParam()
{
    glm::vec4 chromaK;
    chromaK.x = mORHMDInfo->ChromaAbCorrection[0];
    chromaK.y = mORHMDInfo->ChromaAbCorrection[1];
    chromaK.z = mORHMDInfo->ChromaAbCorrection[2];
    chromaK.w = mORHMDInfo->ChromaAbCorrection[3];
    return chromaK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// optional rendering nuild-in shaders:
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char *VS_SOURCE = "#version 150 core \n\
\n\
        out vec2 vTexCoord;\n\
\n\
        const vec2 quad[4] = vec2[] (\n\
            vec2(-1.0, 1.0),\n\
            vec2(-1.0,-1.0),\n\
            vec2( 1.0, 1.0),\n\
            vec2( 1.0,-1.0)\n\
        );\n\
\n\
        void main()\n\
        {\n\
            vec2 p = quad[ gl_VertexID ];\n\
\n\
            vTexCoord   = p * vec2(0.5, 0.5) + vec2(0.5, 0.5);\n\
            gl_Position = vec4(p, 0.0, 1.0);\n\
        }";

const char *FS_SOURCE_SIDE_BY_SIDE = "#version 150 core\n\
        \n\
        uniform sampler2D uSamplerColor;\n\
        \n\
        // defaults are the values of the first dev kit rendered at 1280 by 800:\n\
        uniform vec4 uLensCenter    = vec4(0.287994, 0.5, 0.712006, 0.5);\n\
        uniform vec4 uScreenCenter  = vec4(0.25, 0.5,  0.75, 0.5);\n\
        uniform vec4 uScale         = vec4(0.25, 0.4,  0.25, 0.4);\n\
        uniform vec4 uScaleIn       = vec4(4.0,  2.5,  4.0,  2.5);\n\
        uniform vec4 uHmdWarpParam  = vec4(1.0,  0.22, 0.24, 0.0);\n\
        uniform vec4 uChromAbParam  = vec4(0.996, -0.004, 1.014, 0.0);\n\
        uniform bool uDistort = true;\n\
        uniform bool uCorrectChromaticAberation = true;\n\
        in vec2 vTexCoord;\n\
        out vec4 oColor;\n\
        \n\
        //\n\
        // taken and adjusted from the SDK sample:\n\
        //\n\
        vec4 getDistortedColorAt( in vec2 position, in bool leftEye )\n\
        {\n\
            vec2 LensCenter;\n\
            vec2 ScreenCenter;\n\
            vec2 Scale   = uScale.xy;\n\
            vec2 ScaleIn = uScaleIn.xy;\n\
            if (leftEye) {\n\
                // left half\n\
                LensCenter   = uLensCenter.xy;\n\
                ScreenCenter = uScreenCenter.xy;\n\
            } else {\n\
                // right half\n\
                LensCenter   = uLensCenter.zw;\n\
                ScreenCenter = uScreenCenter.zw;\n\
            }\n\
            // vector from the lens center to the current point:\n\
            vec2  theta = (position - LensCenter) * ScaleIn; \n\
            // scaled distance from the lens center:\n\
            float rSq = theta.x * theta.x + theta.y * theta.y;\n\
            vec2  theta1 = theta * (uHmdWarpParam.x + uHmdWarpParam.y * rSq + uHmdWarpParam.z * rSq * rSq + uHmdWarpParam.w * rSq * rSq * rSq);\n\
            // Detect whether blue texture coordinates are out of range since these will scaled out the furthest.\n\
            vec2 thetaBlue = theta1 * (uChromAbParam.z + uChromAbParam.w * rSq);\n\
            vec2 tcBlue = Scale * thetaBlue + LensCenter;\n\
            if (!all(equal(clamp(tcBlue, ScreenCenter-vec2(0.25,0.5), ScreenCenter+vec2(0.25,0.5)), tcBlue)))\n\
            {\n\
                return vec4(0.0);\n\
            }\n\
            // Do green lookup (no scaling).\n\
            vec2  tcGreen = Scale * theta1 + LensCenter;\n\
            vec4  center = texture(uSamplerColor, tcGreen);\n\
            if (!uCorrectChromaticAberation) {\n\
                return center;\n\
            }\n\
            // Now do blue texture lookup.\n\
            float blue = texture(uSamplerColor, tcBlue).b;\n\
            // Do red scale and lookup.\n\
            vec2  thetaRed = theta1 * (uChromAbParam.x + uChromAbParam.y * rSq);\n\
            vec2  tcRed = Scale * thetaRed + LensCenter;\n\
            float red = texture(uSamplerColor, tcRed).r;\n\
            return vec4(red, center.g, blue, center.a);\n\
        }\n\
 \n\
        void main()\n\
        {\n\
            if (uDistort) {\n\
                bool leftEye = (vTexCoord.x < 0.5);\n\
                oColor = getDistortedColorAt( vTexCoord, leftEye );\n\
            } else {\n\
                oColor = texture(uSamplerColor, vTexCoord);\n\
            }\n\
        }\n\
";

const char *FS_SOURCE_TWO_TEXTURE = "#version 150 core\n\
        \n\
        uniform sampler2D uSamplerColorLeft;\n\
        uniform sampler2D uSamplerColorRight;\n\
        \n\
        // defaults are the values of the first dev kit rendered at 1280 by 800:\n\
        uniform vec4 uLensCenter    = vec4(0.287994, 0.5, 0.712006, 0.5);\n\
        uniform vec4 uScreenCenter  = vec4(0.25, 0.5,  0.75, 0.5);\n\
        uniform vec4 uScale         = vec4(0.25, 0.4,  0.25, 0.4);\n\
        uniform vec4 uScaleIn       = vec4(4.0,  2.5,  4.0,  2.5);\n\
        uniform vec4 uHmdWarpParam  = vec4(1.0,  0.22, 0.24, 0.0);\n\
        uniform vec4 uChromAbParam  = vec4(0.996, -0.004, 1.014, 0.0);\n\
        uniform bool uDistort = true;\n\
        uniform bool uCorrectChromaticAberation = true;\n\
        in vec2 vTexCoord;\n\
        out vec4 oColor;\n\
        \n\
        //\n\
        // taken and adjusted from the SDK sample:\n\
        //\n\
        vec4 getDistortedColorAt( in vec2 position, in bool leftEye )\n\
        {\n\
            vec2 LensCenter;\n\
            vec2 ScreenCenter;\n\
            vec2 Scale   = uScale.xy;\n\
            vec2 ScaleIn = uScaleIn.xy;\n\
            if (leftEye) {\n\
                // left half\n\
                LensCenter   = uLensCenter.xy;\n\
                ScreenCenter = uScreenCenter.xy;\n\
            } else {\n\
                // right half\n\
                LensCenter   = uLensCenter.zw;\n\
                ScreenCenter = uScreenCenter.zw;\n\
            }\n\
        \n\
            // vector from the lens center to the current point:\n\
            vec2  theta = (position - LensCenter) * ScaleIn; // Scales to [-1, 1]\n\
        \n\
            // scaled distance from the lens center:\n\
            float rSq = theta.x * theta.x + theta.y * theta.y;\n\
        \n\
            vec2  theta1 = theta * (uHmdWarpParam.x + uHmdWarpParam.y * rSq + uHmdWarpParam.z * rSq * rSq + uHmdWarpParam.w * rSq * rSq * rSq);\n\
        \n\
            // Detect whether blue texture coordinates are out of range since these will scaled out the furthest.\n\
            vec2 thetaBlue = theta1 * (uChromAbParam.z + uChromAbParam.w * rSq);\n\
            vec2 tcBlue = Scale * thetaBlue + LensCenter;\n\
            if (!all(equal(clamp(tcBlue, ScreenCenter-vec2(0.25,0.5), ScreenCenter+vec2(0.25,0.5)), tcBlue)))\n\
            {\n\
                return vec4(0.0);\n\
            }\n\
            // Do green lookup (no scaling).\n\
            vec2  tcGreen = Scale * theta1 + LensCenter;\n\
            vec4  center;\n\
            if (leftEye) {\n\
                tcGreen.x *= 2.0;\n\
                center = texture(uSamplerColorLeft, tcGreen);\n\
            } else {\n\
                tcGreen.x -= 0.5;\n\
                tcGreen.x *= 2.0;\n\
                center = texture(uSamplerColorRight, tcGreen);\n\
            }\n\
            if (!uCorrectChromaticAberation) {\n\
                return center;\n\
            }\n\
            // Now do blue texture lookup.\n\
            float blue;\n\
            if (leftEye) {\n\
                tcBlue.x *= 2.0;\n\
                blue = texture(uSamplerColorLeft, tcBlue).b;\n\
            } else {\n\
                tcBlue.x -= 0.5;\n\
                tcBlue.x *= 2.0;\n\
                blue = texture(uSamplerColorRight, tcBlue).b;\n\
            }\n\
            // Do red scale and lookup.\n\
            vec2  thetaRed = theta1 * (uChromAbParam.x + uChromAbParam.y * rSq);\n\
            vec2  tcRed = Scale * thetaRed + LensCenter;\n\
            float red;\n\
            if (leftEye) {\n\
                tcRed.x *= 2.0;\n\
                red = texture(uSamplerColorLeft, tcRed).r;\n\
            } else {\n\
                tcRed.x -= 0.5;\n\
                tcRed.x *= 2.0;\n\
                red = texture(uSamplerColorRight, tcRed).r;\n\
            }\n\
            return vec4(red, center.g, blue, center.a);\n\
        }\n\
        \n\
        void main()\n\
        {\n\
            bool leftEye = (vTexCoord.x < 0.5);\n\
            if (uDistort) {\n\
                oColor = getDistortedColorAt( vTexCoord, leftEye );\n\
            } else {\n\
                vec2 inTextureCoordinate = vTexCoord;\n\
                inTextureCoordinate.x *= 2.0;\n\
                if (leftEye) {\n\
                    oColor = texture( uSamplerColorLeft, inTextureCoordinate );\n\
                } else {\n\
                    oColor = texture( uSamplerColorRight, inTextureCoordinate );\n\
                }\n\
            }\n\
        }\n\
\n\
";

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// optional rendering:
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SimpleRiftController::renderDistorted( OpenGL::ConstSharedTexture2D _sideBySideTexture )
{
    if (!mDistortShaderSideBySide) {
        // initialize shaders:
        mDistortShaderSideBySide = OpenGL::ShaderProgramFileManager::the()->get( OpenGL::ShaderProgramCreator("RiftDistortSideBySide.fsh").andFile("RiftDistort.vsh") );
        if (!mDistortShaderSideBySide) {
            ACGL::Utils::debug() << "SimpleRiftController: using build-in shaders" << std::endl;

            mDistortShaderSideBySide = SharedShaderProgram( new ShaderProgram() );

            SharedShader vs = SharedShader( new Shader( GL_VERTEX_SHADER ) );
            vs->setSource( VS_SOURCE );

            SharedShader fs = SharedShader( new Shader( GL_FRAGMENT_SHADER ) );
            fs->setSource( FS_SOURCE_SIDE_BY_SIDE );

            mDistortShaderSideBySide->attachShader( vs );
            mDistortShaderSideBySide->attachShader( fs );
            mDistortShaderSideBySide->link();
            mDistortShaderSideBySide->use();


            uLensCenter    = mDistortShaderSideBySide->getUniformLocation("uLensCenter");
            uScreenCenter  = mDistortShaderSideBySide->getUniformLocation("uScreenCenter");
            uScale         = mDistortShaderSideBySide->getUniformLocation("uScale");
            uScaleIn       = mDistortShaderSideBySide->getUniformLocation("uScaleIn");
            uHmdWarpParam  = mDistortShaderSideBySide->getUniformLocation("uHmdWarpParam");
            uChromAbParam  = mDistortShaderSideBySide->getUniformLocation("uChromAbParam");
            uDistort       = mDistortShaderSideBySide->getUniformLocation("uDistort");
            uCorrectChromaticAberation = mDistortShaderSideBySide->getUniformLocation("uCorrectChromaticAberation");

            mBuildInShader = true;
            mDistortShaderTwoTextures->setUniform(uLensCenter,   getLensCenter() );
            mDistortShaderTwoTextures->setUniform(uScreenCenter, getScreenCenter() );
            mDistortShaderTwoTextures->setUniform(uScale,        getScale() );
            mDistortShaderTwoTextures->setUniform(uHmdWarpParam, getHmdWarpParam() );
            mDistortShaderTwoTextures->setUniform(uChromAbParam, getChromAbParam() );
        } else {
            ACGL::Utils::debug() << "SimpleRiftController: using application provided shaders" << std::endl;
            mBuildInShader = false;
        }
    }

    mDistortShaderSideBySide->use();
    mDistortShaderSideBySide->setTexture("uSamplerColor", _sideBySideTexture, 0 );
    renderDistortedP( mDistortShaderSideBySide );
}

void SimpleRiftController::renderDistorted( OpenGL::ConstSharedTexture2D _leftTexture, OpenGL::ConstSharedTexture2D _rightTexture )
{
    if (!mDistortShaderTwoTextures) {
        // initialize shaders:
        mDistortShaderTwoTextures = OpenGL::ShaderProgramFileManager::the()->get( OpenGL::ShaderProgramCreator("RiftDistortTwoTexture.fsh").andFile("RiftDistort.vsh") );
        if (!mDistortShaderTwoTextures) {
            ACGL::Utils::debug() << "SimpleRiftController: using build-in shaders" << std::endl;

            mDistortShaderTwoTextures = SharedShaderProgram( new ShaderProgram() );

            SharedShader vs = SharedShader( new Shader( GL_VERTEX_SHADER ) );
            vs->setSource( VS_SOURCE );

            SharedShader fs = SharedShader( new Shader( GL_FRAGMENT_SHADER ) );
            fs->setSource( FS_SOURCE_TWO_TEXTURE );

            mDistortShaderTwoTextures->attachShader( vs );
            mDistortShaderTwoTextures->attachShader( fs );
            mDistortShaderTwoTextures->link();
            mDistortShaderTwoTextures->use();

            uLensCenter    = mDistortShaderTwoTextures->getUniformLocation("uLensCenter");
            uScreenCenter  = mDistortShaderTwoTextures->getUniformLocation("uScreenCenter");
            uScale         = mDistortShaderTwoTextures->getUniformLocation("uScale");
            uScaleIn       = mDistortShaderTwoTextures->getUniformLocation("uScaleIn");
            uHmdWarpParam  = mDistortShaderTwoTextures->getUniformLocation("uHmdWarpParam");
            uChromAbParam  = mDistortShaderTwoTextures->getUniformLocation("uChromAbParam");
            uDistort       = mDistortShaderTwoTextures->getUniformLocation("uDistort");
            uCorrectChromaticAberation = mDistortShaderTwoTextures->getUniformLocation("uCorrectChromaticAberation");

            mBuildInShader = true;
            mDistortShaderTwoTextures->setUniform(uLensCenter,   getLensCenter() );
            mDistortShaderTwoTextures->setUniform(uScreenCenter, getScreenCenter() );
            mDistortShaderTwoTextures->setUniform(uScale,        getScale() );
            mDistortShaderTwoTextures->setUniform(uHmdWarpParam, getHmdWarpParam() );
            mDistortShaderTwoTextures->setUniform(uChromAbParam, getChromAbParam() );

        } else {
            ACGL::Utils::debug() << "SimpleRiftController: using application provided shaders" << std::endl;
            mBuildInShader = false;
        }
    }

    mDistortShaderTwoTextures->use();
    mDistortShaderTwoTextures->setTexture("uSamplerColorLeft",  _leftTexture,  0 );
    mDistortShaderTwoTextures->setTexture("uSamplerColorRight", _rightTexture, 1 );

    renderDistortedP( mDistortShaderTwoTextures );
}

void SimpleRiftController::renderDistortedP( ACGL::OpenGL::ConstSharedShaderProgram _program )
{
    // if the user defined an output size, use that, otherwise default to the Rifts size:
    glm::uvec2 windowSize = mOutputViewport;
    if (windowSize.x == 0) {
        windowSize = getPhysicalScreenResolution();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport( 0, 0, windowSize.x, windowSize.y );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // we assume that _program is in use as this should only get called from our own functions

    if (!mBuildInShader) {
        // the shader might have been reloaded
        _program->setUniform("uLensCenter",   getLensCenter() );
        _program->setUniform("uScreenCenter", getScreenCenter() );
        _program->setUniform("uScale",        getScale() );
        _program->setUniform("uScaleIn",      getScaleIn() );
        _program->setUniform("uHmdWarpParam", getHmdWarpParam() );
        _program->setUniform("uChromAbParam", getChromAbParam() );
        _program->setUniform("uDistort",      (int)mUseDistortion );
        _program->setUniform("uCorrectChromaticAberation", (int)mUseChromaticAberation );
    } else {
        _program->setUniform(uScaleIn,      getScaleIn() );
        _program->setUniform(uDistort,      (int)mUseDistortion );
        _program->setUniform(uCorrectChromaticAberation, (int)mUseChromaticAberation );
    }

    // attribute-less rendering:
    // just rendering a fullscreen quad
    if (!mVAO) {
        mVAO = SharedVertexArrayObject( new VertexArrayObject() );
    }
    mVAO->bind(); // 'empty' VAO -> no attributes are defined
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 ); // create 2 triangles with no attributes
}

#else


#include <ACGL/HardwareSupport/RiftSdk.hh>

using namespace ACGL;
using namespace ACGL::Utils;
using namespace ACGL::Scene;
using namespace ACGL::HardwareSupport;
using namespace ACGL::OpenGL;
using namespace std;


SimpleRiftController::SimpleRiftController(uint32_t _riftnumber) : mRenderingConfigured(false), mNextExpectedEyeNumber(0) {
#ifdef ACGL_COMPILE_WITH_GLFW
	mGLFWWindow = NULL;
#endif
	OVR::initSDK();
	mHMD = OVR::createHMD(_riftnumber);

	mCamera = SharedOculusRiftCamera(new OculusRiftCamera());
	mCamera->connectWithRift(mHMD);
	// set a good default viewport:
	glm::uvec2 viewport = getPhysicalScreenResolution();
	viewport.x /= 2;
	mCamera->setViewportSize(viewport);
	
}

SimpleRiftController::~SimpleRiftController() {
	OVR::destroyHMD(mHMD);
	OVR::shutdownSDK();
}

void SimpleRiftController::configureRendering(GLuint _leftTexture, GLuint _rightTexture, glm::uvec2 _size ) {
	// mGLTexture will be set:
	ACGL::HardwareSupport::OVR::generateEyeTextureDescription(_leftTexture, _rightTexture, _size, mGLTexture);
	configureRendering();
}

void SimpleRiftController::configureRendering(ACGL::OpenGL::ConstSharedTexture2D _leftTexture, ACGL::OpenGL::ConstSharedTexture2D _rightTexture) {
	// mGLTexture will be set:
	ACGL::HardwareSupport::OVR::generateEyeTextureDescription(_leftTexture, _rightTexture, mGLTexture);
	configureRendering();
}

void SimpleRiftController::configureRendering() {
	if (!mHMD) return;

	int width, height;
#ifdef ACGL_COMPILE_WITH_GLFW
	assert(mGLFWWindow && "call setGLFWWindow first!");
	glfwGetWindowSize(mGLFWWindow, &width, &height);
	// mEyeRenderDesc will be set by this:
	ACGL::HardwareSupport::OVR::configureRendering(mHMD, mCamera->mEyeRenderDesc, glm::uvec2(width,height), mGLFWWindow);
#elif ACGL_COMPILE_WITH_QT
	assert(mQWidget && "call setQWidget first!");
	width  = mQWidget->size().width();
	height = mQWidget->size().height();
	ACGL::HardwareSupport::OVR::configureRendering(mHMD, mCamera->mEyeRenderDesc, glm::uvec2(width,height), mQWidget);
#else
	assert(0 && "can't configure SDK rendering as no OS Window is provided!");
#endif

	mRenderingConfigured = true;
}

void SimpleRiftController::renderDistorted() {
	if (!mHMD) return;

	assert(mRenderingConfigured && "SimpleRiftController::renderDistorted(): configure rendering first, call configureRendering() !");
	assert(mNextExpectedEyeNumber == 2 && "call updateCameraEye for both eyes first!");
	mNextExpectedEyeNumber = 0; // for the next frame

	ovrHmd_EndFrame(mHMD, mCamera->getPoseOfEyes(), (ovrTexture*)mGLTexture);
}

glm::uvec2 SimpleRiftController::getPhysicalScreenResolution() { 
	return glm::uvec2( mHMD->Resolution.w, mHMD->Resolution.h );
}

GenericCamera::Eye SimpleRiftController::updateCameraEye(int _eyeNumber)
{
	if (!mHMD) return GenericCamera::EYE_LEFT;

	// make sure to switch between eye 0 and 1 exactly once per frame
	assert(_eyeNumber == 0 || _eyeNumber == 1);
	assert(_eyeNumber == mNextExpectedEyeNumber);
	mNextExpectedEyeNumber++; // will wrap around in renderDistorted!

	if (_eyeNumber == 0) {
		// has to be called at the beginning of the frame, one reason why the eye order is important!
		OVR::beginFrame(mHMD);
		OVR::deactivateHealthWarning(mHMD); // just quit this asap -> apps should also call this as this might not stay here
	}

	return mCamera->setPoseForEyeFromRift(_eyeNumber);
	
}

#endif
#endif
