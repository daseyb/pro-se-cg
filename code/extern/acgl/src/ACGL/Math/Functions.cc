#include <ACGL/Math/Math.hh> // will include the functions.hh but also the GLM

namespace ACGL{
namespace Math{
namespace Functions{

glm::mat3 rotationMatrixX(float degree)
{
    glm::mat3 matrix;
    matrix[0][0] = 1.0f; matrix[1][0] = 0.0f;           matrix[2][0] =  0.0f;
    matrix[0][1] = 0.0f; matrix[1][1] = cosDeg(degree); matrix[2][1] = -sinDeg(degree);
    matrix[0][2] = 0.0f; matrix[1][2] = sinDeg(degree); matrix[2][2] =  cosDeg(degree);
    return matrix;
}

glm::mat3 rotationMatrixY(float degree)
{
    glm::mat3 matrix;
    matrix[0][0] =  cosDeg(degree); matrix[1][0] = 0.0f; matrix[2][0] = sinDeg(degree);
    matrix[0][1] =  0.0f;           matrix[1][1] = 1.0f; matrix[2][1] = 0.0f;
    matrix[0][2] = -sinDeg(degree); matrix[1][2] = 0.0f; matrix[2][2] = cosDeg(degree);
    return matrix;
}

glm::mat3 rotationMatrixZ(float degree)
{
    glm::mat3 matrix;
    matrix[0][0] = cosDeg(degree); matrix[1][0] = -sinDeg(degree); matrix[2][0] = 0.0f;
    matrix[0][1] = sinDeg(degree); matrix[1][1] =  cosDeg(degree); matrix[2][1] = 0.0f;
    matrix[0][2] = 0.0f;           matrix[1][2] =  0.0f;           matrix[2][2] = 1.0f;
    return matrix;
}

bool isApproxEqual(const glm::mat4 &_v1, const glm::mat4 &_v2, float _eps)
{
    glm::mat4 diff = _v1 - _v2;
    float d = 0;
    d += glm::abs(diff[0][0]);
    d += glm::abs(diff[0][1]);
    d += glm::abs(diff[0][2]);
    d += glm::abs(diff[0][3]);
    d += glm::abs(diff[1][0]);
    d += glm::abs(diff[1][1]);
    d += glm::abs(diff[1][2]);
    d += glm::abs(diff[1][3]);
    d += glm::abs(diff[2][0]);
    d += glm::abs(diff[2][1]);
    d += glm::abs(diff[2][2]);
    d += glm::abs(diff[2][3]);
    d += glm::abs(diff[3][0]);
    d += glm::abs(diff[3][1]);
    d += glm::abs(diff[3][2]);
    d += glm::abs(diff[3][3]);
    return d < _eps;
}

bool isApproxEqual(const glm::mat3 &_v1, const glm::mat3 &_v2, float _eps)
{
    glm::mat3 diff = _v1 - _v2;
    float d = 0;
    d += glm::abs(diff[0][0]);
    d += glm::abs(diff[0][1]);
    d += glm::abs(diff[0][2]);
    d += glm::abs(diff[1][0]);
    d += glm::abs(diff[1][1]);
    d += glm::abs(diff[1][2]);
    d += glm::abs(diff[2][0]);
    d += glm::abs(diff[2][1]);
    d += glm::abs(diff[2][2]);
    return d < _eps;
}

bool isOrthonormalMatrix(const glm::mat3 &_matrix)
{
    return isApproxEqual(glm::inverse(_matrix), glm::transpose(_matrix));
}

} // namespace
} // namespace
} // namespace
