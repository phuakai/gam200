/* Start Header ************************************************************************/
/*!
\file		physicsRigidBody.h
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Sep 23, 2022
\brief		This file contains the function declarations and structs
			used by physicsRigidBody.cpp

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include <vec2D.h>
#include <string>
#include <vector>

//#if false
enum class ShapeType
{
	circle = 0, 
	box
};

class physicsRigidBody
{
private:
	vector2D::vec2D pos;
	vector2D::vec2D linearVel;
	float rot;
	float rotVel;

	float density;
	float mass;
	float restitution;				//how bouncy
	float area;
	
	bool isStatic;					//will not be affected by physics

	float radius;					//for circle
	float width;					//for box
	float height;					//for box
	float halfWidth;				//for box
	float halfHeight;				//for box

	ShapeType shapeType;
	
	std::vector <vector2D::vec2D> vertices;
	std::vector <int> tri;
	std::vector <vector2D::vec2D> transformedVertices;
	bool plsUpdateTfm;				//change to true whenever there is rot/scale/trans

public:
	/******************************
	* Constructors
	******************************/
	/******************************************************************************/
	/*!
		This is the default constructor for the rigid body struct
	*/
	/******************************************************************************/
	physicsRigidBody();

	/******************************************************************************/
	/*!
		This is a constructor for the rigid body struct
	*/
	/******************************************************************************/
	physicsRigidBody(vector2D::vec2D Pos, vector2D::vec2D LinearVel, float Rot, float RotVel, float Density, float Mass,
	float Restituition, float Area, bool IsStatic, float Radius, float Width, float Height);
	
	/******************************
	* Deconstructors
	******************************/
	/******************************************************************************/
	/*!
		This is the default deconstructor for the rigid body struct
	*/
	/******************************************************************************/
	~physicsRigidBody();

	/******************************
	* Creator
	******************************/
	/******************************************************************************/
	/*!
		This is a creater function for a circle body
	*/
	/******************************************************************************/
	static bool createCircleBody(float rad, vector2D::vec2D pos, float density, bool isStatic, float restituition, physicsRigidBody *body);
	/******************************************************************************/
	/*!
		This is a creater function for a box body
	*/
	/******************************************************************************/
	static bool createBoxBody(float width, float height, vector2D::vec2D pos, float density, bool isStatic, float restituition, physicsRigidBody *body);

	/******************************
	* Getters
	******************************/

	/******************************************************************************/
	/*!
		This is getter for the rigid body's center position
	*/
	/******************************************************************************/
	vector2D::vec2D getPos();

	/******************************************************************************/
	/*!
		This is getter for the rigid body's radius, if applicable
	*/
	/******************************************************************************/
	float getRad();

	/******************************************************************************/
	/*!
		This is getter for the rigid body's width, if applicable
	*/
	/******************************************************************************/
	float getWidth();

	/******************************************************************************/
	/*!
		This is getter for the rigid body's height, if applicable
	*/
	/******************************************************************************/
	float getHeight();

	/******************************************************************************/
	/*!
		This is getter for the rigid body's half width, if applicable
	*/
	/******************************************************************************/
	float getHalfWidth();

	/******************************************************************************/
	/*!
		This is getter for the rigid body's half height, if applicable
	*/
	/******************************************************************************/
	float getHalfHeight();

	/******************************************************************************/
	/*!
		This is getter for the rigid body's shape type
	*/
	/******************************************************************************/
	ShapeType getShape();

	/******************************************************************************/
	/*!
		This is getter for the rigid body's transformed vertices
	*/
	/******************************************************************************/
	std::vector<vector2D::vec2D> getTfmVtx();

	/******************************************************************************/
	/*!
		This is getter for the rigid body's transform status
	*/
	/******************************************************************************/
	bool getPlsUpdateTfm();

	/******************************
	* Setters
	******************************/
	/******************************************************************************/
	/*!
		This is setter for the rigid body's center position
	*/
	/******************************************************************************/
	void setPos(vector2D::Point2D position);

	/******************************************************************************/
	/*!
		This is setter for the rigid body's radius
	*/
	/******************************************************************************/
	void setRad(float rad);

	/******************************
	* Transformations
	******************************/
	/******************************************************************************/
	/*!
		This function transforms the vertices of a polygon
	*/
	/******************************************************************************/
	void transformVertices();									// Transform

	/******************************************************************************/
	/*!
		This function applies velocity to the center position of the rigid body
	*/
	/******************************************************************************/
	void move(vector2D::vec2D& vel);							// Translate

	/******************************************************************************/
	/*!
		This function rotates the rigid body
	*/
	/******************************************************************************/
	void rotate(float angle);									// Rotate
	//void setTransformRequired(bool transform);				

};
//#endif

