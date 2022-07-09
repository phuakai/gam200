/*!
@file    glapp.cpp
@author  pghali@digipen.edu, lee.g@digipen.edu
@date    17/06/2022

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and to extract function pointers
to OpenGL implementations.
*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glapp.h>
#define _USE_MATH_DEFINES
#include <math.h> // for PI
#include <iostream>
#include <sstream> // for stringstream
#include <array>
#include <iterator>
#include <cstdlib>
#include <fstream> // for ifstream

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
// static data members declared in GLHelper
std::vector<GLApp::GLModel> GLApp::models;
std::unordered_map<std::string, GLSLShader>::iterator GLApp::shd_ref;
std::unordered_map <std::string, GLSLShader> GLApp::shdrpgms; // singleton

// texture object names defined in global scope ...
GLuint texobj_hdl0;

GLuint taskNumber;
bool modulate, alphaBlend;
GLfloat timer, TileSize;

/*  _________________________________________________________________________ */
/*! init

@param none

@return none

This function initializes necessary variables, inserts the shader program, initializes
the model and prints out information
about opengl context and GPU specs.
*/
void GLApp::init() 
{
	taskNumber = 0;
	TileSize = 32;
	modulate = alphaBlend = false;

	// Part 1: Initialize OpenGL state ...
	glClearColor(1.f, 1.f, 1.f, 1.f);

	// Part 2: use entire window as viewport
	glViewport(0, 0, GLHelper::width, GLHelper::height);

	PSS basicShader = std::make_pair("../shaders/my-tutorial-5.vert", "../shaders/my-tutorial-5.frag");

	GLApp::insert_shdrpgm("basicShader", basicShader);

	GLApp::models.emplace_back(GLApp::modelInit());

	texobj_hdl0 = setup_texobj("../images/duck-rgba-256.tex");

	shd_ref = shdrpgms.begin();

	GLHelper::print_specs();
}

/*  _________________________________________________________________________ */
/*! update (GLApp)

@param none

@return none

This function updates variables according to the keyboard input. It also updates the
timer for task 2.
*/
void GLApp::update() 
{
	if (GLHelper::keystateT)
	{
		taskNumber > 5 ? taskNumber = 0 : ++taskNumber;
		GLHelper::keystateT = GL_FALSE;
	}
	else if (GLHelper::keystateM)
	{
		modulate = !modulate;
		GLHelper::keystateM = GL_FALSE;
	}
	else if (GLHelper::keystateA)
	{
		alphaBlend = !alphaBlend;
		GLHelper::keystateA = GL_FALSE;
	}

	static bool zoomedIn{ false };

	if (taskNumber != 2)
		zoomedIn == false ? timer = 0.f : timer = 30.f;
	else
	{
		float tmpTime = timer / 30.f;
		if (timer > 30.f || timer < 0.f)
			zoomedIn = !zoomedIn;
		zoomedIn == false ? timer += static_cast<float>(GLHelper::delta_time) : timer -= static_cast<float>(GLHelper::delta_time);
		float tmpEase = (sin(PI * tmpTime - PI / 2.f) + 1.f) / 2.f;
		TileSize = 16 + tmpEase * (256 - 16);
	}
}

