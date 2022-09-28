/* Start Header ************************************************************************/
/*!
\file		Vector2D.h
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Mar 21, 2022
\brief		This file contains the function declarations and structs
			used by Vector2D.cpp

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef CSD1130_VECTOR2D_H_
#define CSD1130_VECTOR2D_H_

#include "vector"


namespace vector2D
{
#ifdef _MSC_VER
	// Supress warning: nonstandard extension used : nameless struct/union
#pragma warning( disable : 4201 )
#endif

/**************************************************************************/
/*!
	Union that consist of a struct, its constructors and operator function
	overloads
 */
 /**************************************************************************/
	typedef union vec2D
	{
		struct
		{
			float x, y;
		};

		float m[2];

		// Constructors
		vec2D() { x = 0.f; y = 0.f; };
		vec2D(float constructX, float constructY);

		// Assignment operators
		vec2D& operator += (const vec2D& rhs);
		vec2D& operator -= (const vec2D& rhs);
		vec2D& operator *= (float rhs);
		vec2D& operator /= (float rhs);

		// Unary operators
		vec2D operator -() const;

	} vec2D, Vec2, Point2D, Pt2;


#ifdef _MSC_VER
	// Supress warning: nonstandard extension used : nameless struct/union
#pragma warning( default : 4201 )
#endif

	// Binary operators
	vec2D operator + (const vec2D& lhs, const vec2D& rhs);
	vec2D operator - (const vec2D& lhs, const vec2D& rhs);
	vec2D operator * (const vec2D& lhs, float rhs);
	vec2D operator * (float lhs, const vec2D& rhs);
	vec2D operator / (const vec2D& lhs, float rhs);

	/**************************************************************************/
	/*!
		In this function, pResult will be the unit vector of pVec0
	 */
	 /**************************************************************************/
	void Vector2DNormalize(vec2D& pResult, const vec2D& pVec0);

	/**************************************************************************/
	/*!
		This function returns the length of the vector pVec0
	 */
	 /**************************************************************************/
	float Vector2DLength(const vec2D& pVec0);

	/**************************************************************************/
	/*!
		This function returns the square of pVec0's length. Avoid the square root
	 */
	 /**************************************************************************/
	float Vector2DSquareLength(const vec2D& pVec0);

	/**************************************************************************/
	/*!
		In this function, pVec0 and pVec1 are considered as 2D points.
		The distance between these 2 2D points is returned
	 */
	 /**************************************************************************/
	float Vector2DDistance(const vec2D& pVec0, const vec2D& pVec1);

	/**************************************************************************/
	/*!
		In this function, pVec0 and pVec1 are considered as 2D points.
		The squared distance between these 2 2D points is returned.
		Avoid the square root
	 */
	 /**************************************************************************/
	float Vector2DSquareDistance(const vec2D& pVec0, const vec2D& pVec1);

	/**************************************************************************/
	/*!
		This function returns the dot product between pVec0 and pVec1
	 */
	 /**************************************************************************/
	float Vector2DDotProduct(const vec2D& pVec0, const vec2D& pVec1);

	/**************************************************************************/
	/*!
		This function returns the cross product magnitude
		between pVec0 and pVec1
	 */
	 /**************************************************************************/
	float Vector2DCrossProductMag(const vec2D& pVec0, const vec2D& pVec1);

	/**************************************************************************/
	/*!
		This function returns the normal to a line segment
	 */
	 /**************************************************************************/
	vec2D normalToLineSegment(const vec2D& point0, const vec2D& point1);

	/**************************************************************************/
	/*!
		This function returns the linear interpolation of two points
	 */
	 /**************************************************************************/
	vec2D lerpVec(const vec2D& point0, const vec2D& point1, const float& percentage);

	/**************************************************************************/
	/*!
		This function uses the law of cosine to compute the angle between two
		vectors and return it
	 */
	 /**************************************************************************/
	float angle(const vec2D& pVec0, const vec2D& pVec1);

	/**************************************************************************/
	/*!
		This function finds the maximum value of two points
	 */
	 /**************************************************************************/
	vec2D maxPointBoundingBox(const vec2D& pVec0, const vec2D& pVec1);

	/**************************************************************************/
	/*!
		This function finds the minimum value of two points
	 */
	 /**************************************************************************/
	vec2D minPointBoundingBox(const vec2D& v1, const vec2D& v2);

	/**************************************************************************/
	/*!
		This function finds the maximum value of a polygon
	 */
	 /**************************************************************************/
	vec2D maxPointsOfPolygonBoundingBox(std::vector<vec2D> polygonPoints);

	/**************************************************************************/
	/*!
		This function finds the minimum value of a polygon
	 */
	 /**************************************************************************/
	vec2D minPointsOfPolygonBoundingBox(std::vector<vec2D> polygonPoints);

}

#endif // CSD1130_VECTOR2D_H_
