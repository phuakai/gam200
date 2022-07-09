/*!
@file    glapp.cpp
@author  pghali@digipen.edu, a.weiren@digipen.edu
@date    03/06/2022

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

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
#define M_PI 3.1415926535897932384626433832795 // Pi macro def
#define WorldRange 5000.0 // Worldrange

std::vector<GLApp::GLViewport> GLApp::vps; // define vps

std::vector<GLSLShader> GLApp::shdrpgms; // define shaders

std::vector<GLApp::GLModel> GLApp::models; // define models

std::list<GLApp::GLObject> GLApp::objects; // define objects

 GLboolean GLApp::reduc;

 int GLApp::objcounter;
 int GLApp::boxcounter;
 int GLApp::mysterycounter;

int polymode;

/*  _________________________________________________________________________*/
/*! GLApp::init

@param none
@return none

This function is called once on initialization to initialize values and call setup functions
It uses OpenGL functions such as:
glClearColor and glViewport to initialize the app
*/

void GLApp::init() {
	reduc = false;
	objcounter = 0;
	boxcounter = 0;
	mysterycounter = 0;
	polymode = 0;
	srand(unsigned int(time(NULL))); // Seeding random with current time
	// Part 1: initialize OpenGL state ...
	glClearColor(1.f, 1.f, 1.f, 1.f); // clear colorbuffer with RGBA value in glClearColor

	// Part 2: use the entire window as viewport ...
	GLint w = GLHelper::width, h = GLHelper::height;
	glViewport(0, 0, w, h);

	GLApp::VPSS shdr_file_names{ // vertex & fragment shader files
		std::make_pair<std::string, std::string>
		("../shaders/my-tutorial-3.vert", "../shaders/my-tutorial-3.frag")
	};

	// Part 3: create as many shared shader programs as required
	// handles to shader programs must be contained in GLApp::shdrpgms
	
	// create shader program from shader files in vector shdr_file_names
	// and insert each shader program into container
	// std::vector<GLSLShader> GLApp::shdrpgms;
	GLApp::init_shdrpgms_cont(shdr_file_names);

	// Part 4: initialize as many geometric models as required
	// these geometric models must be contained in GLApp::models

	GLApp::init_models_cont();
	// We don't need to add any objects to container GLApp::objects
	// since the simulation begins with no objects being displayed

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
	// Part 1: Update polygon rasterization mode ...
	// Check if key 'P' is pressed
	// If pressed, update polygon rasterization mode

	if (polymode == 0)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (polymode == 1)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else if (polymode == 2)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	else if (polymode == 3)
	{
		polymode = 0;
	}
	if (GLHelper::keystateP == GL_TRUE)
	{
		GLHelper::keystateP = GL_FALSE;
		polymode++;
	}
	// Part 2: Spawn or kill objects ...
	// Check if left mouse button is pressed
	// If maximum object limit is not reached, spawn new object(s)
	// Otherwise, kill oldest objects
	if (GLHelper::mousestateLeft == GL_TRUE)
	{
		GLHelper::mousestateLeft = GL_FALSE;
		int cursize = objects.size();
		if (cursize <= 32767 && reduc == false)
		{
			for (int i = 0; i < std::max(cursize, 1); i++)
			{
				GLApp::GLObject model;
				model.init();
				objects.push_back(model);
				objcounter++;

			}
		}
		else
		{
			
			reduc = true;
			for (int i = 0; i < std::max(cursize/2, 1); i++)
			{	
				if ((objects.front()).mdl_ref == 0)
				{
					boxcounter--;
				}
				else if ((objects.front()).mdl_ref == 1)
				{
					mysterycounter--;
				}
				objects.pop_front();
				objcounter--;
			}
			if (objcounter == 1)
			{
				reduc = false;
			}
		}
	}
	// Part 3:
	// for each object in container GLApp::objects
	// Update object's orientation
	// A more elaborate implementation would animate the object's movement
	// A much more elaborate implementation would animate the object's size
	// Using updated attributes, compute world-to-ndc transformation matrix
	for (GLApp::GLObject &i : objects)
	{
		i.update(GLHelper::delta_time);
	}
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
	// Part 1: write window title with the following (see sample executable):
	// tutorial name - this should be "Tutorial 3"
	// object count - how many objects are being displayed?
	// how many of these objects are boxes?
	// and, how many of these objects are the mystery model?
	// current fps
	// separate each piece of information using " | "
	// see sample executable for example ...
	std::stringstream title;
	title << "Tutorial 3 | Ang Wei Ren | Obj: "  << objcounter;
	title << " | Box: " << boxcounter;
	title << " | Mystery: " << mysterycounter;
	title << " | " << int(GLHelper::fps*100)/100.0 << std::endl;

	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());

	// Part 2: Clear back buffer of color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Part 3: Special rendering modes
	// Use status of flag GLHelper::keystateP to set appropriate polygon
	// rasterization mode using glPolygonMode
	// if rendering GL_POINT, control diameter of rasterized points
	// using glPointSize
	// if rendering GL_LINE, control width of rasterized lines
	// using glLineWidth

	if (polymode == 1)
	{
		glLineWidth(3.f);
	}
	else if (polymode == 2)
	{
		glPointSize(10.f);
	}

	// Part 4: Render each object in container GLApp::objects
	for (auto const& i : GLApp::objects) {
		i.draw(); // call member function GLObject::draw()
	}
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
/*! init_models_cont

@param none
@return none

This function emplaces back the available models into the model vector
*/
void GLApp::init_models_cont() {
	GLApp::models.emplace_back(GLApp::box_model());
	GLApp::models.emplace_back(GLApp::mystery_model());
}

