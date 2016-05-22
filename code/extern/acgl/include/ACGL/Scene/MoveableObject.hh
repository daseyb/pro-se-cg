/***********************************************************************
 * Copyright 2014 Computer Graphics Group RWTH Aachen University.      *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

/*
 * A moveable object is an interface for something that can be moved around and has an orientation.
 * This means it has access functions for a modelmatrix containing a position and orientation as
 * well as just getting the position or the rotation.
 * It can be moved relative to its own orientation.
 *
 *
 * ModelMatrix = Rotation * Translation
 * ( it is translated in space and then rotated around that spot )
 *
 */

#pragma once
#include <ACGL/ACGL.hh>
#include <ACGL/Math/Math.hh>
#include <glm/gtx/quaternion.hpp>

namespace ACGL{
namespace Scene{

class MoveableObject
{
public:
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Constructor / Destructor / save&store
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * Default Constructor.
     */
    MoveableObject();

    /**
     * Destructor.
     */
    virtual ~MoveableObject() {}

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Set / Get the matrices:
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // Generic rotation and translation matrices.
    //

    /// Gets the orthonormal rotation matrix (mat3)
    const glm::mat3 &getRotationMatrix3() const { return mRotationMatrix; }

    /// Gets the inverse orthonormal rotation matrix (mat3)
    glm::mat3 getInverseRotationMatrix3() const { return glm::transpose(mRotationMatrix); }

    /// Gets the orthonormal rotation matrix as a mat4
    glm::mat4 getRotationMatrix4() const { return glm::mat4(mRotationMatrix); }


    /// Sets the rotation matrix (mat3)
    void setRotationMatrix( glm::mat3 _matrix );

    /// Sets the rotation matrix (mat3-part of a mat4)
    void setRotationMatrix( glm::mat4 _matrix );

    /// Sets the complete lookat (position and rotation)
    void setLookAtMatrix( const glm::vec3 &_position, const glm::vec3 &_target, const glm::vec3 &_up );

    /// Gets the translation matrix of this object (no rotational element)
    glm::mat4 getTranslationMatrix4() const;

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // Generic model matrices.
    //

    /// Gets the currently active view matrix (depends on stereo mode and current eye)
    glm::mat4 getModelMatrix() const;

    /// Gets the currently active view matrix (depends on stereo mode and current eye)
    glm::mat4 getInverseModelMatrix() const;


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Set / Get properties that move the object around (or rotate etc.)
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Set the position of the camera.
     * @param _position          New position of the object.
     */
    void setPosition( const glm::vec3 &_position ) { mPosition = _position; }
    glm::vec3 getPosition() const { return mPosition; }

    /// Moves the object by a given vector (relative to own orientation)
    void move( const glm::vec3 &_vector );
    void moveRight(   float _distance ) { move( glm::vec3(_distance,0,0) ); }
    void moveBack(    float _distance ) { move( glm::vec3(0,0,_distance) ); }
    void moveUp(      float _distance ) { move( glm::vec3(0,_distance,0) ); }
    void moveLeft(    float _distance ) { move( glm::vec3(-_distance,0,0) ); }
    void moveForward( float _distance ) { move( glm::vec3(0,0,-_distance) ); }
    void moveDown(    float _distance ) { move( glm::vec3(0,-_distance,0) ); }

    /**
     * Set the distance of the object to the object it's looking at.
     * Will change the target!
     * @param _distance     New distance of the object this is pointed at.
     */
    void setLookAtDistance( float _distance );
    /// Gets the look-at distance
    float getLookAtDistance() const { return mLookAtDistance; }

    /// Will change the look at distance!
    /// Will change the rotation!
    /// Uses stored up-vector as reference
    void setTarget( const glm::vec3 &_target ) { setTarget(_target, getUpDirection()); }

    /// Will change the look at distance!
    /// Will change the rotation!
    /// Uses given up-vector as reference
    void setTarget( const glm::vec3 &_target, const glm::vec3 &_up );

    /// Gets the reconstructed target
    glm::vec3 getTarget() const { return mPosition + getForwardDirection() * getLookAtDistance(); }

    /// Get the unit up direction
    glm::vec3 getUpDirection     () const;
    /// Get the unit right direction
    glm::vec3 getRightDirection  () const;
    /// Get the unit forward direction
    glm::vec3 getForwardDirection() const;

protected:
    /// Current position
    glm::vec3 mPosition;

    /// Current rotation
    glm::mat3 mRotationMatrix;

    /// might be used later to rotate around this position:
    float mLookAtDistance;
};

} // Scene
} // ACGL
