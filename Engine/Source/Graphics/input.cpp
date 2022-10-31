/* !
@file    input.cpp
@author  a.weiren@digipen.edu
@date    23/8/2022

This file controls the window inputs
*//*__________________________________________________________________________*/

#include <input.h>
#include <camera.h>
//#include <GL/glew.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <transform.h>
#include <iostream>

// static data members declared in GLHelper
GLint Graphics::Input::screenwidth; // Current screen width
GLint Graphics::Input::screenheight; // Current screen height
std::string Graphics::Input::windowtitle; // Title of window
GLFWwindow* Graphics::Input::ptr_to_window; // Pointer to window 

GLdouble Graphics::Input::fps; // FPS
GLdouble Graphics::Input::delta_time; // time per frame


GLboolean Graphics::Input::keystateW;
GLboolean Graphics::Input::keystateA;
GLboolean Graphics::Input::keystateS;
GLboolean Graphics::Input::keystateD;

GLboolean Graphics::Input::keystateZ;  // Zoom
//GLboolean Graphics::Input::keystateV;	// View

GLboolean Graphics::Input::keystateC;  // Collision

GLboolean Graphics::Input::keystateI;  // ImGui
//GLboolean Graphics::Input::keystateK;  // Player backward
//GLboolean Graphics::Input::keystateJ;  // Player left
//GLboolean Graphics::Input::keystateL;  // Player right

GLboolean Graphics::Input::keystateP;  // Pause game
GLboolean Graphics::Input::keystateE;  // Add new square
GLboolean Graphics::Input::keystateQ;  // Add new circle

GLboolean Graphics::Input::keystateG;  // Toggle for graphics module
GLboolean Graphics::Input::keystateM;  // Modulate
GLboolean Graphics::Input::keystateT;  // Toggle Texture
GLboolean Graphics::Input::keystateB;  // Toggle Alpha blend
GLboolean Graphics::Input::keystateX;  // Toggle collision debug
GLboolean Graphics::Input::keystateO;  // Toggle velocity direction debug

GLboolean Graphics::Input::keystatePlus; // Object scaling
GLboolean Graphics::Input::keystateMinus; // Object scaling
GLboolean Graphics::Input::keystateSquareBracketLeft; // Object rotation
GLboolean Graphics::Input::keystateSquareBracketRight; // Object rotation

GLboolean Graphics::Input::mousestateLeft = GL_FALSE;
GLboolean Graphics::Input::mousestateRight = GL_FALSE;

GLboolean Graphics::Input::mousestateMiddle;


bool Graphics::Input::init(GLint w, GLint h, std::string t)
{
	Graphics::Input::screenwidth = w;
	Graphics::Input::screenheight = h;
	Graphics::Input::windowtitle = t;

	Graphics::Input::keystateW = false;
	Graphics::Input::keystateA = false;
	Graphics::Input::keystateS = false;
	Graphics::Input::keystateD = false;

	Graphics::Input::keystateZ = false;  // Zoom

	Graphics::Input::keystateC = false;  // Collision
	Graphics::Input::keystateI = false;  // Collision

	Graphics::Input::keystateP = false;  // Pause game
	Graphics::Input::keystateE = false;  // Add new square
	Graphics::Input::keystateQ = false;  // Add new circle

	Graphics::Input::keystateG = false;  // Toggle for graphics module
	Graphics::Input::keystateM = false;  // Modulate
	Graphics::Input::keystateT = false;  // Toggle Texture
	Graphics::Input::keystateB = false;  // Toggle Alpha blend
	Graphics::Input::keystateX = false;  // Toggle collision debug
	Graphics::Input::keystateO = false;  // Toggle velocity direction debug

	Graphics::Input::keystatePlus = false; // Object scaling
	Graphics::Input::keystateMinus = false; // Object scaling
	Graphics::Input::keystateSquareBracketLeft = false; // Object rotation
	Graphics::Input::keystateSquareBracketRight = false; // Object rotation


	Graphics::Input::mousestateMiddle = GL_FALSE; // Object rotation

	if (!glfwInit()) 
	{
		std::cout << "GLFW init has failed - Aborting program!!!" << std::endl;
		return false;
	}
	glfwSetErrorCallback(Graphics::Input::error_callback); // Error callback if a glfw function fails

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Set major OPENGL version to 4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); // Set minor OPENGL version to 5 (4.5)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set to opengl core profile
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); // Enable double buffering
	glfwWindowHint(GLFW_RED_BITS, 8); 
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8); 
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Set window to not be resizable

	 // Creates a glfw window
	// Width, height, window title, monitor for fullscreen (or NULL for windowed), window who's context to share resources with (or NULL to not share)
	Graphics::Input::ptr_to_window = glfwCreateWindow(screenwidth, screenheight, windowtitle.c_str(), NULL, NULL);
	if (!Graphics::Input::ptr_to_window)
	{
		std::cerr << "GLFW unable to create OpenGL context - Aborting program\n";
		glfwTerminate(); // Terminate glfw after failing to create context
		return false;
	}

	//Makes the context of the window pointer given the current (meaning being used / on the calling thread)
	glfwMakeContextCurrent(Graphics::Input::ptr_to_window);

	glfwSetFramebufferSizeCallback(Graphics::Input::ptr_to_window, Graphics::Input::framebuffersize_callback);

	glfwSetKeyCallback(Graphics::Input::ptr_to_window, Graphics::Input::key_callback);
	glfwSetMouseButtonCallback(Graphics::Input::ptr_to_window, Graphics::Input::mousebutton_callback);
	glfwSetCursorPosCallback(Graphics::Input::ptr_to_window, Graphics::Input::mousepos_callback);
	glfwSetScrollCallback(Graphics::Input::ptr_to_window, Graphics::Input::mousescroll_callback);

	glfwSetInputMode(Graphics::Input::ptr_to_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Normal cursor mode (Hidden makes cursor hidden instead)

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// OpenGL loaded successfully
	std::cout << "OpenGL version loaded: " << GLVersion.major << "." << GLVersion.minor << std::endl;

	glfwSwapInterval(0);
	return true;
}

