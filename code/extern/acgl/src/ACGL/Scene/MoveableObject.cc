/***********************************************************************
 * Copyright 2014 Computer Graphics Group RWTH Aachen University.      *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/Scene/MoveableObject.hh>

#include <cassert>

#include <ACGL/Math/Math.hh>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/euler_angles.hpp>

using namespace ACGL;
using namespace ACGL::Math::Functions;
using namespace ACGL::Scene;
using namespace std;

MoveableObject::MoveableObject() : mLookAtDistance(500.0)       // half a kilometer away of the screen
{
    setRotationMatrix( glm::mat3(1.0f) );
}

void MoveableObject::setRotationMatrix(glm::mat3 _matrix)
{
    mRotationMatrix = _matrix;
    assert(isOrthonormalMatrix(mRotationMatrix));
}

void MoveableObject::setRotationMatrix(glm::mat4 _matrix)
{
    mRotationMatrix = glm::mat3(_matrix);
    assert(isOrthonormalMatrix(mRotationMatrix));
}

void MoveableObject::setLookAtMatrix(const glm::vec3 &_position, const glm::vec3 &_target, const glm::vec3 &_up)
{
    setPosition(_position);
    setTarget(_target, _up);
}

glm::mat4 MoveableObject::getTranslationMatrix4() const
{
    glm::mat4 trans;
    trans[3][0] = -mPosition.x;
    trans[3][1] = -mPosition.y;
    trans[3][2] = -mPosition.z;
    return trans;
}

glm::vec3 MoveableObject::getUpDirection     () const
{
    glm::vec3 up(mRotationMatrix[0][1], mRotationMatrix[1][1], mRotationMatrix[2][1]);
    assert(glm::distance(getInverseRotationMatrix3() * glm::vec3(0.0f, 1.0f, 0.0f), up) < .01);
    return up;
}
glm::vec3 MoveableObject::getRightDirection  () const
{
    glm::vec3 right(mRotationMatrix[0][0], mRotationMatrix[1][0], mRotationMatrix[2][0]);
    assert(glm::distance(getInverseRotationMatrix3() * glm::vec3(1.0f, 0.0f, 0.0f), right) < .01);
    return right;
}
glm::vec3 MoveableObject::getForwardDirection() const
{
    glm::vec3 forward(-mRotationMatrix[0][2], -mRotationMatrix[1][2], -mRotationMatrix[2][2]);
    assert(glm::distance(getInverseRotationMatrix3() * glm::vec3(0.0f, 0.0f, -1.0f), forward) < .01);
    return forward;
}

void MoveableObject::setTarget(const glm::vec3 &_target, const glm::vec3 &_up)
{
    glm::vec3 forwardVector = _target - mPosition;
    mLookAtDistance = glm::length( forwardVector );

    forwardVector         = forwardVector / (float)mLookAtDistance; // normalize
    glm::vec3 rightVector = glm::normalize(glm::cross( forwardVector, _up ));
    glm::vec3 upVector    = glm::cross( rightVector, forwardVector );

    glm::mat3 rotMatrix;
    rotMatrix[0][0] =  rightVector.x;
    rotMatrix[0][1] =  upVector.x;
    rotMatrix[0][2] = -forwardVector.x;
    rotMatrix[1][0] =  rightVector.y;
    rotMatrix[1][1] =  upVector.y;
    rotMatrix[1][2] = -forwardVector.y;
    rotMatrix[2][0] =  rightVector.z;
    rotMatrix[2][1] =  upVector.z;
    rotMatrix[2][2] = -forwardVector.z;

    setRotationMatrix( rotMatrix );
}

void MoveableObject::setLookAtDistance(float _distance)
{
    assert (_distance > 0.0f);
    mLookAtDistance = _distance;
}

glm::mat4 MoveableObject::getModelMatrix() const
{
    glm::mat4 m(mRotationMatrix);
    m[3][0] = -(m[0][0] * mPosition.x + m[1][0] * mPosition.y + m[2][0] * mPosition.z);
    m[3][1] = -(m[0][1] * mPosition.x + m[1][1] * mPosition.y + m[2][1] * mPosition.z);
    m[3][2] = -(m[0][2] * mPosition.x + m[1][2] * mPosition.y + m[2][2] * mPosition.z);
    assert( isApproxEqual( getRotationMatrix4() * getTranslationMatrix4(), m ) );
    return m;
}


glm::mat4 MoveableObject::getInverseModelMatrix() const
{
    glm::mat4 modelMatrix = getModelMatrix();
    return glm::inverse( modelMatrix );
}


void MoveableObject::move( const glm::vec3 &_vector )
{
    glm::mat3 inverseRotation = getInverseRotationMatrix3();
    mPosition += (inverseRotation * _vector);
}




