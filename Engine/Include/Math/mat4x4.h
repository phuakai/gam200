/******************************************************************************/
/*!
\file		Matrix4x4.h
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Oct 19, 2022
\brief		This file contains the function declarations and structs
			used by Matrix4x4.cpp

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#pragma once

#ifndef MAT4X4_H_
#define MAT4X4_H_

#include "vec3D.h"

namespace matrix4x4
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
	typedef union mat4x4
	{
		struct
		{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};

		float m[16];
		float m2[4][4];

		mat4x4() {
			m00 = 0.f, m01 = 0.f, m02 = 0.f, m03 = 0.f;
			m10 = 0.f, m11 = 0.f, m12 = 0.f, m13 = 0.f;
			m20 = 0.f, m21 = 0.f, m22 = 0.f, m23 = 0.f;
			m30 = 0.f, m31 = 0.f, m32 = 0.f, m33 = 0.f;
		}

		mat4x4(const float* pArr);
		mat4x4(float _00, float _01, float _02, float _03,
				float _10, float _11, float _12, float _13,
				float _20, float _21, float _22, float _23,
				float _30, float _31, float _32, float _33);
		mat4x4& operator=(const mat4x4& rhs);

		// Assignment operators
		mat4x4& operator *= (const mat4x4& rhs);

	} mat4x4, Mtx44;

#ifdef _MSC_VER
	// Supress warning: nonstandard extension used : nameless struct/union
#pragma warning( default : 4201 )
#endif

	mat4x4 operator * (const mat4x4& lhs, const mat4x4& rhs);

	/**************************************************************************/
	/*!
		This operator multiplies the matrix pMtx with the vector rhs
		and returns the result as a vector
	 */
	 /**************************************************************************/
	vector3D::vec3D  operator * (const mat4x4& pMtx, const vector3D::vec3D& rhs);

	/**************************************************************************/
	/*!
		This function sets the matrix pResult to the identity matrix
	 */
	 /**************************************************************************/
	void Mtx44Identity(mat4x4& pResult);

	/**************************************************************************/
	/*!
		This function creates a translation matrix from x & y
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx44Translate(mat4x4& pResult, float x, float y, float z);

	/**************************************************************************/
	/*!
		This function creates a scaling matrix from x & y
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx44Scale(mat4x4& pResult, float x, float y, float z);

	/**************************************************************************/
	/*!
		This matrix creates a rotation matrix from "angle" whose value
		is in radian. Save the resultant matrix in pResult.
	 */
	 /**************************************************************************/
	void Mtx44RotRad(mat4x4& pResult, float angle);

	/**************************************************************************/
	/*!
		This matrix creates a rotation matrix from "angle" whose value
		is in degree. Save the resultant matrix in pResult.
	 */
	 /**************************************************************************/
	void Mtx44RotDeg(mat4x4& pResult, float angle);

	/**************************************************************************/
	/*!
		This functions calculated the transpose matrix of pMtx
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx44Transpose(mat4x4& pResult, const mat4x4& pMtx);

	///**************************************************************************/
	///*!
	//	This function calculates the inverse matrix of pMtx and saves the
	//	result in pResult. If the matrix inversion fails, pResult
	//	would be set to NULL.
	//*/
	///**************************************************************************/
	//void Mtx44Inverse(mat4x4* pResult, mat4x4 const& pMtx);
}

#endif // MAT4X4_H_