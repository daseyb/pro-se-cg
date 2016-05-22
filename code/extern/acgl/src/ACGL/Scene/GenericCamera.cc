/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/Scene/GenericCamera.hh>
#include <ACGL/Utils/StringHelpers.hh>

#include <cassert>

#include <ACGL/Math/Math.hh>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace ACGL;
using namespace ACGL::Math::Functions;
using namespace ACGL::Scene;
using namespace ACGL::Utils::StringHelpers;
using namespace std;

GenericCamera::GenericCamera() :
    mProjectionMode(PERSPECTIVE_PROJECTION),
    mStereoMode(MONO),
    mCurrentEye(EYE_LEFT),
    mHorizontalFieldOfView(75.0f),
    mAspectRatio( 4.0f/3.0f ),
    mInterpupillaryDistance( 0.064f ), // 0.064 m = 6.4 cm - mean human eye distance: 6.47cm (male), 6.23cm (female)
    mNearClippingPlane(0.1f),     // 10 cm
    mFarClippingPlane(5000.0f)   // 5000 meter
{
    setRotationMatrix( glm::mat3(1.0f) );
}

GenericCamera::GenericCamera( const std::string &_state )
{
    setStateFromString( _state );
}

void GenericCamera::FPSstyleLookAround( float _deltaX, float _deltaY )
{
    float yaw   = 0.0f;
    float pitch = 0.0f;
    glm::mat3 R = getRotationMatrix3();

    // get roll / pitch / yaw from the current rotation matrix:
    float yaw1 = asin(-R[2][0]);
    float yaw2 = (float)M_PI - asin(-R[2][0]);

    float pitch1  = (cos(yaw1) > 0)? atan2(R[2][1], R[2][2]): atan2(-R[2][1], -R[2][2]);
    float pitch2  = (cos(yaw2) > 0)? atan2(R[2][1], R[2][2]): atan2(-R[2][1], -R[2][2]);

    float roll1   = (cos(yaw1) > 0)? atan2(R[1][0], R[0][0]): atan2(-R[1][0], -R[0][0]);
    float roll2   = (cos(yaw2) > 0)? atan2(R[1][0], R[0][0]): atan2(-R[1][0], -R[0][0]);

    // we assume no roll at all, in that case one of the roll variants will be 0.0
    // if not, use the smaller one -> minimize the camera "jump" as this will destroy
    // information
    if ( std::abs(roll1) <= std::abs(roll2) ) {
        yaw   = -yaw1;
        pitch = -pitch1;
    } else {
        yaw   = -yaw2;
        pitch = -pitch2;
    }

    // add rotation diffs given:
    yaw = yaw + _deltaX;
    pitch = glm::clamp( pitch + _deltaY, -0.5f * (float)M_PI, 0.5f*(float)M_PI );

    // create rotation matices, seperated so we have full control over the order:
    glm::mat4 newRotY = glm::yawPitchRoll( yaw, 0.0f, 0.0f );
    glm::mat4 newRotX = glm::yawPitchRoll( 0.0f, pitch, 0.0f );

    // multiplication order is important to prevent roll:
    setRotationMatrix( newRotX * newRotY );
}

void GenericCamera::rotateAroundTaget_GlobalAxes( float _x, float _y, float _z)
{
    // move camera so, that the target is the center, then rotate around the
    // global coordinate system

    glm::mat3 t = glm::mat3( glm::vec3(1.0f, 0.0f, 0.0f),
                             glm::vec3(0.0f, 1.0f, 0.0f),
                             glm::vec3(0.0f, 0.0f, 1.0f) );

    rotateAroundTaget_helper(_x,_y,_z, t);
}

void GenericCamera::rotateAroundTaget_LocalAxes( float _x, float _y, float _z)
{
    glm::mat3 R = getRotationMatrix3();
    R = glm::transpose( R );

    rotateAroundTaget_helper(_x,_y,_z, R);
}

