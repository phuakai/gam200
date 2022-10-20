/* !
@file    graphics.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file handles the batch rendering of the game
*//*__________________________________________________________________________*/

#pragma once

//#include <GL/glew.h> // for access to OpenGL API declarations 
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <app.h>
#include <vec2D.h>
#include <vec3D.h>
#include <mat3x3.h>
#include <model.h>
#include <texture.h>

namespace Graphics {

	class InstancedRenderer
	{
	public:
		GLSLShader instanceshader{}; // Shader of all objects in instance
		std::vector<vertexData> headerdata{}; // Main (control) object in instance
		std::vector<matrix3x3::mat3x3> instancedata{}; // All transformations for different instances
		std::vector<GLushort> ebodata{}; // Ebo/indices data
		GLuint vaoid{};

		/******************************************************************************/
		/*!
			This function calls the instance renderer to render all data
		*/
		/******************************************************************************/
		//static void InstanceRender(std::vector<Texture>& texobjs, GLSLShader shader, GLuint vaoid, std::vector<vertexData> data, std::vector<vector2D::vec2D> offsetdata); // instance renders all objects in render
		void InstanceRender(std::vector<Texture>& texobjs, int entitycount); // instance renders all objects in render
			
		/******************************************************************************/
		/*!
			This function clear the instance
		*/
		/******************************************************************************/
		void InstanceClear(); // instance renders all objects in render

		/******************************************************************************/
		/*!
			This function deletes the instance
		*/
		/******************************************************************************/
		void InstanceDelete();


	};
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
