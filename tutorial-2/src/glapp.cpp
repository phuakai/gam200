/*!
@file    glapp.cpp
@author  pghali@digipen.edu, a.weiren@digipen.edu
@date    28/05/2022

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and to extract function pointers
to OpenGL implementations.

BONUS TASK ADDED
Rendering via Primitive Restart method is added, to enable, comment lines 486-487 and uncomment line 485

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glapp.h>
#include <iostream>
#include <array>
#include <time.h>
#include <random>
#include <sstream>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
#define M_PI 3.1415926535897932384626433832795 // Pi macro def

std::vector<GLApp::GLViewport> GLApp::vps; // define vps

std::vector<GLApp::GLModel> GLApp::models; // define models

/*  _________________________________________________________________________*/
/*! init

@param none
@return none

This function is called once on initialization to initialize values and call setup functions
It uses OpenGL functions such as:
glClearColor and glViewport to initialize the app
*/

void GLApp::init() {
	srand(unsigned int(time(NULL))); // Seeding random with current time
	// Part 1
	glClearColor(1.f, 1.f, 1.f, 1.f); // clear colorbuffer with RGBA value in glClearColor

	// Part 2
	GLint w = GLHelper::width, h = GLHelper::height;
	glViewport(w / 2, h / 4, w / 2, h / 4);

	vps.emplace_back(0, h / 2, w / 2, h / 2); //Top Left
	vps.emplace_back(w / 2, h / 2, w / 2, h / 2); //Top Right
	vps.emplace_back(0, 0, w / 2, h / 2); //Bottom Left
	vps.emplace_back(w / 2, 0, w / 2, h / 2); //Bottom Right

	// Part 3: create different geometries and insert them into
	// repository container GLApp::models ...
	GLApp::models.emplace_back(GLApp::points_model(20, 20,
		"../shaders/my-tutorial-2.vert",
		"../shaders/my-tutorial-2.frag"));

	GLApp::models.emplace_back(GLApp::lines_model(40, 40,
		"../shaders/my-tutorial-2.vert",
		"../shaders/my-tutorial-2.frag"));

	GLApp::models.emplace_back(GLApp::trifans_model(50,
		"../shaders/my-tutorial-2.vert",
		"../shaders/my-tutorial-2.frag"));

	GLApp::models.emplace_back(GLApp::tristrip_model(10, 15,
		"../shaders/my-tutorial-2.vert",
		"../shaders/my-tutorial-2.frag"));
}


/*  _________________________________________________________________________*/
/*! update

@param none
@return none


For now this function is empty

*/
void GLApp::update() {
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

/*  _________________________________________________________________________*/
/*! GLApp::draw

@param none
@return none

This function is called once per frame to draw objects in different viewports and set the title of the window
It uses OpenGL functions such as:
glClear to set the color buffer bit, glfwSetWindowTitle to set the window title and glViewport to create different viewports 
*/
void GLApp::draw() {

	// Part 1: clear color buffer using OpenGL function glClear
	glClear(GL_COLOR_BUFFER_BIT);

	//Setting of window title
	std::stringstream title;
	title << "Tutorial 2 | Ang Wei Ren | POINTS: "  << models[0].primitive_cnt << ", " << models[0].draw_cnt;
	title << " | LINES: " << models[1].primitive_cnt << ", " << models[1].draw_cnt;
	title << " | FAN: " << models[2].primitive_cnt << ", " << models[2].draw_cnt;
	title << " | STRIP: " << models[3].primitive_cnt << ", " << models[3].draw_cnt;
	title << " | " << int(GLHelper::fps*100)/100.0 << std::endl;

	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());
	// Part 2: Render distinct geometry to four viewports:
	// First, use OpenGL function glViewport to specify appropriate
	// viewport transform that will render to a specific portion of
	// display screen
	// Next, use member function GLModel::draw to render geometry
	// Repeat previous two steps for remaining three viewports

	// render points in top-left viewport
	glViewport(vps[0].x, vps[0].y, vps[0].width, vps[0].height);
	models[0].draw();

	// render line segments in top-right viewport
	glViewport(vps[1].x, vps[1].y, vps[1].width, vps[1].height);
	models[1].draw();

	// render line segments in bottom-left viewport
	glViewport(vps[2].x, vps[2].y, vps[2].width, vps[2].height);
	models[2].draw();

	// render line segments in bottom-right viewport
	glViewport(vps[3].x, vps[3].y, vps[3].width, vps[3].height);
	models[3].draw();
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
/*! setup_shdrpgm

@param std::string vtx_shdr
Vertex shader file path

@param std::string frg_shdr
Fragment shader file path

@return none

This function is called once on initialization to compile/link/validate the shader programs
*/

void GLApp::GLModel::setup_shdrpgm(std::string vtx_shdr,
	std::string frg_shdr) {
	std::vector<std::pair<GLenum, std::string>> shdr_files;
	shdr_files.emplace_back(std::make_pair(GL_VERTEX_SHADER, vtx_shdr));
	shdr_files.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, frg_shdr));
	shdr_pgm.CompileLinkValidate(shdr_files);
	if (GL_FALSE == shdr_pgm.IsLinked()) {
		std::cout << "Unable to compile/link/validate shader programs\n";
		std::cout << shdr_pgm.GetLog() << "\n";
		std::exit(EXIT_FAILURE);
	}
}


