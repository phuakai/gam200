/* !
@file    input.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file controls the window inputs
*//*__________________________________________________________________________*/

#pragma once

/*#include <GL/glew.h>*/ // for access to OpenGL API declarations 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vec2D.h>

namespace Graphics
{

	struct Input // Input class
	{       
		// Variables
		static GLint screenwidth, screenheight; // Current screen width and height
		static std::string windowtitle; // Title of window
		static GLFWwindow* ptr_to_window; // Pointer to window 
		static GLboolean mousestateLeft; // Boolean to check left mouse click
		static GLboolean mousestateRight; // Boolean to check left mouse click

		static GLboolean mousestateMiddle; // Boolean to check left mouse click

		static GLdouble fps;
		static GLdouble delta_time; // time taken to complete most recent game loop

		static GLboolean keystateW;  // Move forward
		static GLboolean keystateA;  // Rotate
		static GLboolean keystateS;  // Move backward
		static GLboolean keystateD;  // Rotate

		static GLboolean keystateZ;  // Zoom
		//static GLboolean keystateV;  // View

		static GLboolean keystateC;  // Collision
		static GLboolean keystateI;  // ImGui
		//static GLboolean keystateK;  // Player backward
		//static GLboolean keystateJ;  // Player left
		//static GLboolean keystateL;  // Player right
		static GLboolean keystateP;  // Pause game
		static GLboolean keystateE;  // Add new square object
		static GLboolean keystateQ;  // Add new circle object

		static GLboolean keystateG;  // Toggle for graphics module
		static GLboolean keystateM;  // Toggle Modulate (Colour on texture)
		static GLboolean keystateT;  // Toggle Texture
		static GLboolean keystateB;  // Toggle Alpha blend
		static GLboolean keystateX;  // Toggle collision debug
		static GLboolean keystateO;  // Toggle velocity direction debug
		static GLboolean keystatePlus; // Object scaling
		static GLboolean keystateMinus; // Object scaling
		static GLboolean keystateSquareBracketLeft; // Object rotation
		static GLboolean keystateSquareBracketRight; // Object rotation


		// Get cursor position in viewspace
		static bool getCursorPosViewspace(double* xpos, double* ypos); // Static allows function to be called without Input class object

		// Get cursor position, returned via address in input parameters xpos and ypos
		// Example usage : getCursorPos(&PositionX, &PositionY);
		static bool getCursorPos(double* xpos, double* ypos); // Static allows function to be called without Input class object
		static bool getCursorPos(vector2D::vec2D* mousePos);
		static bool getCursorScreenPos(vector2D::vec2D* mousePos);

		static bool init(GLint w, GLint h, std::string t);
		static void cleanup();

		// Callbacks
		static void error_callback(int error, char const* description); // Error callback function
		static void framebuffersize_callback(GLFWwindow* ptr_win, int width, int height);
		// I/O callbacks ...
		/******************************************************************************/
		/*!
			This function does the keyboard callback
		*/
		/******************************************************************************/
		static void key_callback(GLFWwindow* pwin, int key, int scancode, int action, int mod);

		/******************************************************************************/
		/*!
			This function does the mouse button callback
		*/
		/******************************************************************************/
		static void mousebutton_callback(GLFWwindow* pwin, int button, int action, int mod);

		/******************************************************************************/
		/*!
			This function does the mouse position callback
		*/
		/******************************************************************************/
		static void mousepos_callback(GLFWwindow* pwin, double xpos, double ypos);
		/******************************************************************************/
		/*!
			This function does the mouse scroll callback
		*/
		/******************************************************************************/
		static void mousescroll_callback(GLFWwindow* pwin, double xoffset, double yoffset);

		/******************************************************************************/
		/*!
			This function updates the fps
		*/
		/******************************************************************************/
		static void update_time(double fps_calc_interval);
	};

}