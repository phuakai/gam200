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