void GenericCamera::rotateAroundTaget_helper( float _x, float _y, float _z, const glm::mat3 &_rotationAxes )
{
    glm::vec4 T = glm::vec4( getTarget(), 1.0f );
    glm::vec4 P = glm::vec4( getPosition(), 1.0f );

    glm::vec4 tempPos = P - T;
    glm::mat4 newRotation = glm::rotate( glm::mat4(), _x, _rotationAxes[0] );
              newRotation = glm::rotate( newRotation, _y, _rotationAxes[1] );
              newRotation = glm::rotate( newRotation, _z, _rotationAxes[2] );

    tempPos = newRotation * tempPos;

    P = tempPos + T; // new position
    glm::vec4 N = glm::vec4( getUpDirection(), 1.0f );
    N = newRotation * N;

    setLookAtMatrix( glm::vec3(P), glm::vec3(T), glm::vec3(N) );
}


void GenericCamera::setHorizontalFieldOfView(float _fovh)
{
    assert( _fovh < 180.0f );
    assert( _fovh > 0.0f );
    mHorizontalFieldOfView = _fovh;
}

void GenericCamera::setVerticalFieldOfView(float _fovv)
{
    assert( _fovv < 180.0f );
    assert( _fovv > 0.0f );
	
    // we only save the aspectRatio and the horizontal FoV
	// so if we change the vertical FoV, we change the aspectRatio
	
    //mAspectRatio = tan( calcDegToRad(0.5f * mHorizontalFieldOfView) ) / tan( calcDegToRad(0.5f * _fovv) );

    float x = tan( calcDegToRad(0.5f * _fovv) ) * mAspectRatio;
    mHorizontalFieldOfView = calcRadToDeg( 2.0f * atan( x ) );
}

float GenericCamera::getVerticalFieldOfView() const
{
    return calcRadToDeg( atan( tan( calcDegToRad(0.5f*mHorizontalFieldOfView) ) / mAspectRatio ) *2.0f );
}
	
void GenericCamera::setNearClippingPlane(float _plane)
{
    assert (_plane > 0.0f);
    mNearClippingPlane = _plane;
}

void GenericCamera::setFarClippingPlane(float _plane)
{
    assert (_plane > 0.0f);
    mFarClippingPlane = _plane;
}

glm::mat4 GenericCamera::getViewMatrix() const
{
    if (mStereoMode == MONO) {
        return getMonoViewMatrix();
    } else {
        // all kinds of stereo
        bool eyeIsLeftEye = (getEye() == EYE_LEFT);
        return getStereoViewMatrix( eyeIsLeftEye, mStereoMode );
    }
}

glm::mat4 GenericCamera::getStereoViewMatrix( bool _leftEye, StereoMode _stereoMode ) const
{
    // The view matrix is independent of the projection mode (isometric or perspective)
    // so only the stereo mode has to be checked.
    assert( _stereoMode != MONO && "mono is not a stereo mode!" );

    float cameraPositionShiftValue = (mInterpupillaryDistance*0.5f); // shift to the right
    if (_leftEye) cameraPositionShiftValue *= -1.0f;      // if left eye shift to the left

    if ( (_stereoMode == PARALLEL_SHIFT) || (_stereoMode == OFF_AXIS) ) {
        //
        // parallel shift and off-axis have the same view matrices:
        // just shift the camera position to the left/right by half the eye-distance
        //

        //ACGL::Utils::debug() << "WARNING: getStereoViewMatrix() is not tested yet" << std::endl; // remove after testing

        glm::mat3 inverseRotation = getInverseRotationMatrix3();
        glm::vec3 eyePosition = mPosition + (inverseRotation * glm::vec3(cameraPositionShiftValue,0.0f,0.0f) );

        glm::mat4 m(mRotationMatrix);
        m[3][0] = -(m[0][0] * eyePosition.x + m[1][0] * eyePosition.y + m[2][0] * eyePosition.z);
        m[3][1] = -(m[0][1] * eyePosition.x + m[1][1] * eyePosition.y + m[2][1] * eyePosition.z);
        m[3][2] = -(m[0][2] * eyePosition.x + m[1][2] * eyePosition.y + m[2][2] * eyePosition.z);
        return m;
    }

    // else it has to be toe-in:
    assert( _stereoMode == TOE_IN && "unsupported stereo mode!" );
    //
    // Toe-in: shift the camera position to the left/right by half the eye-distance and
    //         rotate a bit inwards so that the two cameras focus the same point
    //         at the look-at distance (focal point)

    assert(0 && "getStereoViewMatrix() for TOE_IN is not implemented yet!");
    return glm::mat4();
}

