/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/Scene/OculusRiftCamera.hh>
#include <ACGL/Utils/Log.hh>
#include <ACGL/HardwareSupport/RiftSdk.hh>

#if ACGL_RIFT_SDK_VERSION == 32

#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ACGL{
namespace Scene{

using namespace std;
using namespace ACGL::Utils;
using namespace ACGL::HardwareSupport;

static glm::quat ovr2glm(const ovrQuatf& _quat)
{
	glm::quat q;
	q.x = _quat.x;
	q.y = _quat.y;
	q.z = _quat.z;
	q.w = _quat.w;
	return q;
}
static glm::vec3 ovr2glm(const ovrVector3f& _vec)
{
	// This cast is ok as ovrVector3f has the same internal structure
	return *(glm::vec3*)&_vec;
}
static glm::mat4 ovr2glm(const ovrMatrix4f& _matrix)
{
	// CAUTION: column-major vs. row-major difference
	return glm::transpose(*(glm::mat4*)&_matrix);
	/*glm::mat4 m;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m[i][j] = _matrix.M[j][i];
		}
	}
	return m;*/
}
	
void OculusRiftCamera::connectWithRift(ovrHmd _hmd)
{
	
	mHmd = _hmd;
	mActiveEye = 0;

	// both eyes are equal,
	// two texture rendering, NOT side-by-side:
	glm::uvec2 renderTargetSizeForOneEye = OVR::getOptimalRenderSizePerEye(_hmd);
	ovrSizei ovrRenderTargetSizeForOneEye;
	ovrRenderTargetSizeForOneEye.w = renderTargetSizeForOneEye.x;
	ovrRenderTargetSizeForOneEye.h = renderTargetSizeForOneEye.y;

	ovrRecti perEyeViewport;
	perEyeViewport.Pos = { 0, 0 };
	perEyeViewport.Size = ovrRenderTargetSizeForOneEye;

	for (int i = 0; i < 2; ++i) {
		// for all eyes:
		mEyeDescription[i].Eye = _hmd->EyeRenderOrder[i];
		mEyeDescription[i].Fov = _hmd->DefaultEyeFov[i];
		mEyeDescription[i].TextureSize = ovrRenderTargetSizeForOneEye;
		mEyeDescription[i].RenderViewport = perEyeViewport;

		// the eye ordering (left-right or right-left) is defined by the SDK based on the
		// hardware!
		if (mEyeDescription[i].Eye == ovrEye_Left) {
				mEyeOrdering[i] = GenericCamera::Eye::EYE_LEFT;
		} else {
				mEyeOrdering[i] = GenericCamera::Eye::EYE_RIGHT;
		}
	}
	
}


GenericCamera::Eye OculusRiftCamera::startRenderingEye(int _eyeNumber)
{
	/*
	mPoseUsedForRendering[_eyeNumber] = ovrHmd_BeginEyeRender(mHmd, mEyeDescription[_eyeNumber].Eye);
	setEye(mEyeOrdering[_eyeNumber]);
	mActiveEye = _eyeNumber;

	debug() << "Render Eye " << mActiveEye << ": "
			<< mPoseUsedForRendering[_eyeNumber].Orientation.w << " "
			<< mPoseUsedForRendering[_eyeNumber].Orientation.x << " "
			<< mPoseUsedForRendering[_eyeNumber].Orientation.y << " "
			<< mPoseUsedForRendering[_eyeNumber].Orientation.z << endl;

	updateFromRift();

	return mEyeOrdering[_eyeNumber];
	*/
	return GenericCamera::Eye::EYE_LEFT;
}

void OculusRiftCamera::stopRenderingEye(int _eyeNumber, ovrTexture* eyeTexture)
{
	//ovrHmd_EndEyeRender(mHmd, mEyeDescription[_eyeNumber].Eye, mPoseUsedForRendering[_eyeNumber], eyeTexture);
}

void OculusRiftCamera::updateFromRift()
{
	// get orientation quaternion and set camera state
	// get translation and set it.

	// empty for now as matrices are constructed on-the-fly
}

glm::mat4 OculusRiftCamera::getProjectionMatrix() const
{
	ovrMatrix4f projectionMatrix = ovrMatrix4f_Projection(mEyeDescription[mActiveEye].Fov, getNearClippingPlane(), getFarClippingPlane(), true);
	return ovr2glm(projectionMatrix);
}

glm::vec3 OculusRiftCamera::getPosition() const
{
	return ovr2glm(mPoseUsedForRendering->Position);
}

glm::mat4 OculusRiftCamera::getViewMatrix() const
{
	return glm::mat4_cast(ovr2glm(mPoseUsedForRendering->Orientation)) * glm::translate(ovr2glm(mPoseUsedForRendering->Position));
}

glm::uvec2 OculusRiftCamera::getViewportSize() const
{
	return glm::uvec2(
		mEyeDescription[mActiveEye].TextureSize.w,
		mEyeDescription[mActiveEye].TextureSize.h);
}

}
}

