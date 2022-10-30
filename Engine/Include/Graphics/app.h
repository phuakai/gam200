/* !
@file    app.h
@author  a.weiren@digipen.edu, ruoyan.go@digipen.edu, p.jiankai@digipen.edu
@date    10/08/2022

This file contains the declaration of namespace App that encapsulates the
functionality required to implement an OpenGL application including
compiling, linking, and validating shader programs
setting up geometry and index buffers,
configuring VAO to present the buffered geometry and index data to
vertex shaders,
*//*__________________________________________________________________________*/

#ifndef APP_H
#define APP_H

/*                                                             
----------------------------------------------------------------------------- */
#include <input.h>
#include <list> 
#include <random>
#include <map>
#include <unordered_map>
#include <vec2D.h>
#include <mat3x3.h>
#include <graphics.h>
#include <model.h>
#include <physicsRigidBody.h>
#include <graphics.h>
#include "input.h"


struct App 
{

	static void init();
	static void update();
	static void draw();
	static void cleanup();


	//added for physics testing
	enum class collisionType
	{
		NIL = 0,					//0
		CircleDetection,			//1
		CirclePushResolution,		//2
		CircleBlockResolution,		//3
		PolygonDetection,			//4
		PolygonPushResolution,		//5
		PolygonBlockResolution,		//6
		PolygonCircleDetection,		//7
		PolygonCircleResolution		//8
	};

	static collisionType currentCollision;
	static bool movableShape;
	static std::unordered_map<collisionType, std::string> collisionInfo;

	using VPSS = std::vector<std::pair<std::string, std::string>>;

	static std::map<std::string, GLSLShader> shdrpgms; // singleton

	static bool coldebug;
	static bool velocitydirectiondebug;
	static bool graphicsmode;
};

#endif /* APP_H */
