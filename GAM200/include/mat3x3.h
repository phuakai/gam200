/******************************************************************************/
/*!
\file		Matrix3x3.h
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Mar 21, 2022
\brief		This file contains the function declarations and structs
			used by Matrix3x3.cpp

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef CSD1130_MATRIX3X3_H_
#define CSD1130_MATRIX3X3_H_

#include "vec2D.h"

namespace matrix3x3
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
	typedef union mat3x3
	{
		struct
		{
			float m00, m01, m02;
			float m10, m11, m12;
			float m20, m21, m22;
		};

		float m[9];
		float m2[3][3];

		mat3x3() {
			m00 = 0.f, m01 = 0.f, m02 = 0.f;
			m10 = 0.f, m11 = 0.f, m12 = 0.f;
			m20 = 0.f, m21 = 0.f, m22 = 0.f;
		}

		mat3x3(const float* pArr);
		mat3x3(	float _00, float _01, float _02,
				float _10, float _11, float _12,
				float _20, float _21, float _22);
		mat3x3& operator=(const mat3x3& rhs);

		// Assignment operators
		mat3x3& operator *= (const mat3x3& rhs);

	} mat3x3, Mtx33;

#ifdef _MSC_VER
	// Supress warning: nonstandard extension used : nameless struct/union
#pragma warning( default : 4201 )
#endif

	mat3x3 operator * (const mat3x3& lhs, const mat3x3& rhs);

	/**************************************************************************/
	/*!
		This operator multiplies the matrix pMtx with the vector rhs
		and returns the result as a vector
	 */
	 /**************************************************************************/
	vector2D::vec2D  operator * (const mat3x3& pMtx, const vector2D::vec2D& rhs);

	/**************************************************************************/
	/*!
		This function sets the matrix pResult to the identity matrix
	 */
	 /**************************************************************************/
	void Mtx33Identity(mat3x3& pResult);

	/**************************************************************************/
	/*!
		This function creates a translation matrix from x & y
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx33Translate(mat3x3& pResult, float x, float y);

	/**************************************************************************/
	/*!
		This function creates a scaling matrix from x & y
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx33Scale(mat3x3& pResult, float x, float y);

	/**************************************************************************/
	/*!
		This matrix creates a rotation matrix from "angle" whose value
		is in radian. Save the resultant matrix in pResult.
	 */
	 /**************************************************************************/
	void Mtx33RotRad(mat3x3& pResult, float angle);

	/**************************************************************************/
	/*!
		This matrix creates a rotation matrix from "angle" whose value
		is in degree. Save the resultant matrix in pResult.
	 */
	 /**************************************************************************/
	void Mtx33RotDeg(mat3x3& pResult, float angle);

	/**************************************************************************/
	/*!
		This functions calculated the transpose matrix of pMtx
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx33Transpose(mat3x3& pResult, const mat3x3& pMtx);

	/**************************************************************************/
	/*!
		This function calculates the inverse matrix of pMtx and saves the
		result in pResult. If the matrix inversion fails, pResult
		would be set to NULL.
	*/
	/**************************************************************************/
	void Mtx33Inverse(mat3x3* pResult, mat3x3 const& pMtx);
}

#endif // CSD1130_MATRIX3X3_H_