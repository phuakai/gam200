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
	void Camera2D::init(GLFWwindow* pWindow, vector2D::Vec2 pos, vector2D::Vec2 orient)
	{
		orientation = orient;
		position = pos;
		orientation.y = 2.f * float(M_PI / 180); // Rotation amount

		// compute camera window's aspect ratio
		GLsizei fb_width, fb_height;
		glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
		ar = static_cast<GLfloat>(fb_width) / fb_height;
		int width = int(ar * height);

		// compute camera's up and right vectors 
		up = { -sin(orientation.x), cos(orientation.x) };
		right = { cos(orientation.x), sin(orientation.x) };
		// at startup, the camera must be initialized to free camera 
		view_xform = matrix3x3::mat3x3(1, 0, -position.x,
			0, 1, -position.y,
			0, 0, 1);

		camwin_to_ndc_xform = matrix3x3::mat3x3(float(2 / width), 0, 0,
			0, float(2 / height), 0,
			0, 0, 1);
		camworld_to_ndc_xform = camwin_to_ndc_xform * view_xform;
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
		// compute camera window's aspect ratio 
		GLsizei fb_width, fb_height;
		glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
		ar = static_cast<GLfloat>(fb_width) / fb_height;
		int width = int(ar * height);

		// compute camera's up and right vectors 
		up = { -sin(orientation.x), cos(orientation.x) };
		right = { cos(orientation.x), sin(orientation.x) };

		// compute other matrices 
		camwin_to_ndc_xform = matrix3x3::mat3x3(2.f / (float)width, 0, 0,
			0, 2.f / (float)height, 0,
			0, 0, 1);

		if (Graphics::Input::keystateW == GL_TRUE)
		{
			//pgo->modelCenterPos = pgo->modelCenterPos + linear_speed * up;
			position.y = position.y + linear_speed;
		}

		if (Graphics::Input::keystateS == GL_TRUE)
		{
			//pgo->modelCenterPos = pgo->modelCenterPos - linear_speed * up;
			position.y = position.y - linear_speed;
		}
		if (Graphics::Input::keystateA == GL_TRUE)
		{
			position.x = position.x - linear_speed;
		}

		if (Graphics::Input::keystateD == GL_TRUE)
		{
			position.x = position.x + linear_speed;
		}

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
			view_xform = matrix3x3::mat3x3(1, 0, -position.x,
				0, 1, -position.y,
				0, 0, 1);
		}
		else
		{
			//std::cout << "isit this\n";
			view_xform = matrix3x3::mat3x3(right.x, right.y, -(right.x * position.x + right.y * position.y),
				up.x, up.y, -(up.x * position.x + up.y * position.y),
				0, 0, 1);
		}

		camworld_to_ndc_xform = camwin_to_ndc_xform * view_xform;

		camera2d.camworld_to_ndc_xform = camworld_to_ndc_xform; // sets static object's world to ndc
	}

	matrix3x3::mat3x3 Camera2D::getViewxForm()
	{
		return view_xform;
	}
	matrix3x3::mat3x3 Camera2D::getCamwintoNDCForm()
	{
		return camwin_to_ndc_xform;
	}
	matrix3x3::mat3x3 Camera2D::getWorldtoNDCxForm()
	{
		return camera2d.camworld_to_ndc_xform;
	}

	vector2D::Vec2 Camera2D::getCamPosition()
	{
		return position;
	}
	
	vector2D::Vec2 Camera2D::getCamOrientation()
	{
		return orientation;
	}

	GLint Camera2D::getHeight()
	{
		return height;
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