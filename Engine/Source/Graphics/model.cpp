/* !
@file    model.cpp
@author  a.weiren@digipen.edu
@date    23/8/2022

This file initializes the model mesh
*//*__________________________________________________________________________*/

#include <model.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <buffer.h>


namespace ModelNS
{
	Model Model::init(std::string modelname)
	{
		Model tempmodel;
		std::cout << "Models " << modelname << std::endl;
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
			case 'l': // line
				while (linestream >> glushortstuff)
				{
					tempmodel.primitive.emplace_back(glushortstuff);
				}
				tempmodel.primitive_type = GL_LINES;

				break;
			default:
				break;
			}
		}
		vbo = BufferNS::VBO::createVBO();
		if (modelname == "square")
		{
			std::vector<vector2D::Vec2> tex_coord
			{
				vector2D::Vec2(0.f, 0.f), vector2D::Vec2(1.f, 0.f),
				vector2D::Vec2(1.f, 1.f), vector2D::Vec2(0.f, 1.f)
			};
			std::vector<vector3D::Vec3> clr_vtx
			{
				vector3D::Vec3(1.f, 0.f, 0.f), vector3D::Vec3(0.f, 1.f, 0.f),
				vector3D::Vec3(0.f, 0.f, 1.f), vector3D::Vec3(1.f, 0.f, 1.f)
			};
			std::vector<modelVtxData> vertexData;
			for (int i = 0; i < tempmodel.pos_vtx.size(); ++i)
			{
				modelVtxData tmpVtxData;
				//std::cout << "Nani Pos " << tempmodel.pos_vtx[i].x << ", " << tempmodel.pos_vtx[i].y << std::endl;
				//std::cout << "Nani Texture " << tex_coord[i].x << ", " << tex_coord[i].y << std::endl;
				tmpVtxData.posVtx = tempmodel.pos_vtx[i];
				if (modelname != "circle")
				{
					tmpVtxData.clrVtx = clr_vtx[i];
					tmpVtxData.txtVtx = tex_coord[i];
				}
				else
				{
					tmpVtxData.clrVtx = vector3D::Vec3(1.f, 0.f, 0.f);
					tmpVtxData.txtVtx = vector2D::Vec2(0.f, 0.f);
				}
				vertexData.emplace_back(tmpVtxData);
			}

			BufferNS::VBO::createVBOstorage(vbo, (int)sizeof(modelVtxData) * (int)vertexData.size(), vertexData);
		}
		if (modelname == "line")
		{
			std::vector<vector2D::Vec2> tex_coord
			{
				vector2D::Vec2(0.f, 0.f), vector2D::Vec2(0.f, 1.f)
			};
			std::vector<vector3D::Vec3> clr_vtx
			{
				vector3D::Vec3(1.f, 0.f, 0.f), vector3D::Vec3(1.f, 0.f, 0.f)
			};
			std::vector<modelVtxData> vertexData;
			for (int i = 0; i < tempmodel.pos_vtx.size(); ++i)
			{
				modelVtxData tmpVtxData;
				//std::cout << "Nani Pos " << tempmodel.pos_vtx[i].x << ", " << tempmodel.pos_vtx[i].y << std::endl;
				//std::cout << "Nani Texture " << tex_coord[i].x << ", " << tex_coord[i].y << std::endl;
				tmpVtxData.posVtx = tempmodel.pos_vtx[i];
				tmpVtxData.clrVtx = clr_vtx[i];
				tmpVtxData.txtVtx = tex_coord[i];
				vertexData.emplace_back(tmpVtxData);
			}
			BufferNS::VBO::createVBOstorage(vbo, (int)sizeof(modelVtxData) * (int)vertexData.size(), vertexData);
		}
		//Graphics::VBO::setdata(vbo, sizeof(float) * 2 * tempmodel.pos_vtx.size(), sizeof(float) * 2 * tempmodel.pos_vtx.size(), tex_coord);

		vao = BufferNS::VAO::createVAO();
		BufferNS::VAO::enableVAOattrib(vao, 0); // Attrib 0
		BufferNS::VBO::bindVBO(vao, 0, vbo, 0, sizeof(float) * 7); // Set buffer binding point 0
		BufferNS::VAO::setVAOattrib(vao, 0, 2); // Attrib format
		BufferNS::VAO::bindVAOattrib(vao, 0, 0); // Bind attrib

		BufferNS::VAO::enableVAOattrib(vao, 1); // Attrib 0
		BufferNS::VBO::bindVBO(vao, 1, vbo, sizeof(float) * 2, sizeof(float) * 7); // Set buffer binding point 0
		BufferNS::VAO::setVAOattrib(vao, 1, 3); // Attrib format
		BufferNS::VAO::bindVAOattrib(vao, 1, 1); // Bind attrib


		BufferNS::VAO::enableVAOattrib(vao, 2); // Attrib 1
		BufferNS::VBO::bindVBO(vao, 2, vbo, sizeof(float) * 5, sizeof(float) * 7); // Set buffer binding point 1
		BufferNS::VAO::setVAOattrib(vao, 2, 2); // Attrib format 1
		BufferNS::VAO::bindVAOattrib(vao, 2, 2); // Bind attrib 1

		ebo = BufferNS::EBO::createEBO();
		for (int i = 0; i < tempmodel.primitive.size(); i++)
		{
			std::cout << "IDS " << tempmodel.primitive[i] << std::endl;
		}
		BufferNS::EBO::createEBOstorage(ebo, (int)sizeof(GLushort) * (int)tempmodel.primitive.size(), tempmodel.primitive);

		BufferNS::EBO::bindEBO(vao, ebo);

		BufferNS::VAO::unbindVAO();


		tempmodel.primitive_cnt = (int)tempmodel.primitive.size();
		tempmodel.posvtx_cnt = (int)tempmodel.pos_vtx.size();
		//std::cout << "Model " << tempmodel.getPosvtxCnt() << std::endl;
		tempmodel.draw_cnt = (int)tempmodel.primitive.size();

		tempmodel.vaoid = vao;
		tempmodel.vboid = vbo;
		tempmodel.eboid = ebo;

		return tempmodel;
	}

	GLenum Model::getPrimitiveType()
	{
		return primitive_type;
	}

	GLuint Model::getPrimitiveCnt()
	{
		return primitive_cnt;
	}
	GLuint Model::getPosvtxCnt()
	{
		return posvtx_cnt;
	}
	GLuint Model::getDrawCnt()
	{
		return draw_cnt;
	}

	std::vector <vector2D::vec2D> Model::getPosVtx()
	{
		return pos_vtx;
	}

	std::vector <vector2D::vec2D> Model::getModelCoords()
	{
		return model_coords;
	}

	std::vector <GLushort> Model::getPrimitive()
	{
		return primitive;
	}

	GLuint Model::getVAOid()
	{
		return vaoid;
	}

	GLuint Model::getVBOid()
	{
		return vboid;
	}

	GLuint Model::getEBOid()
	{
		return eboid;
	}
}