void Graphics::Input::error_callback(int error, char const* description)
{
	#ifdef _DEBUG
			std::cerr << "GLFW error: " << description << std::endl;
	#endif
}

void Graphics::Input::framebuffersize_callback(GLFWwindow* ptr_win, int width, int height)
{
	#ifdef _DEBUG
		std::cout << "fbsize_cb getting called!!!" << std::endl;
	#endif
	// Use entire window as viewport framebuffer
	glViewport(0, 0, width, height);
}

void Graphics::Input::key_callback(GLFWwindow* pwin, int key, int scancode, int action, int mod)
{
	if (GLFW_PRESS == action) {
		if (GLFW_KEY_ESCAPE == key)
		{
			glfwSetWindowShouldClose(pwin, GLFW_TRUE);
		}
		if (key == GLFW_KEY_W)
		{
			keystateW = GL_TRUE;
		}
		if (key == GLFW_KEY_A)
		{
			keystateA = GL_TRUE;
		}
		if (key == GLFW_KEY_S)
		{
			keystateS = GL_TRUE;
		}
		if (key == GLFW_KEY_D)
		{
			keystateD = GL_TRUE;
		}

		if (key == GLFW_KEY_Z) // Zoom
		{
			keystateZ = GL_TRUE;
		}

		if (key == GLFW_KEY_C) // Collision
		{
			keystateC = GL_TRUE;
		}

		if (key == GLFW_KEY_I) // Collision
		{
			keystateI= GL_TRUE;
		}

		if (key == GLFW_KEY_P) // Pause
		{
			keystateP = GL_TRUE;
		}
		if (key == GLFW_KEY_E) // Create Squares
		{
			keystateE = GL_TRUE;
		}
		if (key == GLFW_KEY_Q) // Create Circles
		{
			keystateQ = GL_TRUE;
		}

		if (key == GLFW_KEY_G) // Enable graphics mode
		{
			keystateG = GL_TRUE;
		}
		if (key == GLFW_KEY_M) // Modulate
		{
			keystateM = GL_TRUE;
		}
		if (key == GLFW_KEY_T) // Enable textures
		{
			keystateT = GL_TRUE;
		}
		if (key == GLFW_KEY_B) // Enable blend mode
		{
			keystateB = GL_TRUE;
		}
		if (key == GLFW_KEY_X) // Collision Debug 
		{
			keystateX = GL_TRUE;
		}
		if (key == GLFW_KEY_O) // Velocity direction
		{
			keystateO = GL_TRUE;
		}

		if (key == GLFW_KEY_EQUAL) // Increase size
		{
			keystatePlus = GL_TRUE;
		}
		if (key == GLFW_KEY_MINUS) // Decrease size
		{
			keystateMinus = GL_TRUE;
		}
		if (key == GLFW_KEY_LEFT_BRACKET) // Rotate left
		{
			keystateSquareBracketLeft = GL_TRUE;
		}
		if (key == GLFW_KEY_RIGHT_BRACKET) // Rotate right
		{
			keystateSquareBracketRight = GL_TRUE;
		}
	}
	else if (GLFW_REPEAT == action)
	{
		if (key == GLFW_KEY_W)
		{
			keystateW = GL_TRUE;
		}
		if (key == GLFW_KEY_A)
		{
			keystateA = GL_TRUE;
		}
		if (key == GLFW_KEY_S)
		{
			keystateS = GL_TRUE;
		}
		if (key == GLFW_KEY_D)
		{
			keystateD = GL_TRUE;
		}

		if (key == GLFW_KEY_Z) // Zoom
		{
			keystateZ = GL_TRUE;
		}

		if (key == GLFW_KEY_C) // Collision
		{
			keystateC = GL_FALSE; // Collision not a toggle
		}

		if (key == GLFW_KEY_I) // Collision
		{
			keystateI = GL_FALSE; // Collision not a toggle
		}

		if (key == GLFW_KEY_P) // Pause
		{
			keystateP = GL_FALSE; // Pause not a toggle
		}
		if (key == GLFW_KEY_E) // Create Squares
		{
			keystateE = GL_TRUE;
		}
		if (key == GLFW_KEY_Q) // Create Circles
		{
			keystateQ = GL_TRUE;
		}

		if (key == GLFW_KEY_G) // Enable graphics mode
		{
			keystateG = GL_FALSE; // Graphics mode not a toggle
		}
		if (key == GLFW_KEY_M) // Modulate
		{
			keystateM = GL_FALSE; // Not a toggle
		}
		if (key == GLFW_KEY_T) // Enable textures
		{
			keystateT = GL_FALSE; // Not a toggle
		}
		if (key == GLFW_KEY_B) // Enable blend mode
		{
			keystateB = GL_FALSE; // Not a toggle
		}
		if (key == GLFW_KEY_X) // Collision Debug 
		{
			keystateX = GL_FALSE; // Not a toggle
		}
		if (key == GLFW_KEY_O) // Velocity direction
		{
			keystateO = GL_FALSE; // Not a toggle
		}

		if (key == GLFW_KEY_EQUAL) // Increase size
		{
			keystatePlus = GL_TRUE;
		}
		if (key == GLFW_KEY_MINUS) // Decrease size
		{
			keystateMinus = GL_TRUE;
		}
		if (key == GLFW_KEY_LEFT_BRACKET) // Rotate left
		{
			keystateSquareBracketLeft = GL_TRUE;
		}
		if (key == GLFW_KEY_RIGHT_BRACKET) // Rotate right
		{
			keystateSquareBracketRight = GL_TRUE;
		}

	}
	else if (GLFW_RELEASE == action) 
	{
		if (key == GLFW_KEY_W)
		{
			keystateW = GL_FALSE;
		}
		if (key == GLFW_KEY_A)
		{
			keystateA = GL_FALSE;
		}
		if (key == GLFW_KEY_S)
		{
			keystateS = GL_FALSE;
		}
		if (key == GLFW_KEY_D)
		{
			keystateD = GL_FALSE;
		}

		if (key == GLFW_KEY_Z) // Zoom
		{
			keystateZ = GL_FALSE;
		}

		if (key == GLFW_KEY_C) // Collision
		{
			keystateC = GL_FALSE;
		}

		if (key == GLFW_KEY_P) // Pause
		{
			keystateP = GL_FALSE;
		}
		if (key == GLFW_KEY_E) // Create Squares
		{
			keystateE = GL_FALSE;
		}
		if (key == GLFW_KEY_Q) // Create Circles
		{
			keystateQ = GL_FALSE;
		}

		if (key == GLFW_KEY_G) // Enable graphics mode
		{
			keystateG = GL_FALSE;
		}
		if (key == GLFW_KEY_M) // Modulate
		{
			keystateM = GL_FALSE;
		}
		if (key == GLFW_KEY_T) // Enable textures
		{
			keystateT = GL_FALSE;
		}
		if (key == GLFW_KEY_B) // Enable blend mode
		{
			keystateB = GL_FALSE;
		}
		if (key == GLFW_KEY_X) // Collision Debug 
		{
			keystateX = GL_FALSE;
		}
		if (key == GLFW_KEY_O) // Velocity direction
		{
			keystateO = GL_FALSE;
		}

		if (key == GLFW_KEY_EQUAL) // Increase size
		{
			keystatePlus = GL_FALSE;
		}
		if (key == GLFW_KEY_MINUS) // Decrease size
		{
			keystateMinus = GL_FALSE;
		}
		if (key == GLFW_KEY_LEFT_BRACKET) // Rotate left
		{
			keystateSquareBracketLeft = GL_FALSE;
		}
		if (key == GLFW_KEY_RIGHT_BRACKET) // Rotate right
		{
			keystateSquareBracketRight = GL_FALSE;
		}

	}
}