/*  _________________________________________________________________________*/
/*! points_model

@param int slices
Slices/Rows of points to be drawn

@param int stacks
Stacks/Columns of points to be drawn

@param std::string vtx_shadr
Vertex shader file path

@param std::string frg_shadr
Fragment shader file path

@return GLModel

 This function generates (slices+1) * (stacks+1) count of points 
 between coordinates (-1, 0) and (1, 0) of the first viewport
*/
GLApp::GLModel GLApp::points_model(int slices, int stacks,
	std::string vtx_shdr,
	std::string frg_shdr) {
	// Compute and store a "stack" of uniformly "sliced" points
	// such that there are (stack+1) number of point sequences
	// with each point sequence consisting of (slice+1) number of
	// points ranging from (-1, y) to (1, y).

	std::vector<glm::vec2> pos_vtx;

	float ysliceintervals = 2.f / stacks;
	float yinterval = -1.f;
	for (int y = 0; y < stacks+1; y++)
	{
		float xsliceintervals = 2.f / slices;
		float xinterval = -1.f;
		for (int x = 0; x < slices+1; x++)
		{
			pos_vtx.emplace_back(glm::vec2(xinterval, yinterval));
			xinterval += xsliceintervals;
		}
		yinterval += ysliceintervals;
	}

	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(),
		pos_vtx.data(), GL_DYNAMIC_STORAGE_BIT);
	GLuint vaoid;
	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 0);
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(glm::vec2));
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vaoid, 0, 0);
	glBindVertexArray(0);

	GLApp::GLModel mdl;
	mdl.vaoid = vaoid;
	mdl.primitive_type = GL_POINTS;
	mdl.setup_shdrpgm(vtx_shdr, frg_shdr);
	mdl.draw_cnt = pos_vtx.size(); // number of vertices
	mdl.primitive_cnt = mdl.draw_cnt; // number of primitives (unused)
	return mdl;

}

/*  _________________________________________________________________________*/
/*! lines_model

@param int slices
Slices/Rows of points for the lines to be drawn

@param int stacks
Stacks/Columns of points for the lines to be drawn

@param std::string vtx_shadr
Vertex shader file path

@param std::string frg_shadr
Fragment shader file path
@return GLModel

 This function generates (slices+1) * (stacks+1) set of lines for
 line segments between coordinates (-1, 0) and (1, 0)
*/
GLApp::GLModel GLApp::lines_model(int slices, int stacks,
	std::string vtx_shdr,
	std::string frg_shdr) {
	std::vector<glm::vec2> pos_vtx;
	float xsliceintervals = 2.f / slices;
	float xinterval = -1.f;
	for (int x = 0; x < slices + 1; x++)
	{
		pos_vtx.emplace_back(glm::vec2(xinterval, -1.f));
		pos_vtx.emplace_back(glm::vec2(xinterval, 1.f));
		xinterval += xsliceintervals;
	}
	float ysliceintervals = 2.f / stacks;
	float yinterval = -1.f;
	for (int y = 0; y < stacks + 1; y++)
	{
		pos_vtx.emplace_back(glm::vec2(-1.f, yinterval));
		pos_vtx.emplace_back(glm::vec2(1.f, yinterval));
		yinterval += ysliceintervals;
	}


	// compute and store endpoints for (slices+1) set of lines
	// for each x from -1 to 1
	// start endpoint is (x, -1) and end endpoint is (x, 1)
	// set up VAO as in GLApp::points_model
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(),
		pos_vtx.data(), GL_DYNAMIC_STORAGE_BIT);
	GLuint vao_hdl;
	glCreateVertexArrays(1, &vao_hdl);
	glEnableVertexArrayAttrib(vao_hdl, 0);
	glVertexArrayVertexBuffer(vao_hdl, 0, vbo_hdl, 0, sizeof(glm::vec2));
	glVertexArrayAttribFormat(vao_hdl, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribBinding(vao_hdl, 0, 0);
	glBindVertexArray(0);

	GLApp::GLModel mdl;
	mdl.vaoid = vao_hdl; // set up VAO same as in GLApp::points_model
	mdl.primitive_type = GL_LINES;
	mdl.setup_shdrpgm(vtx_shdr, frg_shdr);
	mdl.draw_cnt = 2 * (slices + 1) + 2 * (stacks + 1); // number of vertices
	mdl.primitive_cnt = mdl.draw_cnt / 2; // number of primitives (not used)
	return mdl;
}

