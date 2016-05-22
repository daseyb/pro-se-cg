/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/
#pragma once

/**
 * The RiftCamera is controlled completely by the Oculus Rift. Some members are
 * compatible with a GenericCamera but only in the getters as all internal state is
 * defined by a connected Rift.
 *
 * The center of the coordinate system is roughly in the users head.
 *
 * This cameras View-Matrix gives the translation/rotation from the center inside the
 * users body to the requested eye.
 *
 */
#if ACGL_RIFT_SDK_VERSION == 32

#include <ACGL/ACGL.hh>
#include <ACGL/Scene/GenericCamera.hh>

#include <OVR_CAPI.h>

namespace ACGL{
namespace Scene {

class OculusRiftCamera : public GenericCamera {
public:
	struct OvrEye
	{
		ovrEyeType  Eye;
		ovrFovPort  Fov;
		ovrSizei         TextureSize;
		ovrRecti         RenderViewport;
	};

    void connectWithRift(ovrHmd _hmd);
    virtual glm::vec3 getPosition() const override;
	virtual glm::mat4 getViewMatrix() const override;
	virtual glm::mat4 getProjectionMatrix() const override;
	virtual glm::uvec2 getViewportSize() const override;

    // Will update the pose based on the Rift tracking
    // and define the correct eye
    // _eyeNumber can be 0 or 1, it is undefined which is the
    // left eye and which is the right eye. The eye set will
    // be returned. Call stopRenderingEye() afterwards!
    // does also call ovrHmd_BeginEyeRender internally!
    GenericCamera::Eye startRenderingEye(int _eyeNumber);
    void stopRenderingEye(int _eyeNumber, ovrTexture* eyeTexture);

	void updateFromRift();
	const OvrEye *getOVREyeDescription() const { return mEyeDescription; }
	
private:
	ovrHmd mHmd;
	OvrEye mEyeDescription[2]; // left, right
	
	GenericCamera::Eye mEyeOrdering[2];
	ovrPosef mPoseUsedForRendering[2];
	int mActiveEye; // to index the two-element arrays
};
ACGL_SMARTPOINTER_TYPEDEFS(OculusRiftCamera)
}
}
#elif ACGL_RIFT_SDK_VERSION >= 40
// SDK 0.4 or later:

#include <ACGL/ACGL.hh>
#include <ACGL/Scene/MoveableObject.hh>
#include <ACGL/Scene/CameraBase.hh>
#include <ACGL/Scene/GenericCamera.hh>

#include <OVR_CAPI.h>

namespace ACGL{
	namespace Scene {

		class OculusRiftCamera : public GenericCamera { //public CameraBase, public MoveableObject {
		public:

			OculusRiftCamera();

			void connectWithRift(ovrHmd _hmd) { mHmd = _hmd; }
			virtual glm::vec3 getPosition() const override;
			virtual glm::mat4 getViewMatrix() const override;
			virtual glm::mat4 getProjectionMatrix() const override;
			virtual glm::uvec2 getViewportSize() const override;

			/**
			* Set the near clipping plane of the camera.
			* The plane is defined only by a distance from the camera.
			* @param _plane        New near clipping plane of the camera.
			*/
			//void setNearClippingPlane(float _plane);
			/// Gets the near clip distance
			//float getNearClippingPlane() const { return mNearClippingPlane; }

			/**
			* Set the far clipping plane of the camera.
			* The plane is defined only by a distance from the camera.
			* @param _plane        New far clipping plane of the camera.
			*/
			//void setFarClippingPlane(float _plane);
			/// Gets the far clip distance
			//float getFarClippingPlane() const { return mFarClippingPlane; }

			void setViewportSize(const glm::uvec2 _size) { mViewportSize = _size; }

			// returns which eye was configured:
			GenericCamera::Eye setPoseForEyeFromRift(int _eyeNumber);

			ovrPosef* getPoseOfEyes() { return mPoseOfEyes; }

			ovrEyeRenderDesc mEyeRenderDesc[2]; // needed for the projection matrix per eye as well as the view matrix

		private:
			ovrHmd mHmd;
			glm::uvec2 mViewportSize;
			ovrPosef mPoseOfEyes[2];

			//float mNearClippingPlane;
			//float mFarClippingPlane;

			glm::vec3 mEyePosition;
			glm::mat4 mViewMatrix;
			glm::mat4 mProjectionMatrix;
		};
		ACGL_SMARTPOINTER_TYPEDEFS(OculusRiftCamera)
	}
}

#endif // ACGL_RIFT_SDK_VERSION
