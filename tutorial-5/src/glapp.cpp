/*!
@file    glapp.cpp
@author  pghali@digipen.edu, a.weiren@digipen.edu
@date    17/06/2022

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and to extract function pointers
to OpenGL implementations.


*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glapp.h>
#include <iostream>
#include <array>
#include <chrono> // Time
#include <random>
#include <sstream>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp> 
#include <fstream>
#include <iomanip>


/*                                                   objects with file scope
----------------------------------------------------------------------------- */
#define M_PI 3.1415926535897932384626433832795 // Pi macro def

//std::map<std::string, GLSLShader> GLApp::shdrpgms; // define shaders

std::map<std::string, GLApp::GLModel> GLApp::models; // define models

std::map<std::string, GLApp::GLObject> GLApp::objects; // define objects


GLApp::GLModel GLApp::mdl; // define mdl

GLApp::DrawVec GLApp::drawer; // define drawer

std::vector<GLApp::DrawVec> drawvector; // draw vector

GLuint texobj;

GLuint tasknum;
bool alphablend, modulate, animChange;
float TileSize;
float currTime;

/*  _________________________________________________________________________*/
/*! GLApp::init

@param none
@return none

This function is called once on initialization to initialize values and call setup functions
It uses OpenGL functions such as:
glClearColor and glViewport to initialize the app
*/

void GLApp::init() {
	//Initialize globals
	tasknum = 0;
	alphablend = false;
	modulate = false;
	TileSize = 16.f;
	currTime = 0.f; // animation
	animChange = false; // animation

	// Part 1: initialize OpenGL state ...
	glClearColor(1.f, 1.f, 1.f, 1.f); // clear colorbuffer with RGBA value in glClearColor

	// Part 2: use the entire window as viewport ...
	GLint w = GLHelper::width, h = GLHelper::height;
	
	glViewport(0, 0, w, h);

	// Part 3: parse scene file $(SolutionDir)scenes/tutorial-4.scn
	// and store repositories of models of type GLModel in container
	// GLApp::models, store shader programs of type GLSLShader in
	// container GLApp::shdrpgms, and store repositories of objects of
	// type GLObject in container GLApp::objects
	//GLApp::init_scene("../scenes/tutorial-4.scn");

	mdl.setup_vao();

	mdl.setup_shdrpgm("../shaders/my-tutorial-5.vert", "../shaders/my-tutorial-5.frag");

	texobj = setup_texobj("../images/duck-rgba-256.tex");

	// Part 4: initialize camera
	//GLApp::camera2d.init(GLHelper::ptr_window, &GLApp::objects.at("Camera"));

	// Part 5: Print OpenGL context and GPU specs
	GLHelper::print_specs();
}


