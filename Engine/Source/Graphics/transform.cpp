/* !
@file    transform.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file creates the transformation matrices and converts NDC to World / World to NDC
*//*__________________________________________________________________________*/
#include <transform.h>
#include <graphics.h>
#include <camera.h>
#include <iostream>
#define _USE_MATH_DEFINES //for pi
#include <math.h>

extern CameraNS::Camera2D camera2d;

matrix3x3::mat3x3 Transform::createNDCtoViewport()
{
	int winWidth = camera2d.getWidth();
	int winHeight = camera2d.getHeight();
	std::cout << "what is width hegith: " << winWidth << ", " << winHeight << std::endl;
	float halfWinWidth{ static_cast<float>(winWidth) / 2 };
	float halfWinHeight{ static_cast<float>(winHeight) / 2 };
	matrix3x3::mat3x3 ndc_to_viewport
	{
		halfWinWidth, 0.f, halfWinWidth,
		0.f, halfWinHeight, -halfWinHeight,
		0.f, 0.f, 1.f
	};
	return ndc_to_viewport;
}

matrix3x3::mat3x3 Transform::createViewporttoNDC()
{
	matrix3x3::mat3x3 ndc_to_viewport = createNDCtoViewport();

	matrix3x3::mat3x3 viewport_to_ndc;
	matrix3x3::Mtx33Inverse(&viewport_to_ndc, ndc_to_viewport);
	return viewport_to_ndc;
}

matrix3x3::mat3x3 Transform::createModeltoNDC()
{
	matrix3x3::mat3x3 tmp;
	return tmp;
}

matrix3x3::mat3x3 Transform::createNDCtoModel()
{
	matrix3x3::mat3x3 tmp;
	return tmp;
}

matrix3x3::mat3x3 Transform::createModeltoWorld()
{
	matrix3x3::mat3x3 tmp;
	return tmp;
}

matrix3x3::mat3x3 Transform::createWorldtoModel()
{
	matrix3x3::mat3x3 tmp;
	return tmp;
}

matrix3x3::mat3x3 Transform::createWorldtoNDC()
{
	int height = (int)Graphics::Input::screenheight;
	int width = (int)Graphics::Input::screenwidth;
	
	
	// at startup, the camera must be initialized to free camera ...
	//matrix3x3::mat3x3 view_xform = matrix3x3::mat3x3(1, 0, Graphics::camera2d.getCamPosition().x,
	//	0, 1, Graphics::camera2d.getCamPosition().y,
	//	0, 0, 1);
	//std::cout << "Position inside " << Graphics::camera2d.getCamPosition().x << ", " << Graphics::camera2d.getCamPosition().y << std::endl;
	//matrix3x3::mat3x3 camwin_to_ndc_xform = matrix3x3::mat3x3(float(2.f / width), 0, 0,
	//	0, float(2.f / height), 0,
	//	0, 0, 1);
	//matrix3x3::mat3x3 world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
	// 
	// 
	// 
	//std::cout << "camwin to ndc in transform " << camwin_to_ndc_xform.m[0] << ", " << camwin_to_ndc_xform.m[1] << ", " << camwin_to_ndc_xform.m[2] << std::endl
	//	<< camwin_to_ndc_xform.m[3] << ", " << camwin_to_ndc_xform.m[4] << ", " << camwin_to_ndc_xform.m[5] << std::endl
	//	<< camwin_to_ndc_xform.m[6] << ", " << camwin_to_ndc_xform.m[7] << ", " << camwin_to_ndc_xform.m[8] << std::endl;
	//std::cout << "view transform in transform " << view_xform.m[0] << ", " << view_xform.m[1] << ", " << view_xform.m[2] << std::endl
	//	<< view_xform.m[3] << ", " << view_xform.m[4] << ", " << view_xform.m[5] << std::endl
	//	<< view_xform.m[6] << ", " << view_xform.m[7] << ", " << view_xform.m[8] << std::endl;
	//std::cout << "World to ndc in transform " << world_to_ndc_xform.m[0] << ", " << world_to_ndc_xform.m[1] << ", " << world_to_ndc_xform.m[2] << std::endl
	//	<< world_to_ndc_xform.m[3] << ", " << world_to_ndc_xform.m[4] << ", " << world_to_ndc_xform.m[5] << std::endl
	//	<< world_to_ndc_xform.m[6] << ", " << world_to_ndc_xform.m[7] << ", " << world_to_ndc_xform.m[8] << std::endl;

	//matrix3x3::mat3x3 tmpcam = Graphics::camera2d.getWorldtoNDCxForm();
	//std::cout << "World to ndc in cam " << tmpcam.m[0] << ", " << tmpcam.m[1] << ", " << tmpcam.m[2] << std::endl
	//	<< tmpcam.m[3] << ", " << tmpcam.m[4] << ", " << tmpcam.m[5] << std::endl
	//	<< tmpcam.m[6] << ", " << tmpcam.m[7] << ", " << tmpcam.m[8] << std::endl;

	//std::cout << "World to ndc in transform " << world_to_ndc_xform.m[0] << ", " << world_to_ndc_xform.m[1] << ", " << world_to_ndc_xform.m[2] << std::endl
	//	<< world_to_ndc_xform.m[3] << ", " << world_to_ndc_xform.m[4] << ", " << world_to_ndc_xform.m[5] << std::endl
	//	<< world_to_ndc_xform.m[6] << ", " << world_to_ndc_xform.m[7] << ", " << world_to_ndc_xform.m[8] << std::endl;

	//return world_to_ndc_xform;

	return camera2d.getWorldtoNDCxForm();
}

