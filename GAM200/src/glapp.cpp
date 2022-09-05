/*!
@file    glapp.cpp
@author  pghali@digipen.edu, a.weiren@digipen.edu
@date    10/06/2022

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
#define _USE_MATH_DEFINES //for pi
#include <math.h>
#include <physics.h>


/*                                                   objects with file scope
----------------------------------------------------------------------------- */
std::map<std::string, GLSLShader> GLApp::shdrpgms; // define shaders

std::map<std::string, GLApp::GLModel> GLApp::models; // define models

std::map<std::string, GLApp::GLObject> GLApp::objects; // define objects

GLApp::Camera2D GLApp::camera2d;

std::unordered_map<GLApp::collisionType, std::string> GLApp::collisionInfo;
short GLApp::currentCollision;
/*  _________________________________________________________________________*/
/*! GLObject::update

@param GLdouble delta_time

@return none

This function is called once per frame to update an object's scale, rotation and translation matrices
*/
void GLApp::GLObject::update(GLdouble delta_time)
{
	overlap = false;					//change overlap to false

	glm::mat3 scale
	(scaling.x, 0, 0,
		0, scaling.y, 0,
		0, 0, 1);

	if (mdl_ref->first != "triangle")	// check if is black triangle
	{
		orientation.x += orientation.y * float(delta_time);
	}

	glm::mat3 rotation
	(cos(orientation.x), sin(orientation.x), 0,
		-sin(orientation.x), cos(orientation.x), 0,
		0, 0, 1);

	glm::mat3 translation
	(1, 0, 0,
		0, 1, 0,
		modelCenterPos.x, modelCenterPos.y, 1);

	mdl_to_world_xform = translation * rotation * scale;
	world_to_ndc_xform = camera2d.world_to_ndc_xform;
	mdl_to_ndc_xform = camera2d.world_to_ndc_xform * mdl_to_world_xform;


	//compute world coordinates for physics calc
	worldCenterPos = mdl_to_world_xform * glm::vec3(0.f, 0.f, 1.f);

	ndc_coords.clear();
	worldVertices.clear();
	for (GLuint i = 0; i < mdl_ref->second.posvtx_cnt; i++)
	{
		worldVertices.emplace_back(mdl_to_world_xform * glm::vec3(mdl_ref->second.model_coords[i], 1.0));
		ndc_coords.emplace_back(world_to_ndc_xform * glm::vec3(worldVertices[i], 1.0f));
	}
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
	// load shader program in use by this object
	shd_ref->second.Use();

	// bind VAO of this object's model
	GLuint buffer;
	glGetVertexArrayIndexediv(mdl_ref->second.vaoid, 6, GL_VERTEX_BINDING_BUFFER, reinterpret_cast<GLint*>(&buffer));


	glNamedBufferSubData(buffer, 0, sizeof(glm::vec2) * mdl_ref->second.posvtx_cnt, ndc_coords.data()); // Set new buffer index with subdata

	glVertexArrayAttribBinding(mdl_ref->second.vaoid, 4, 6);

	glBindVertexArray(mdl_ref->second.vaoid); // Rebind VAO

	// copy object's color to fragment shader uniform variable uColor
	shd_ref->second.SetUniform("uColor", color);

	// copy object's model-to-NDC matrix to vertex shader's
	// uniform variable uModelToNDC
	//shd_ref->second.SetUniform("uModel_to_NDC", mdl_to_ndc_xform);

	// call glDrawElements with appropriate arguments
	glDrawElements(mdl_ref->second.primitive_type, mdl_ref->second.draw_cnt, GL_UNSIGNED_SHORT, NULL);

	// unbind VAO and unload shader program
	glBindVertexArray(0);
	shd_ref->second.UnUse();
}


/*  _________________________________________________________________________*/
/*! GLApp::init

@param none
@return none

This function is called once on initialization to initialize values and call setup functions
It uses OpenGL functions such as:
glClearColor and glViewport to initialize the app
*/

void GLApp::init()
{
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
	GLApp::init_scene("../scenes/gam200.scn");

	// Part 4: initialize camera
	GLApp::camera2d.init(GLHelper::ptr_window, &GLApp::objects.at("Camera"));

	// Store physics related info to be printed in title bar
	currentCollision = 0;
	collisionInfo[collisionType::NIL] = "NIL";
	collisionInfo[collisionType::SAT] = "SAT";
	collisionInfo[collisionType::DIAG] = "DIAG";
	collisionInfo[collisionType::SNAPDIAGSTATIC] = "SNAPDIAGSTATIC";

	// Part 5: Print OpenGL context and GPU specs
	//GLHelper::print_specs();
}

