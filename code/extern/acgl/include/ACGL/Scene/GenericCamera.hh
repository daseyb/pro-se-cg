/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once
#include <ACGL/ACGL.hh>
#include <ACGL/Math/Math.hh>
#include <glm/gtx/quaternion.hpp>

#include "CameraBase.hh"
#include "MoveableObject.hh"

/*
 * What you definitly want to set:
 * - a position in 3D space (a vec3)
 * - a viewing direction, this can be defined by:
 *   - roll/pitch/jaw rotations
 *   - up/left/forward vectors
 * - the aspect ratio (width/height)
 *
 * What you maybe want to change:
 * - a lookAtDistance, this is internaly only used for the orthographic
 *   projection, can be be used externaly e.g. for field of view effects
 *   (if no focal distance is given, a default will be used, but often this
 *   value is not used at all), also a lookAt point can be calculated with this
 * - Stereo settings:
 *   - the eyedistance
 *   - the StereoMode
 * - Horizontal/Vertical FoV
 * - near- and far-clipping plane
 *
 * A Camera can calculate:
 * - a ViewMatrix
 * - a ProjectionMatrix for Mono view
 * - ProjectionMatrices for Stereo view
 * - etc.
 *
 *
 * Note: To get from world to camera space, the translation is applied first, then the
 *       rotation. getViewMatrix() provides one matrix for this.
 *       Other camera models rotate first and translate later (e.g. bundler)! The rotation
 *       is the same, the translation differs.
 *
 * TODO: support more stereo modes!
 */

namespace ACGL{
namespace Scene{

class GenericCamera : public CameraBase, public MoveableObject
{
	public:
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Helping enums:
        //
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum ProjectionMode
        {
            ISOMETRIC_PROJECTION   = 0,
            PERSPECTIVE_PROJECTION
        };

        enum StereoMode
        {
            MONO = 0,
            PARALLEL_SHIFT,
            OFF_AXIS,
            TOE_IN
        };

        enum Eye
        {
            EYE_LEFT = 0,
            EYE_RIGHT
        };
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Constructor / Destructor / save&store
        //
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /**
         * Default Constructor of a camera.
         */
        GenericCamera();

        /**
         * Constructs a camera from a string which holds all camera parameters. Note that this sets the viewport which
         * might not fix the currect screen!
         */
        GenericCamera( const std::string &_state );

        /**
         * Destructor of a camera.
         */
        ~GenericCamera() {}

        /// Writes all internal state to one string
        std::string storeStateToString() const;

        /// Sets all internal state from a string
        void setStateFromString( const std::string &_state );
	
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Set / Get basic camera properties:
        //
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
        /**
         * Set the distance between the pupils (only needed for stereo rendering).
         * @param _interpupillaryDistance Inter pupil distance in meter(!) (distance between the centers of the eyes)
         */
        void setInterpupillaryDistance( float _interpupillaryDistance ) { mInterpupillaryDistance = _interpupillaryDistance; }
        float getInterpupillaryDistance() const { return mInterpupillaryDistance; }
        
		/**
         * Set the projection mode of the camera.
         * @param _projection           New projection mode of the camera.
		 */
        void setProjectionMode( ProjectionMode _projection ) { mProjectionMode = _projection; }
        ProjectionMode getProjectionMode() const { return mProjectionMode; }

        /**
         * Set the stereo mode of the camera. In mode MONO the set eye will get ignored (see below).
         * @param _stereoMode New stereo mode of the camera.
         */
        void setStereoMode( StereoMode _stereoMode ) { mStereoMode = _stereoMode; }
        StereoMode getStereoMode() const { return mStereoMode; }

        /**
         * Sets the currently active eye. In stereo mode MONO this setting is ignored.
         * In the stereo modes (PARALLEL_SHIFT, OFF_AXIS, TOE_IN) it is used to
         * define the default eye that is assumed for the generic get*Matrix() functions.
         * (Matrices for specific eyes can still get queried directly without setting the
         * eye explicitly before each call).
         */
        void setEye( Eye _eye ) { mCurrentEye = _eye; }
        Eye getEye() const { return mCurrentEye; }
	
		/**
         * Set the horizontal field of view of the camera in degree.
         * vertical FoV will get (implicitly) changed!
         * @param _fovh         New horizontal field of view of the camera.
		 */
        void setHorizontalFieldOfView( float _fovh );
        float getHorizontalFieldOfView() const { return mHorizontalFieldOfView; }

		/**
         * Set the vertical field of view of the camera in degree.
         * Horizontal FoV will get changed!
         * @param _fovv         New vertical field of view of the camera.
		 */
        void setVerticalFieldOfView( float _fovv );
        float getVerticalFieldOfView() const;
	