/*  _________________________________________________________________________*/
/*! GLApp::update

@param none
@return none


This function updates the polygon rasterization mode when 'P' is pressed,
it also spawns new objects until reaching the maximum object limit, where it will
despawn the oldest objects, and respawn objects again once no objects are left.

*/
void GLApp::update() {
	// first, update camera

	// next, iterate through each element of container objects
	// for each object of type GLObject in container objects
	// call update function GLObject::update(delta_time) except on
	// object which has camera embedded in it - this is because
	// the camera has already updated the object's orientation
	if (GLHelper::keystateT == GL_TRUE)
	{
		if (animChange == false)
		{
			currTime = 0.f;
		}
		else
		{
			currTime = 30.f;
		}
		std::cout << "T Pressed\n";
		tasknum > 5 ? tasknum = 0 : tasknum++;
		//tasknum++;
		GLHelper::keystateT = GL_FALSE;
		//if (tasknum > 6)
		//{
		//	tasknum = 0;
		//}
	}
	if (GLHelper::keystateM == GL_TRUE)
	{
		std::cout << "M Pressed\n";
		modulate = !modulate;
		GLHelper::keystateM = GL_FALSE;
	}
	if (GLHelper::keystateA == GL_TRUE)
	{
		std::cout << "A Pressed\n";
		alphablend = !alphablend;
		GLHelper::keystateA = GL_FALSE;
	}
	if (currTime <= 30.f && animChange == false)
	{
		currTime += (float)GLHelper::delta_time;
	}
	else
	{
		currTime -= (float)GLHelper::delta_time;
		animChange = true;
		if (currTime <= 0.f)
		{
			animChange = false;
		}
	}
	float t = currTime / 30.f;
	float e = ((float)sin(M_PI * t - M_PI / 2.0) + 1) / 2;
	std::cout << e << std::endl;
	TileSize = 16 + e * (256 - 16);
	std::cout << "Tile size " << TileSize << std::endl;
}
/*  _________________________________________________________________________*/
/*! GLApp::draw

@param none
@return none

This function is called once per frame to set the title of the window, 
update the rendering mode and renders all objects out onto the viewport
It uses OpenGL functions such as:
glClear to set the color buffer bit, glfwSetWindowTitle to set the window title
*/
void GLApp::draw() 
{
	// write window title with appropriate information using
	// glfwSetWindowTitle() ...

	std::stringstream title;
	title << std::fixed;
	title << std::setprecision(2);
	title << "Tutorial 5 | Ang Wei Ren | ";
	title << "Task " << tasknum;
	switch (tasknum)
	{
		case 0:
			title << ": Paint Color | ";
			break;
		case 1:
			title << ": Fixed-Size Checkerboard | ";
			break;
		case 2:
			title << ": Animated Checkerboard | ";
			break;
		case 3:
			title << ": Texture Mapping | ";
			break;
		case 4:
			title << ": Repeating | ";
			break;
		case 5:
			title << ": Mirroring | ";
			break;
		case 6:
			title << ": Clamping | ";
			break;
	}

	title << "Alpha Blend: ";
	alphablend == false ? title << "OFF | " : title << "ON | ";

	title << "Modulate: ";
	modulate == false ? title << "OFF" : title << "ON";

	//title << std::setprecision(2) << " | FPS " << int(GLHelper::fps*100)/100.0;

	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());

	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Part 4: Render each object in container GLApp::objects
	mdl.draw();
}

