#include <input.h>
#include <glhelper.h>
#include <GLFW/glfw3.h>
#include <transform.h>
#include <iostream>

/*  _________________________________________________________________________*/
/*! Graphics::Input::getCursorPosViewSpace

@param float* xpos
X position to be returned

@param float* ypos
Y position to be returned

@return bool
Returns false if error in getting cursor position, true if cursor position is successfully returned

This function can be called to get the current cursor position in view space
*/
bool Graphics::Input::getCursorPosViewspace(double* xpos, double* ypos)
{
	double tmpx;
	double tmpy;
	glfwGetCursorPos(GLHelper::ptr_window, &tmpx, &tmpy);
	if (tmpx == NULL || tmpy == NULL)
	{
		return false;
	}
	else
	{	
		*xpos = tmpx;
		*ypos = tmpy;
		return true;
	}
}

/*  _________________________________________________________________________*/
/*! Graphics::Input::getCursorPos

@param float* xpos
X position to be returned

@param float* ypos
Y position to be returned

@return bool
Returns false if error in getting cursor position, true if cursor position is successfully returned

This function can be called to get the current cursor position
*/
bool Graphics::Input::getCursorPos(double* xpos, double* ypos)
{
	double tmpx;
	double tmpy;
	glfwGetCursorPos(GLHelper::ptr_window, &tmpx, &tmpy);
	if (tmpx == NULL || tmpy == NULL)
	{
		return false;
	}
	else
	{
		matrix3x3::mat3x3 viewport_to_ndc = transform.createViewporttoNDC();

		matrix3x3::mat3x3 ndc_to_world = transform.createNDCtoWorld();

		vector2D::Vec2 worldCursorPos = ndc_to_world * viewport_to_ndc * vector2D::Vec2((float)tmpx, (float) -tmpy);
		*xpos = worldCursorPos.x;
		*ypos = worldCursorPos.y;
		return true;
	}
}