		/**
        * Set the aspect ratio of the camera. The horizontal FoV stays the same, the
		* vertical FoV gets updated.
		* @param aspectRatio	New aspect ratio (width/height)
		*/
        void setAspectRatio( float _aspectRatio ) { mAspectRatio = _aspectRatio; }
        float getAspectRatio() const { return mAspectRatio; }

		/**
		 * Set the near clipping plane of the camera.
		 * The plane is defined only by a distance from the camera.
         * @param _plane        New near clipping plane of the camera.
		 */
        void setNearClippingPlane( float _plane );
        /// Gets the near clip distance
        float getNearClippingPlane() const { return mNearClippingPlane; }

		/**
		 * Set the far clipping plane of the camera.
		 * The plane is defined only by a distance from the camera.
         * @param _plane        New far clipping plane of the camera.
		 */
        void setFarClippingPlane( float _plane );
        /// Gets the far clip distance
        float getFarClippingPlane() const { return mFarClippingPlane; }

        /// Gets size of the viewport
        virtual glm::uvec2 getViewportSize() const { return mViewportSize; }
        /// Gets width of the viewport
        unsigned int getViewportWidth() const { return mViewportSize.x; }
        /// Gets height of the viewport
        unsigned int getViewportHeight() const { return mViewportSize.y; }
        /// Sets size of the viewport. NOTE: DOES NOT CHANGE THE ASPECT RATIO! Use resize() if you want to change that as well!
        void setViewportSize( glm::uvec2 _size ) { mViewportSize = _size; }
        /// Sets size of the viewport. NOTE: DOES NOT CHANGE THE ASPECT RATIO! Use resize() if you want to change that as well!
        void setViewportSize( unsigned int _width, unsigned int _height ) { setViewportSize(glm::uvec2(_width, _height)); }

        /// Sets new viewport size and calculates new aspect ratio
        void resize(int _newWidth, int _newHeight) { setViewportSize(_newWidth, _newHeight); setAspectRatio(_newWidth / (float)_newHeight); }

        /// The focal length is coupled to the sensor size in real cameras. As this camera does not model a
        /// sensor size in mm, the focal length is given in pixels and is in relation to the viewports resolution.
        /// This model is also used by bundler.
        /// Note that this gives only useful results if the viewports aspect ratio is the same as the
        /// projections aspect ratio!
        float getFocalLenghtInPixel() const;

        /// Sets the focal length in pixel relative to the viewport dimension. This will change the FoV.
        /// See getFocalLenghtInPixel() for more information.
        void setFocalLengthInPixel( float _focalLengthInPixel );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Set / Get the matrices:
        //
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        // Generic rotation and translation matrices: In mono rendering these represent the camera.
        // In stereo rendering these represent the virtual camera position between the two cameras
        // actually used.
        //

        /// forward to MovableObject to implement the full CameraBase interface:
        glm::vec3 getPosition() const { return MoveableObject::getPosition(); }

        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        // Generic view and projection matrices. These obey the set stereo/eye settings.
        // (When in doubt, use these!)
        //

        /// Gets the currently active view matrix (depends on stereo mode and current eye)
        virtual glm::mat4 getViewMatrix() const;
        /// Gets the currently active view matrix (depends on stereo mode and current eye)
        glm::mat4 getInverseViewMatrix() const;
        /// Gets the currently active projection matrix (depends on stereo mode and current eye)
        virtual glm::mat4 getProjectionMatrix() const;


        ///////////////////////////////////////////////////////////////////////////////////////////
        //
        // Explicit view and projection matrices. These DON'T obey the set stereo/eye settings.
        //

        /// Gets the view matrix for non-stereo rendering EVEN IF A STEREO MODE IS SET!
        glm::mat4 getMonoViewMatrix() const;
        glm::mat4 getMonoInverseViewMatrix() const;

        /**
         * Compute a camera view matrix for stereo rendering.
         * In stereo mode, the view matrix is the mono view matrix but also the shift
         * by half the eye distance to the left/right and a small rotation inwards in
         * case of toe in mode.
         *
         * These methods get the stereo matrix independent of the set mode for this camera.
         */
        glm::mat4 getStereoViewMatrix( bool _leftEye, StereoMode _stereoMode = OFF_AXIS ) const;
        glm::mat4 getLeftStereoViewMatrix() const { return getStereoViewMatrix(true,  mStereoMode); }
        glm::mat4 getRightStereoViewMatrix()const { return getStereoViewMatrix(false, mStereoMode); }
        glm::mat4 getLeftParallelShiftStereoViewMatrix() const { return getStereoViewMatrix(true,  PARALLEL_SHIFT); }
        glm::mat4 getRightParallelShiftStereoViewMatrix()const { return getStereoViewMatrix(false, PARALLEL_SHIFT); }
        glm::mat4 getLeftOffAxisStereoViewMatrix()       const { return getStereoViewMatrix(true,  OFF_AXIS); }
        glm::mat4 getRightOffAxisStereoViewMatrix()      const { return getStereoViewMatrix(false, OFF_AXIS); }
        glm::mat4 getLeftToeInStereoViewMatrix()         const { return getStereoViewMatrix(true,  TOE_IN); }
        glm::mat4 getRightToeInStereoViewMatrix()        const { return getStereoViewMatrix(false, TOE_IN); }

