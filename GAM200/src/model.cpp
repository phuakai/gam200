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
	std::vector<vector2D::Vec2> tex_coord
	{
		vector2D::Vec2(0.f, 0.f), vector2D::Vec2(1.f, 0.f),
		vector2D::Vec2(1.f, 1.f), vector2D::Vec2(0.f, 1.f)
	};
	std::vector<Graphics::vertexData> vertexData;
	for (int i = 0; i < tempmodel.pos_vtx.size(); ++i)
	{
		Graphics::vertexData tmpVtxData;
		//std::cout << "Nani Pos " << tempmodel.pos_vtx[i].x << ", " << tempmodel.pos_vtx[i].y << std::endl;
		//std::cout << "Nani Texture " << tex_coord[i].x << ", " << tex_coord[i].y << std::endl;
		tmpVtxData.posVtx = tempmodel.pos_vtx[i];
		tmpVtxData.txtVtx = tex_coord[i];
		vertexData.emplace_back(tmpVtxData);
	}

	vbo = Graphics::VBO::init();
	Graphics::VBO::store(vbo, sizeof(Graphics::vertexData) * vertexData.size(), vertexData);
	
	//Graphics::VBO::setdata(vbo, sizeof(float) * 2 * tempmodel.pos_vtx.size(), sizeof(float) * 2 * tempmodel.pos_vtx.size(), tex_coord);

	vao = Graphics::VAO::init();
	Graphics::VAO::enableattrib(vao, 0); // Attrib 0

	Graphics::VBO::bind(vao, 0, vbo, 0, sizeof(float) * 4); // Set buffer binding point 0

	Graphics::VAO::setattrib(vao, 0); // Attrib format
	Graphics::VAO::bindattrib(vao, 0, 0); // Bind attrib


	Graphics::VAO::enableattrib(vao, 1); // Attrib 1
	Graphics::VBO::bind(vao, 1, vbo, sizeof(float) * 2, sizeof(float) * 4); // Set buffer binding point 1
	Graphics::VAO::setattrib(vao, 1); // Attrib format 1
	Graphics::VAO::bindattrib(vao, 1, 1); // Bind attrib 1

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