/*  _________________________________________________________________________*/
/*! GLApp::GLModel::draw

@param none
@return none

This function is called once per frame to bind the vao, textures, 
set the uniform variables and draw the elements
*/
void GLApp::GLModel::draw()
{

	// load shader program in use by this object
	shdr_pgm.Use();
	//glBindTexture(GL_TEXTURE_2D, texture);
	// bind VAO of this object's model
	glBindVertexArray(vaoid);

	// suppose texture object is to use texture image unit 6
	glBindTextureUnit(6, texobj);
	if (tasknum == 4)
	{
		// set what happens when sampler accesses textures outside [0, 1]
		glTextureParameteri(texobj, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(texobj, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	if (tasknum == 5)
	{
		// set what happens when sampler accesses textures outside [0, 1]
		glTextureParameteri(texobj, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTextureParameteri(texobj, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	}
	if (tasknum == 6)
	{
		// set what happens when sampler accesses textures outside [0, 1]
		glTextureParameteri(texobj, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(texobj, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	if (alphablend == true)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		std::cout << "Alpha blend\n";
	}
	else
	{
		glDisable(GL_BLEND);
	}
	// tell fragment shader sampler uTex2d will use texture image unit 6
	GLuint tex_loc = glGetUniformLocation(shdr_pgm.GetHandle(), "ourTexture");
	glUniform1i(tex_loc, 6); // For textures

	GLuint taskNum = glGetUniformLocation(shdr_pgm.GetHandle(), "taskNum");
	glUniform1i(taskNum, tasknum); // For task number

	GLuint modul = glGetUniformLocation(shdr_pgm.GetHandle(), "modul");
	glUniform1i(modul, modulate); // For modulate

	GLuint tilesize = glGetUniformLocation(shdr_pgm.GetHandle(), "uTileSize");
	glUniform1f(tilesize, TileSize); // For task 2

	// copy object's color to fragment shader uniform variable uColor
	//shdr_pgm.SetUniform("uColor", color);

	// copy object's model-to-NDC matrix to vertex shader's
	// uniform variable uModelToNDC
	//shdr_pgm.SetUniform("uModel_to_NDC", mdl_to_ndc_xform);

	// call glDrawElements with appropriate arguments
	glDrawElements(primitive_type, draw_cnt, GL_UNSIGNED_SHORT, NULL);


	// unbind VAO and unload shader program
	glBindVertexArray(0);
	shdr_pgm.UnUse();
}


/*  _________________________________________________________________________*/
/*! GLApp::setup_texobj

@param std::string pathname
Path of texture file to be opened

@return GLuint texture handle
Returns the handle of the texture

This function is called once on initialization to read and create
a texture to be used
*/
GLuint GLApp::setup_texobj(std::string pathname) 
{
	/*
	Ensure that folder "images" is copied to your $(SolutionDir) directory.
	$(SolutionDir)images will be the default location for all image files.
	Since we're not using an image loading library, we'll standardize file
	sizes to width and height of 256 texels with each texel representing a
	32-bit RGBA value.
	Names are more descriptive than handles & std::map container will allow
	programmers to conveniently reference specific texture objects by name.
	Although this exercise will use a single texture image stored in file
	$(SolutionDir)images/duck-rgba-256.tex, the function can be used in a
	general way to initialize as many texture objects as required.
	*/
	// remember all our images have width and height of 256 texels and
	// use 32-bit RGBA texel format
	GLuint width{ 256 }, height{ 256 }, bytes_per_texel{ 4 };
	// TODO: use the standard C++ library to open binary file
	// $(SolutionDir)images/duck-rgba-256.tex
	// and copy its contents to heap memory pointed by ptr_texels
	std::ifstream ifs{pathname, std::ios::in | std::ios::binary};
	if (!ifs)
	{
		std::cout << "ERROR: Unable to open image file: " << pathname << "\n";
		exit(EXIT_FAILURE);
	}
	char* ptr_texels = new char[width * height * bytes_per_texel];

	//const void* ptr_texels = new float[width * height];
	ifs.read(ptr_texels, width * height * bytes_per_texel);

	GLuint texobj_hdl;
	// define and initialize a handle to texture object that will
	// encapsulate two-dimensional textures
	glCreateTextures(GL_TEXTURE_2D, 1, &texobj_hdl);
	// allocate GPU storage for texture image data loaded from file
	glTextureStorage2D(texobj_hdl, 1, GL_RGBA8, width, height);
	// copy image data from client memory to GPU texture buffer memory
	glTextureSubImage2D(texobj_hdl, 0, 0, 0, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE, ptr_texels);
	// client memory not required since image is buffered in GPU memory
	delete[] ptr_texels;
	// nothing more to do - return handle to texture object
	return texobj_hdl;
}

/*  _________________________________________________________________________*/
/*! cleanup

@param none
@return none

This function is empty for now
*/
void GLApp::cleanup() {
  // empty for now
}

/*  _________________________________________________________________________*/
/*!  GLApp::setup_shdrpgm

@param std::string vtx_shdr
Name of vertex shader to be used

@param std::string frg_shdr
Name of fragment shader to be used

@return none

This function is called at the initialization of the scene to setup the different shader programs 
*/

void GLApp::GLModel::setup_shdrpgm(std::string vtx_shdr, std::string frg_shdr)
{
	std::vector<std::pair<GLenum, std::string>> shdr_files
	{
		std::make_pair(GL_VERTEX_SHADER, vtx_shdr),
		std::make_pair(GL_FRAGMENT_SHADER, frg_shdr)
	};
	shdr_pgm.CompileLinkValidate(shdr_files);
	if (GL_FALSE == shdr_pgm.IsLinked()) {
		std::cout << "Unable to compile/link/validate shader programs\n";
		std::cout << shdr_pgm.GetLog() << "\n";
		std::exit(EXIT_FAILURE);
	}
}
/*  _________________________________________________________________________*/
/*!  GLApp::GLModel::setup_vao

@param none

@return none

This function is called at the initialization to setup the VAO 
*/
void GLApp::GLModel::setup_vao()
{
	// Define a rect in normalized device coordinates (NCD)
	// that has one-fourth the area of the window

	
	drawer.pos = glm::vec2(-1.f, -1.f);
	drawer.color = glm::vec3(1.f, 0.f, 0.f);
	drawer.textcoord = glm::vec2(0.f, 0.f);
	drawvector.emplace_back(drawer);

	drawer.pos = glm::vec2(1.f, -1.f);
	drawer.color = glm::vec3(0.f, 1.f, 0.f);
	drawer.textcoord = glm::vec2(1.f, 0.f);
	drawvector.emplace_back(drawer);

	drawer.pos = glm::vec2(1.f, 1.f);
	drawer.color = glm::vec3(0.0f, 0.f, 1.f);
	drawer.textcoord = glm::vec2(1.f, 1.f);
	drawvector.emplace_back(drawer);

	drawer.pos = glm::vec2(-1.f, 1.f);
	drawer.color = glm::vec3(1.f, 0.f, 1.f);
	drawer.textcoord = glm::vec2(0.f, 1.f);
	drawvector.emplace_back(drawer);

	

	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);

	glNamedBufferStorage(vbo_hdl, drawvector.size() * sizeof(drawer), drawvector.data(), GL_DYNAMIC_STORAGE_BIT);
	//glNamedBufferSubData(vbo_hdl, 0, sizeof(drawvector), drawvector.data());

	//GLint max_vtx_attribs;
	//glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vtx_attribs);
	//std::cout << "Maximum vertex attributes: " << max_vtx_attribs << std::endl;

	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 0);  // For vertex pos array, we use attrib index 0 
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(drawer)); // buffer binding point 0, step is struct DrawVec
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0); // attrib index is 0
	glVertexArrayAttribBinding(vaoid, 0, 0);

	glEnableVertexArrayAttrib(vaoid, 1); // For vertex color array, we use attrib index 1 and buffer binding point 1
	glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl, sizeof(float) * 2, sizeof(drawer));
	glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0); // attrib index is 1
	glVertexArrayAttribBinding(vaoid, 1, 1);

	glEnableVertexArrayAttrib(vaoid, 2); // For vertex color array, we use attrib index 2 and buffer binding point 2
	glVertexArrayVertexBuffer(vaoid, 2, vbo_hdl, sizeof(float) * 5, sizeof(drawer));
	glVertexArrayAttribFormat(vaoid, 2, 2, GL_FLOAT, GL_FALSE, 0); // attrib index is 2
	glVertexArrayAttribBinding(vaoid, 2, 2);


	// represents indices of vertices that will define 2 triangles with
	// counterclockwise winding
	std::array<GLushort, 6> idx_vtx{
		0, 1, 2, // 1st triangle with counterclockwise winding is specified by
		// vertices in VBOs with indices 0, 1, 2
		2, 3, 0 // 2nd triangle with counterclockwise winding
	};

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl,
		sizeof(GLushort) * idx_vtx.size(),
		reinterpret_cast<GLvoid*>(idx_vtx.data()),
		GL_DYNAMIC_STORAGE_BIT);

	glVertexArrayElementBuffer(vaoid, ebo_hdl);
	glBindVertexArray(0);


	primitive_type = GL_TRIANGLE_STRIP;
	primitive_cnt = idx_vtx.size();
	draw_cnt = idx_vtx.size();
}

	

/*  _________________________________________________________________________*/
/*! GLObject::init

@param none
@return none

This function is currently empty
*/
void GLApp::GLObject::init()
{

}
