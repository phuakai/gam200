#include <glhelper.h>
#include <vector>
#include <vec2D.h>
#include <string>
#include <map>
#include <unordered_map>

#ifndef MODEL_H
#define MODEL_H

namespace Graphics
{
	class Model
	{
	public:
		// you could add member functions for convenience if you so wish ...
		Model init(std::string modelname); // read mesh data from file ...
		GLenum getPrimitiveType();
		GLuint getPrimitiveCnt();
		GLuint getPosvtxCnt();
		GLuint getDrawCnt();
		std::vector <vector2D::vec2D> getPosVtx();
		std::vector <vector2D::vec2D> getModelCoords();
		std::vector <GLushort> getPrimitive();
		GLuint getVAOid();
		GLuint getVBOid();
		GLuint getEBOid();

		//void release(); // return buffers back to GPU ...

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
	
	//std::map<std::string, Model> models;
}

#endif