glm::mat4 GenericCamera::getInverseViewMatrix() const
{
    if (mStereoMode == MONO) {
        return getMonoInverseViewMatrix();
    }

    glm::mat4 viewMatrix = getViewMatrix();
    return glm::inverse( viewMatrix );
}

glm::mat4 GenericCamera::getProjectionMatrix() const
{
    if (mStereoMode == MONO) {
        return getMonoProjectionMatrix();
    } else {
        // all kinds of stereo
        bool eyeIsLeftEye = (getEye() == EYE_LEFT);
        return getStereoProjectionMatrix( eyeIsLeftEye, mStereoMode );
    }
}

glm::mat4 GenericCamera::getMonoProjectionMatrix() const
{
    glm::mat4 projectionMatrix; // identity matrix

    if ( getProjectionMode() == ISOMETRIC_PROJECTION )
    {
        // we don't set the left/right/top/bottom values explicitly, so we want that
        // all object at our focal distance appear the same in perspective and isometric view
        float right  = tan( calcDegToRad(getHorizontalFieldOfView() * 0.5f) ) * mLookAtDistance;
        float left   = -right;
        float top    = tan( calcDegToRad(getVerticalFieldOfView() * 0.5f) ) * mLookAtDistance;
        float bottom = -top;

        // we do the same here as a glOrtho call would do.
        projectionMatrix[ 0][0] =  2.0f / (right - left);
        projectionMatrix[ 1][1] =  2.0f / (top   - bottom);
        projectionMatrix[ 2][2] = -2.0f / (mFarClippingPlane - mNearClippingPlane);
        projectionMatrix[ 0][3] = -(right+left) / (right-left);
        projectionMatrix[ 1][3] = -(top+bottom) / (top-bottom);
        projectionMatrix[ 2][3] = -(mFarClippingPlane+mNearClippingPlane)/(mFarClippingPlane-mNearClippingPlane);
        projectionMatrix[ 3][3] =  1.0;
    }
    else if ( mProjectionMode == PERSPECTIVE_PROJECTION )
    {
        projectionMatrix = glm::perspective( glm::radians( (float)getHorizontalFieldOfView()), (float)getAspectRatio(), (float)mNearClippingPlane, (float)mFarClippingPlane );
    }
    else assert(0 && "unsupported projection mode");

    return projectionMatrix;
}

glm::mat4 GenericCamera::getMonoViewMatrix() const
{
    glm::mat4 m(mRotationMatrix);
    m[3][0] = -(m[0][0] * mPosition.x + m[1][0] * mPosition.y + m[2][0] * mPosition.z);
    m[3][1] = -(m[0][1] * mPosition.x + m[1][1] * mPosition.y + m[2][1] * mPosition.z);
    m[3][2] = -(m[0][2] * mPosition.x + m[1][2] * mPosition.y + m[2][2] * mPosition.z);
    assert( isApproxEqual( getRotationMatrix4() * getTranslationMatrix4(), m ) );
    return m;
}

glm::mat4 GenericCamera::getMonoInverseViewMatrix() const
{
    glm::mat4 m(glm::transpose(mRotationMatrix));
    m[3][0] = mPosition.x;
    m[3][1] = mPosition.y;
    m[3][2] = mPosition.z;
    assert( isApproxEqual( glm::inverse(getViewMatrix()), m ) );
    return m;
}