matrix3x3::mat3x3 Transform::createNDCtoWorld()
{
	matrix3x3::mat3x3 world_to_ndc = createWorldtoNDC();
	//std::cout << "World to ndc " << world_to_ndc.m[0] << ", " << world_to_ndc.m[1] << ", " << world_to_ndc.m[2] << std::endl
	//	<< world_to_ndc.m[3] << ", " << world_to_ndc.m[4] << ", " << world_to_ndc.m[5] << std::endl
	//	<< world_to_ndc.m[6] << ", " << world_to_ndc.m[7] << ", " << world_to_ndc.m[8] << std::endl;
	matrix3x3::mat3x3 ndc_to_world;
	matrix3x3::Mtx33Inverse(&ndc_to_world, world_to_ndc);


	return ndc_to_world;
}

vector2D::vec2D Transform::convertNDCtoWorld(vector2D::vec2D ndc)
{
	vector2D::vec2D converted = createNDCtoWorld() * vector2D::vec2D(ndc.x, ndc.y);
	return converted;
}

vector2D::vec2D Transform::convertWorldtoNDC(vector2D::vec2D world)
{
	vector2D::vec2D converted = createWorldtoNDC() * vector2D::vec2D(world.x, world.y);
	return converted;
}

matrix3x3::mat3x3 Transform::createTranslationMat(vector2D::vec2D translation)
{
	matrix3x3::mat3x3 final;
	matrix3x3::Mtx33Identity(final);
	final.m2[0][2] = translation.x;
	final.m2[1][2] = translation.y;
	return final;
}

matrix3x3::mat3x3 Transform::createRotationMat(float rot)
{
	float rotrad = rot * (M_PI/180.f);
	matrix3x3::mat3x3 final
	(cos(rotrad), sin(rotrad), 0,
		-sin(rotrad), cos(rotrad), 0,
		0, 0, 1);
	return final;
}

matrix3x3::mat3x3 Transform::createScaleMat(vector2D::vec2D scale)
{
	matrix3x3::mat3x3 final;
	matrix3x3::Mtx33Identity(final);
	final.m2[0][0] = scale.x;
	final.m2[1][1] = scale.y;
	return final;
}