/*  _________________________________________________________________________*/
/*! trifans_model

@param int slices
Number of triangles to be generated as part of the triangle fan

@param std::string vtx_shadr
Vertex shader file path

@param std::string frg_shadr
Fragment shader file path

@return GLModel

 This function generates (slices+2) count of vertices for
 triangle fan of a circle with unit radius and centered at (0, 0)
*/
GLApp::GLModel GLApp::trifans_model(int slices, std::string vtx_shdr,
	std::string frg_shdr) {
	// Step 1: Generate the (slices+2) count of vertices required to
	// render a triangle fan parameterization of a circle with unit
	// radius and centered at (0, 0)

	// Step 2: In addition to vertex position coordinates, compute
	// (slices+2) count of vertex color coordinates.
	// Each RGB component must be randomly computed.
	std::vector<glm::vec2> pos_vtx;
	std::vector<glm::vec3> clr_vtx;
	float xsliceintervals = 360.f/slices;
	float xinterval = 0.f;

	pos_vtx.emplace_back(glm::vec2(0.f, 0.f)); // Pushback for pivot vertex
	clr_vtx.emplace_back(glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f))); // Pivot vertex color

	for (int slicecnt = 0; slicecnt < slices + 1; slicecnt++)
	{
		pos_vtx.emplace_back(glm::vec2(cos(xinterval * M_PI / 180.0), sin(xinterval * M_PI / 180.0)));
		clr_vtx.emplace_back(glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f)));
		xinterval += xsliceintervals;
		
	}
	
	// Step 3: Generate a VAO handle to encapsulate the VBO(s) and
	// state of this triangle mesh
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size() + sizeof(glm::vec3) * clr_vtx.size(),
		nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vbo_hdl, 0, sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
	glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec3) * clr_vtx.size(), clr_vtx.data());
	GLuint vao_hdl;
	glCreateVertexArrays(1, &vao_hdl);
	glEnableVertexArrayAttrib(vao_hdl, 0); // Attrib 0
	glVertexArrayVertexBuffer(vao_hdl, 0, vbo_hdl, 0, sizeof(glm::vec2)); // buffer binding point 0
	glVertexArrayAttribFormat(vao_hdl, 0, 2, GL_FLOAT, GL_FALSE, 0); // attrib index and size
	glVertexArrayAttribBinding(vao_hdl, 0, 0); // Attrib index, buffer binding point

	glEnableVertexArrayAttrib(vao_hdl, 1); // Attrib 1
	glVertexArrayVertexBuffer(vao_hdl, 1, vbo_hdl, sizeof(glm::vec2)*pos_vtx.size(), sizeof(glm::vec3)); // buffer binding point 1
	glVertexArrayAttribFormat(vao_hdl, 1, 3, GL_FLOAT, GL_FALSE, 0); // attrib index and size
	glVertexArrayAttribBinding(vao_hdl, 1, 1); // Attrib index, buffer binding point
	
	std::vector<GLushort> idx_vtx{};

	idx_vtx.emplace_back(0); // pivot
	for (int i = 1; i <= slices; i++)
	{
		if (i == 0)
		{
			idx_vtx.emplace_back(i++);
		}
		idx_vtx.emplace_back(i);
		if (i == slices)
		{
			idx_vtx.emplace_back(1);
		}
	}

	GLApp::GLModel mdl;
	mdl.draw_cnt = idx_vtx.size();
	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * mdl.draw_cnt, reinterpret_cast<GLvoid*>(idx_vtx.data()), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vao_hdl, ebo_hdl);

	glBindVertexArray(0);
	// Step 4: Return an appropriately initialized instance of GLApp::GLModel
	mdl.vaoid = vao_hdl; // set up VAO same as in GLApp::points_model
	mdl.primitive_type = GL_TRIANGLE_FAN;
	mdl.setup_shdrpgm(vtx_shdr, frg_shdr);
	mdl.primitive_cnt = mdl.draw_cnt - 2; // number of primitives (not used)
	return mdl;
}

