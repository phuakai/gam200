/* !
@file    camera.cpp
@author  a.weiren@digipen.edu
@date    23/8/2022

This file controls the camera of the game
*//*__________________________________________________________________________*/

#include <camera.h>
#define _USE_MATH_DEFINES //for pi
#include <math.h>
#include <iostream>

namespace CameraNS
{
	Camera2D::Camera2D()
	{
		//position = vector2D::Vec2(21, 21);
		//orientation = vector2D::Vec2(20, 20);
	}
	/*  _________________________________________________________________________*/
	/*! Graphics::Camera2D::init

	@param GLFWwindow* pWindow
	Pointer to GLFW window currently in use

	@param GLApp::GLObject* ptr
	Pointer to Camera object

	@return none

	This function is called once at the initialization of the camera to compute and initialize the camera window
	*/
	void Camera2D::init(GLFWwindow* pWindow, GLApp::GLObject* ptr)
	{
		// assign address of object of type GLApp::GLObject with
		// name "Camera" in std::map container GLApp::objects ...
		pgo = ptr;
		pgo->orientation.y = 2.f * float(M_PI / 180); // Rotation amount
		// compute camera window's aspect ratio ...
		GLsizei fb_width, fb_height;
		glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
		ar = static_cast<GLfloat>(fb_width) / fb_height;
		int width = int(ar * height);

		// compute camera's up and right vectors ...
		up = { -sin(pgo->orientation.x), cos(pgo->orientation.x) };
		right = { cos(pgo->orientation.x), sin(pgo->orientation.x) };
		// at startup, the camera must be initialized to free camera ...
		view_xform = matrix3x3::mat3x3(1, 0, -pgo->modelCenterPos.x,
			0, 1, -pgo->modelCenterPos.y,
			0, 0, 1);

		camwin_to_ndc_xform = matrix3x3::mat3x3(float(2 / width), 0, 0,
			0, float(2 / height), 0,
			0, 0, 1);
		camworld_to_ndc_xform = camwin_to_ndc_xform * view_xform;
		//world_to_ndc_xform = world_to_ndc_xform;
		//camera2d.position = vector2D::vec2D(pgo->modelCenterPos.x, pgo->modelCenterPos.y);
		//position = vector2D::vec2D(pgo->modelCenterPos.x, pgo->modelCenterPos.y);
		//orientation = vector2D::vec2D(pgo->orientation.x, pgo->orientation.y);
		//std::cout << "Width and height " << float( 2.f / width) << ", " << float(2.f / height) << std::endl;
		//std::cout << "World to ndc " << world_to_ndc_xform.m[0] << ", " << world_to_ndc_xform.m[1] << ", " << world_to_ndc_xform.m[2] << std::endl
		//	<< world_to_ndc_xform.m[3] << ", " << world_to_ndc_xform.m[4] << ", " << world_to_ndc_xform.m[5] << std::endl
		//	<< world_to_ndc_xform.m[6] << ", " << world_to_ndc_xform.m[7] << ", " << world_to_ndc_xform.m[8] << std::endl;
	}

