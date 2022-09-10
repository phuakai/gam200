/* Start Header ************************************************************************/
/*!
\file		Vector2D.cpp
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Mar 21, 2022
\brief		This file contains code that manipulates 2D vectors

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "vec2D.h"
#include "math.h"
#include <algorithm>
#include <iostream>

namespace vector2D
{
	/**************************************************************************/
	/*!
		Constructor initialized with x and y parameters
	 */
	 /**************************************************************************/
	vec2D::vec2D(float constructX, float constructY)
	{
		x = constructX;
		y = constructY;
	}

	/**************************************************************************/
	/*!
		Operator overload that adds the vector from the member class
		and the vector in the parameter
	 */
	 /**************************************************************************/
	vec2D& vec2D::operator += (const vec2D& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	/**************************************************************************/
	/*!
		Operator overload that subtracts the vector from the member class
		and the vector in the parameter
	 */
	 /**************************************************************************/
	vec2D& vec2D::operator -= (const vec2D& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	/**************************************************************************/
	/*!
		Operator overload that multiplies the vector from the member class
		and the vector in the parameter
	 */
	 /**************************************************************************/
	vec2D& vec2D::operator *= (float rhs)
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}

	/**************************************************************************/
	/*!
		Operator overload that divides the vector from the member class
		and the vector in the parameter
	 */
	 /**************************************************************************/
	vec2D& vec2D::operator /= (float rhs)
	{
		x /= rhs;
		y /= rhs;
		return *this;
	}


	/**************************************************************************/
	/*!
		Unary operator overload that negates the vector from the member class
	 */
	 /**************************************************************************/
	vec2D vec2D::operator -() const
	{
		return vec2D(-x, -y);
	}

	/**************************************************************************/
	/*!
		Binary operator overload that adds the two input vector
	 */
	 /**************************************************************************/
	vec2D operator + (const vec2D& lhs, const vec2D& rhs)
	{
		return vec2D(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	/**************************************************************************/
	/*!
		Binary operator overload that subtracts the two input vector
	 */
	 /**************************************************************************/
	vec2D operator - (const vec2D& lhs, const vec2D& rhs)
	{
		return vec2D(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	/**************************************************************************/
	/*!
		Binary operator overload that multiplies an input vector with a scalar
	 */
	 /**************************************************************************/
	vec2D operator * (const vec2D& lhs, float rhs)
	{
		return vec2D(lhs.x * rhs, lhs.y * rhs);
	}

	/**************************************************************************/
	/*!
		Binary operator overload that multiplies an input vector with a scalar
	 */
	 /**************************************************************************/
	vec2D operator * (float lhs, const vec2D& rhs)
	{
		return vec2D(rhs.x * lhs, rhs.y * lhs);
	}

	/**************************************************************************/
	/*!
		Binary operator overload that divides an input vector with a scalar
	 */
	 /**************************************************************************/
	vec2D operator / (const vec2D& lhs, float rhs)
	{
		return vec2D(lhs.x / rhs, lhs.y / rhs);
	}

	/**************************************************************************/
	/*!
		In this function, pResult will be the unit vector of pVec0
	 */
	 /**************************************************************************/
	void Vector2DNormalize(vec2D& pResult, const vec2D& pVec0)
	{
		pResult = pVec0 / Vector2DLength(pVec0);
	}

	/**************************************************************************/
	/*!
		This function returns the length of the vector pVec0
	 */
	 /**************************************************************************/
	float Vector2DLength(const vec2D& pVec0)
	{
		return float(sqrt(Vector2DSquareLength(pVec0)));
	}

	/**************************************************************************/
	/*!
		This function returns the square of pVec0's length. Avoid the square root
	 */
	 /**************************************************************************/
	float Vector2DSquareLength(const vec2D& pVec0)
	{
		return pVec0.x * pVec0.x + pVec0.y * pVec0.y;
	}

	/**************************************************************************/
	/*!
		In this function, pVec0 and pVec1 are considered as 2D points.
		The distance between these 2 2D points is returned
	 */
	 /**************************************************************************/
	float Vector2DDistance(const vec2D& pVec0, const vec2D& pVec1)
	{
		return float(sqrt(Vector2DSquareDistance(pVec0, pVec1)));
	}

	/**************************************************************************/
	/*!
		In this function, pVec0 and pVec1 are considered as 2D points.
		The squared distance between these 2 2D points is returned.
		Avoid the square root
	 */
	 /**************************************************************************/
	float Vector2DSquareDistance(const vec2D& pVec0, const vec2D& pVec1)
	{
		return Vector2DSquareLength(pVec1 - pVec0);
	}

	/**************************************************************************/
	/*!
		This function returns the dot product between pVec0 and pVec1
	 */
	 /**************************************************************************/
	float Vector2DDotProduct(const vec2D& pVec0, const vec2D& pVec1)
	{
		return pVec0.x * pVec1.x + pVec0.y * pVec1.y;
	}

	/**************************************************************************/
	/*!
		This function returns the cross product magnitude
		between pVec0 and pVec1
	 */
	 /**************************************************************************/
	float Vector2DCrossProductMag(const vec2D& pVec0, const vec2D& pVec1)
	{
		return pVec0.x * pVec1.y - pVec0.y * pVec1.x;
	}

	/**************************************************************************/
	/*!
		This function returns the normal to a line segment
	 */
	 /**************************************************************************/
	vec2D normalToLineSegment(const vec2D& point0, const vec2D& point1)
	{
		return vec2D(-(point1.y - point0.y), point1.x - point0.x);
	}

	/**************************************************************************/
	/*!
		This function returns the linear interpolation of two points
	 */
	 /**************************************************************************/
	vec2D lerpVec(const vec2D& point0, const vec2D& point1, const float& percentage)
	{
		return vec2D(point0 + (point1 - point0) * percentage);
	}

	/**************************************************************************/
	/*!
		This function uses the law of cosine to compute the angle between two
		vectors and return it
	*/
	/**************************************************************************/
	float angle(const vec2D& pVec0, const vec2D& pVec1)		// Applying law of cosines.
	{
		//float cosAngle{ Vector2DDotProduct(pVec0, pVec1) / (Vector2DLength(pVec0) * Vector2DLength(pVec1)) };
		return (acosf(Vector2DDotProduct(pVec0, pVec1) / (Vector2DLength(pVec0) * Vector2DLength(pVec1))));
	}

	/**************************************************************************/
	/*!
		This function finds the maximum value of two points
	 */
	 /**************************************************************************/
	vec2D maxPointBoundingBox(const vec2D& pVec0, const vec2D& pVec1)
	{
		return vec2D(std::max(pVec0.x, pVec1.x), std::max(pVec0.y, pVec1.y));
	}

	/**************************************************************************/
	/*!
		This function finds the minimum value of two points
	 */
	 /**************************************************************************/
	vec2D minPointBoundingBox(const vec2D& pVec0, const vec2D& pVec1)
	{
		return vec2D(std::min(pVec0.x, pVec1.x), std::min(pVec0.y, pVec1.y));
	}

	/**************************************************************************/
	/*!
		This function finds the maximum value of a polygon
	 */
	 /**************************************************************************/
	vec2D maxPointsOfPolygonBoundingBox(std::vector<vec2D> polygonPoints)
	{
		vec2D maxPt{ -FLT_MAX , -FLT_MAX };

		for (size_t i{ 0 }; i < polygonPoints.size(); ++i)
		{
			maxPt = vec2D(std::max(maxPt.x, polygonPoints[i].x), std::max(maxPt.y, polygonPoints[i].y));
		}

		return maxPt;
	}

	/**************************************************************************/
	/*!
		This function finds the minimum value of a polygon
	 */
	 /**************************************************************************/
	vec2D minPointsOfPolygonBoundingBox(std::vector<vec2D> polygonPoints)
	{
		vec2D minPt{ FLT_MAX , FLT_MAX };

		for (size_t i{ 0 }; i < polygonPoints.size(); ++i)
		{
			minPt = vec2D(std::min(minPt.x, polygonPoints[i].x), std::min(minPt.y, polygonPoints[i].y));
		}

		return minPt;
	}

}