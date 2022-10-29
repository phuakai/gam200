/******************************************************************************/
/*!
\file		collisiondebug.cpp
\author		Ang Wei Ren, a.weiren
\par		a.weiren\@digipen.edu
\date		Sep 23, 2022
\brief		This file contains the function definitions used to create and initialize the collision debug objects

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/

#include <app.h>
#include <collisiondebug.h>
#include <transform.h>
#include <mat3x3.h>
#include <vec2D.h>
#include <iostream>
#include <sstream>

static int tmpcollisionobjcounter{ 0 };

#if false
void Graphics::collisionDebugInit(App::GLObject& object)
{
	std::vector<vector2D::vec2D> tmp = object.body.getTfmVtx();
	matrix3x3::mat3x3 worldtondc = transform.createWorldtoNDC();
	float width{ 0 }, height{ 0 };
	for (int i = 0; i < object.body.getTfmVtx().size(); i++)
	{
		vector2D::Vec2 newndc = worldtondc * tmp[i];
		width = object.body.getWidth();
		height = object.body.getHeight();

	}
	std::string tmpobjname = "CollisionObj";
	tmpcollisionobjcounter++;
	std::stringstream tmpstream;
	tmpstream << tmpobjname << tmpcollisionobjcounter;
	std::string finalobjname = tmpstream.str();
	//std::cout << "Final obj name " << finalobjname << std::endl;
	//App::GLObject::gimmeObject("square", finalobjname, vector2D::vec2D(width, height), vector2D::vec2D(object.modelCenterPos.x, object.modelCenterPos.y), vector3D::vec3D(0.0f, 1.0f, 0.0f), tmpcollisionobjcounter, false);

}
#endif
#if false
void Graphics::collisionDebugCreate(App::GLObject& object)
{
	//std::cout << "Vertice size " << object.body.getTfmVtx().size() << std::endl;
	std::vector<vector2D::vec2D> tmp = object.body.getTfmVtx();

	matrix3x3::mat3x3 worldtondc = transform.createWorldtoNDC();
	float width{0}, height{0};

	for (int i = 0; i < object.body.getTfmVtx().size(); i++)
	{
		
		//std::cout << "Vertices " << tmp[i].x << ", " << tmp[i].y << std::endl;
		vector2D::Vec2 newndc = worldtondc * tmp[i];
		width = object.body.getWidth();
		height = object.body.getHeight();

	}
	//std::cout << "Width & Height " << width << ", " << height << std::endl;
	
	std::string tmpobjname = "CollisionObj";
	tmpcollisionobjcounter++;
	std::stringstream tmpstream;
	tmpstream << tmpobjname << tmpcollisionobjcounter;
	std::string finalobjname = tmpstream.str();
	//std::cout << "Final obj name " << finalobjname << std::endl;
	//App::GLObject::gimmeObject("square", finalobjname, vector2D::vec2D(width, height), vector2D::vec2D(object.modelCenterPos.x, object.modelCenterPos.y), vector3D::vec3D(1.0f, 1.0f, 1.0f), tmpcollisionobjcounter, false);
	
}
#endif