	/*  _________________________________________________________________________*/
	/*! Graphics::Camera2D::update

	@param GLFWwindow* pWindow
	Pointer to GLFW window currently in use

	@return none

	This function is called once per frame to compute and update the camera window
	*/
	void Camera2D::update(GLFWwindow* pWindow)
	{
		//std::cout << "Top of cam " << position.x << ", " << position.y << std::endl;
		// compute camera window's aspect ratio ...
		GLsizei fb_width, fb_height;
		glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
		ar = static_cast<GLfloat>(fb_width) / fb_height;
		int width = int(ar * height);

		//std::cout << "Height and width " << height << ", " << width << std::endl;
		// compute camera's up and right vectors ...
		up = { -sin(pgo->orientation.x), cos(pgo->orientation.x) };
		right = { cos(pgo->orientation.x), sin(pgo->orientation.x) };
		// at startup, the camera must be initialized to free camera ...

		// compute other matrices ...
		camwin_to_ndc_xform = matrix3x3::mat3x3(2.f / (float)width, 0, 0,
			0, 2.f / (float)height, 0,
			0, 0, 1);

		if (Graphics::Input::keystateW == GL_TRUE)
		{
			//pgo->modelCenterPos = pgo->modelCenterPos + linear_speed * up;
			pgo->modelCenterPos.y = pgo->modelCenterPos.y + linear_speed;
		}

		if (Graphics::Input::keystateS == GL_TRUE)
		{
			//pgo->modelCenterPos = pgo->modelCenterPos - linear_speed * up;
			pgo->modelCenterPos.y = pgo->modelCenterPos.y - linear_speed;
		}
		//std::cout << "Top of cam " << pgo->modelCenterPos.x << ", " << pgo->modelCenterPos.y << std::endl;
		if (Graphics::Input::keystateA == GL_TRUE)
		{
			pgo->modelCenterPos.x = pgo->modelCenterPos.x - linear_speed;
			//if (pgo->orientation.x / M_PI * 180 >= 360)
			//{
			//	pgo->orientation.x = 0;
			//}
			//pgo->orientation.x += pgo->orientation.y;
		}

		if (Graphics::Input::keystateD == GL_TRUE)
		{
			pgo->modelCenterPos.x = pgo->modelCenterPos.x + linear_speed;
			//if (pgo->orientation.x / M_PI * 180 <= -360)
			//{
			//	pgo->orientation.x = 0;
			//}
			//pgo->orientation.x -= pgo->orientation.y;
		}

		// Not needed to show
		//if (Graphics::Input::keystateV == GL_TRUE)
		//{
		//	if (camtype_flag == GL_TRUE)
		//	{
		//		camtype_flag = GL_FALSE;
		//	}
		//	else
		//	{
		//		camtype_flag = GL_TRUE;
		//	}
		//	Graphics::Input::keystateV = GL_FALSE;
		//}

		if (Graphics::Input::keystateZ == GL_TRUE)
		{
			if (height >= max_height)
			{
				height_chg_dir = -1;
			}
			else if (height <= min_height)
			{
				height_chg_dir = 1;
			}
			height += height_chg_val * height_chg_dir;
		}

		if (camtype_flag == GL_FALSE)
		{
			view_xform = matrix3x3::mat3x3(1, 0, -pgo->modelCenterPos.x,
				0, 1, -pgo->modelCenterPos.y,
				0, 0, 1);
		}
		else
		{
			//std::cout << "isit this\n";
			view_xform = matrix3x3::mat3x3(right.x, right.y, -(right.x * pgo->modelCenterPos.x + right.y * pgo->modelCenterPos.y),
				up.x, up.y, -(up.x * pgo->modelCenterPos.x + up.y * pgo->modelCenterPos.y),
				0, 0, 1);
		}

		camworld_to_ndc_xform = camwin_to_ndc_xform * view_xform;
		//std::cout << "camwin to ndc in cam " << camwin_to_ndc_xform.m[0] << ", " << camwin_to_ndc_xform.m[1] << ", " << camwin_to_ndc_xform.m[2] << std::endl
		//	<< camwin_to_ndc_xform.m[3] << ", " << camwin_to_ndc_xform.m[4] << ", " << camwin_to_ndc_xform.m[5] << std::endl
		//	<< camwin_to_ndc_xform.m[6] << ", " << camwin_to_ndc_xform.m[7] << ", " << camwin_to_ndc_xform.m[8] << std::endl;
		//std::cout << "view transform in cam " << camworld_to_ndc_xform.m[0] << ", " << camworld_to_ndc_xform.m[1] << ", " << camworld_to_ndc_xform.m[2] << std::endl
		//	<< camworld_to_ndc_xform.m[3] << ", " << camworld_to_ndc_xform.m[4] << ", " << camworld_to_ndc_xform.m[5] << std::endl
		//	<< camworld_to_ndc_xform.m[6] << ", " << camworld_to_ndc_xform.m[7] << ", " << camworld_to_ndc_xform.m[8] << std::endl;
		//std::cout << "camera's view transform in cam " << camera2d.camworld_to_ndc_xform.m[0] << ", " << camera2d.camworld_to_ndc_xform.m[1] << ", " << camera2d.camworld_to_ndc_xform.m[2] << std::endl
		//	<< camera2d.camworld_to_ndc_xform.m[3] << ", " << camera2d.camworld_to_ndc_xform.m[4] << ", " << camera2d.camworld_to_ndc_xform.m[5] << std::endl
		//	<< camera2d.camworld_to_ndc_xform.m[6] << ", " << camera2d.camworld_to_ndc_xform.m[7] << ", " << camera2d.camworld_to_ndc_xform.m[8] << std::endl;

		camera2d.camworld_to_ndc_xform = camworld_to_ndc_xform; // sets static object's world to ndc
		//std::cout << "World to ndc in cam " << world_to_ndc_xform.m[0] << ", " << world_to_ndc_xform.m[1] << ", " << world_to_ndc_xform.m[2] << std::endl
		//	<< world_to_ndc_xform.m[3] << ", " << world_to_ndc_xform.m[4] << ", " << world_to_ndc_xform.m[5] << std::endl
		//	<< world_to_ndc_xform.m[6] << ", " << world_to_ndc_xform.m[7] << ", " << world_to_ndc_xform.m[8] << std::endl;
		//std::cout << "Bottom of cam " << pgo->modelCenterPos.x << ", " << pgo->modelCenterPos.y << std::endl;
		//std::cout << "Bottom of cam " << position.x << ", " << position.y << std::endl;
		//position = vector2D::vec2D(pgo->modelCenterPos.x, pgo->modelCenterPos.y);
		//orientation = vector2D::vec2D(pgo->orientation.x, pgo->orientation.y);


	}

