/******************************************************************************
**
** Copyright (C) 2009-2011 Kyle Lutz <kyle.r.lutz@gmail.com>
** All rights reserved.
**
** This file is a part of the chemkit project. For more information
** see <http://www.chemkit.org>.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in the
**     documentation and/or other materials provided with the distribution.
**   * Neither the name of the chemkit project nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
******************************************************************************/

#include "graphicstransform.h"

#include <Eigen/LU>

#include <chemkit/constants.h>

namespace chemkit {

// === GraphicsTransform =================================================== //
/// \class GraphicsTransform graphicstransform.h chemkit/graphicstransform.h
/// \ingroup chemkit-graphics
/// \brief The GraphicsTransform class represents a transformation
///        matrix.

// --- Construction and Destruction ---------------------------------------- //
/// Creates a new, empty graphics transform.
///
/// The tranformation returned is:
/** \f[
///   \left[
///   {
///     \begin{array}{cccc}
///       0 & 0 & 0 & 0 \\
///       0 & 0 & 0 & 0 \\
///       0 & 0 & 0 & 0 \\
///       0 & 0 & 0 & 0 \\
///     \end{array}
///   }
///   \right]
/// \f]
**/
GraphicsTransform::GraphicsTransform()
{
    m_matrix = new Eigen::Matrix<float, 4, 4>();

    // set transformation matrix to all zeros
    m_matrix->setZero();
}

/// Creates a new transform as a copy of \p transform.
GraphicsTransform::GraphicsTransform(const GraphicsTransform &transform)
{
    m_matrix = new Eigen::Matrix<float, 4, 4>(*transform.m_matrix);
}

/// Creates a new transform that contains \p matrix.
GraphicsTransform::GraphicsTransform(const Eigen::Matrix<float, 4, 4> &matrix)
{
    m_matrix = new Eigen::Matrix<float, 4, 4>(matrix);
}

/// Destroys the graphics transform.
GraphicsTransform::~GraphicsTransform()
{
    delete m_matrix;
}

// --- Properties ---------------------------------------------------------- //
/// Returns the data for the transform.
///
/// Use the following code to load a GraphicsTransform into
/// OpenGL:
/// \code
/// glLoadMatrixf(transform.data());
/// \endcode
const float* GraphicsTransform::data() const
{
    return m_matrix->data();
}

// --- Math ---------------------------------------------------------------- //
/// Inverts the transform.
void GraphicsTransform::invert()
{
    *m_matrix = m_matrix->inverse();
}

/// Returns the inverted version of the transform.
GraphicsTransform GraphicsTransform::inverted() const
{
    GraphicsTransform transform = *this;
    transform.invert();
    return transform;
}

/// Multiplies \p ray by the transform.
GraphicsRay GraphicsTransform::multiply(const GraphicsRay &ray) const
{
    Point3f origin = multiplyPoint(ray.origin());
    Point3f direction = multiplyVector(ray.direction());

    return GraphicsRay(origin, direction);
}

/// Multiplies \p point by the transform.
Point3f GraphicsTransform::multiplyPoint(const Point3f &point) const
{
    Eigen::Matrix<float, 4, 1> vector4;
    vector4[0] = point.x();
    vector4[1] = point.y();
    vector4[2] = point.z();
    vector4[3] = 1;

    vector4 = *m_matrix * vector4;

    return Point3f(vector4[0], vector4[1], vector4[2]);
}

/// Multiplies \p vector by the transform.
Vector3f GraphicsTransform::multiplyVector(const Vector3f &vector) const
{
    Eigen::Matrix<float, 4, 1> vector4;
    vector4[0] = vector.x();
    vector4[1] = vector.y();
    vector4[2] = vector.z();
    vector4[3] = 0;

    vector4 = *m_matrix * vector4;

    return Vector3f(vector4[0], vector4[1], vector4[2]);
}

/// Multiplies \p transform by the transform.
GraphicsTransform GraphicsTransform::multiply(const GraphicsTransform &transform) const
{
    return GraphicsTransform(*m_matrix * *transform.m_matrix);
}

Eigen::Matrix<float, 4, 1> GraphicsTransform::multiply(const Eigen::Matrix<float, 4, 1> &vector) const
{
    return *m_matrix * vector;
}

/// Multiplies \p point by the inverse of the transform.
Point3f GraphicsTransform::inverseMultiplyPoint(const Point3f &point) const
{
    Eigen::Matrix<float, 4, 1> vector4;
    vector4[0] = point.x();
    vector4[1] = point.y();
    vector4[2] = point.z();
    vector4[3] = 1;

    vector4 = m_matrix->inverse() * vector4;

    return Point3f(vector4[0], vector4[1], vector4[2]);
}

/// Multiplies \p vector by the inverse of the transform.
Vector3f GraphicsTransform::inverseMultiplyVector(const Vector3f &vector) const
{
    Eigen::Matrix<float, 4, 1> vector4;
    vector4[0] = vector.x();
    vector4[1] = vector.y();
    vector4[2] = vector.z();
    vector4[3] = 0;

    vector4 = m_matrix->inverse() * vector4;

    return Vector3f(vector4[0], vector4[1], vector4[2]);
}

Eigen::Matrix<float, 4, 1> GraphicsTransform::inverseMultiply(const Eigen::Matrix<float, 4, 1> &vector) const
{
    return m_matrix->inverse() * vector;
}

// --- Operators ----------------------------------------------------------- //
float GraphicsTransform::operator()(int row, int column) const
{
    return m_matrix->operator()(row, column);
}

float& GraphicsTransform::operator()(int row, int column)
{
    return m_matrix->operator()(row, column);
}

GraphicsRay GraphicsTransform::operator*(const GraphicsRay &ray) const
{
    return multiply(ray);
}

Point3f GraphicsTransform::operator*(const Point3f &point) const
{
    return multiplyPoint(point);
}

GraphicsTransform GraphicsTransform::operator*(const GraphicsTransform &transform) const
{
    return multiply(transform);
}

GraphicsTransform& GraphicsTransform::operator*=(const GraphicsTransform &transform)
{
    *m_matrix *= *transform.m_matrix;
    return *this;
}

GraphicsTransform& GraphicsTransform::operator=(const GraphicsTransform &transform)
{
    *m_matrix = *transform.m_matrix;
    return *this;
}

// --- Static Methods ------------------------------------------------------ //
/// Returns the identity transform.
///
/// The transformation returned is the following:
/** \f[
///   \left[
///   {
///     \begin{array}{cccc}
///       1 & 0 & 0 & 0 \\
///       0 & 1 & 0 & 0 \\
///       0 & 0 & 1 & 0 \\
///       0 & 0 & 0 & 1 \\
///     \end{array}
///   }
///   \right]
/// \f]
**/
GraphicsTransform GraphicsTransform::identity()
{
    GraphicsTransform transform;
    transform.m_matrix->setIdentity();
    return transform;
}

/// Returns a transformation matrix that represents the translation by
/// \p vector.
///
/// The transformation returned is the following:
/** \f[
///   \left[
///   {
///     \begin{array}{cccc}
///       1 & 0 & 0 & vector_{x} \\
///       0 & 1 & 0 & vector_{y} \\
///       0 & 0 & 1 & vector_{z} \\
///       0 & 0 & 0 & 1 \\
///     \end{array}
///   }
///   \right]
/// \f]
**/
GraphicsTransform GraphicsTransform::translation(const Vector3f &vector)
{
    GraphicsTransform transform = identity();

    transform(0, 3) = vector.x();
    transform(1, 3) = vector.y();
    transform(2, 3) = vector.z();

    return transform;
}

/// Returns a transform that represents a rotation by \p angle
/// degrees around \p axis.
GraphicsTransform GraphicsTransform::rotation(const Vector3f &axis, float angle)
{
    GraphicsTransform transform = identity();

    Vector3f v = axis.normalized();
    float c = cos(angle * chemkit::constants::DegreesToRadians);
    float s = sin(angle * chemkit::constants::DegreesToRadians);

    transform(0, 0) = v.x() * v.x() + (1 - v.x() * v.x()) * c;
    transform(0, 1) = v.x() * v.y() * (1 - c) - v.z() * s;
    transform(0, 2) = v.x() * v.z() * (1 - c) + v.y() * s;
    transform(1, 0) = v.x() * v.y() * (1 - c) + v.z() * s;
    transform(1, 1) = v.y() * v.y() + (1 - v.y() * v.y()) * c;
    transform(1, 2) = v.y() * v.z() * (1 - c) - v.x() * s;
    transform(2, 0) = v.x() * v.z() * (1 - c) - v.y() * s;
    transform(2, 1) = v.y() * v.z() * (1 - c) + v.x() * s;
    transform(2, 2) = v.z() * v.z() + (1 - v.z() * v.z()) * c;

    return transform;
}

/// Returns a perspective transform.
///
/// The transformation returned is the following:
/// \f[ f = cot(\frac{angle}{2}) \f]
/** \f[
///   \left[
///   {
///     \begin{array}{cccc}
///       \frac{f}{aspectRatio} & 0 & 0 & 0 \\
///       0 & f & 0 & 0 \\
///       0 & 0 & \frac{nearDistance+farDistance}{nearDistance-farDistance} & \frac{2 \cdot nearDistance \cdot farDistance}{nearDistance-farDistance} \\
///       0 & 0 & -1 & 0 \\
///     \end{array}
///   }
///   \right]
/// \f]
**/
GraphicsTransform GraphicsTransform::perspective(float angle, float aspectRatio, float nearDistance, float farDistance)
{
    GraphicsTransform transform;

    float f = 1.0 / tan(angle / 2.0);

    transform(0, 0) = f / aspectRatio;
    transform(1, 1) = f;
    transform(2, 2) = (nearDistance + farDistance) / (nearDistance - farDistance);
    transform(2, 3) = (2 * nearDistance * farDistance) / (nearDistance - farDistance);
    transform(3, 2) = -1;

    return transform;
}

/// Returns a frustum transform.
///
/// The transformation returned is the following:
/** \f[
///   \left[
///   {
///     \begin{array}{cccc}
///       \frac{2 \cdot nearDistance}{right-left} & 0 & \frac{right+left}{right-left} & 0 \\
///       0 & \frac{2 \cdot nearDistance}{top-bottom} & \frac{top+bottom}{top-bottom} & 0 \\
///       0 & 0 & -\frac{farDistance+nearDistance}{farDistance-nearDistance} & -\frac{2 \cdot farDistance \cdot nearDistance}{farDistance-nearDistance} \\
///       0 & 0 & -1 & 0 \\
///     \end{array}
///   }
///   \right]
/// \f]
**/
GraphicsTransform GraphicsTransform::frustum(float left, float right, float top, float bottom, float nearDistance, float farDistance)
{
    GraphicsTransform transform;

    transform(0, 0) = (2 * nearDistance) / (right - left);
    transform(1, 1) = (2 * nearDistance) / (top - bottom);
    transform(2, 0) = (right + left) / (right - left);
    transform(2, 1) = (top + bottom) / (top - bottom);
    transform(2, 2) = -(farDistance + nearDistance) / (farDistance - nearDistance);
    transform(2, 3) = -(2 * farDistance * nearDistance) / (farDistance - nearDistance);
    transform(3, 2) = -1;

    return transform;
}

/// Returns a orthographic transform.
///
/// The transformation returned is the following:
/** \f[
///   \left[
///   {
///     \begin{array}{cccc}
///       \frac{2}{right-left} & 0 & 0 & -\frac{right+left}{right-left} \\
///       0 & \frac{2}{top-bottom} & 0 & -\frac{top+bottom}{top-bottom} \\
///       0 & 0 & -\frac{2}{farDistance-nearDistance} & -\frac{farDistance+nearDistance}{farDistance-nearDistance} \\
///       0 & 0 & 0 & 1 \\
///     \end{array}
///   }
///   \right]
/// \f]
**/
GraphicsTransform GraphicsTransform::orthographic(float left, float right, float top, float bottom, float nearDistance, float farDistance)
{
    GraphicsTransform transform;

    transform(0, 0) = 2.0 / (right - left);
    transform(0, 3) = -(right + left) / (right - left);
    transform(1, 1) = 2.0 / (top - bottom);
    transform(1, 3) = -(top + bottom) / (top - bottom);
    transform(2, 2) = -2.0 / (farDistance - nearDistance);
    transform(2, 3) = -(farDistance + nearDistance) / (farDistance - nearDistance);
    transform(3, 3) = 1;

    return transform;
}

} // end chemkit namespace