/*  _________________________________________________________________________*/
/*! tristrip_model

@param int slices
Slices/Rows of points as vertices to draw a rectangle as part of tristrip

@param int stacks
Stacks/Columns of points as vertices to draw a rectangle as part of tristrip

@param std::string vtx_shadr
Vertex shader file path

@param std::string frg_shadr
Fragment shader file path
@return GLModel

 This function generates (slices+1) * (stacks+1) count of vertices for
 triangle strip parameterization 
*/
GLApp::GLModel GLApp::tristrip_model(int slices, int stacks,
	std::string vtx_shdr, std::string frg_shdr)
{
	
	std::vector<glm::vec2> pos_vtx;
	std::vector<glm::vec3> clr_vtx;
	
	glEnable(GL_PRIMITIVE_RESTART); // For bonus task
	glPrimitiveRestartIndex(65535); // For bonus task

	float stackintervals = 2.f / stacks;
	float stackint = -1.f;
	for (int stackcnt = 0; stackcnt < stacks + 1; stackcnt++)
	{
		float sliceintervals = 2.f / slices;
		float sliceint = -1.f;
		for (int slicecnt = 0; slicecnt < slices + 1; slicecnt++)
		{
			pos_vtx.emplace_back(glm::vec2(sliceint, stackint));
			clr_vtx.emplace_back(glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f)));
			sliceint += sliceintervals;
		}
		stackint += stackintervals;
	}
	
	// compute and store endpoints for (slices+1) set of lines
	// for each x from -1 to 1
	// start endpoint is (x, -1) and end endpoint is (x, 1)
	// set up VAO as in GLApp::points_model
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size() + sizeof(glm::vec3) * clr_vtx.size(),
		nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vbo_hdl, 0, sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
	glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec3) * clr_vtx.size(), clr_vtx.data());
	GLuint vao_hdl;
	glCreateVertexArrays(1, &vao_hdl);
	glEnableVertexArrayAttrib(vao_hdl, 0); // Attrib 0
	glVertexArrayVertexBuffer(vao_hdl, 0, vbo_hdl, 0, sizeof(glm::vec2)); // buffer binding point 0
	glVertexArrayAttribFormat(vao_hdl, 0, 2, GL_FLOAT, GL_FALSE, 0); // attrib index and size
	glVertexArrayAttribBinding(vao_hdl, 0, 0); // Attrib index, buffer binding point

	glEnableVertexArrayAttrib(vao_hdl, 1); // Attrib 1
	glVertexArrayVertexBuffer(vao_hdl, 1, vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec3)); // buffer binding point 1
	glVertexArrayAttribFormat(vao_hdl, 1, 3, GL_FLOAT, GL_FALSE, 0); // attrib index and size
	glVertexArrayAttribBinding(vao_hdl, 1, 1); // Attrib index, buffer binding point


	std::vector<GLushort> idx_vtx{};

	for (int y = 0; y <= stacks; y++)
	{
		for (int x = 0; x <= slices; x++)
		{
			if (y + 1 <= stacks)
			{
				idx_vtx.emplace_back((y + 1) * (slices + 1) + x); 
				idx_vtx.emplace_back(y * (slices + 1) + x); 
				if (x == slices && y + 1 != stacks)
				{
					//idx_vtx.emplace_back(65535); // UNCOMMENT FOR PRIMITIVE RESTART
					idx_vtx.emplace_back(y * (slices + 1) + x); // COMMENT FOR PRIMITIVE RESTART
					idx_vtx.emplace_back((y + 1) * (slices + 1) + x + 1); // COMMENT FOR PRIMITIVE RESTART
				}
			}
		}
	}
	GLApp::GLModel mdl;
	mdl.draw_cnt = idx_vtx.size();

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * mdl.draw_cnt, reinterpret_cast<GLvoid*>(idx_vtx.data()), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vao_hdl, ebo_hdl);

	glBindVertexArray(0);

	mdl.vaoid = vao_hdl; // set up VAO same as in GLApp::points_model
	mdl.primitive_type = GL_TRIANGLE_STRIP;
	mdl.setup_shdrpgm(vtx_shdr, frg_shdr);
	mdl.primitive_cnt = slices * stacks * 2; // number of primitives (not used)
	return mdl;
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
	switch (primitive_type) {
	case GL_POINTS:
		glPointSize(10.f);
		glVertexAttrib3f(1, 1.f, 0.f, 0.f); 
		glDrawArrays(primitive_type, 0, draw_cnt);
		glPointSize(1.f);
		break;
	case GL_LINES:
		glLineWidth(3.f);
		glVertexAttrib3f(1, 0.f, 0.f, 1.f); 
		glDrawArrays(primitive_type, 0, draw_cnt);
		glLineWidth(1.f);
		break;
	case GL_TRIANGLE_FAN:
		glLineWidth(3.f);
		glDrawElements(primitive_type, draw_cnt, GL_UNSIGNED_SHORT, NULL);
		glLineWidth(1.f);
		break;
	case GL_TRIANGLE_STRIP:
		glLineWidth(3.f);
		glDrawElements(primitive_type, draw_cnt, GL_UNSIGNED_SHORT, NULL);
		glLineWidth(1.f);
		break;
	}

	// after completing the rendering, we tell the driver that VAO
	// vaoid and current shader program are no longer current
	glBindVertexArray(0);
	shdr_pgm.UnUse();

}