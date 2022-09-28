/* Start Header ************************************************************************/
/*!
\file		physicsPartition.cpp
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Sep 23, 2022
\brief		This file contains the function declarations and structs
			used by physicsPartition.cpp

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include <physicsRigidBody.h>
#include <physicsTransform.h>
#include <physicsWorld.h>
#define _USE_MATH_DEFINES //for M_PI
#include <math.h>
#include <sstream>
#include <iostream>
#include <assert.h>

//#if false

/******************************
* Constructors
******************************/
physicsRigidBody::physicsRigidBody()
{
	pos = vector2D::vec2D{ 0.f, 0.f };
	linearVel = vector2D::vec2D{ 0.f, 0.f };
	rot = 0.f;
	rotVel = 0.f;
	density = 0.f;
	mass = 0.f;
	restitution = 0.f;
	area = 0.f;
	isStatic = false;
	radius = 0.f;
	width = 0.f;
	height = 0.f;
	halfWidth = 0.f;
	halfHeight = 0.f;
	shapeType = ShapeType::box;
	plsUpdateTfm = true;
}

physicsRigidBody::physicsRigidBody(vector2D::vec2D Pos, vector2D::vec2D LinearVel, float Rot, float RotVel, float Density, float Mass,
	float Restituition, float Area, bool IsStatic, float Radius, float Width, float Height)
{
	pos = Pos;
	linearVel = LinearVel;
	rot = Rot;
	rotVel = RotVel;
	density = Density;
	mass = Mass;
	restitution = Restituition;
	area = Area;
	isStatic = IsStatic;
	radius = Radius;
	width = Width;
	height = Height;
	halfWidth = Width/2.f;
	halfHeight = Height/2.f;
	shapeType = ShapeType::box;
	plsUpdateTfm = false;
}

/******************************
* Deconstructors
******************************/
physicsRigidBody::~physicsRigidBody()
{

}

/******************************
* Creator
******************************/
bool physicsRigidBody::createCircleBody(float rad, vector2D::vec2D pos, float density, bool isStatic, float restituition, physicsRigidBody * body)
{
	float area = (float)(rad * rad * M_PI);

	// Check area
	assert(area >= world.minBodySize);
	assert(area <= world.maxBodySize);

	body->pos = pos;
	body->mass = area * 1.f * density;
	body->restitution = restituition;
	body->area = area;
	body->isStatic = isStatic;
	body->radius = rad;
	body->shapeType = ShapeType::circle;
	body->plsUpdateTfm = false;
	return true;
}


bool physicsRigidBody::createBoxBody(float width, float height, vector2D::vec2D pos, float density, bool isStatic, float restituition, physicsRigidBody * body)
{
	float area = width * height;

	// Check area
	assert(area >= world.minBodySize);
	assert(area <= world.maxBodySize);

	body->pos = pos;
	body->mass = area * 1.f * density;
	body->restitution = restituition;
	body->area = area;
	body->isStatic = isStatic;
	body->width = width;
	body->height = height;
	body->halfWidth = width / 2.f;
	body->halfHeight = height / 2.f ;
	body->shapeType = ShapeType::box;

	// storage: bottom left -> bottom right -> top right -> top left
	body->vertices.emplace_back(0.f - body->halfWidth, 0.f - body->halfHeight);
	body->vertices.emplace_back(0.f - body->halfWidth, 0.f + body->halfHeight);
	body->vertices.emplace_back(0.f + body->halfWidth, 0.f + body->halfHeight);
	body->vertices.emplace_back(0.f + body->halfWidth, 0.f - body->halfHeight);
	
	body->transformedVertices = body->vertices;
	//body->transformedVertices.resize(body->vertices.size());

	body->plsUpdateTfm = true;
		
	body->tri.emplace_back(0);
	body->tri.emplace_back(1);
	body->tri.emplace_back(2);
	body->tri.emplace_back(0);
	body->tri.emplace_back(2);
	body->tri.emplace_back(3);
	return true;
}

/******************************
* Getters
******************************/
vector2D::vec2D physicsRigidBody::getPos()
{
	return pos;
}

float physicsRigidBody::getWidth()
{
	return width;
}

float physicsRigidBody::getHeight()
{
	return height;
}

float physicsRigidBody::getRad()
{
	return radius;
}

float physicsRigidBody::getHalfWidth()
{
	return halfWidth;
}
float physicsRigidBody::getHalfHeight()
{
	return halfHeight;
}

std::vector<vector2D::vec2D> physicsRigidBody::getTfmVtx()
{
	return transformedVertices;
}

ShapeType physicsRigidBody::getShape()
{
	return shapeType;
}

bool physicsRigidBody::getPlsUpdateTfm()
{
	return plsUpdateTfm;
}

/******************************
* Setters
******************************/
void physicsRigidBody::setPos(vector2D::Point2D position)
{
	pos = position;
}

void physicsRigidBody::setRad(float rad)
{
	radius = rad;
}

/******************************
* Transformations
******************************/
void physicsRigidBody::rotate(float angle)
{
	rot = angle;
	plsUpdateTfm = true;
}

void physicsRigidBody::move(vector2D::vec2D& vel)
{
	pos += vel;
	plsUpdateTfm = true;
}

void physicsRigidBody::transformVertices()
{
	if (plsUpdateTfm == true)
	{
		physicsTransform transformMtx{pos, rot};
		for (size_t i {0} ; i < vertices.size() ; ++i)
			transformedVertices[i] = physicsTransform::phyTransform(vertices[i], transformMtx);
		plsUpdateTfm = false;
	}
}

//void physicsRigidBody::setTransformRequired(bool transform)
//{
//	plsUpdateTfm = transform;
//}



//#endif
