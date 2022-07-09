/*!
@file    glpbo.cpp
@author  a.weiren@digipen.edu
@date    29/06/2022

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and to extract function pointers
to OpenGL implementations.


*//*__________________________________________________________________________*/
#include "glpbo.h"
#include "glhelper.h"
#include "dpml.h"
#include <algorithm>
#include <iomanip>

GLsizei GLPbo::width, GLPbo::height;
GLsizei GLPbo::pixel_cnt, GLPbo::byte_cnt;
GLPbo::Color* GLPbo::ptr_to_pbo;
GLuint GLPbo::vaoid;
GLuint GLPbo::elem_cnt;
GLuint GLPbo::pboid;
GLuint GLPbo::texid;       
GLSLShader GLPbo::shdr_pgm;
GLPbo::Color GLPbo::clear_clr;
GLPbo::Model GLPbo::modelinfo;

/*  _________________________________________________________________________*/
/*! GLPbo::init

@param GLsizei w
Width of window

@param GLsizei h
Height of window

@return none

This function is called once on initialization to initialize values and call setup functions
*/
void GLPbo::init(GLsizei w, GLsizei h)
{

	
	GLPbo::width = w;
	GLPbo::height = h;
	GLPbo::pixel_cnt = GLPbo::width * GLPbo::height;
	GLPbo::byte_cnt = GLPbo::pixel_cnt * 4;

	set_clear_color(255, 255, 255, 255);


	glCreateTextures(GL_TEXTURE_2D, 1, &texid);

	glTextureStorage2D(texid, 1, GL_RGBA8, width, height);

	glCreateBuffers(1, &pboid);
	glNamedBufferStorage
	(	
		pboid,
		byte_cnt,
		nullptr,
		GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT
	);

	setup_quad_vao();

	setup_shdrpgm();
}

/*  _________________________________________________________________________*/
/*! GLPbo::set_clear_color

@param GLPbo::Color color
Input color to be set

@return none

This function is called to set the clear color
*/
void GLPbo::set_clear_color(GLPbo::Color color)
{
	GLPbo::clear_clr = color;
}

/*  _________________________________________________________________________*/
/*! GLPbo::set_clear_color

@param GLubyte r
Red color to be set

@param GLubyte g
Green color to be set

@param GLubyte b
Blue color to be set

@param GLubyte a
Alpha color to be set

@return none

This function is called to set the clear color
*/
void GLPbo::set_clear_color(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	GLPbo::clear_clr = { r,g,b,a };
}

/*  _________________________________________________________________________*/
/*! GLPbo::clear_color_buffer

@param none

@return none

This function is called to clear the color buffer
*/
void GLPbo::clear_color_buffer()
{
	std::fill_n(GLPbo::ptr_to_pbo, width * height, GLPbo::clear_clr);
}

/*  _________________________________________________________________________*/
/*! GLPbo::setup_quad_vao

@param none

@return none

This function is called to setup the quad vao
*/
void GLPbo::setup_quad_vao()
{
	std::vector<glm::vec2> pos_vtx
	{
		glm::vec2(1.f, -1.f), glm::vec2(1.f, 1.f),
		glm::vec2(-1.f, 1.f), glm::vec2(-1.f, -1.f)
	};
	std::vector<glm::vec2> tex_coord
	{
		glm::vec2(1.f, -1.f), glm::vec2(1.f, 1.f),
		glm::vec2(-1.f, 1.f), glm::vec2(-1.f, -1.f)
	};
	// compute and store endpoints for (slices+1) set of lines
	// for each x from -1 to 1
	// start endpoint is (x, -1) and end endpoint is (x, 1)
	// set up VAO as in GLApp::points_model
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size() + sizeof(glm::vec2) * tex_coord.size(),
		nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vbo_hdl, 0, sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
	glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec2) * tex_coord.size(), tex_coord.data());

	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 0); // Attrib 0
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(glm::vec2)); // buffer binding point 0
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0); // attrib index and size
	glVertexArrayAttribBinding(vaoid, 0, 0); // Attrib index, buffer binding point

	glEnableVertexArrayAttrib(vaoid, 1); // Attrib 1
	glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec2)); // buffer binding point 1
	glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0); // attrib index and size
	glVertexArrayAttribBinding(vaoid, 1, 1); // Attrib index, buffer binding point


	std::vector<GLushort> idx_vtx{
		0, 1, 2, // 1st triangle with counterclockwise winding is specified by
		// vertices in VBOs with indices 0, 1, 2
		2, 3, 0 // 2nd triangle with counterclockwise winding
	};

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(), reinterpret_cast<GLvoid*>(idx_vtx.data()), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);
	glBindVertexArray(0);

	GLPbo::elem_cnt = idx_vtx.size();
}

