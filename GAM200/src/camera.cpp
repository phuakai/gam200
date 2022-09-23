#include <camera.h>
#define _USE_MATH_DEFINES //for pi
#include <math.h>
#include <iostream>


/*  _________________________________________________________________________*/
/*! Graphics::Camera2D::init

@param GLFWwindow* pWindow
Pointer to GLFW window currently in use

@param GLApp::GLObject* ptr
Pointer to Camera object

@return none

This function is called once at the initialization of the camera to compute and initialize the camera window
*/
void Graphics::Camera2D::init(GLFWwindow* pWindow, GLApp::GLObject* ptr)
{
	std::cout << "Function\n";
	// assign address of object of type GLApp::GLObject with
	// name "Camera" in std::map container GLApp::objects ...
	pgo = ptr;
	
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
	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
	camera2d.world_to_ndc_xform = world_to_ndc_xform;
	position = vector2D::vec2D(pgo->modelCenterPos.x, pgo->modelCenterPos.y);
	orientation = vector2D::vec2D(pgo->orientation.x, pgo->orientation.y);
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
void Graphics::Camera2D::update(GLFWwindow* pWindow)
{
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

	if (GLHelper::keystateW == GL_TRUE)
	{
		pgo->modelCenterPos = pgo->modelCenterPos + linear_speed * up;
	}

	if (GLHelper::keystateS == GL_TRUE)
	{
		pgo->modelCenterPos = pgo->modelCenterPos - linear_speed * up;
	}

	if (GLHelper::keystateA == GL_TRUE)
	{
		if (pgo->orientation.x / M_PI * 180 >= 360)
		{
			pgo->orientation.x = 0;
		}
		pgo->orientation.x += pgo->orientation.y;
	}

	if (GLHelper::keystateD == GL_TRUE)
	{
		if (pgo->orientation.x / M_PI * 180 <= -360)
		{
			pgo->orientation.x = 0;
		}
		pgo->orientation.x -= pgo->orientation.y;
	}

	if (GLHelper::keystateV == GL_TRUE)
	{
		if (camtype_flag == GL_TRUE)
		{
			camtype_flag = GL_FALSE;
		}
		else
		{
			camtype_flag = GL_TRUE;
		}
		GLHelper::keystateV = GL_FALSE;
	}

	if (GLHelper::keystateZ == GL_TRUE)
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
		std::cout << "isit this\n";
		view_xform = matrix3x3::mat3x3(right.x, right.y, -(right.x * pgo->modelCenterPos.x + right.y * pgo->modelCenterPos.y),
			up.x, up.y, -(up.x * pgo->modelCenterPos.x + up.y * pgo->modelCenterPos.y),
			0, 0, 1);
	}

	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
	//std::cout << "camwin to ndc in cam " << camwin_to_ndc_xform.m[0] << ", " << camwin_to_ndc_xform.m[1] << ", " << camwin_to_ndc_xform.m[2] << std::endl
	//	<< camwin_to_ndc_xform.m[3] << ", " << camwin_to_ndc_xform.m[4] << ", " << camwin_to_ndc_xform.m[5] << std::endl
	//	<< camwin_to_ndc_xform.m[6] << ", " << camwin_to_ndc_xform.m[7] << ", " << camwin_to_ndc_xform.m[8] << std::endl;
	//std::cout << "view transform in cam " << view_xform.m[0] << ", " << view_xform.m[1] << ", " << view_xform.m[2] << std::endl
	//	<< view_xform.m[3] << ", " << view_xform.m[4] << ", " << view_xform.m[5] << std::endl
	//	<< view_xform.m[6] << ", " << view_xform.m[7] << ", " << view_xform.m[8] << std::endl;
	camera2d.world_to_ndc_xform = world_to_ndc_xform; // sets static object's world to ndc
	//std::cout << "World to ndc in cam " << world_to_ndc_xform.m[0] << ", " << world_to_ndc_xform.m[1] << ", " << world_to_ndc_xform.m[2] << std::endl
	//	<< world_to_ndc_xform.m[3] << ", " << world_to_ndc_xform.m[4] << ", " << world_to_ndc_xform.m[5] << std::endl
	//	<< world_to_ndc_xform.m[6] << ", " << world_to_ndc_xform.m[7] << ", " << world_to_ndc_xform.m[8] << std::endl;
	position = vector2D::vec2D(pgo->modelCenterPos.x, pgo->modelCenterPos.y);
	//std::cout << "Position " << Graphics::camera2d.getCamPosition().x << ", " << pgo->modelCenterPos.x << std::endl;
	orientation = vector2D::vec2D(pgo->orientation.x, pgo->orientation.y);
	// check keyboard button presses to enable camera interactivity

	// update camera aspect ratio - this must be done every frame
	// because it is possible for the user to change viewport
	// dimensions

	// update camera's orientation (if required)

	// update camera's up and right vectors (if required)

	// update camera's position (if required)

	// implement camera's zoom effect (if required)

	// compute appropriate world-to-camera view transformation matrix

	// compute window-to-NDC transformation matrix

	// compute world-to-NDC transformation matrix

}

matrix3x3::mat3x3 Graphics::Camera2D::getViewxForm()
{
	return Graphics::Camera2D::view_xform;
}
matrix3x3::mat3x3 Graphics::Camera2D::getCamwintoNDCForm()
{
	return Graphics::Camera2D::camwin_to_ndc_xform;
}
matrix3x3::mat3x3 Graphics::Camera2D::getWorldtoNDCxForm()
{
	//std::cout << "World to ndc in cam " << world_to_ndc_xform.m[0] << ", " << world_to_ndc_xform.m[1] << ", " << world_to_ndc_xform.m[2] << std::endl
	//	<< world_to_ndc_xform.m[3] << ", " << world_to_ndc_xform.m[4] << ", " << world_to_ndc_xform.m[5] << std::endl
	//	<< world_to_ndc_xform.m[6] << ", " << world_to_ndc_xform.m[7] << ", " << world_to_ndc_xform.m[8] << std::endl;
	return Graphics::camera2d.world_to_ndc_xform;
}

vector2D::Vec2 Graphics::Camera2D::getCamPosition()
{
	return position;
}

vector2D::Vec2 Graphics::Camera2D::getCamOrientation()
{
	return orientation;
}


GLApp::GLObject Graphics::Camera2D::getCameraObject()
{
	return *Graphics::Camera2D::pgo;
}
GLint Graphics::Camera2D::getHeight()
{
	return Graphics::Camera2D::height;
}

// Get Window Height
GLint Graphics::Camera2D::getWinHeight()
{
	return (int)GLHelper::height;
}
// Get Window Width
GLint Graphics::Camera2D::getWinWidth()
{
	return (int)GLHelper::width;
}