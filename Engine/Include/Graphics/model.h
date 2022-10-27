/* !
@file    model.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file initializes the model mesh
*//*__________________________________________________________________________*/

#ifndef MODEL_H
#define MODEL_H
#include <input.h>
#include <vector>
#include <vec2D.h>
#include <vec3D.h>
#include <string>
#include <map>
#include <unordered_map>


namespace ModelNS
{
	struct modelVtxData {
		vector2D::vec2D posVtx;
		vector3D::vec3D clrVtx;
		vector2D::vec2D txtVtx;
		float txtIndex;
	};
	class Model
	{
	public:
		/******************************************************************************/
		/*!
			This function initializes the models
		*/
		/******************************************************************************/
		Model init(std::string modelname); // read mesh data from file ...
		// Gets the primitive type of model
		GLenum getPrimitiveType();
		// Gets the primitive count of the model
		GLuint getPrimitiveCnt();
		// Gets the position vertex count of the model
		GLuint getPosvtxCnt();
		// Gets the total draw count of the model
		GLuint getDrawCnt();
		// Gets the vector of all position vertices of the model
		std::vector <vector2D::vec2D> getPosVtx();
		// Gets the vector of all model coords of the model
		std::vector <vector2D::vec2D> getModelCoords();
		// Gets the vector of the indices in order for the EBO
		std::vector <GLushort> getPrimitive();
		// Gets the VAOid
		GLuint getVAOid();
		// Gets the VBOid
		GLuint getVBOid();
		// Gets the EBOid
		GLuint getEBOid();

	public:
		GLenum primitive_type = GL_TRIANGLES; // which OpenGL primitive to be rendered?
		GLuint primitive_cnt = 0; // added for tutorial 2
		GLuint posvtx_cnt = 0;
		GLuint draw_cnt = 0;

		std::vector <vector2D::vec2D> pos_vtx;
		std::vector <vector2D::vec2D> model_coords;
		std::vector <GLushort> primitive;

		GLuint vaoid = 0; // handle to VAO
		GLuint vboid = 0;
		GLuint eboid = 0;
	};
}

#endif