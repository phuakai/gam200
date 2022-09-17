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
	physicsTransform();
	physicsTransform(vector2D::vec2D pos, float angle);
	physicsTransform(float x, float y, float angle);

	static vector2D::vec2D phyTransform(vector2D::vec2D v, physicsTransform transform);
	//vector2D::vec2D phyRot(float angle);

};