/* !
@file    camera.cpp
@author  a.weiren@digipen.edu
@date    23/8/2022

This file controls the camera of the game
*//*__________________________________________________________________________*/

#include <camera.h>
#define _USE_MATH_DEFINES //for pi
#include <math.h>
#include <input.h>
#include <iostream>

namespace CameraNS
{
	bool FirstClick = true;
	Camera2D::Camera2D()
	{
		right = { 0, 0 };
		up = { 0, 0 };

		position = { 0, 0 }; // Camera position
		orientation = { 0, 0 }; // Orientation

		camworld_to_ndc_xform =
		{ 0, 0, 0,
		  0, 0, 0,
		  0, 0, 0 };
		view_xform =
		{ 0, 0, 0,
		  0, 0, 0,
		  0, 0, 0 };
		camwin_to_ndc_xform =
		{ 0, 0, 0,
		  0, 0, 0,
		  0, 0, 0 };

		width = 0;
		height = 0;
		aspectratio = 0;
		movespeed = 2.f;
		camtype_flag = GL_FALSE;
	}
	/*  _________________________________________________________________________*/
	/*! Graphics::Camera2D::init

	@param GLFWwindow* pWindow
	Pointer to GLFW window currently in use

	@param App::GLObject* ptr
	Pointer to Camera object

	@return none

	This function is called once at the initialization of the camera to compute and initialize the camera window
	*/
	void Camera2D::init(GLFWwindow* pWindow, vector2D::Vec2 pos, vector2D::Vec2 orient)
	{
		FirstClick = true;
		orientation = orient;
		position = pos;
		orientation.y = 2.f * float(M_PI / 180); // Rotation amount

		// compute camera window's aspect ratio
		GLsizei fb_width, fb_height;
		glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
		width = fb_width;
		height = fb_height;
		aspectratio = static_cast<float>(width) / height;


		// compute camera's up and right vectors 
		up = { -sin(orientation.x), cos(orientation.x) };
		right = { cos(orientation.x), sin(orientation.x) };
		// at startup, the camera must be initialized to free camera 
		view_xform = matrix3x3::mat3x3
		(1, 0, -position.x,
			0, 1, -position.y,
			0, 0, 1);

		camwin_to_ndc_xform = matrix3x3::mat3x3
		(float(2 / width), 0, 0,
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
		//height = fb_height;
		aspectratio = static_cast<float>(fb_width) / fb_height;
		width = int(aspectratio * height); // To recalculate width from height
		//std::cout << "Width and height " << width << ", " << height << std::endl;

		// compute camera's up and right vectors 
		up = { -sin(orientation.x), cos(orientation.x) };
		right = { cos(orientation.x), sin(orientation.x) };

		// compute other matrices 
		camwin_to_ndc_xform = matrix3x3::mat3x3
		(2.f / (float)width, 0, 0,
			0, 2.f / (float)height, 0,
			0, 0, 1);

		if (Graphics::Input::keystateW == GL_TRUE)
		{
			//pgo->modelCenterPos = pgo->modelCenterPos + linear_speed * up;
			position.y = position.y + movespeed;
		}

		if (Graphics::Input::keystateS == GL_TRUE)
		{
			//pgo->modelCenterPos = pgo->modelCenterPos - linear_speed * up;
			position.y = position.y - movespeed;
		}
		if (Graphics::Input::keystateA == GL_TRUE)
		{
			position.x = position.x - movespeed;
		}

		if (Graphics::Input::keystateD == GL_TRUE)
		{
			position.x = position.x + movespeed;
		}

		if (Graphics::Input::keystateZ == GL_TRUE)
		{
			static int height_chg_dir = 1;
			static int height_chg_val = 5;
			if (height >= 2000)
			{
				height_chg_dir = -1;
			}
			else if (height <= 500)
			{
				height_chg_dir = 1;
			}
			height += height_chg_val * height_chg_dir;
		}
		static vector2D::vec2D oldpos{};
		vector2D::vec2D newpos{};
		if (Graphics::Input::mousestateMiddle == GL_TRUE)
		{
			double xpos{};
			double ypos{};
			Graphics::Input::getCursorPos(&xpos, &ypos);
			if (FirstClick == true)
			{
				oldpos = vector2D::vec2D((float)xpos - position.x, (float)ypos - position.y);
				//std::cout << "FIRST CLICK " << oldpos.x << ", " << oldpos.y << std::endl;
				FirstClick = false;
			}
			else
			{
				newpos = vector2D::vec2D((float)xpos - position.x, (float)ypos - position.y);
				vector2D::vec2D change = oldpos - newpos;
				position.x += change.x;
				position.y += change.y;
				//std::cout << "Right " << xpos << ", " << ypos << std::endl;
				//std::cout << "Cam " << position.x << ", " << position.y << std::endl;
				//std::cout << "Old pos " << oldpos.x << ", " << oldpos.y << std::endl;
				//std::cout << "New pos " << newpos.x << ", " << newpos.y << std::endl;
				//std::cout << "Change is movement " << change.x << ", " << change.y << std::endl << std::endl;
				oldpos = newpos;
			}
			// Save distance of previous frame
			// When moving mouse, save new distance. New distance - old distance = Change
			// If old is let's say x=500, new is x=550, change will be 50
			// Since we are dragging towards Right side, since change is positive, move the position -50 to the left
			//double xpos{};
			//double ypos{};
			//Graphics::Input::getCursorPos(&xpos, &ypos);
			//std::cout << "Right " << xpos << ", " << ypos << std::endl;
			//std::cout << "Cam " << position.x << ", " << position.y << std::endl;
			//vector2D::vec2D distance = vector2D::vec2D(xpos - position.x, ypos - position.y);
			//vector2D::vec2D newdistance = vector2D::vec2D(xpos - position.x, ypos - position.y);

			//vector2D::vec2D change = distance - newdistance;
			//distance = newdistance;
			//position.x += change.x;
			//position.y += change.y;
			//std::cout << "Change is movement " << change.x << ", " << change.y << std::endl;
		}
		if (Graphics::Input::mousestateMiddle == GL_FALSE && FirstClick == false)
		{
			FirstClick = true;
		}

		if (camtype_flag == GL_FALSE)
		{
			view_xform = matrix3x3::mat3x3
			(1, 0, -position.x,
				0, 1, -position.y,
				0, 0, 1);
		}
		else
		{
			//std::cout << "isit this\n";
			view_xform = matrix3x3::mat3x3
			(right.x, right.y, -(right.x * position.x + right.y * position.y),
				up.x, up.y, -(up.x * position.x + up.y * position.y),
				0, 0, 1);
		}

		camworld_to_ndc_xform = camwin_to_ndc_xform * view_xform;
		//std::cout << "camworld " << camworld_to_ndc_xform.m[0] << std::endl;
		//std::cout << "cam2d camworld in camera " << camera2d.camworld_to_ndc_xform.m[0] << std::endl;
		//camera2d.camworld_to_ndc_xform = camworld_to_ndc_xform; // sets static object's world to ndc
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
		return camworld_to_ndc_xform;
	}

	vector2D::Vec2 Camera2D::getCamPosition()
	{
		return position;
	}

	vector2D::Vec2 Camera2D::getCamOrientation()
	{
		return orientation;
	}

	int Camera2D::getWidth()
	{
		return width;
	}

	int Camera2D::getHeight()
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

	float Camera2D::getCamSpeed()
	{
		return movespeed;
	}

	// Setters
	void Camera2D::setCamSpeed(float speed)
	{
		movespeed = speed;
	}
}