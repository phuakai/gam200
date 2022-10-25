/* Start Header ************************************************************************/
/*!
\file		Matrix4x4.cpp
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Oct 19, 2022
\brief		This file contains code that manipulates 4x4 matrices

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "mat4x4.h"
#define _USE_MATH_DEFINES //for M_PI
#include "math.h"
#include <iostream>
#include <sstream>
#include <string>

namespace matrix4x4
{
	/**************************************************************************/
	/*!
		Constructor initialized with given input parameters from a pointer
	 */
	 /**************************************************************************/
	mat4x4::mat4x4(const float* pArr)
	{
		for (int i = 0; i < 16; ++i)
		{
			m[i] = pArr[i];
		}
	}

	/**************************************************************************/
	/*!
		Constructor initialized with given input parameters
	 */
	 /**************************************************************************/
	mat4x4::mat4x4(float _00, float _01, float _02, float _03, 
					float _10, float _11, float _12, float _13, 
					float _20, float _21, float _22, float _23,
					float _30, float _31, float _32, float _33)
	{
		m00 = _00; m01 = _01; m02 = _02; m03 = _03;
		m10 = _10; m11 = _11; m12 = _12; m13 = _13;
		m20 = _20; m21 = _21; m22 = _22; m23 = _23;
		m30 = _30; m31 = _31; m32 = _32; m33 = _33;
	}

	/**************************************************************************/
	/*!
		Operator overload that assigns the input matrix to the matrix from the
		member class
	 */
	 /**************************************************************************/
	mat4x4& mat4x4::operator=(const mat4x4& rhs)
	{
		for (int i = 0; i < 16; ++i)
		{
			m[i] = rhs.m[i];
		}
		return *this;
	}

	/**************************************************************************/
	/*!
		Operator overload that multiplies the input matrix with the matrix
		from the member class
	 */
	 /**************************************************************************/
	mat4x4& mat4x4::operator *= (const mat4x4& rhs)
	{
		mat4x4 tmp = *this;
		for (size_t i = 0; i < 4; ++i)
			for (size_t j = 0; j < 4; ++j)
				for (size_t k = 0; k < 4; ++k)
					m[i * 4 + j] += tmp.m[i * 4 + k] * rhs.m[k * 4 + j];
		return *this;
	}

	/**************************************************************************/
	/*!
		Operator overload that multiplies the two input matrices
	*/
	/**************************************************************************/
	mat4x4 operator * (const mat4x4& lhs, const mat4x4& rhs)
	{
		mat4x4 tmp;
		for (size_t i = 0; i < 4; ++i)
			for (size_t j = 0; j < 4; ++j)
				for (size_t k = 0; k < 4; ++k)
					tmp.m[i * 4 + j] += lhs.m[i * 4 + k] * rhs.m[k * 4 + j];
		return tmp;
	}

	/**************************************************************************/
	/*!
		This operator multiplies the matrix pMtx with the vector rhs
		and returns the result as a vector
	 */
	 /**************************************************************************/
	vector3D::vec3D  operator * (const mat4x4& pMtx, const vector3D::vec3D& rhs)
	{
		vector3D::vec3D tmp;
		tmp.x = pMtx.m00 * rhs.x + pMtx.m01 * rhs.y + pMtx.m02 * rhs.z + pMtx.m03 * 1;
		tmp.y = pMtx.m10 * rhs.x + pMtx.m11 * rhs.y + pMtx.m12 * rhs.z + pMtx.m13 * 1;
		tmp.z = pMtx.m20 * rhs.x + pMtx.m21 * rhs.y + pMtx.m22 * rhs.z + pMtx.m23 * 1;
		return tmp;
	}

	/**************************************************************************/
	/*!
		This function sets the matrix pResult to the identity matrix
	 */
	 /**************************************************************************/
	void Mtx44Identity(mat4x4& pResult)
	{
		pResult.m00 = 1; pResult.m01 = 0; pResult.m02 = 0; pResult.m03 = 0;
		pResult.m10 = 0; pResult.m11 = 1; pResult.m12 = 0; pResult.m13 = 0;
		pResult.m20 = 0; pResult.m21 = 0; pResult.m22 = 1; pResult.m23 = 0;
		pResult.m30 = 0; pResult.m31 = 0; pResult.m32 = 0; pResult.m33 = 0;
	}

	/**************************************************************************/
	/*!
		This function creates a translation matrix from x & y
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx44Translate(mat4x4& pResult, float x, float y, float z)
	{
		Mtx44Identity(pResult);
		pResult.m03 = x;
		pResult.m13 = y;
		pResult.m23 = z;
	}

	/**************************************************************************/
	/*!
		This function creates a scaling matrix from x & y
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx44Scale(mat4x4& pResult, float x, float y, float z)
	{
		Mtx44Identity(pResult);
		pResult.m00 = x;
		pResult.m11 = y;
		pResult.m22 = z;
	}

	/**************************************************************************/
	/*!
		This matrix creates a rotation matrix from "angle" whose value
		is in radian. Save the resultant matrix in pResult.
	 */
	 /**************************************************************************/
	void Mtx44RotRad(mat4x4& pResult, float angle)
	{
		Mtx44Identity(pResult);
		pResult.m00 = float(cos(angle));
		pResult.m01 = -float(sin(angle));
		pResult.m10 = float(sin(angle));
		pResult.m11 = float(cos(angle));
	}

	/**************************************************************************/
	/*!
		This matrix creates a rotation matrix from "angle" whose value
		is in degree. Save the resultant matrix in pResult.
	 */
	 /**************************************************************************/
	void Mtx44RotDeg(mat4x4& pResult, float angle)
	{
		Mtx44RotRad(pResult, angle / 180.f * static_cast<float>(M_PI));
	}

	/**************************************************************************/
	/*!
		This functions calculated the transpose matrix of pMtx
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx44Transpose(mat4x4& pResult, const mat4x4& pMtx)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				pResult.m[i + j * 4] = pMtx.m[j + i * 4];
			}
		}
	}

	///**************************************************************************/
	///*!
	//	This function calculates the inverse matrix of pMtx and saves the
	//	result in pResult. If the matrix inversion fails, pResult
	//	would be set to NULL.
	//*/
	///**************************************************************************/
	//void Mtx44Inverse(mat4x4* pResult, mat4x4 const& pMtx)
	//{
	//	float determinant = pMtx.m[0] * (pMtx.m[4] * pMtx.m[8] - pMtx.m[5] * pMtx.m[7])
	//						- pMtx.m[1] * (pMtx.m[3] * pMtx.m[8] - pMtx.m[5] * pMtx.m[6])
	//						+ pMtx.m[2] * (pMtx.m[3] * pMtx.m[7] - pMtx.m[4] * pMtx.m[6]);
	//	
	//	if (determinant == 0)
	//	{
	//		pResult = nullptr;
	//		//std::stringstream msg { "Determinant is 0!" };
	//		//std::string msg { "Determinant is 0!" };
	//		return;
	//	}
	//	else
	//	{
	//		mat4x4 tmp;
	//		Mtx44Transpose(tmp, pMtx);
	//		pResult->m[0] = (tmp.m[4] * tmp.m[8] - tmp.m[5] * tmp.m[7]) / determinant;
	//		pResult->m[1] = -(tmp.m[3] * tmp.m[8] - tmp.m[5] * tmp.m[6]) / determinant;
	//		pResult->m[2] = (tmp.m[3] * tmp.m[7] - tmp.m[6] * tmp.m[4]) / determinant;
	//		pResult->m[3] = -(tmp.m[1] * tmp.m[8] - tmp.m[2] * tmp.m[7]) / determinant;
	//		pResult->m[4] = (tmp.m[0] * tmp.m[8] - tmp.m[2] * tmp.m[6]) / determinant;
	//		pResult->m[5] = -(tmp.m[0] * tmp.m[7] - tmp.m[1] * tmp.m[6]) / determinant;
	//		pResult->m[6] = (tmp.m[1] * tmp.m[5] - tmp.m[2] * tmp.m[4]) / determinant;
	//		pResult->m[7] = -(tmp.m[0] * tmp.m[5] - tmp.m[2] * tmp.m[3]) / determinant;
	//		pResult->m[8] = (tmp.m[0] * tmp.m[4] - tmp.m[1] * tmp.m[3]) / determinant;
	//	}
	//}
}