glm::mat4 GenericCamera::getStereoProjectionMatrix( bool _leftEye, StereoMode _stereoMode ) const
{
    assert( _stereoMode != MONO && "mono is not a stereo mode!" );

    if ( getProjectionMode() == ISOMETRIC_PROJECTION )
    {
        // very unusual, prepare for headaches!
        return getMonoProjectionMatrix();
    }

    if ((_stereoMode == PARALLEL_SHIFT) || (_stereoMode == TOE_IN))
    {
        // the view matrix changes but the projection matrix stays the same
        return getMonoProjectionMatrix();
    }

    // so off-axis it is!
    assert( _stereoMode == OFF_AXIS && "unknown projection mode!" );
    //
    // In this mode the camera positions (view matrix) is shifted to the left/right still looking
    // straight ahead. The projection is also looking ahead but the projection center is
    // off (hence off-axis).
    // There is one plane in front of the cameras where the view-frusta match.
    // This should be the distance to the physical screen from the users position.


    assert(0 && "getStereoViewMatrix() is not implemented for OFF_AXIS yet!");
    return glm::mat4();

}


/// Writes all internal state to one string
/// Elements are seperated by pipes ('|'), spaces can get ignored.
std::string GenericCamera::storeStateToString() const
{
    std::string state;

    state  = "ACGL_GenericCamera | "; // "magic number", for every version the same
    state += "1 | "; // version, always an integer

    state += toString( mPosition ) + " | ";
    state += toString( mRotationMatrix ) + " | ";
    if ( mProjectionMode == ISOMETRIC_PROJECTION )   state += "ISOMETRIC_PROJECTION | ";
    if ( mProjectionMode == PERSPECTIVE_PROJECTION ) state += "PERSPECTIVE_PROJECTION | ";
    if ( mStereoMode     == MONO)                    state += "MONO | ";
    if ( mStereoMode     == PARALLEL_SHIFT)          state += "PARALLEL_SHIFT | ";
    if ( mStereoMode     == OFF_AXIS)                state += "OFF_AXIS | ";
    if ( mStereoMode     == TOE_IN)                  state += "TOE_IN | ";
    if ( mCurrentEye     == EYE_LEFT)                state += "EYE_LEFT | ";
    if ( mCurrentEye     == EYE_RIGHT)               state += "EYE_RIGHT | ";
    state += toString( mHorizontalFieldOfView ) + " | ";
    state += toString( mAspectRatio ) + " | ";
    state += toString( mInterpupillaryDistance ) + " | ";
    state += toString( mNearClippingPlane ) + " | ";
    state += toString( mFarClippingPlane ) + " | ";
    state += toString( mLookAtDistance ) + " | ";
    state += toString( mViewportSize );

    return state;
}

/// Sets all internal state from a string
void GenericCamera::setStateFromString( const std::string &_state )
{
    vector< string > token = split( _state, '|' );
    for (size_t i = 0; i < token.size(); i++) {
        token[i] = stripOfWhiteSpaces( token[i] );
    }
    if ((token.size() < 14) || (token[0] != "ACGL_GenericCamera")) {
        ACGL::Utils::error() << "Generic camera state string is invalid: " << _state << std::endl;
        return;
    }
    if ( to<int>(token[1]) != 1) {
        ACGL::Utils::error() << "Generic camera state version not supported: " << to<int>(token[1]) << std::endl;
        return;
    }

    int pos = 2;
    mPosition       = toVec3( token[pos++] );
    mRotationMatrix = toMat3( token[pos++] );
    if ( token[pos] == "ISOMETRIC_PROJECTION" )   mProjectionMode = ISOMETRIC_PROJECTION;
    if ( token[pos] == "PERSPECTIVE_PROJECTION" ) mProjectionMode = PERSPECTIVE_PROJECTION;
    pos++;
    if ( token[pos]     == "MONO")                mStereoMode = MONO;
    if ( token[pos]     == "PARALLEL_SHIFT")      mStereoMode = PARALLEL_SHIFT;
    if ( token[pos]     == "OFF_AXIS")            mStereoMode = OFF_AXIS;
    if ( token[pos]     == "TOE_IN")              mStereoMode = TOE_IN;
    pos++;
    if ( token[pos]     == "EYE_LEFT")            mCurrentEye = EYE_LEFT;
    if ( token[pos]     == "EYE_RIGHT")           mCurrentEye = EYE_RIGHT;
    pos++;

    mHorizontalFieldOfView  = to<float>( token[pos++] );
    mAspectRatio            = to<float>( token[pos++] );
    mInterpupillaryDistance = to<float>( token[pos++] );
    mNearClippingPlane      = to<float>( token[pos++] );
    mFarClippingPlane       = to<float>( token[pos++] );
    mLookAtDistance         = to<float>( token[pos++] );
    mViewportSize           = toUvec2( token[pos++] );
}