/*  _________________________________________________________________________*/
/*! init_shdrpgms_cont

@param GLApp::VPSS vpss
@return none

This function creates shader programs for each pair of shader files in vector
VPSS and inserts each shader program into container std::vector<GLSLShader> GLApp::shdrpgms
*/
void GLApp::init_shdrpgms_cont(GLApp::VPSS const& vpss) {
	for (auto const& x : vpss) {
		std::vector<std::pair<GLenum, std::string>> shdr_files;
		shdr_files.emplace_back(std::make_pair(GL_VERTEX_SHADER, x.first));
		shdr_files.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, x.second));
		GLSLShader shdr_pgm;
		shdr_pgm.CompileLinkValidate(shdr_files);
		// insert shader program into container
		GLApp::shdrpgms.emplace_back(shdr_pgm);
	}
}

/*  _________________________________________________________________________*/
/*! box_model

@return GLModel

 This function generates a box of unit length and width
*/

GLApp::GLModel GLApp::box_model()
{
	std::vector<glm::vec2> pos_vtx
	{
		glm::vec2(0.5f, -0.5f), glm::vec2(0.5f, 0.5f),
		glm::vec2(-0.5f, 0.5f), glm::vec2(-0.5f, -0.5f)
	};
	std::vector<glm::vec3> clr_vtx
	{
		glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f)), glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f)),
		glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f)), glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f))
	};
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
	
	
	std::vector<GLushort> idx_vtx{ 
		0, 1, 2, // 1st triangle with counterclockwise winding is specified by
		// vertices in VBOs with indices 0, 1, 2
		2, 3, 0 // 2nd triangle with counterclockwise winding
	};
	GLApp::GLModel mdl;
	mdl.draw_cnt = idx_vtx.size();
	
	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * mdl.draw_cnt, reinterpret_cast<GLvoid*>(idx_vtx.data()), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vao_hdl, ebo_hdl);
	glBindVertexArray(0);
	
	mdl.vaoid = vao_hdl; // set up VAO same as in GLApp::points_model
	mdl.primitive_type = GL_TRIANGLES;
	mdl.primitive_cnt = idx_vtx.size(); // number of primitives (not used)
	return mdl;
}

/*  _________________________________________________________________________*/
/*! mystery_model

@return GLModel

 This function generates a diamond made of trifans with random colours
*/
GLApp::GLModel GLApp::mystery_model()
{
	int slices = 3;
	std::vector<glm::vec2> pos_vtx;
	std::vector<glm::vec3> clr_vtx;
	float xsliceintervals = 180.f/(float)slices;
	float xinterval = 0.f;

	pos_vtx.emplace_back(glm::vec2(0.f, 0.f)); // Pushback for pivot vertex
	clr_vtx.emplace_back(glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f))); // Pivot vertex color

	for (int slicecnt = 0; slicecnt < (float)slices + 1; slicecnt++)
	{
		pos_vtx.emplace_back(glm::vec2(cos(xinterval * M_PI / 180.0), sin(xinterval * M_PI / 180.0)));
		clr_vtx.emplace_back(glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f)));
		xinterval += xsliceintervals;
	}
	
	pos_vtx.emplace_back(glm::vec2(0, -1.5));
	clr_vtx.emplace_back(glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f)));
	pos_vtx.emplace_back(glm::vec2(1.0, 0));
	clr_vtx.emplace_back(glm::vec3(((rand() % 10) / 10.f), ((rand() % 10) / 10.f), ((rand() % 10) / 10.f)));
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
	}
	idx_vtx.emplace_back(slices+1);
	idx_vtx.emplace_back(slices+2);
	idx_vtx.emplace_back(1);
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
	mdl.primitive_cnt = mdl.draw_cnt - 2; // number of primitives (not used)
	return mdl;
}

