/* Start Header ************************************************************************/
/*!
\file		Matrix3x3.cpp
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Sep 23, 2022
\brief		This file contains code that manipulates 3x3 matrices

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "mat3x3.h"
#include "math.h"
#include <iostream>
#include <sstream>
#include <string>

namespace matrix3x3
{
	/**************************************************************************/
	/*!
		Constructor initialized with given input parameters from a pointer
	 */
	 /**************************************************************************/
	mat3x3::mat3x3(const float* pArr)
	{
		for (int i = 0; i < 9; ++i)
		{
			m[i] = pArr[i];
		}
	}

	/**************************************************************************/
	/*!
		Constructor initialized with given input parameters
	 */
	 /**************************************************************************/
	mat3x3::mat3x3(float _00, float _01, float _02, float _10, float _11, float _12, float _20, float _21, float _22)
	{
		m00 = _00;
		m01 = _01;
		m02 = _02;
		m10 = _10;
		m11 = _11;
		m12 = _12;
		m20 = _20;
		m21 = _21;
		m22 = _22;
	}

	/**************************************************************************/
	/*!
		Operator overload that assigns the input matrix to the matrix from the
		member class
	 */
	 /**************************************************************************/
	mat3x3& mat3x3::operator=(const mat3x3& rhs)
	{
		for (int i = 0; i < 9; ++i)
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
	mat3x3& mat3x3::operator *= (const mat3x3& rhs)
	{
		mat3x3 tmp = *this;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				m[i * 3 + j] = tmp.m[i * 3] * rhs.m[j] + tmp.m[i * 3 + 1] * rhs.m[1 * 3 + j] + tmp.m[i * 3 + 2] * rhs.m[2 * 3 + j];
			}
		}
		return *this;
	}

	/**************************************************************************/
	/*!
		Operator overload that multiplies the two input matrices
	*/
	/**************************************************************************/
	mat3x3 operator * (const mat3x3& lhs, const mat3x3& rhs)
	{
		mat3x3 tmp;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				tmp.m[i * 3 + j] = lhs.m[i * 3] * rhs.m[j] + lhs.m[i * 3 + 1] * rhs.m[1 * 3 + j] + lhs.m[i * 3 + 2] * rhs.m[2 * 3 + j];
			}
		}
		return tmp;
	}

	/**************************************************************************/
	/*!
		This operator multiplies the matrix pMtx with the vector rhs
		and returns the result as a vector
	 */
	 /**************************************************************************/
	vector2D::vec2D  operator * (const mat3x3 & pMtx, const vector2D::vec2D & rhs)
	{
		vector2D::vec2D tmp;
		tmp.x = pMtx.m00 * rhs.x + pMtx.m01 * rhs.y + pMtx.m02 * 1;
		tmp.y = pMtx.m10 * rhs.x + pMtx.m11 * rhs.y + pMtx.m12 * 1;
		return tmp;
	}

	/**************************************************************************/
	/*!
		This function sets the matrix pResult to the identity matrix
	 */
	 /**************************************************************************/
	void Mtx33Identity(mat3x3& pResult)
	{
		pResult.m00 = 1;
		pResult.m01 = 0;
		pResult.m02 = 0;
		pResult.m10 = 0;
		pResult.m11 = 1;
		pResult.m12 = 0;
		pResult.m20 = 0;
		pResult.m21 = 0;
		pResult.m22 = 1;
	}

	/**************************************************************************/
	/*!
		This function creates a translation matrix from x & y
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx33Translate(mat3x3& pResult, float x, float y)
	{
		Mtx33Identity(pResult);
		pResult.m02 = x;
		pResult.m12 = y;
	}

	/**************************************************************************/
	/*!
		This function creates a scaling matrix from x & y
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx33Scale(mat3x3& pResult, float x, float y)
	{
		Mtx33Identity(pResult);
		pResult.m00 = x;
		pResult.m11 = y;
	}

	/**************************************************************************/
	/*!
		This matrix creates a rotation matrix from "angle" whose value
		is in radian. Save the resultant matrix in pResult.
	 */
	 /**************************************************************************/
	void Mtx33RotRad(mat3x3& pResult, float angle)
	{
		Mtx33Identity(pResult);
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
	void Mtx33RotDeg(mat3x3& pResult, float angle)
	{
		Mtx33RotRad(pResult, angle / 180.f * 3.1415926f);
	}

	/**************************************************************************/
	/*!
		This functions calculated the transpose matrix of pMtx
		and saves it in pResult
	 */
	 /**************************************************************************/
	void Mtx33Transpose(mat3x3& pResult, const mat3x3& pMtx)
	{
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				pResult.m[i + j * 3] = pMtx.m[j + i * 3];
			}
		}
	}

	/**************************************************************************/
	/*!
		This function calculates the inverse matrix of pMtx and saves the
		result in pResult. If the matrix inversion fails, pResult
		would be set to NULL.
	*/
	/**************************************************************************/
	void Mtx33Inverse(mat3x3* pResult, mat3x3 const& pMtx)
	{
		float determinant = pMtx.m[0] * (pMtx.m[4] * pMtx.m[8] - pMtx.m[5] * pMtx.m[7])
							- pMtx.m[1] * (pMtx.m[3] * pMtx.m[8] - pMtx.m[5] * pMtx.m[6])
							+ pMtx.m[2] * (pMtx.m[3] * pMtx.m[7] - pMtx.m[4] * pMtx.m[6]);
		if (determinant == 0)
		{
			pResult = nullptr;
			//std::stringstream msg { "Determinant is 0!" };
			//std::string msg { "Determinant is 0!" };
			return;
		}
		else
		{
			mat3x3 tmp;
			Mtx33Transpose(tmp, pMtx);
			pResult->m[0] = (tmp.m[4] * tmp.m[8] - tmp.m[5] * tmp.m[7]) / determinant;
			pResult->m[1] = -(tmp.m[3] * tmp.m[8] - tmp.m[5] * tmp.m[6]) / determinant;
			pResult->m[2] = (tmp.m[3] * tmp.m[7] - tmp.m[6] * tmp.m[4]) / determinant;
			pResult->m[3] = -(tmp.m[1] * tmp.m[8] - tmp.m[2] * tmp.m[7]) / determinant;
			pResult->m[4] = (tmp.m[0] * tmp.m[8] - tmp.m[2] * tmp.m[6]) / determinant;
			pResult->m[5] = -(tmp.m[0] * tmp.m[7] - tmp.m[1] * tmp.m[6]) / determinant;
			pResult->m[6] = (tmp.m[1] * tmp.m[5] - tmp.m[2] * tmp.m[4]) / determinant;
			pResult->m[7] = -(tmp.m[0] * tmp.m[5] - tmp.m[2] * tmp.m[3]) / determinant;
			pResult->m[8] = (tmp.m[0] * tmp.m[4] - tmp.m[1] * tmp.m[3]) / determinant;
		}
	}
}