float GenericCamera::getFocalLenghtInPixel() const
{
    return ( (float) mViewportSize.x ) / ( 2.0f * tan( calcDegToRad(0.5f * mHorizontalFieldOfView) ) );
}

void GenericCamera::setFocalLengthInPixel( float _focalLengthInPixel )
{
    float hFoVrad = 2.0f * atan( ( 0.5f * mViewportSize.x ) * (1.0f / _focalLengthInPixel ) );
    setHorizontalFieldOfView( calcRadToDeg( hFoVrad ) );
}


/////////////// TESTS /////////////

#define DISABLE_TESTING
#ifndef DISABLE_TESTING
#include <gtest/gtest.h>

TEST(GenericCamera, SimpleTest)
{
    GenericCamera cam;

    cam.setNearClippingPlane(.33f);
    cam.setFarClippingPlane(3333.33f);
    cam.setLookAtMatrix(glm::vec3(0,0,-10), glm::vec3(0,0,10), glm::vec3(0,1,0));

    ASSERT_TRUE(isApproxEqual(cam.getLookAtDistance(), 20.0f));
    ASSERT_TRUE(isApproxEqual(cam.getPosition(), glm::vec3(0,0,-10)));
    ASSERT_TRUE(isApproxEqual(cam.getTarget(), glm::vec3(0,0,10)));

    ASSERT_TRUE(isApproxEqual(cam.getUpDirection(), glm::vec3(0,1,0)));
    ASSERT_TRUE(isApproxEqual(cam.getForwardDirection(), glm::vec3(0,0,1)));
    ASSERT_TRUE(isApproxEqual(cam.getRightDirection(), glm::cross(cam.getForwardDirection(), cam.getUpDirection())));

    ASSERT_TRUE(isApproxEqual(cam.getNearClippingPlane(), .33f));
    ASSERT_TRUE(isApproxEqual(cam.getFarClippingPlane(), 3333.33f));

    ASSERT_TRUE(isApproxEqual(cam.getViewMatrix(), glm::lookAt(glm::vec3(0,0,-10), glm::vec3(0,0,10), glm::vec3(0,1,0))));

    cam.moveForward(5.0f);
    ASSERT_TRUE(isApproxEqual(cam.getPosition(), glm::vec3(0,0,-5)));
    ASSERT_TRUE(isApproxEqual(cam.getTarget(), glm::vec3(0,0,15)));

    cam.moveUp(5.0f);
    ASSERT_TRUE(isApproxEqual(cam.getPosition(), glm::vec3(0,5,-5)));
    ASSERT_TRUE(isApproxEqual(cam.getTarget(), glm::vec3(0,5,15)));

    cam.moveLeft(5.0f);
    ASSERT_TRUE(isApproxEqual(cam.getPosition(), glm::vec3(5,5,-5)));
    ASSERT_TRUE(isApproxEqual(cam.getTarget(), glm::vec3(5,5,15)));
}

#endif
