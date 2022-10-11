/* Start Header ************************************************************************/
/*!
\file		Vector3D.cpp
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

#include "vec3D.h"

namespace vector3D
{
	/**************************************************************************/
	/*!
		Constructor initialized with x and y parameters
	 */
	 /**************************************************************************/
	vec3D::vec3D(float constructX, float constructY, float constructZ)
	{
		x = constructX;
		y = constructY;
		z = constructZ;
	}

	/**************************************************************************/
	/*!
		Operator overload that adds the vector from the member class
		and the vector in the parameter
	 */
	 /**************************************************************************/
	vec3D& vec3D::operator += (const vec3D& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	/**************************************************************************/
	/*!
		Operator overload that subtracts the vector from the member class
		and the vector in the parameter
	 */
	 /**************************************************************************/
	vec3D& vec3D::operator -= (const vec3D& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	/**************************************************************************/
	/*!
		Operator overload that multiplies the vector from the member class
		and the vector in the parameter
	 */
	 /**************************************************************************/
	vec3D& vec3D::operator *= (float rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}

	/**************************************************************************/
	/*!
		Operator overload that divides the vector from the member class
		and the vector in the parameter
	 */
	 /**************************************************************************/
	vec3D& vec3D::operator /= (float rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}


	/**************************************************************************/
	/*!
		Unary operator overload that negates the vector from the member class
	 */
	 /**************************************************************************/
	vec3D vec3D::operator -() const
	{
		return vec3D(-x, -y, -z);
	}

	/**************************************************************************/
	/*!
		Binary operator overload that adds the two input vector
	 */
	 /**************************************************************************/
	vec3D operator + (const vec3D& lhs, const vec3D& rhs)
	{
		return vec3D(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	/**************************************************************************/
	/*!
		Binary operator overload that subtracts the two input vector
	 */
	 /**************************************************************************/
	vec3D operator - (const vec3D& lhs, const vec3D& rhs)
	{
		return vec3D(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	/**************************************************************************/
	/*!
		Binary operator overload that multiplies an input vector with a scalar
	 */
	 /**************************************************************************/
	vec3D operator * (const vec3D& lhs, float rhs)
	{
		return vec3D(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
	}

	/**************************************************************************/
	/*!
		Binary operator overload that multiplies an input vector with a scalar
	 */
	 /**************************************************************************/
	vec3D operator * (float lhs, const vec3D& rhs)
	{
		return vec3D(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs);
	}

	/**************************************************************************/
	/*!
		Binary operator overload that divides an input vector with a scalar
	 */
	 /**************************************************************************/
	vec3D operator / (const vec3D& lhs, float rhs)
	{
		return vec3D(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
	}


}