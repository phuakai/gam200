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
#include <vec2D.h>
#include <vec3D.h>
#include <mat3x3.h>
#include <mat4x4.h>
#include <model.h>
#include <texture.h>
#include "framebuffer.h"

namespace RenderNS
{

	class InstancedRenderer
	{
	public:
		int entitycounter{};
		GLSLShader instanceshader{}; // Shader of all objects in instance
		GLSLShader frameshader{}; // Shader of all objects in instance
		std::vector<ModelNS::modelVtxData> headerdata{}; // Main (control) object in instance
		std::vector<matrix4x4::mat4x4> instancedata{}; // All transformations for different instances
		std::vector<GLushort> ebodata{}; // Ebo/indices data

		GLenum primtype{};
		GLuint vaoid{};

		/******************************************************************************/
		/*!
			This function calls the instance renderer to render all data
		*/
		/******************************************************************************/
		//static void InstanceRender(std::vector<Texture>& texobjs, GLSLShader shader, GLuint vaoid, std::vector<vertexData> data, std::vector<vector2D::vec2D> offsetdata); // instance renders all objects in render
		void InstanceRender(TextureNS::Texture& texobjs); // instance renders all objects in render

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

		ModelNS::Model batchmodel{}; // Model of all objects in batch
		GLSLShader batchshader{}; // Shader of all objects in batch
		GLenum primtype{}; // Primitive type for all objects in batch
		int totaldrawcnt{}; // Total count of all vertices to be drawn
		int totalsize{}; // To add via subdata
		std::vector<ModelNS::modelVtxData> batchdata{}; // All vertices in batch
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
		void BatchRender(std::vector<TextureNS::Texture>& texobjs); // batch renders all objects in render

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
	void DrawFunc(std::vector<RenderNS::InstancedRenderer>& instanceobj, FrameBufferNS::frameBuffer& frame, GLSLShader shadertouse, std::map<std::string, ModelNS::Model> models, TextureNS::Texture texobj, std::vector<GLenum> primitive);
	void QueueEntity(RenderNS::InstancedRenderer& instanceobj, std::map<std::string, ModelNS::Model> models);

}
