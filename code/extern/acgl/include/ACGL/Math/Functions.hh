/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_MATH_FUNCTIONS_HH
#define ACGL_MATH_FUNCTIONS_HH

/*
 * Some basic math functions.
 *
 * DON'T INCLUDE THIS DIRECTLY! Include <ACGL/Math.hh> instead!
 */
#ifndef ACGL_MATH_HH
#warning "Include Math.hh instead of Functions.hh directly"
#endif

#include <ACGL/ACGL.hh>
#include <ACGL/Math/Constants.hh>

#include <cmath>
#include <limits>
#include <algorithm>

namespace ACGL{
namespace Math{
namespace Functions{

//functions to change from degree to radians and back
inline float  calcDegToRad(float  d) {return (d * Constants::DEG_TO_RAD_FLOAT);}
inline double calcDegToRad(double d) {return (d * Constants::DEG_TO_RAD_DOUBLE);}
inline float  calcRadToDeg(float  r) {return (r * Constants::RAD_TO_DEG_FLOAT);}
inline double calcRadToDeg(double r) {return (r * Constants::RAD_TO_DEG_DOUBLE);}
 
//sine, cosine and tangens
inline float sinRad(float r) {return (::sinf(r));}
inline float cosRad(float r) {return (::cosf(r));}
inline float tanRad(float r) {return (::tanf(r));}
inline float sinDeg(float d) {return (::sinf(d * Constants::DEG_TO_RAD_FLOAT));}
inline float cosDeg(float d) {return (::cosf(d * Constants::DEG_TO_RAD_FLOAT));}
inline float tanDeg(float d) {return (::tanf(d * Constants::DEG_TO_RAD_FLOAT));}

inline double sinRad(double r) {return (::sin(r));}
inline double cosRad(double r) {return (::cos(r));}
inline double tanRad(double r) {return (::tan(r));}
inline double sinDeg(double d) {return (::sin(d * Constants::DEG_TO_RAD_DOUBLE));}
inline double cosDeg(double d) {return (::cos(d * Constants::DEG_TO_RAD_DOUBLE));}
inline double tanDeg(double d) {return (::tan(d * Constants::DEG_TO_RAD_DOUBLE));}

//and back! (asin, acos and atan)
inline float asinRad (float v)          {return (::asinf (v));}
inline float acosRad (float v)          {return (::acosf (v));}
inline float atanRad (float v)          {return (::atanf (v));}
inline float atan2Rad(float a, float b) {return (::atan2f(a, b));}
inline float asinDeg (float v)          {return (::asinf (v) * Constants::RAD_TO_DEG_FLOAT);}
inline float acosDeg (float v)          {return (::acosf (v) * Constants::RAD_TO_DEG_FLOAT);}
inline float atanDeg (float v)          {return (::atanf (v) * Constants::RAD_TO_DEG_FLOAT);}
inline float atan2Deg(float a, float b) {return (::atan2f(a, b) * Constants::RAD_TO_DEG_FLOAT);}

inline double asinRad(double v)            {return (::asin (v));}
inline double acosRad(double v)            {return (::acos (v));}
inline double atanRad(double v)            {return (::atan (v));}
inline double atan2Rad(double a, double b) {return (::atan2(a, b));}
inline double asinDeg(double v)            {return (::asin (v) * Constants::RAD_TO_DEG_DOUBLE);}
inline double acosDeg(double v)            {return (::acos (v) * Constants::RAD_TO_DEG_DOUBLE);}
inline double atanDeg(double v)            {return (::atan (v) * Constants::RAD_TO_DEG_DOUBLE);}
inline double atan2Deg(double a, double b) {return (::atan2(a, b) * Constants::RAD_TO_DEG_DOUBLE);}

//Helpers
inline int32_t round(float a)  {return (int32_t) ( (a < 0.5f)? ceil(a-0.5f) : floor(a+0.5f) );}
inline int32_t round(double a) {return (int32_t) ( (a < 0.5 )? ceil(a-0.5 ) : floor(a+0.5 ) );}

inline double  randD   (double  _from = 0.0, double  _to = 1.0) { return (_to - _from) * (double(rand()) / double(RAND_MAX)) + _from; }
inline int32_t randI32 (int32_t _from =   0, int32_t _to =   1) { return (rand() % (_to - _from + 1)) + _from; }

inline uint_t ring(int_t a, uint_t mod) {bool b = a < 0; a = abs(a) % mod; return(b ? mod - a : a);}

/**
 * @brief Returns a rotation matrix that rotates a givn degree around the X-axis.
 * @param degree is the angle in degree, not radians!
 * @return The rotation matrix.
 */
glm::mat3 rotationMatrixX(float degree);

/**
 * @brief Returns a rotation matrix that rotates a givn degree around the Y-axis.
 * @param degree is the angle in degree, not radians!
 * @return The rotation matrix.
 */
glm::mat3 rotationMatrixY(float degree);

/**
 * @brief Returns a rotation matrix that rotates a givn degree around the Z-axis.
 * @param degree is the angle in degree, not radians!
 * @return The rotation matrix.
 */
glm::mat3 rotationMatrixZ(float degree);

/**
 * @brief Returns the inverse transpose of the given matrix to use as a normal matrix.
 * @param The e.g. modelview matrix.
 * @return The inverse transposed.
 */
inline glm::mat3 normalMatrix(const glm::mat4& matrix)
{
    return glm::inverseTranspose( glm::mat3( matrix ) );
}

/**
 * @brief Test if two glm vectors are equal with respect to a given epsilon.
 */
template<typename T>
bool isApproxEqual(const T &_v1, const T &_v2, float _eps = .01)
{
    return glm::distance(_v1, _v2) < _eps;
}

/**
 * @brief isApproxEqual returns whether two glm 4x4 matrices are equal with respect to a small epsilon.
 * @param _v1: first matrix
 * @param _v2: second matrix
 * @param _eps: small numerical value
 * @return true if the sum off the componentwise difference is smaller than _eps.
 */
bool isApproxEqual(const glm::mat4 &_v1, const glm::mat4 &_v2, float _eps = .01);

/**
 * @brief isApproxEqual returns whether two glm 3x3 matrices are equal with respect to a small epsilon.
 * @param _v1: first matrix
 * @param _v2: second matrix
 * @param _eps: small numerical value
 * @return true if the sum off the componentwise difference is smaller than _eps.
 */
bool isApproxEqual(const glm::mat3 &_v1, const glm::mat3 &_v2, float _eps = .01);

/**
 * @brief Checks if a given matrix is orthonormal.
 */
bool isOrthonormalMatrix(const glm::mat3 &_matrix);

} // Functions
} // Math
} // ACGL

#endif // ACGL_MATH_FUNCTIONS_HH