/*  _________________________________________________________________________*/
/*! GLApp::update

@param none
@return none

This function updates the polygon rasterization mode when 'P' is pressed,
it also spawns new objects until reaching the maximum object limit, where it will
despawn the oldest objects, and respawn objects again once no objects are left.
*/
void GLApp::update()
{
	// first, update camera
	GLApp::camera2d.update(GLHelper::ptr_window);
	objects["Camera"].update(GLHelper::delta_time);

	// update other inputs for physics
	if (GLHelper::keystateC)
	{
		currentCollision = ++currentCollision % 4;
		GLHelper::keystateC = false;
	}

	// next, iterate through each element of container objects
	// for each object of type GLObject in container objects
	// call update function GLObject::update(delta_time) except on
	// object which has camera embedded in it - this is because
	// the camera has already updated the object's orientation
	for (std::map <std::string, GLObject> ::iterator obj = objects.begin(); obj != objects.end(); ++obj)
	{
		if (obj->first != "Camera")
		{
			obj->second.update(GLHelper::delta_time);

			//check for physics collision after update

			switch (currentCollision)
			{
			case 1: //collisionType::SAT
				if (physics::shapeOverlapSAT(objects["Camera"], obj->second))
				{
					obj->second.overlap = true;
					objects["Camera"].overlap = true;
				}
				break;
			case 2: //collisionType::DIAG
				if (physics::shapeOverlapDIAGONAL(objects["Camera"], obj->second))
				{
					obj->second.overlap = true;
					objects["Camera"].overlap = true;
				}
				break;
			case 3: //collisionType::SNAPDIAGSTATIC
				//if (physics::shapeOverlapSnapStaticDIAGONAL(objects["Camera"], obj->second))
				physics::shapeOverlapSnapStaticDIAGONAL(objects["Camera"], obj->second);
				if (objects["Camera"].overlap)
				{
					//objects["Camera"].modelCenterPos = objects["Camera"].worldToMdlXform * glm::vec3(objects["Camera"].worldCenterPos, 1.f);
				}
				break;
			default:
				break;
			}
			for (GLuint i = 0; i < obj->second.mdl_ref->second.posvtx_cnt; i++)
			{
				obj->second.ndc_coords[i] = obj->second.world_to_ndc_xform * glm::vec3(obj->second.worldVertices[i], 1.f);
			}
		}
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
	// write window title with appropriate information using
	// glfwSetWindowTitle() ...

	std::stringstream title;
	title << std::fixed;
	title << std::setprecision(2);
	title << "GAM200";
	title << " | Camera Position (" << camera2d.pgo->modelCenterPos.x << ", " << camera2d.pgo->modelCenterPos.y << ")";
	title << " | Orientation: " << std::setprecision(0) << (camera2d.pgo->orientation.x / M_PI * 180) << " degrees";
	title << " | Window height: " << camera2d.height;
	title << " | Collision Type: " << collisionInfo[static_cast<collisionType>(currentCollision)];
	title << std::setprecision(2) << " | FPS " << int(GLHelper::fps * 100) / 100.0;

	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());

	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Part 4: Render each object in container GLApp::objects
	for (std::map <std::string, GLObject>::iterator obj = objects.begin(); obj != objects.end(); ++obj)
	{
		switch (currentCollision)
		{
		case 0: //collisionType::NIL
			if (obj->first != "Camera")
			{
				obj->second.draw();
			}

			break;
		case 1: //collisionType::SAT
			if (obj->first != "Camera")
			{
				obj->second.draw();
				obj->second.color = green;
			}
			else
				obj->second.color = blue;

			if (obj->second.overlap)
			{
				obj->second.color = red;
			}
			break;
		case 2: //collisionType::DIAG
			if (obj->first != "Camera")
			{
				obj->second.draw();
				obj->second.color = green;
			}
			else
				obj->second.color = blue;

			if (obj->second.overlap)
			{
				obj->second.color = red;
			}
			break;
		case 3: //collisionType::SNAPDIAGSTATIC
			if (obj->first != "Camera")
			{
				obj->second.draw();
				obj->second.color = green;
			}
			else
				obj->second.color = blue;

			if (obj->second.overlap)
			{
				obj->second.color = red;
			}
			break;
		default:
			break;
		}
	}

	objects["Camera"].draw();
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
/*!  GLApp::insert_shdrpgm

@param std::string shdr_pgm_name
Name of shader program to be used

@param std::string vtx_shdr
Name of vertex shader to be used

@param std::string frg_shdr
Name of fragment shader to be used

@return none

This function is called at the initialization of the scene to insert the different shader programs into a map container
*/
void GLApp::insert_shdrpgm(std::string shdr_pgm_name, std::string vtx_shdr, std::string frg_shdr)
{
	std::vector<std::pair<GLenum, std::string>> shdr_files
	{
		std::make_pair(GL_VERTEX_SHADER, vtx_shdr),
		std::make_pair(GL_FRAGMENT_SHADER, frg_shdr)
	};
	GLSLShader shdr_pgm;
	shdr_pgm.CompileLinkValidate(shdr_files);
	if (GL_FALSE == shdr_pgm.IsLinked()) {
		std::cout << "Unable to compile/link/validate shader programs\n";
		std::cout << shdr_pgm.GetLog() << "\n";
		std::exit(EXIT_FAILURE);
	}
	// add compiled, linked, and validated shader program to
	// std::map container GLApp::shdrpgms
	GLApp::shdrpgms[shdr_pgm_name] = shdr_pgm;
}

/*  _________________________________________________________________________*/
/*! GLApp::init_scene

@param std::string scene_filename
Name of scene to be used

@return none

This function is called once at the initialization of the scene to compute and initialize the objects
*/
void GLApp::init_scene(std::string scene_filename)
{
	std::ifstream ifs{ scene_filename, std::ios::in };
	if (!ifs)
	{
		std::cout << "ERROR: Unable to open scene file: "
			<< scene_filename << "\n";
		exit(EXIT_FAILURE);
	}
	ifs.seekg(0, std::ios::beg);
	std::string line;
	getline(ifs, line); // first line is count of objects in scene
	std::istringstream line_sstm{ line };
	int obj_cnt;
	line_sstm >> obj_cnt; // read count of objects in scene
	while (obj_cnt--)
	{
		/*
		add code to do this:
		read remaining parameters of object from file:
		object's name
		RGB parameters for rendering object's model geometry
		scaling factors to be applied on object's model
		orientation factors: initial angular orientation and angular speed
		object's position in game world
		set data member GLApp::GLObject::mdl_ref to iterator that points to
		model instantiated by this object
		set data member GLApp::GLObject::shd_ref to iterator that points to
		shader program used by this object
		insert this object to std::map container objects
		*/
		GLObject Object;
		// read each object's parameters
		getline(ifs, line); // 1st parameter: model's name
		std::istringstream line_modelname{ line };
		std::string model_name;
		line_modelname >> model_name;

		getline(ifs, line); // 2nd parameter: object's name
		std::istringstream line_model_obj_name{ line };
		std::string model_object;
		line_model_obj_name >> model_object;

		getline(ifs, line); // 3rd parameter: shader's name
		std::istringstream line_model_shader{ line };
		std::string model_shader_pgm, model_vert_shader, model_frag_shader;
		line_model_shader >> model_shader_pgm >> model_vert_shader >> model_frag_shader;

		getline(ifs, line); // 4th parameter: color 
		std::istringstream line_model_color{ line };
		line_model_color >> Object.color.r >> Object.color.g >> Object.color.b;

		getline(ifs, line); // 5th parameter: scale 
		std::istringstream line_model_scale{ line };
		line_model_scale >> Object.scaling.x >> Object.scaling.y;

		getline(ifs, line); // 6th parameter: orientation 
		std::istringstream line_model_orientation{ line };
		line_model_orientation >> Object.orientation.x >> Object.orientation.y;
		Object.orientation.x *= float(M_PI / 180);
		Object.orientation.y *= float(M_PI / 180);

		getline(ifs, line); // 7th parameter: position 
		std::istringstream line_model_position{ line };
		line_model_position >> Object.modelCenterPos.x >> Object.modelCenterPos.y;

		//for physics
		Object.overlap = false;


		/*
		add code to do this:
		if model with name model_name is not present in std::map container
		called models, then add this model to the container
		*/
		if (models.find(model_name) != models.end())
		{
			Object.mdl_ref = models.find(model_name);

		}
		else
		{
			GLModel Model;
			std::ifstream ifs{ "../meshes/" + model_name + ".msh" , std::ios::in };
			if (!ifs)
			{
				std::cout << "ERROR: Unable to open mesh file: " << model_name << "\n";
				exit(EXIT_FAILURE);
			}
			ifs.seekg(0, std::ios::beg);
			std::string line_mesh;
			getline(ifs, line_mesh);
			std::istringstream linestream{ line_mesh };
			std::string meshname;
			char prefix;
			linestream >> prefix >> meshname;
			std::vector <glm::vec2> pos_vtx;
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
						primitive.emplace_back(glushortstuff);
					}
					Model.primitive_type = GL_TRIANGLES;

					break;
				case 'f': //trifan 
					while (linestream >> glushortstuff)
					{
						primitive.emplace_back(glushortstuff);
					}
					Model.primitive_type = GL_TRIANGLE_FAN;

					break;
				case 'v':
					//while (linestream >> floatstuff)
					//{
					//pos_vtx.emplace_back(floatstuff);
					//}

					linestream >> x >> y;
					pos_vtx.emplace_back(glm::vec2(x, y));
					Model.model_coords.emplace_back(glm::vec2(x, y));
					break;
				default:
					break;
				}
			}

			glCreateBuffers(1, &vbo); // Creates a buffer named vbo (can replace vbo with an array if multiple buffers)

			glNamedBufferStorage(vbo, sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data(), GL_DYNAMIC_STORAGE_BIT); // Creates a buffer object's storage
			// vbo is buffer name, followed by size of buffer (float type * number of data), data stored in buffer, and finally the flag of the storage system
			// GL_DYNAMIC_STORAGE_BIT allows contents of the data to be updated after creation by calling glBufferSubData, 
			// else you can only use server-side calls such as glCopyBufferSubData and glClearBufferSubData.
			glCreateVertexArrays(1, &vao); // Creates a vertex array object (can replace vao with an array if multiple buffers)

			glEnableVertexArrayAttrib(vao, 4); // Enables the vertex array attrib for index 4 of vao
			// When enabled, vertex attribute array will be accessed and used for rendering 
			// when calls are made to vertex array commands such as glDrawArrays, glDrawElements, glDrawRangeElements, glMultiDrawElements, or glMultiDrawArrays.
			glVertexArrayVertexBuffer(vao, 6, vbo, 0, sizeof(glm::vec2)); // Binds a buffer to a vertex array object
			// Name of vertex array object, index for vertex buffer object to bind to, name of buffer to be binded, offset of first element, stride/step (distance between elements of buffer)

			glVertexArrayAttribFormat(vao, 4, 2, GL_FLOAT, GL_FALSE, 0); // Specify the organisation of vertex arrays
			// Name of vertex array object, index of vertex array object being set, size (num of values per vertex that is stored in array), type of data,
			// GL_TRUE = data should be normalized, GL_FALSE = data converted directly as fixed-point values, offset (distance between elements of buffer)
			glVertexArrayAttribBinding(vao, 4, 6); // Associates a vertex attribute and a vertex buffer binding for a VAO
			// Name of vertex array object, index of vertex attrib, index of vertex buffer binding index

			glCreateBuffers(1, &ebo); // Creates a buffer named ebo (can replace ebo with an array if multiple buffers)
			glNamedBufferStorage(ebo, sizeof(GLushort) * primitive.size(), primitive.data(), GL_DYNAMIC_STORAGE_BIT);
			glVertexArrayElementBuffer(vao, ebo); // Configures element array buffer binding of a vertex array object
			// Name of vertex array object, name of buffer object used for element array buffer binding
			glBindVertexArray(0); // Unbind vertex array object (0 is used to unbind)

			Model.vaoid = vao;
			Model.primitive_cnt = primitive.size();
			Model.posvtx_cnt = pos_vtx.size();
			Model.draw_cnt = primitive.size();

			models[model_name] = Model;
			Object.mdl_ref = models.find(model_name);

		}


		/*
		add code to do this:
		if shader program listed in the scene file is not present in
		std::map container called shdrpgms, then add this model to the
		container
		*/
		if (shdrpgms.find(model_shader_pgm) != shdrpgms.end())
		{
			Object.shd_ref = shdrpgms.find(model_shader_pgm);
		}
		else
		{
			insert_shdrpgm(model_shader_pgm, model_vert_shader, model_frag_shader);
			Object.shd_ref = shdrpgms.find(model_shader_pgm);
		}
		objects[model_object] = Object;
	}
}

