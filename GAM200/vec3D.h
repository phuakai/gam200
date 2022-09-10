/* Start Header ************************************************************************/
/*!
\file		Vector3D.h
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

namespace vector3D
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
	typedef union vec3D
	{
		struct
		{
			float x, y, z;
		};

		float m[3];

		// Constructors
		vec3D() { x = 0.f; y = 0.f; z = 0.f; };
		vec3D(float constructX, float constructY, float constructZ);

		// Assignment operators
		vec3D& operator += (const vec3D& rhs);
		vec3D& operator -= (const vec3D& rhs);
		vec3D& operator *= (float rhs);
		vec3D& operator /= (float rhs);

		// Unary operators
		vec3D operator -() const;

	} vec3D, Vec3;


#ifdef _MSC_VER
	// Supress warning: nonstandard extension used : nameless struct/union
#pragma warning( default : 4201 )
#endif

	// Binary operators
	vec3D operator + (const vec3D& lhs, const vec3D& rhs);
	vec3D operator - (const vec3D& lhs, const vec3D& rhs);
	vec3D operator * (const vec3D& lhs, float rhs);
	vec3D operator * (float lhs, const vec3D& rhs);
	vec3D operator / (const vec3D& lhs, float rhs);

}

#endif // CSD1130_VECTOR2D_H_
