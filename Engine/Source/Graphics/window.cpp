#include "window.h"
#include <string.h>
#include <glad/glad.h>

#if false
namespace window
{

	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
	{
		std::cout << "mousePos: " << xpos << " , " << ypos << std::endl;
	}

	void cursorEnterCallback(GLFWwindow* window, int entered)
	{
		if (entered)
		{
			std::cout << "Entered Window" << std::endl;
		}
		else
		{
			std::cout << "Left window" << std::endl;
		}
	}

	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			std::cout << "Right button pressed" << std::endl;
		}
	}

	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		std::cout << xoffset << " : " << yoffset << std::endl;
	}

	void glfwErrorCallback(int error, const char* description)
	{
		fprintf(stderr, "Error: %s\n", description);
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_E && action == GLFW_PRESS)
		{
			std::cout << "e key is pressed\n";
		}
	}

	void glfwWindowCloseCallback(GLFWwindow* win)
	{
		std::cout << "Window closed\n";
	}

	void win::winInit()
	{
		glfwSetErrorCallback(glfwErrorCallback);
		glfwInit();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

		appWin = glfwCreateWindow(winWidth, winHeight, "title", nullptr, nullptr);

		glfwMakeContextCurrent(appWin);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			//return false;
		}

		glfwSetWindowCloseCallback(appWin, glfwWindowCloseCallback);

		glfwSetCursorPosCallback(appWin, cursorPositionCallback);
		glfwSetInputMode(appWin, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		glfwSetCursorEnterCallback(appWin, cursorEnterCallback);

		glfwSetMouseButtonCallback(appWin, mouseButtonCallback);
		//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

		glfwSetScrollCallback(appWin, scrollCallback);

	}

	void win::winUpdate()
	{

	}

	void win::winDestroy()
	{
		glfwTerminate();
	}

	int	win::getWinWidth()
	{
		return winWidth;
	}

	int	win::getWinHeight()
	{
		return winHeight;
	}

}
#endif