void Graphics::Input::mousebutton_callback(GLFWwindow* pwin, int button, int action, int mod) 
{
	static int prevButton = -1;
	if (GLFW_MOUSE_BUTTON_LEFT == button)
	{
		if (GLFW_PRESS == action)
		{
			mousestateLeft = GL_TRUE; // Enable mouse left state on click
			std::cout << " mouse clicked\n";
		}
		else if (GLFW_RELEASE == action)
		{
			mousestateLeft = GL_FALSE; // Disable mouse left state on release
			std::cout << "mouse released\n";
		}
	}

	if (GLFW_MOUSE_BUTTON_RIGHT == button)
	{
		if (GLFW_PRESS == action)
		{
			mousestateRight = GL_TRUE; // Enable mouse left state on click
		}
		if (GLFW_RELEASE == action)
		{
			mousestateRight = GL_FALSE; // Disable mouse left state on release
		}
	}
	if (GLFW_MOUSE_BUTTON_MIDDLE == button)
	{
		if (GLFW_PRESS == action)
		{
			mousestateMiddle = GL_TRUE; // Enable mouse left state on click
		}
		if (GLFW_RELEASE == action)
		{
			mousestateMiddle = GL_FALSE; // Disable mouse left state on release
		}
	}
}

//bool Graphics::Input::mouseIsTriggered(Graphics::mouse mouseInput)
//{
//	if (mouseInput == )
//}