/*  _________________________________________________________________________*/
/*! GLApp::Camera2D::init

@param GLFWwindow* pWindow
Pointer to GLFW window currently in use

@param GLApp::GLObject* ptr
Pointer to Camera object

@return none

This function is called once at the initialization of the camera to compute and initialize the camera window
*/
void GLApp::Camera2D::init(GLFWwindow* pWindow, GLApp::GLObject* ptr)
{
	// assign address of object of type GLApp::GLObject with
	// name "Camera" in std::map container GLApp::objects ...
	pgo = ptr;

	// compute camera window's aspect ratio ...
	GLsizei fb_width, fb_height;
	glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
	ar = static_cast<GLfloat>(fb_width) / fb_height;
	int width = int(ar * height);

	// compute camera's up and right vectors ...
	up = { -sin(pgo->orientation.x), cos(pgo->orientation.x) };
	right = { cos(pgo->orientation.x), sin(pgo->orientation.x) };
	// at startup, the camera must be initialized to free camera ...

	//glm is row-major
	view_xform = glm::mat3(1, 0, 0,
		0, 1, 0,
		-pgo->modelCenterPos.x, -pgo->modelCenterPos.y, 1);

	// compute other matrices ...
	camwin_to_ndc_xform = glm::mat3(2 / width, 0, 0,
		0, 2 / height, 0,
		0, 0, 1);
	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
}