        /// Gets the projection matrix for non-stereo rendering EVEN IF A STEREO MODE IS SET!
        glm::mat4 getMonoProjectionMatrix() const;

        /**
         * Compute a camera projection matrix for stereo rendering.
         * In stereo mode, the Cameras position is the point in the middle between the two eyes.
         * So we just need one additional info to calculate two matrices:
         */
        glm::mat4 getStereoProjectionMatrix( bool _leftEye, StereoMode _stereoMode = OFF_AXIS ) const;
        glm::mat4 getLeftStereoProjectionMatrix()              const { return getStereoProjectionMatrix(true,  mStereoMode); }
        glm::mat4 getRightStereoProjectionMatrix()             const { return getStereoProjectionMatrix(false, mStereoMode); }
        glm::mat4 getLeftParallelShiftStereoProjectionMatrix() const { return getStereoProjectionMatrix(true,  PARALLEL_SHIFT); }
        glm::mat4 getRightParallelShiftStereoProjectionMatrix()const { return getStereoProjectionMatrix(false, PARALLEL_SHIFT); }
        glm::mat4 getLeftOffAxisStereoProjectionMatrix()       const { return getStereoProjectionMatrix(true,  OFF_AXIS); }
        glm::mat4 getRightOffAxisStereoProjectionMatrix()      const { return getStereoProjectionMatrix(false, OFF_AXIS); }
        glm::mat4 getLeftToeInStereoProjectionMatrix()         const { return getStereoProjectionMatrix(true,  TOE_IN); }
        glm::mat4 getRightToeInStereoProjectionMatrix()        const { return getStereoProjectionMatrix(false, TOE_IN); }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Set / Get properties that move the camera around (or rotate etc.)
        //
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * Look around with a mouse, works like a FPS camera:
         *  No roll possible.
         *  Up/down is limited to 90 degree.
         * This method assumes there is no roll in the current camera rotation, if
         * there is a roll component, it will get destroyed -> don't mix this
         * way to stear with other, more flexible methods!
         * @param _deltaX How much the mouse moved on the viewport (0..1, 1 = full viewport width)
         * @param _deltaY How much the mouse moved on the viewport (0..1, 1 = full viewport height)
         */
        void FPSstyleLookAround( float _deltaX, float _deltaY );

        // rotate around the target, x,y,z are angles to rotate by IN RADIANCE
        // the rotation is around the global goordinate system (1,0,0 / 0,1,0 / 0,0,1)
        // "turntable style" rotation if _x is set and _y,_z == 0
        void rotateAroundTaget_GlobalAxes( float _x, float _y, float _z );

        // rotate around the current coordinate system
        void rotateAroundTaget_LocalAxes( float _x, float _y, float _z );

	private:
        ///
        /// States: update the storeStateToString() & setStateFromString() functions whenever adding a new state!
        ///

		/// Current camera projection mode
        ProjectionMode mProjectionMode;
        /// stereo view mode
        StereoMode mStereoMode;
        /// Current eye
        Eye mCurrentEye;
		/// Current camera horizontal field of view
        float mHorizontalFieldOfView;
		/// Current aspect ratio: width/height.
        float mAspectRatio;
	
        /// Distance of the eyes for stereo projection. In that case, the left eye is 0.5*InterpupillaryDistance
        /// shifted to the left of position and the right eye is 0.5*InterpupillaryDistance to the right shifted.
		/// We assume that 1 unit equals 1 meter. The mean eye distance is 6.5 cm == 0.065 units
        float mInterpupillaryDistance;
	
		/// Current camera near clipping plane
        float mNearClippingPlane;
		/// Current camera far clipping plane
        float mFarClippingPlane;

        /// viewport in pixel:
        glm::uvec2 mViewportSize;

        // helper:
        void rotateAroundTaget_helper( float _x, float _y, float _z, const glm::mat3 &_rotationAxes );
};

ACGL_SMARTPOINTER_TYPEDEFS(GenericCamera)

} // Scene
} // ACGL

