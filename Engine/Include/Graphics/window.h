#pragma once

#ifndef WINDOW_H
#define WINDOW_H
#if false

#include <GLFW/glfw3.h>
#include <iostream>

namespace window
{
	struct win
	{
	public:
		void					winInit();
		void					winUpdate();
		void					winDestroy();
		int						getWinWidth();
		int						getWinHeight();

		static GLFWwindow*		appWin;
	private:
		int						winWidth;
		int						winHeight;
		std::string				winTitle;

	};

} // window namespace

#endif // WINDOW_H

#endif