/*  _________________________________________________________________________ */
/*! draw (GLApp)

@param none

@return none

This function prints the window title with the name of the project, student's
name, number of vertices and primites of all models, and the fps. It renders 
the background color, box model and also texture in the application.
*/
void GLApp::draw()
{
	//set window title
	std::stringstream winTitle;

	winTitle << "Tutorial 5 | Grace Lee"
			 << " | Task " << taskNumber << ": ";//0 - 6, name
	switch (taskNumber)
	{
		case 0:
			winTitle << "Paint Color";
			break;
		case 1:
			winTitle << "Fixed-Size Checkerboard";
			break;
		case 2:
			winTitle << "Animated Checkerboard";
			break;
		case 3:
			winTitle << "Texture Mapping";
			break;
		case 4:
			winTitle << "Repeating";
			break;
		case 5:
			winTitle << "Mirroring";
			break;
		case 6:
			winTitle << "Clamping";
			break;
	}

	winTitle << " | Alpha Blend: ";
	alphaBlend == true ? winTitle << "ON" : winTitle << "OFF";

	winTitle << " | Modulate: ";//OFF/ON
	modulate == true ? winTitle << "ON" : winTitle << "OFF";

	glfwSetWindowTitle(GLHelper::ptr_window, winTitle.str().c_str());

	// clear back buffer as before
	glClear(GL_COLOR_BUFFER_BIT);


	(*shd_ref).second.Use();
	glBindVertexArray(models[0].vaoid);

	if (alphaBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glBindTextureUnit(6, texobj_hdl0);

	switch (taskNumber)
	{
	case 4:
		glTextureParameteri(texobj_hdl0, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(texobj_hdl0, GL_TEXTURE_WRAP_T, GL_REPEAT);
		break;
	case 5:
		glTextureParameteri(texobj_hdl0, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTextureParameteri(texobj_hdl0, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		break;
	case 6:
		glTextureParameteri(texobj_hdl0, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(texobj_hdl0, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		break;
	default:
		break;
	}

	GLuint tex_loc = glGetUniformLocation((*shd_ref).second.GetHandle(), "uniTexture");
	glUniform1i(tex_loc, 6);
	GLint UniformTask = glGetUniformLocation((*shd_ref).second.GetHandle(), "myUniformTask");
	glUniform1i(UniformTask, taskNumber);
	GLboolean UniformModulate = glGetUniformLocation((*shd_ref).second.GetHandle(), "myUniformModulate");
	glUniform1i(UniformModulate, modulate);
	GLboolean UniformSize = glGetUniformLocation((*shd_ref).second.GetHandle(), "uTileSize");
	glUniform1f(UniformSize, TileSize);

	glDrawElements(models[0].primitive_type, models[0].draw_cnt, GL_UNSIGNED_SHORT, NULL);
	glDisable(GL_BLEND);
	glBindVertexArray(0);
	(*shd_ref).second.UnUse();
}

/*  _________________________________________________________________________ */
/*! cleanup

@param none

@return none

For now, this function is empty. The function will need to delete and 
invalidate the buffer data and delete the buffers.
*/
void GLApp::cleanup()
{
  // empty for now
}


//
/*  _________________________________________________________________________ */
/*! init (GLModel)

@param none

@return none

This function initializes a box model.
*/
GLApp::GLModel GLApp::modelInit()
{
	std::vector<glm::vec2> tmpPosVtx
	{
		glm::vec2(-1.f, -1.f), glm::vec2(1.f, -1.f),
		glm::vec2(1.f, 1.f), glm::vec2(-1.f, 1.f)
	};

	std::vector<glm::vec3> tmpClrVtx
	{
		glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
		glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 0.f, 1.f)
	};
	
	std::vector<glm::vec2> tmpTxtVtx
	{
		glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f),
		glm::vec2(1.f, 1.f), glm::vec2(0.f, 1.f)
	};

	std::vector<vertexData> vtxData;

	for (int i = 0; i < 4; ++i) 
	{
		vertexData tmpVtxData;
		tmpVtxData.posVtx = tmpPosVtx[i];
		tmpVtxData.clrVtx = tmpClrVtx[i];
		tmpVtxData.txtVtx = tmpTxtVtx[i];
		vtxData.emplace_back(tmpVtxData);
	}

	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(float) * 7 * vtxData.size(), vtxData.data(), GL_DYNAMIC_STORAGE_BIT);

	GLuint vao_hdl;
	glCreateVertexArrays(1, &vao_hdl);
	glEnableVertexArrayAttrib(vao_hdl, 0);
	glVertexArrayVertexBuffer(vao_hdl, 3, vbo_hdl, 0, sizeof(float) * 7);
	glVertexArrayAttribFormat(vao_hdl, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao_hdl, 0, 3);

	glEnableVertexArrayAttrib(vao_hdl, 1);
	glVertexArrayVertexBuffer(vao_hdl, 4, vbo_hdl, sizeof(float) * 2, sizeof(float) * 7);
	glVertexArrayAttribFormat(vao_hdl, 1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao_hdl, 1, 4);

	glEnableVertexArrayAttrib(vao_hdl, 2);
	glVertexArrayVertexBuffer(vao_hdl, 5, vbo_hdl, sizeof(float) * 5, sizeof(float) * 7);
	glVertexArrayAttribFormat(vao_hdl, 2, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao_hdl, 2, 5);


	std::vector<GLushort> idx_vtx{0, 1, 2, 2, 3, 0};

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(), reinterpret_cast<GLvoid*>(idx_vtx.data()), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vao_hdl, ebo_hdl);

	GLApp::GLModel mdl;
	mdl.vaoid = vao_hdl;
	mdl.primitive_type = GL_TRIANGLE_STRIP;
	mdl.draw_cnt = idx_vtx.size();// number of vertices

	return mdl;
}

/*  _________________________________________________________________________ */
/*! insert_shdrpgm

@param key
The name of the shader program.

@param address
The full path to the file to the fragment and vertex shader program.

@return none

This function complies/links/validates the vertex and fragment shader programs.
*/
void GLApp::insert_shdrpgm(std::string key, PSS address)
{
	std::vector<std::pair<GLenum, std::string>> shdr_files
	{
		std::make_pair(GL_VERTEX_SHADER, address.first),
		std::make_pair(GL_FRAGMENT_SHADER, address.second)
	};

	GLSLShader shdr_pgm;
	shdr_pgm.CompileLinkValidate(shdr_files);
	if (GL_FALSE == shdr_pgm.IsLinked())
	{
		std::cout << "Unable to compile/link/validate shader programs\n";
		std::cout << shdr_pgm.GetLog() << "\n";
		std::exit(EXIT_FAILURE);
	}

	GLApp::shdrpgms[key] = shdr_pgm;
}

/*  _________________________________________________________________________ */
/*! setup_texobj

@param pathname
The name of the shader program

@return none

This function reads and sets up the texture object handle. 
*/
GLuint GLApp::setup_texobj(std::string pathname)
{
	std::ifstream input(pathname, std::ios::binary);
	if (!input) // exit program if file does not exist
	{
		std::cout << "ERROR: Unable to open tex file: " << pathname << "\n";
		exit(EXIT_FAILURE);
	}

	GLuint width{ 256 }, height{ 256 }, bytes_per_texel{ 4 };
	
	char* ptr_texels = new char[width * height * bytes_per_texel];
	input.read(ptr_texels, width * height * bytes_per_texel);

	GLuint texobj_hdl;
	glCreateTextures(GL_TEXTURE_2D, 1, &texobj_hdl);
	glTextureStorage2D(texobj_hdl, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(texobj_hdl, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, ptr_texels);
	delete[] ptr_texels;

	return texobj_hdl;
}