	matrix3x3::mat3x3 Camera2D::getViewxForm()
	{
		return Camera2D::view_xform;
	}
	matrix3x3::mat3x3 Camera2D::getCamwintoNDCForm()
	{
		return Camera2D::camwin_to_ndc_xform;
	}
	matrix3x3::mat3x3 Camera2D::getWorldtoNDCxForm()
	{
		//std::cout << "World to ndc in cam " << world_to_ndc_xform.m[0] << ", " << world_to_ndc_xform.m[1] << ", " << world_to_ndc_xform.m[2] << std::endl
		//	<< world_to_ndc_xform.m[3] << ", " << world_to_ndc_xform.m[4] << ", " << world_to_ndc_xform.m[5] << std::endl
		//	<< world_to_ndc_xform.m[6] << ", " << world_to_ndc_xform.m[7] << ", " << world_to_ndc_xform.m[8] << std::endl;
		return camera2d.camworld_to_ndc_xform;
	}

	//vector2D::Vec2 Graphics::Camera2D::getCamPosition()
	//{
	//	//std::cout << "\nPosition " << pgo->modelCenterPos.x << ", " << pgo->modelCenterPos.y << std::endl;
	//	return pgo->modelCenterPos;
	//}
	//
	//vector2D::Vec2 Graphics::Camera2D::getCamOrientation()
	//{
	//	return pgo->orientation;
	//}


	GLApp::GLObject Camera2D::getCameraObject()
	{
		return *Camera2D::pgo;
	}
	GLint Camera2D::getHeight()
	{
		return Camera2D::height;
	}

	// Get Window Height
	GLint Camera2D::getWinHeight()
	{
		return (int)Graphics::Input::screenheight;
	}
	// Get Window Width
	GLint Camera2D::getWinWidth()
	{
		return (int)Graphics::Input::screenwidth;
	}
}