/*  _________________________________________________________________________*/
/*! GLObject::init

@param none
@return none

This function is called once on object initialization to initialize values of an object
*/
void GLApp::GLObject::init()
{
	// create default engine as source of randomness
	dre.seed((int)std::chrono::high_resolution_clock::now().time_since_epoch().count());
	// get numbers in range [-1, 1) - notice that interval is half-
	// open on the right. When you submit the tutorial, specify a
	// closed range [-1, 1]
	std::uniform_real_distribution<float> urdf(-1.0f, float(std::nextafter(1.0, std::numeric_limits<double>::max())));
	std::uniform_real_distribution<float> urdfloat(0.3f, float(std::nextafter(1.0, std::numeric_limits<double>::max())));
	std::uniform_int_distribution<int> intrand(0, 1);

	// position object in game world such that its x and y coordinates
	// are in range [-5,000, +5,000) ...
	position = glm::vec2(urdf(dre) * WorldRange,
		urdf(dre) * WorldRange);
	// a call urdf(dre) gives a new random value that is uniformly
	// distributed in range [-1.0, 1.0)

	// initialize initial angular displacement and angular speed of object
	angle_disp = urdf(dre) * 360.f; // current orientation
	angle_speed = urdf(dre) * 30.f; // degrees per frame

	scaling = glm::vec2(urdfloat(dre) * (WorldRange / 10), urdfloat(dre) * (WorldRange / 10));
	mdl_to_ndc_xform = glm::mat3
	(1, 0, 0,
		0, 1, 0,
		0, 0, 1);
	mdl_ref = intrand(dre);
	if (mdl_ref == 1)
	{
		mysterycounter++;
	}
	else
	{
		boxcounter++;
	}
	shd_ref = 0;
}
/*  _________________________________________________________________________*/
/*! GLObject::update

@param GLdouble delta_time

@return none

This function is called once per frame to update an object's scale, rotation and translation matrices
*/
void GLApp::GLObject::update(GLdouble delta_time)
{
	glm::mat3 scale
	(scaling.x, 0, 0,
		0, scaling.y, 0,
		0, 0, 1);
	angle_disp += angle_speed * float(delta_time);
	glm::mat3 rotation
	(cos(angle_disp * (M_PI / 180.0)), sin(angle_disp * (M_PI / 180.0)), 0,
		-sin(angle_disp * (M_PI / 180.0)), cos(angle_disp * (M_PI / 180.0)), 0,
		0, 0, 1);
	glm::mat3 translation
	(1, 0, 0,
		0, 1, 0,
		position.x, position.y, 1);

	glm::mat3 mapscale
	(1.f / WorldRange, 0, 0,
		0, 1.f / WorldRange, 0,
		0, 0, 1);

	mdl_to_ndc_xform = mapscale * translation * rotation * scale;
}


/*  _________________________________________________________________________*/
/*! GLObject::draw

@param none
@return none

This function is called once per frame to install the shader program used by the model,
set the transformation matrix for the model and render using glDrawElements
*/
void GLApp::GLObject::draw() const
{
	// Part 1: Install the shader program used by this object to
	// render its model using GLSLShader::Use()
	shdrpgms[shd_ref].Use();

	// Part 2: Bind object's VAO handle using glBindVertexArray
	glBindVertexArray(models[mdl_ref].vaoid);

	// Part 3: Copy object's 3x3 model-to-NDC matrix to vertex shader
	shdrpgms[shd_ref].SetUniform("uModel_to_NDC", mdl_to_ndc_xform);
	
	// Part 4: Render using glDrawElements or glDrawArrays
	glDrawElements(models[mdl_ref].primitive_type, models[mdl_ref].draw_cnt, GL_UNSIGNED_SHORT, NULL);
	
	// Part 5: Clean up
	// Breaking the binding set up in Part 2: glBindVertexArray(0);
	// Deinstall the shader program installed in Part 1 using
	// GLSLShader::UnUse()
	glBindVertexArray(0);
	shdrpgms[shd_ref].UnUse();
}