/*  _________________________________________________________________________*/
/*! GLPbo::setup_shdrpgm

@param none

@return none

This function is called to setup the shdrpgm
*/
void GLPbo::setup_shdrpgm()
{
	std::string vtx_shdr
	{
		"#version 450 core\n"
		"layout(location = 0) in vec2 aVertexPosition;\n"
		"layout(location = 1) in vec2 aTextureCoord;\n"
		"layout(location = 0) out vec2 vTextureCoord;\n"
		"void main() {\n"
		"	gl_Position = vec4(aVertexPosition, 0.0, 1.0);\n"
		"	vTextureCoord = aTextureCoord;\n"
		"}\n"
	};
	std::string frg_shdr
	{
		"#version 450 core\n"
		"layout (location=0) in vec2 vTexturecoord;\n"
		"layout (location=0) out vec4 fFragColor;\n"
		"uniform sampler2D ourTexture;\n"
		"void main() {\n"
		"	fFragColor = texture(ourTexture, vTexturecoord);\n"
		"}\n"
	};	
	std::vector<std::pair<GLenum, std::string>> shdr_files;
	shdr_files.emplace_back(std::make_pair(GL_VERTEX_SHADER, vtx_shdr));
	shdr_files.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, frg_shdr));
	for (auto& elem : shdr_files) {
		if (GL_FALSE == shdr_pgm.CompileShaderFromString(elem.first, elem.second)) {
			std::cout << "Unable to compile/link/validate shader programs\n";
			std::cout << shdr_pgm.GetLog() << "\n";
			std::exit(EXIT_FAILURE);
		}
	}
	if (GL_FALSE == shdr_pgm.Link()) {
		std::cout << "Unable to compile/link/validate shader programs\n";
	}
	if (GL_FALSE == shdr_pgm.Validate()) {
		std::cout << "Unable to compile/link/validate shader programs\n";
	}
	shdr_pgm.PrintActiveAttribs();
	shdr_pgm.PrintActiveUniforms();
}

/*  _________________________________________________________________________*/
/*! GLPbo::emulate

@param none

@return none

This function is called to emulate the program
*/
void GLPbo::emulate()
{
	double val1 = cos(glfwGetTime()) * (255.0 / 2.0);
	double val2 = sin(glfwGetTime()) * (255.0 / 2.0);
	val1 += 255.0/2.0;
	val2 += 255.0 / 2.0;
	set_clear_color(GLubyte(val2), GLubyte(val1), GLubyte(val2));
	
	GLPbo::ptr_to_pbo = reinterpret_cast<GLPbo::Color*>(glMapNamedBuffer(pboid, GL_WRITE_ONLY));
	clear_color_buffer();
	glUnmapNamedBuffer(pboid);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboid);
	glTextureSubImage2D(texid, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

}

/*  _________________________________________________________________________*/
/*! GLPbo::draw_fullwindow_quad

@param none

@return none

This function is called to draw the window
*/
void GLPbo::draw_fullwindow_quad()
{
	shdr_pgm.Use();
	glBindVertexArray(vaoid);
	glBindTextureUnit(6, texid);
	GLuint tex_loc = glGetUniformLocation(shdr_pgm.GetHandle(), "ourTexture");
	glUniform1i(tex_loc, 6);
	glDrawElements(GL_TRIANGLE_STRIP, elem_cnt, GL_UNSIGNED_SHORT, nullptr);
	glBindVertexArray(0);
	shdr_pgm.UnUse();

	std::stringstream title;
	title << std::fixed;
	title << std::setprecision(2);
	title << "Tutorial 6 | Ang Wei Ren ";
	title << " | PBO Size: " << width << " x " << height;
	title << std::setprecision(2) << " | FPS " << int(GLHelper::fps * 100) / 100.0;

	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());
}

/*  _________________________________________________________________________*/
/*! GLPbo::cleanup

@param none

@return none

This function is called to cleanup the created objects	
*/
void GLPbo::cleanup()
{
	glDeleteVertexArrays(1, &vaoid);
	glDeleteBuffers(1, &pboid);
	glDeleteTextures(1, &texid);
}