/*  _________________________________________________________________________*/
/*! GLApp::Camera2D::update

@param GLFWwindow* pWindow
Pointer to GLFW window currently in use

@return none

This function is called once per frame to compute and update the camera window
*/
void GLApp::Camera2D::update(GLFWwindow* pWindow)
{
	// compute camera window's aspect ratio ...
	GLsizei fb_width, fb_height;
	glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
	ar = static_cast<GLfloat>(fb_width) / fb_height;
	int width = int(ar * height);

	// compute camera's up and right vectors ...
	up = { -sin(pgo->orientation.x), cos(pgo->orientation.x) };
	right = { cos(pgo->orientation.x), sin(pgo->orientation.x) };
	// at startup, the camera must be initialized to free camera ...

	// compute other matrices ...
	camwin_to_ndc_xform = glm::mat3(2.f / (float)width, 0, 0,
		0, 2.f / (float)height, 0,
		0, 0, 1);

	if (GLHelper::keystateW == GL_TRUE)
	{
		//std::cout << "before w: " << pgo->modelCenterPos.x << " " << pgo->modelCenterPos.y << std::endl;
		pgo->modelCenterPos = pgo->modelCenterPos + linear_speed * up;
		//std::cout << "after w: " << pgo->modelCenterPos.x << " " << pgo->modelCenterPos.y << std::endl;
	}

	if (GLHelper::keystateS == GL_TRUE)
	{
		pgo->modelCenterPos = pgo->modelCenterPos - linear_speed * up;
	}

	if (GLHelper::keystateA == GL_TRUE)
	{
		if (pgo->orientation.x / M_PI * 180 >= 360)
		{
			pgo->orientation.x = 0;
		}
		pgo->orientation.x += pgo->orientation.y;
	}

	if (GLHelper::keystateD == GL_TRUE)
	{
		if (pgo->orientation.x / M_PI * 180 <= -360)
		{
			pgo->orientation.x = 0;
		}
		pgo->orientation.x -= pgo->orientation.y;
	}

	if (GLHelper::keystateV == GL_TRUE)
	{
		if (camtype_flag == GL_TRUE)
		{
			camtype_flag = GL_FALSE;
		}
		else
		{
			camtype_flag = GL_TRUE;
		}
		GLHelper::keystateV = GL_FALSE;
	}

	if (GLHelper::keystateZ == GL_TRUE)
	{
		if (height >= max_height)
		{
			height_chg_dir = -1;
		}
		else if (height <= min_height)
		{
			height_chg_dir = 1;
		}
		height += height_chg_val * height_chg_dir;
	}

	if (camtype_flag == GL_FALSE)
	{
		//glm is row-major
		view_xform = glm::mat3(1, 0, 0,
			0, 1, 0,
			-pgo->modelCenterPos.x, -pgo->modelCenterPos.y, 1);
	}
	else
	{
		view_xform = glm::mat3(right.x, up.x, 0,
			right.y, up.y, 0,
			-(right.x * pgo->modelCenterPos.x + right.y * pgo->modelCenterPos.y), -(up.x * pgo->modelCenterPos.x + up.y * pgo->modelCenterPos.y), 1);
	}

	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
	// check keyboard button presses to enable camera interactivity

	// update camera aspect ratio - this must be done every frame
	// because it is possible for the user to change viewport
	// dimensions

	// update camera's orientation (if required)

	// update camera's up and right vectors (if required)

	// update camera's position (if required)

	// implement camera's zoom effect (if required)

	// compute appropriate world-to-camera view transformation matrix

	// compute window-to-NDC transformation matrix

	// compute world-to-NDC transformation matrix

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
