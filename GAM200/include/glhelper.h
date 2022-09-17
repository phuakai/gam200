/* !
@file    glhelper.h
@author  pghali@digipen.edu, a.weiren@digipen.edu
@date    10/06/2022

This file contains the declaration of namespace Helper that encapsulates the
functionality required to create an OpenGL context using GLFW; use GLEW
to load OpenGL extensions; initialize OpenGL state; and finally initialize
the OpenGL application by calling initalization functions associated with
objects participating in the application.

*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLHELPER_H
#define GLHELPER_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <GL/glew.h> // for access to OpenGL API declarations 
#include <GLFW/glfw3.h>
#include <string>

/*  _________________________________________________________________________ */
struct GLHelper
  /*! GLHelper structure to encapsulate initialization stuff ...
  */
{
  static bool init(GLint w, GLint h, std::string t);
  static void cleanup();

  // callbacks ...
  static void error_cb(int error, char const* description);
  static void fbsize_cb(GLFWwindow *ptr_win, int width, int height);
  // I/O callbacks ...
  static void key_cb(GLFWwindow *pwin, int key, int scancode, int action, int mod);
  static void mousebutton_cb(GLFWwindow *pwin, int button, int action, int mod);
  static void mousescroll_cb(GLFWwindow *pwin, double xoffset, double yoffset);
  static void mousepos_cb(GLFWwindow *pwin, double xpos, double ypos);

  static void update_time(double fpsCalcInt = 1.0);

  static void print_specs();

  static GLint width, height;
  static GLdouble fps;
  static GLdouble delta_time; // time taken to complete most recent game loop
  static std::string title;
  static GLFWwindow *ptr_window;

  static GLboolean keystateZ;  // Zoom
  static GLboolean keystateW;  // Move forward
  static GLboolean keystateS;  // Move backward
  static GLboolean keystateA;  // Rotate
  static GLboolean keystateD;  // Rotate
  static GLboolean keystateV;  // View
  static GLboolean keystateC;  // Collision
  static GLboolean keystateI;  // Player forward
  static GLboolean keystateK;  // Player backward
  static GLboolean keystateJ;  // Player left
  static GLboolean keystateL;  // Player right
  static GLboolean keystateP;  // Pause game
 // static GLboolean keystateQ;  // Add new object
  static GLboolean keystateE;  // Pause game
  static GLboolean keystateQ;  // Add new object
  static GLboolean keystateM;  // Toggle Modulate (Colour on texture)
  static GLboolean keystateT;  // Toggle Texture
  static GLboolean keystateB;  // Toggle Alpha blend
  static GLboolean mousestateLeft;
};

#endif /* GLHELPER_H */
