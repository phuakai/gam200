#include <model.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <buffer.h>


Graphics::Model Graphics::Model::init(std::string modelname)
{
	Graphics::Model tempmodel;
	std::ifstream ifs{ "../meshes/" + modelname + ".msh" , std::ios::in };
	if (!ifs)
	{
		std::cout << "ERROR: Unable to open mesh file: " << modelname << "\n";
		exit(EXIT_FAILURE);
	}
	ifs.seekg(0, std::ios::beg);
	std::string line_mesh;
	getline(ifs, line_mesh);
	std::istringstream linestream{ line_mesh };
	std::string meshname;
	char prefix;
	linestream >> prefix >> meshname;
	//std::vector <vector2D::vec2D> pos_vtx;
	std::vector <GLushort> primitive;

	GLuint vbo, vao, ebo;
	while (getline(ifs, line_mesh))
	{
		std::istringstream linestream{ line_mesh };
		//float floatstuff;
		GLfloat x, y;
		GLushort glushortstuff;
		linestream >> prefix;

		switch (prefix)
		{
		case 't': // triangle
			while (linestream >> glushortstuff)
			{
				tempmodel.primitive.emplace_back(glushortstuff);
			}
			tempmodel.primitive_type = GL_TRIANGLES;

			break;
		case 'f': //trifan 
			while (linestream >> glushortstuff)
			{
				tempmodel.primitive.emplace_back(glushortstuff);
			}
			tempmodel.primitive_type = GL_TRIANGLE_FAN;

			break;
		case 'v':
			linestream >> x >> y;
			tempmodel.pos_vtx.emplace_back(vector2D::vec2D(x, y));
			tempmodel.model_coords.emplace_back(vector2D::vec2D(x, y));
			break;
		default:
			break;
		}
	}


	vbo = Graphics::VBO::init();
	Graphics::VBO::store(vbo, sizeof(vector2D::vec2D) * tempmodel.pos_vtx.size(), tempmodel.pos_vtx);

	vao = Graphics::VAO::init();
	Graphics::VAO::enableattrib(vao);

	Graphics::VBO::bind(vao, vbo, sizeof(vector2D::vec2D));

	Graphics::VAO::setattrib(vao);
	Graphics::VAO::bindattrib(vao);

	ebo = Graphics::EBO::init();
	Graphics::EBO::store(ebo, sizeof(GLushort) * tempmodel.primitive.size(), tempmodel.primitive);

	Graphics::EBO::bind(vao, ebo);

	Graphics::VAO::unbind();


	tempmodel.primitive_cnt = tempmodel.primitive.size();
	tempmodel.posvtx_cnt = tempmodel.pos_vtx.size();
	//std::cout << "Model " << tempmodel.getPosvtxCnt() << std::endl;
	tempmodel.draw_cnt = tempmodel.primitive.size();

	tempmodel.vaoid = vao;
	tempmodel.vboid = vbo;
	tempmodel.eboid = ebo;

	return tempmodel;
}

GLenum Graphics::Model::getPrimitiveType()
{
	return primitive_type;
}

GLuint Graphics::Model::getPrimitiveCnt()
{
	return primitive_cnt;
}
GLuint Graphics::Model::getPosvtxCnt()
{
	return posvtx_cnt;
}
GLuint Graphics::Model::getDrawCnt()
{
	return draw_cnt;
}

std::vector <vector2D::vec2D> Graphics::Model::getPosVtx()
{
	return pos_vtx;
}

std::vector <vector2D::vec2D> Graphics::Model::getModelCoords()
{
	return model_coords;
}

std::vector <GLushort> Graphics::Model::getPrimitive()
{
	return primitive;
}

GLuint Graphics::Model::getVAOid()
{
	return vaoid;
}

GLuint Graphics::Model::getVBOid()
{
	return vboid;
}

GLuint Graphics::Model::getEBOid()
{
	return eboid;
}
