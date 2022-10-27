/* !
@file    buffer.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file handles the VAO, VBO and EBO of the game 
*//*__________________________________________________________________________*/

#pragma once

#include <model.h>
#include <vec2D.h>
#include <mat3x3.h>
#include "framebuffer.h"

namespace BufferNS
{
	struct VAO
	{
		public:
		
			/******************************************************************************/
			/*!
				This function initializes the VAO
			*/
			/******************************************************************************/
			static GLuint createVAO(); // Create vao

			/******************************************************************************/
			/*!
				This function binds the VAO
			*/
			/******************************************************************************/
			static void bindVAO(GLuint vao); // Binds vao

			/******************************************************************************/
			/*!
				This function unbinds the VAO
			*/
			/******************************************************************************/
			static void unbindVAO(); // Unbinds vao

			/******************************************************************************/
			/*!
				This function enables certain attributes of the VAO
			*/
			/******************************************************************************/
			static void enableVAOattrib(GLuint vao, int attrib); // Enable attrib

			/******************************************************************************/
			/*!
				This function binds a vbo with the VAO
			*/
			/******************************************************************************/
			static void bindVAOattrib(GLuint vao, int attribindex, int bufferindex); // Bind attrib

			/******************************************************************************/
			/*!
				This function sets some attributes of the VAO
			*/
			/******************************************************************************/
			static void setVAOattrib(GLuint vao, int index, int unitsize = 2, int offset = 0); // Set attrib

			/******************************************************************************/
			/*!
				This function deletes the VAO
			*/
			/******************************************************************************/
			static void delVAO(GLuint vao); // Delete vao

	};

	struct VBO
	{
	public:

		/******************************************************************************/
		/*!
			This function initializes the VBO
		*/
		/******************************************************************************/
		static GLuint createVBO(); // Create buffer

		/******************************************************************************/
		/*!
			This function binds the VBO
		*/
		/******************************************************************************/
		static void bindVBO(GLuint vao, int index, GLuint vbo, int offset, GLsizei step); // Binds buffer

		/******************************************************************************/
		/*!
			This function unbinds the VBO
		*/
		/******************************************************************************/		
		static void unbindVBO(); // Unbinds buffer

		/******************************************************************************/
		/*!
			This function stores the vector data in the VBO
		*/
		/******************************************************************************/
		static void createVBOstorage(GLuint vbo, int size, std::vector <Graphics::vertexData> data);

		/******************************************************************************/
		/*!
			This function stores the vector data in the VBO
		*/
		/******************************************************************************/
		static void createVBOstorage(GLuint vbo, int size, std::vector <FrameBufferNS::fBvertexData> data);

		/******************************************************************************/
		/*!
			This function stores the vector data in the VBO
		*/
		/******************************************************************************/
		static void createVBOstorage(GLuint vbo, int size, std::vector <matrix3x3::mat3x3> data);

		/******************************************************************************/
		/*!
			This function adds data into the VBO
		*/
		/******************************************************************************/
		static void editVBOstorage(GLuint vbo, int offset, int size, std::vector <vector2D::vec2D> data);

		/******************************************************************************/
		/*!
			This function deletes the VBO
		*/
		/******************************************************************************/
		static void delVBO(GLuint vbo); // Delete buffer
	};

	struct EBO
	{
	public:

		/******************************************************************************/
		/*!
			This function initializes the EBO
		*/
		/******************************************************************************/
		static GLuint createEBO(); // Create buffer

		/******************************************************************************/
		/*!
			This function binds the EBO
		*/
		/******************************************************************************/
		static void bindEBO(GLuint vao, GLuint ebo); // Binds buffer

		/******************************************************************************/
		/*!
			This function unbinds the EBO
		*/
		/******************************************************************************/
		static void unbindEBO(); // Unbinds buffer

		/******************************************************************************/
		/*!
			This function stores data into the EBO
		*/
		/******************************************************************************/
		static void createEBOstorage(GLuint ebo, int size, std::vector <GLushort> data);

		/******************************************************************************/
		/*!
			This function adds data into the EBO
		*/
		/******************************************************************************/
		static void editEBOstorage(GLuint ebo, int size, std::vector <GLushort> data);

		/******************************************************************************/
		/*!
			This function deletes the EBO
		*/
		/******************************************************************************/
		static void delEBO(GLuint ebo); // Delete buffer
	};
}
