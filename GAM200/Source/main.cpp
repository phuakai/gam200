/*!
@file    main.cpp
@author  a.weiren@digipen.edu
@date    20/8/2022

This file uses functionality defined in types GLHelper and GLApp to initialize 
an OpenGL context and implement a game loop.

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
// Extension loader library's header must be included before GLFW's header!!!
#include <input.h>
#include <app.h>
#include <iostream>
#include <fstream>
#include "mainHeader.h"
#include "levelEditor.h"

/*                                                   type declarations
----------------------------------------------------------------------------- */

//template<class C>
//extern class ECS ecs;

/*                                                      function declarations
----------------------------------------------------------------------------- */
static void draw();
static void update();
static void init();
static void cleanup();

bool imguiShow;
double imguiMouseX;
double imguiMouseY;

/*                                                      function definitions
----------------------------------------------------------------------------- */
/*  _________________________________________________________________________ */
/*! main

@param none

@return int

Indicates how the program existed. Normal exit is signaled by a return value of
0. Abnormal termination is signaled by a non-zero return value.
Note that the C++ compiler will insert a return 0 statement if one is missing.
*/
int main() 
{
    init();

    while (!glfwWindowShouldClose(Graphics::Input::ptr_to_window)) 
    {
        update();
        draw();
    }

    cleanup();
}

/*  _________________________________________________________________________ */
/*! init
@param none
@return none

The OpenGL context initialization stuff is abstracted away in Graphics::Input::init.
The specific initialization of OpenGL state and geometry data is
abstracted away in GLApp::init
*/
static void init() 
{
   
    engineInit();
    imguiInit();
}

/*  _________________________________________________________________________ */
/*! update
@param none
@return none

Uses Graphics::Input::GLFWWindow* to get handle to OpenGL context.
For now, there are no objects to animate nor keyboard, mouse button click,
mouse movement, and mouse scroller events to be processed.
*/
static void update() 
{
    engineUpdate();

    if (Graphics::Input::keystateI)
    {
        imguiShow = imguiShow ? false : true;
        Graphics::Input::keystateI = false;
    }

    if (imguiShow)
    {
        imguiUpdate();
    }
}

/*  _________________________________________________________________________ */
/*! draw
@param none
@return none

Call application to draw and then swap front and back frame buffers ...
Uses Graphics::Input::GLFWWindow* to get handle to OpenGL context.
*/
static void draw() 
{
    engineDraw();

    if (imguiShow)
    {
        imguiDraw();
    }

    swapBuffer();
}

/*  _________________________________________________________________________ */
/*! cleanup
@param none
@return none

Return allocated resources for window and OpenGL context thro GLFW back
to system.
Return graphics memory claimed through 
*/
void cleanup() 
{
    engineFree();
}
