/* !
@file    transform.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file creates the transformation matrices and converts NDC to World / World to NDC
*//*__________________________________________________________________________*/
#pragma once

#include <vec2D.h>
#include <mat3x3.h>
#include <camera.h>
#include <input.h>

class Transform // Input class
{
public:

	matrix3x3::mat3x3 createNDCtoViewport();

	matrix3x3::mat3x3 createViewporttoNDC();
					  
	matrix3x3::mat3x3 createModeltoNDC(); // temp
					  
	matrix3x3::mat3x3 createNDCtoModel(); // temp
					  
	matrix3x3::mat3x3 createModeltoWorld(); // temp
					  
	matrix3x3::mat3x3 createWorldtoModel(); // temp
					  
	matrix3x3::mat3x3 createWorldtoNDC();// vector2D::vec2D position);
					  
	matrix3x3::mat3x3 createNDCtoWorld();// vector2D::vec2D position);

	vector2D::vec2D convertNDCtoWorld(vector2D::vec2D ndc);

	vector2D::vec2D convertWorldtoNDC(vector2D::vec2D world);

	static matrix3x3::mat3x3 createTranslationMat(vector2D::vec2D translation);

	static matrix3x3::mat3x3 createRotationMat(float rot);

	static matrix3x3::mat3x3 createScaleMat(vector2D::vec2D scale);
private:
	//// Model to NDC
	//matrix3x3::mat3x3 model_to_ndc_xform{};
	//// NDC to Model
	//matrix3x3::mat3x3 ndc_to_model_xform{};
	//// Model to World
	//matrix3x3::mat3x3 model_to_world_xform{};
	//// World to Model
	//matrix3x3::mat3x3 world_to_model_xform{};
	//// World to NDC
	//matrix3x3::mat3x3 world_to_ndc_xform{};
	//// NDC to World
	//matrix3x3::mat3x3 ndc_to_world_xform{};

	//// Temp
	//matrix3x3::mat3x3 viewport_to_ndc_xform{};
	//// Temp
	//matrix3x3::mat3x3 ndc_to_viewport_xform{};
};

//inline matrix3x3::mat3x3 Transform::createNDCtoViewport()
//{
//
//	int winWidth = Graphics::camera2d.getWinWidth();
//	int winHeight = Graphics::camera2d.getWinHeight();
//
//	float halfWinWidth{ static_cast<float>(winWidth) / 2 };
//	float halfWinHeight{ static_cast<float>(winHeight) / 2 };
//	matrix3x3::mat3x3 ndc_to_viewport
//	{
//		halfWinWidth, 0.f, halfWinWidth,
//		0.f, halfWinHeight, -halfWinHeight,
//		0.f, 0.f, 1.f
//	};
//	return ndc_to_viewport;
//}
//
//inline matrix3x3::mat3x3 Transform::createViewporttoNDC()
//{
//	matrix3x3::mat3x3 ndc_to_viewport = createNDCtoViewport();
//
//	matrix3x3::mat3x3 viewport_to_ndc;
//	matrix3x3::Mtx33Inverse(&viewport_to_ndc, ndc_to_viewport);
//	return viewport_to_ndc;
//}
//
//inline matrix3x3::mat3x3 Transform::createModeltoNDC()
//{
//	matrix3x3::mat3x3 tmp;
//	return tmp;
//}
//
//inline matrix3x3::mat3x3 Transform::createNDCtoModel()
//{
//	matrix3x3::mat3x3 tmp;
//	return tmp;
//}
//
//inline matrix3x3::mat3x3 Transform::createModeltoWorld()
//{
//	matrix3x3::mat3x3 tmp;
//	return tmp;
//}
//
//inline matrix3x3::mat3x3 Transform::createWorldtoModel()
//{
//	matrix3x3::mat3x3 tmp;
//	return tmp;
//}
//
//inline matrix3x3::mat3x3 Transform::createWorldtoNDC(vector2D::vec2D position)
//{
//	int height = Graphics::Input::height;
//	int width = Graphics::Input::width;
//
//
//	// at startup, the camera must be initialized to free camera ...
//	matrix3x3::mat3x3 view_xform = matrix3x3::mat3x3(1, 0, position.x,
//		0, 1, position.y,
//		0, 0, 1);
//
//	matrix3x3::mat3x3 camwin_to_ndc_xform = matrix3x3::mat3x3(float(2 / width), 0, 0,
//		0, float(2 / height), 0,
//		0, 0, 1);
//	matrix3x3::mat3x3 world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
//
//	return world_to_ndc_xform;
//}
//
//inline matrix3x3::mat3x3 Transform::createNDCtoWorld(vector2D::vec2D position)
//{
//	matrix3x3::mat3x3 world_to_ndc = createWorldtoNDC(position);
//	//std::cout << "World to ndc " << world_to_ndc.m[0] << ", " << world_to_ndc.m[1] << ", " << world_to_ndc.m[2] << std::endl
//	//	<< world_to_ndc.m[3] << ", " << world_to_ndc.m[4] << ", " << world_to_ndc.m[5] << std::endl
//	//	<< world_to_ndc.m[6] << ", " << world_to_ndc.m[7] << ", " << world_to_ndc.m[8] << std::endl;
//	matrix3x3::mat3x3 ndc_to_world;
//	matrix3x3::Mtx33Inverse(&ndc_to_world, world_to_ndc);
//
//
//	return ndc_to_world;
//}
//
//inline vector2D::vec2D Transform::convertNDCtoWorld(vector2D::vec2D ndc)
//{
//	vector2D::vec2D converted = createNDCtoWorld(ndc) * vector2D::vec2D(ndc.x, ndc.y);
//	return converted;
//}

static Transform transform;