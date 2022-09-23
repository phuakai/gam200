#pragma once

#include <GL/glew.h> // for access to OpenGL API declarations 
#include <glm/glm.hpp>
#include <glapp.h>
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

		BatchRenderer();
		void BatchRender(std::vector<Texture>& texobjs); // batch renders all objects in render
		void BatchClear(); // batch renders all objects in render
		void BatchDelete();

	};

	//static BatchRenderer basicbatch;

	

}