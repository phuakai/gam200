/*!
@file    glapp.cpp
@author  pghali@digipen.edu, a.weiren@digipen.edu
@date    10/11/2016

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

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
#define ANIMTIMER 4.0 // Animation timer in seconds
bool coloranim; // Color animation boolean
double animtimer[2]; // Current animation timer
//double oldtime; // Old timer to calculate delta time
//double deltatime; // Obsolete


GLApp::GLModel GLApp::mdl; // define mdl

/*  _________________________________________________________________________*/
/*! init

@param none
@return none

This function is called once on initialization to initialize values and call setup functions
It uses OpenGL functions such as:
glClearColor and glViewport to initialize the app
*/

void GLApp::init() {
	// Part 1
	glClearColor(1.f, 0.f, 0.f, 1.f); // clear colorbuffer with RGBA value in glClearColor

	// Part 2
	GLint w = GLHelper::width, h = GLHelper::height;
	glViewport(0, 0, w, h); 


	mdl.setup_vao();
	mdl.setup_shdrpgm();

	coloranim = false;

	//oldtime = 0.0;
	animtimer[0] = 0.0;
	animtimer[1] = ANIMTIMER;
}

/*  _________________________________________________________________________*/
/*! update

@param none
@return none

This function is called once per frame to interpolate the colors of the background
It uses OpenGL functions such as:
glClearColor to set the color of the background
*/
void GLApp::update() {

	//double currtime = glfwGetTime();
	//deltatime = currtime - oldtime;
	//oldtime = currtime;
	//std::cout << deltatime << std::endl;

	if (coloranim)
	{
		animtimer[0] += GLHelper::delta_time;
		animtimer[1] -= GLHelper::delta_time;
		if (animtimer[0] > ANIMTIMER)
		{
			coloranim = false;
		}
	}
	else
	{
		animtimer[0] -= GLHelper::delta_time;
		animtimer[1] += GLHelper::delta_time;
		if (animtimer[0] <= 0)
		{
			coloranim = true;
		}
	}
	glClearColor(float(animtimer[0] / ANIMTIMER), 0.f, float(animtimer[1] / ANIMTIMER), 1.f);
}

/*  _________________________________________________________________________*/
/*! GLApp::draw

@param none
@return none

This function is called once per frame to draw objects and set the title of the window
It uses OpenGL functions such as:
glClear to set the color buffer bit and glfwSetWindowTitle to set the window title
*/
void GLApp::draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	// now, render rectangular model from NDC coordinates to viewport
	char title[100];
	sprintf_s(title, 100, "Tutorial 1 | Ang Wei Ren | %.2lf", GLHelper::fps);
	glfwSetWindowTitle(GLHelper::ptr_window, title);
	mdl.draw();

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
/*! setup_vao

@param none
@return none

This function is called once on initialization to setup the vertex array objects
required to draw objects on the screen
*/
void GLApp::GLModel::setup_vao()
{
	// Define a rect in normalized device coordinates (NCD)
	// that has one-fourth the area of the window
	std::array<glm::vec2, 4> pos_vtx 
	{
		glm::vec2(0.5f, -0.5f), glm::vec2(0.5f, 0.5f),
		glm::vec2(-0.5f, 0.5f), glm::vec2(-0.5f, -0.5f)
	};

	std::array<glm::vec3, 4> clr_vtx
	{
		glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
		glm::vec3(0.f, 0.f, 1.f), glm::vec3(1.f, 1.f, 1.f)
	};



	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);

	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size() + sizeof(glm::vec3) * clr_vtx.size(), nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vbo_hdl, 0, sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
	glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec3) * clr_vtx.size(), clr_vtx.data());

	//GLint max_vtx_attribs;
	//glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vtx_attribs);
	//std::cout << "Maximum vertex attributes: " << max_vtx_attribs << std::endl;

	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 8);  // For vertex pos array, we use attrib index 8 and buffer binding point 3
	glVertexArrayVertexBuffer(vaoid, 3, vbo_hdl, 0, sizeof(glm::vec2));
	glVertexArrayAttribFormat(vaoid, 8, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 8, 3);

	glEnableVertexArrayAttrib(vaoid, 9); // For vertex color array, we use attrib index 9 and buffer binding point 4
	glVertexArrayVertexBuffer(vaoid, 4, vbo_hdl, sizeof(glm::vec2)*pos_vtx.size(), sizeof(glm::vec3));
	glVertexArrayAttribFormat(vaoid, 9, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 9, 4);

	primitive_type = GL_TRIANGLES;

	// represents indices of vertices that will define 2 triangles with
	// counterclockwise winding
	std::array<GLushort, 6> idx_vtx{
		0, 1, 2, // 1st triangle with counterclockwise winding is specified by
		// vertices in VBOs with indices 0, 1, 2
		2, 3, 0 // 2nd triangle with counterclockwise winding
	};
	idx_elem_cnt = idx_vtx.size();

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl,
		sizeof(GLushort) * idx_elem_cnt,
		reinterpret_cast<GLvoid*>(idx_vtx.data()),
		GL_DYNAMIC_STORAGE_BIT);

	glVertexArrayElementBuffer(vaoid, ebo_hdl);
	glBindVertexArray(0);
}

/*  _________________________________________________________________________*/
/*! setup_shdrpgm

@param none
@return none

This function is called once on initialization to compile/link/validate the shader programs
*/
void GLApp::GLModel::setup_shdrpgm() {
	std::vector<std::pair<GLenum, std::string>> shdr_files;
	shdr_files.emplace_back(std::make_pair(
		GL_VERTEX_SHADER,
		"../shaders/my-tutorial-1.vert"));
	shdr_files.emplace_back(std::make_pair(
		GL_FRAGMENT_SHADER,
		"../shaders/my-tutorial-1.frag"));
	shdr_pgm.CompileLinkValidate(shdr_files);
	if (GL_FALSE == shdr_pgm.IsLinked()) {
		std::cout << "Unable to compile/link/validate shader programs" << "\n";
		std::cout << shdr_pgm.GetLog() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

/*  _________________________________________________________________________*/
/*! GLApp::GLModel::draw

@param none
@return none

This function is called once per frame to draw the objects
It uses OpenGL functions such as:
glBindVertexArray and glDrawElements to draw the objects
*/
void GLApp::GLModel::draw() {
	// there are many shader programs initialized - here we're saying
	// which specific shader program should be used to render geometry
	shdr_pgm.Use();
	// there are many models, each with their own initialized VAO object
	// here, we're saying which VAO's state should be used to set up pipe
	glBindVertexArray(vaoid);
	// here, we're saying what primitive is to be rendered and how many
	// such primitives exist.
	// the graphics driver knows where to get the indices because the VAO
	// containing this state information has been made current ...
	glDrawElements(primitive_type, idx_elem_cnt, GL_UNSIGNED_SHORT, NULL);
	// after completing the rendering, we tell the driver that VAO
	// vaoid and current shader program are no longer current
	glBindVertexArray(0);
	shdr_pgm.UnUse();
}