#elif ACGL_RIFT_SDK_VERSION >= 40
// 0.4 version:

namespace ACGL{
namespace Scene{

using namespace std;
using namespace ACGL::Utils;
using namespace ACGL::HardwareSupport;

//OculusRiftCamera::OculusRiftCamera() : MoveableObject(), CameraBase()
OculusRiftCamera::OculusRiftCamera() : GenericCamera()
{
	mHmd = NULL;
	//mNearClippingPlane = 0.1;     // 10 cm
	//mFarClippingPlane  = 5000.0;  // 5000 meter
}

glm::vec3  OculusRiftCamera::getPosition() const
{
	return mEyePosition;
}
glm::mat4  OculusRiftCamera::getViewMatrix() const
{
	return mViewMatrix;
}
glm::mat4  OculusRiftCamera::getProjectionMatrix() const
{
	return mProjectionMatrix;
}
glm::uvec2  OculusRiftCamera::getViewportSize() const
{
	return mViewportSize;
}

GenericCamera::Eye OculusRiftCamera::setPoseForEyeFromRift(int _eyeNumber)
{
	assert(_eyeNumber == 0 || _eyeNumber == 1);

	ovrEyeType eyeType = mHmd->EyeRenderOrder[_eyeNumber];
	ovrPosef eyePose = ovrHmd_GetEyePose(mHmd, eyeType);
	mPoseOfEyes[eyeType] = eyePose;

	GenericCamera::Eye eye;
	if (eyeType == ovrEye_Left)  eye = GenericCamera::EYE_LEFT;
	if (eyeType == ovrEye_Right) eye = GenericCamera::EYE_RIGHT;

	// pose:
	glm::quat rotation( eyePose.Orientation.w, eyePose.Orientation.x, eyePose.Orientation.y, eyePose.Orientation.z );
	glm::vec3 translation( eyePose.Position.x, eyePose.Position.y, eyePose.Position.z );

	//std::cout << glm::to_string(translation) << std::endl;

	// projection:
	ovrMatrix4f ovrp = ovrMatrix4f_Projection(mEyeRenderDesc[_eyeNumber].Fov, getNearClippingPlane(), getFarClippingPlane(), true);
	glm::mat4 projection(
		ovrp.M[0][0], ovrp.M[1][0], ovrp.M[2][0], ovrp.M[3][0],
		ovrp.M[0][1], ovrp.M[1][1], ovrp.M[2][1], ovrp.M[3][1],
		ovrp.M[0][2], ovrp.M[1][2], ovrp.M[2][2], ovrp.M[3][2],
		ovrp.M[0][3], ovrp.M[1][3], ovrp.M[2][3], ovrp.M[3][3]
	);

	mProjectionMatrix = projection;

	glm::vec3 eyeOffset( mEyeRenderDesc[_eyeNumber].ViewAdjust.x, mEyeRenderDesc[_eyeNumber].ViewAdjust.y, mEyeRenderDesc[_eyeNumber].ViewAdjust.z );

	glm::vec4 basePosition = glm::vec4(MoveableObject::getPosition(), 1.0);
	mEyePosition = glm::vec3( glm::inverse(getViewMatrix()) * basePosition );

	mViewMatrix = glm::translate(eyeOffset) * glm::inverse( glm::translate(translation) * glm::mat4_cast(rotation) ) * MoveableObject::getModelMatrix();

	//mCamera.setHeadEyeOffset(eyeOffset);
	//mCamera.setHeadOffsetMatrix(glm::translate(translation) * glm::mat4_cast(rotation));

	return eye;
}

}
}

#endif // ACGL_RIFT_SDK_VERSION 
