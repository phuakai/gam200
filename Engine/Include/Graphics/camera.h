/* !
@file    camera.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file controls the camera of the game
*//*__________________________________________________________________________*/

#pragma once

#include <vector>
#include <vec2D.h>
#include <mat3x3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include <app.h>

namespace CameraNS
{
	struct GLViewport
	{
		GLint x, y;
		GLsizei width, height;
	};

	class Camera2D
	{
	public:
		Camera2D();
		void init(GLFWwindow* pWindow, vector2D::Vec2 pos, vector2D::Vec2 orient);
		void update(GLFWwindow* pWindow);

		// Getters
		matrix3x3::mat3x3 getViewxForm();
		matrix3x3::mat3x3 getCamwintoNDCForm();
		// Get world to NDC transformation matrix
		matrix3x3::mat3x3 getWorldtoNDCxForm();

		vector2D::Vec2 getCamPosition();
		vector2D::Vec2 getCamOrientation();
		// Get width
		int getWidth();
		// Get height
		int getHeight();
		// Get Window Height
		int getWinHeight();
		// Get Window Width
		int getWinWidth();

		float getCamSpeed();

		// Setters
		void setCamSpeed(float speed);
	public:

		vector2D::Vec2 right;
		vector2D::Vec2 up;

		vector2D::Vec2 position; // Camera position
		vector2D::vec2D orientation; // Orientation

		matrix3x3::mat3x3 camworld_to_ndc_xform;

		matrix3x3::mat3x3 view_xform;
		matrix3x3::mat3x3 camwin_to_ndc_xform;

		int width;
		int height;

		float aspectratio;

		float movespeed;

		// keyboard button press flags
		GLboolean camtype_flag; 

	};

	static std::vector<GLViewport> vps; // container for viewports

}