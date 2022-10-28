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
#include <app.h>

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

		// Temp
		matrix3x3::mat3x3 getViewxForm();
		// Temp
		matrix3x3::mat3x3 getCamwintoNDCForm();
		// Get world to NDC transformation matrix
		matrix3x3::mat3x3 getWorldtoNDCxForm();

		vector2D::Vec2 getCamPosition();
		vector2D::Vec2 getCamOrientation();

		// Get height
		GLint getHeight();

		// Get Window Height
		int getWinHeight();
		// Get Window Width
		int getWinWidth();
	public:

		vector2D::Vec2 right{};
		vector2D::Vec2 up{};

		vector2D::Vec2 position{}; // Camera position
		vector2D::vec2D orientation{}; // Orientation

		matrix3x3::mat3x3 camworld_to_ndc_xform{};
		GLApp::GLObject* pgo{}; // pointer to game object that embeds camera

		matrix3x3::mat3x3 view_xform{};
		matrix3x3::mat3x3 camwin_to_ndc_xform{};

		GLint height{ 900 };
		GLfloat ar{};

		// window change parameters ...
		GLint min_height{ 500 }, max_height{ 2000 };
		// height is increasing if 1 and decreasing if -1
		GLint height_chg_dir{ 1 };
		// increments by which window height is changed per Z key press
		GLint height_chg_val{ 5 };
		// camera's speed when button U is pressed
		GLfloat linear_speed{ 2.f };
		// keyboard button press flags
		GLboolean camtype_flag{ GL_FALSE }; // button V
		GLboolean zoom_flag{ GL_FALSE }; // button Z
		GLboolean left_turn_flag{ GL_FALSE }; // button H
		GLboolean right_turn_flag{ GL_FALSE }; // button K
		GLboolean move_flag{ GL_FALSE }; // button U

	// you can implement these functions as you wish ...

	};
	// define object of type Camera2D ...
	static Camera2D camera2d;

	static std::vector<GLViewport> vps; // container for viewports

}