void Graphics::Input::mousepos_callback(GLFWwindow* pwin, double xpos, double ypos) 
{
	#ifdef _DEBUG
		// DEBUG
		std::cout << "Mouse cursor position: (" << xpos << ", " << ypos << ")" << std::endl;
	#endif
}

void Graphics::Input::mousescroll_callback(GLFWwindow* pwin, double xoffset, double yoffset)
{
	#ifdef _DEBUG
		std::cout << "Mouse scroll wheel offset: ("
			<< xoffset << ", " << yoffset << ")" << std::endl;
	#endif
}

void Graphics::Input::update_time(double fps_calc_interval)
{
	static double prev_time = glfwGetTime();
	double curr_time = glfwGetTime();
	delta_time = curr_time - prev_time;
	prev_time = curr_time;

	static double count = 0.0; // number of game loop iterations
	static double start_time = glfwGetTime();
	double elapsed_time = curr_time - start_time;

	++count;

	fps_calc_interval = (fps_calc_interval < 0.0) ? 0.0 : fps_calc_interval;
	fps_calc_interval = (fps_calc_interval > 10.0) ? 10.0 : fps_calc_interval;

	if (elapsed_time > fps_calc_interval) 
	{
		Graphics::Input::fps = count / elapsed_time;
		start_time = curr_time;
		count = 0.0;
	}
}

void Graphics::Input::cleanup()
{
	// Part 1
	glfwTerminate();
}

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
	glfwGetCursorPos(Graphics::Input::ptr_to_window, &tmpx, &tmpy);
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

	//std::cout << "Other thingy when click " << Graphics::camera2d.getWorldtoNDCxForm().m[0] << std::endl;
	glfwGetCursorPos(Graphics::Input::ptr_to_window, &tmpx, &tmpy);
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
		//std::cout << "Position in input " << xpos << ", " << ypos << std::endl;
		return true;
	}
}

bool Graphics::Input::getCursorPos(vector2D::vec2D * mousePos)
{
	double tmpx;
	double tmpy;

	//std::cout << "Other thingy when click " << Graphics::camera2d.getWorldtoNDCxForm().m[0] << std::endl;
	glfwGetCursorPos(Graphics::Input::ptr_to_window, &tmpx, &tmpy);
	if (tmpx == NULL || tmpy == NULL)
	{
		return false;
	}
	else
	{
		matrix3x3::mat3x3 viewport_to_ndc = transform.createViewporttoNDC();
		matrix3x3::mat3x3 ndc_to_world = transform.createNDCtoWorld();

		*mousePos = ndc_to_world * viewport_to_ndc * vector2D::Vec2((float)tmpx, (float)-tmpy);
		//std::cout << "Position in input " << mousePos->x << ", " << mousePos->y << std::endl;
		return true;
	}
}
