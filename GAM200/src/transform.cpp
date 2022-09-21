#include <transform.h>
#include <graphics.h>
#include <iostream>

matrix3x3::mat3x3 Transform::getNDCtoViewport()
{

	int winWidth = Graphics::camera2d.getWinWidth();
	int winHeight = Graphics::camera2d.getWinHeight();

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

matrix3x3::mat3x3 Transform::getViewporttoNDC()
{
	matrix3x3::mat3x3 ndc_to_viewport = getNDCtoViewport();
	
	matrix3x3::mat3x3 viewport_to_ndc;
	matrix3x3::Mtx33Inverse(&viewport_to_ndc, ndc_to_viewport);
	return viewport_to_ndc;
}

matrix3x3::mat3x3 Transform::getModeltoNDC()
{
	matrix3x3::mat3x3 tmp;
	return tmp;
}

matrix3x3::mat3x3 Transform::getNDCtoModel()
{
	matrix3x3::mat3x3 tmp;
	return tmp;
}

matrix3x3::mat3x3 Transform::getModeltoWorld()
{
	matrix3x3::mat3x3 tmp;
	return tmp;
}

matrix3x3::mat3x3 Transform::getWorldtoModel()
{
	matrix3x3::mat3x3 tmp;
	return tmp;
}

matrix3x3::mat3x3 Transform::getWorldtoNDC()
{
	return Graphics::camera2d.getWorldtoNDCxForm();
}

matrix3x3::mat3x3 Transform::getNDCtoWorld()
{
	matrix3x3::mat3x3 world_to_ndc = Graphics::camera2d.getWorldtoNDCxForm();
	//std::cout << "World to ndc " << world_to_ndc.m[0] << ", " << world_to_ndc.m[1] << ", " << world_to_ndc.m[2] << std::endl
	//	<< world_to_ndc.m[3] << ", " << world_to_ndc.m[4] << ", " << world_to_ndc.m[5] << std::endl
	//	<< world_to_ndc.m[6] << ", " << world_to_ndc.m[7] << ", " << world_to_ndc.m[8] << std::endl;
	matrix3x3::mat3x3 ndc_to_world;
	matrix3x3::Mtx33Inverse(&ndc_to_world, world_to_ndc);
	
	
	return ndc_to_world;
}

vector2D::vec2D convertNDCtoWorld(vector2D::vec2D ndc)
{
	vector2D::vec2D bottomleft = transform.getNDCtoWorld() * vector2D::vec2D(-1.0f, -1.0f);
	return bottomleft;
}