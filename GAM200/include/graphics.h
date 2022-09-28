/* !
@file    graphics.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file handles the batch rendering of the game
*//*__________________________________________________________________________*/

#pragma once

#include <GL/glew.h> // for access to OpenGL API declarations 
#include <glm/glm.hpp>
#include <app.h>
#include <vec2D.h>
#include <vec3D.h>
#include <mat3x3.h>
#include <model.h>
#include <texture.h>

namespace Graphics {

	class BatchRenderer
	{
	public:

		Model batchmodel{}; // Model of all objects in batch
		GLSLShader batchshader{}; // Shader of all objects in batch
		GLenum primtype{}; // Primitive type for all objects in batch
		int totaldrawcnt{}; // Total count of all vertices to be drawn
		int totalsize{}; // To add via subdata
		std::vector<vertexData> batchdata{}; // All vertices in batch
		int totalindicesize{}; // For ebo
		std::vector<GLushort> ebodata{}; // Ebo/indices data

		GLuint vboid{};
		GLuint vaoid{};
		GLuint eboid{};

		/******************************************************************************/
		/*!
			This function constructs the batch renderer
		*/
		/******************************************************************************/
		BatchRenderer();

		/******************************************************************************/
		/*!
			This function calls the batch renderer to render all data
		*/
		/******************************************************************************/
		void BatchRender(std::vector<Texture>& texobjs); // batch renders all objects in render

		/******************************************************************************/
		/*!
			This function clear the batch 
		*/
		/******************************************************************************/
		void BatchClear(); // batch renders all objects in render

		/******************************************************************************/
		/*!
			This function deletes the batch
		*/
		/******************************************************************************/
		void BatchDelete();

	};

	//static BatchRenderer basicbatch;


}
