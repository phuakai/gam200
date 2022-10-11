/* Start Header ************************************************************************/
/*!
\file		physicsTransform.h
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Sep 23, 2022
\brief		This file contains code that computes rotate, scale and translate for the
			physics world

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "vec2D.h"

struct physicsTransform
{
public:
	float positionX;
	float positionY;
	float sin;
	float cos;

	/********************
	* Constructors
	********************/
	/******************************************************************************/
	/*!
		This is the default constructor for the physics transform struct
	*/
	/******************************************************************************/
	physicsTransform();

	/******************************************************************************/
	/*!
		This is a constructor for the physics transform struct
	*/
	/******************************************************************************/
	physicsTransform(vector2D::vec2D pos, float angle);

	/******************************************************************************/
	/*!
		This is a constructor for the physics transform struct
	*/
	/******************************************************************************/
	physicsTransform(float x, float y, float angle);

	/******************************************************************************/
	/*!
		This function transforms the model points to world points
	*/
	/******************************************************************************/
	static vector2D::vec2D phyTransform(vector2D::vec2D v, physicsTransform transform);
	//vector2D::vec2D phyRot(float angle);

};