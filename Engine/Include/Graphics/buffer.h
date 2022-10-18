/* !
@file    buffer.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file handles the VAO, VBO and EBO of the game 
*//*__________________________________________________________________________*/

#pragma once

#include <model.h>
#include <vec2D.h>

namespace Graphics
{
	class VAO
	{
		public:
		
			/******************************************************************************/
			/*!
				This function initializes the VAO
			*/
			/******************************************************************************/
			static GLuint init(); // Create vao

			/******************************************************************************/
			/*!
				This function binds the VAO
			*/
			/******************************************************************************/
			static void bind(GLuint vao); // Binds vao

			/******************************************************************************/
			/*!
				This function unbinds the VAO
			*/
			/******************************************************************************/
			static void unbind(); // Unbinds vao

			/******************************************************************************/
			/*!
				This function enables certain attributes of the VAO
			*/
			/******************************************************************************/
			static void enableattrib(GLuint vao, int attrib); // Enable attrib

			/******************************************************************************/
			/*!
				This function binds a vbo with the VAO
			*/
			/******************************************************************************/
			static void bindattrib(GLuint vao, int attribindex, int bufferindex); // Bind attrib

			/******************************************************************************/
			/*!
				This function sets some attributes of the VAO
			*/
			/******************************************************************************/
			static void setattrib(GLuint vao, int index, int unitsize = 2); // Set attrib

			/******************************************************************************/
			/*!
				This function deletes the VAO
			*/
			/******************************************************************************/
			static void del(); // Delete vao

	};

	class VBO
	{
	public:

		/******************************************************************************/
		/*!
			This function initializes the VBO
		*/
		/******************************************************************************/
		static GLuint init(); // Create buffer

		/******************************************************************************/
		/*!
			This function binds the VBO
		*/
		/******************************************************************************/
		static void bind(GLuint vao, int index, GLuint vbo, int offset, GLsizei step); // Binds buffer

		/******************************************************************************/
		/*!
			This function unbinds the VBO
		*/
		/******************************************************************************/		
		static void unbind(); // Unbinds buffer

		/******************************************************************************/
		/*!
			This function stores the vector data in the VBO
		*/
		/******************************************************************************/
		static void store(GLuint vbo, int size, std::vector <Graphics::vertexData> data);

		/******************************************************************************/
		/*!
			This function stores the vector data in the VBO
		*/
		/******************************************************************************/
		static void store(GLuint vbo, int size, std::vector <vector2D::vec2D> data);

		/******************************************************************************/
		/*!
			This function adds data into the VBO
		*/
		/******************************************************************************/
		static void setdata(GLuint vbo, int offset, int size, std::vector <vector2D::vec2D> data);

		/******************************************************************************/
		/*!
			This function deletes the VBO
		*/
		/******************************************************************************/
		static void del(); // Delete buffer
	};

	class EBO
	{
	public:

		/******************************************************************************/
		/*!
			This function initializes the EBO
		*/
		/******************************************************************************/
		static GLuint init(); // Create buffer

		/******************************************************************************/
		/*!
			This function binds the EBO
		*/
		/******************************************************************************/
		static void bind(GLuint vao, GLuint ebo); // Binds buffer

		/******************************************************************************/
		/*!
			This function unbinds the EBO
		*/
		/******************************************************************************/
		static void unbind(); // Unbinds buffer

		/******************************************************************************/
		/*!
			This function stores data into the EBO
		*/
		/******************************************************************************/
		static void store(GLuint ebo, int size, std::vector <GLushort> data);

		/******************************************************************************/
		/*!
			This function adds data into the EBO
		*/
		/******************************************************************************/
		static void setdata(GLuint ebo, int size, std::vector <GLushort> data);

		/******************************************************************************/
		/*!
			This function deletes the EBO
		*/
		/******************************************************************************/
		static void del(); // Delete buffer
	};
}
