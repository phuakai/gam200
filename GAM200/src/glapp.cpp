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
#include <collision.h>
#include <physics.h>
#include <graphics.h>
#include <input.h>
#include <collisiondebug.h>
#include <buffer.h>
#include <model.h>
#include <texture.h>
#include <random>
#include <physicsRigidBody.h>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
std::map<std::string, GLSLShader> GLApp::shdrpgms; // define shaders

std::map<std::string, Graphics::Model> models; // define models

std::map<std::string, GLApp::GLObject> GLApp::objects; // define objects

std::unordered_map<GLApp::collisionType, std::string> GLApp::collisionInfo;

Graphics::Texture texobj;

short GLApp::currentCollision;
bool GLApp::stepByStepCollision;

bool modulate, alphablend, textures;

int tmpobjcounter{};
/*  _________________________________________________________________________*/
/*! GLObject::update

@param GLdouble delta_time

@return none

This function is called once per frame to update an object's scale, rotation and translation matrices
*/
void GLApp::GLObject::update(GLdouble delta_time)
{
	overlap = false;					//change overlap to false

	matrix3x3::mat3x3 scale
	(scaling.x, 0, 0,
	0, scaling.y, 0,
	0, 0, 1);

	if (mdl_ref->first != "triangle")	// check if is black triangle
	{
		orientation.x += orientation.y * float(delta_time);
	}

	matrix3x3::mat3x3 rotation
	(cos(orientation.x), -sin(orientation.x), 0,
	sin(orientation.x), cos(orientation.x), 0,
	0, 0, 1);

	matrix3x3::mat3x3 translation
	(1, 0, modelCenterPos.x,
	0, 1, modelCenterPos.y,
	0, 0, 1);

	mdl_to_world_xform = translation * rotation * scale;
	//world_to_ndc_xform = Graphics::camera2d.world_to_ndc_xform;
	//mdl_to_ndc_xform = Graphics::camera2d.world_to_ndc_xform * mdl_to_world_xform;
	matrix3x3::mat3x3 world_to_ndc_notglm = Graphics::camera2d.getWorldtoNDCxForm();
	world_to_ndc_xform = matrix3x3::mat3x3
	(
		world_to_ndc_notglm.m[0], world_to_ndc_notglm.m[1], world_to_ndc_notglm.m[2],
		world_to_ndc_notglm.m[3], world_to_ndc_notglm.m[4], world_to_ndc_notglm.m[5],
		world_to_ndc_notglm.m[6], world_to_ndc_notglm.m[7], world_to_ndc_notglm.m[8]
	);

	mdl_to_ndc_xform = world_to_ndc_xform * mdl_to_world_xform;

	//compute world coordinates for physics calc
	worldCenterPos = mdl_to_world_xform * vector2D::vec2D(0.f, 0.f);

	ndc_coords.clear();
	worldVertices.clear();
	std::vector <vector2D::vec2D> modelcoord = mdl_ref->second.getModelCoords();
	for (GLuint i = 0; i < mdl_ref->second.posvtx_cnt; i++)
	{
		worldVertices.emplace_back(mdl_to_world_xform * modelcoord[i]);
		ndc_coords.emplace_back(world_to_ndc_xform * worldVertices[i]);
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
	glBindVertexArray(mdl_ref->second.getVAOid()); // Rebind VAO
	//glGetVertexArrayIndexediv(mdl_ref->second.getVAOid(), 0, GL_VERTEX_BINDING_BUFFER, reinterpret_cast<GLint*>(&buffer));
	
	std::vector<vector2D::Vec2> tex_coord
	{
		vector2D::Vec2(1.f, 1.f), vector2D::Vec2(0.f, 1.f),
		vector2D::Vec2(0.f, 0.f), vector2D::Vec2(1.f, 0.f)
	};
	std::vector<vector3D::Vec3> clr_vtx
	{
		vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b),
		vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b)
	};
	std::vector<Graphics::vertexData> vertexData;
	for (int i = 0; i < ndc_coords.size(); ++i)
	{
		Graphics::vertexData tmpVtxData;
		tmpVtxData.posVtx = ndc_coords[i];
		tmpVtxData.clrVtx = clr_vtx[i];
		tmpVtxData.txtVtx = tex_coord[i];
		vertexData.emplace_back(tmpVtxData);
	}
	glNamedBufferSubData(mdl_ref->second.getVBOid(), 0, sizeof(Graphics::vertexData) * vertexData.size(), vertexData.data()); // Set new buffer index with subdata

	//glVertexArrayAttribBinding(mdl_ref->second.getVAOid(), 0, 0);


	

	
	//std::cout << "Tex obj id " << texobj.getTexid() << std::endl;
	

	// copy object's color to fragment shader uniform variable uColor
	//shd_ref->second.SetUniform("uColor", color);

	// copy object's model-to-NDC matrix to vertex shader's
	// uniform variable uModelToNDC
	glm::mat3 glm_mdl_to_ndc_xform
	{
		mdl_to_ndc_xform.m[0], mdl_to_ndc_xform.m[1], mdl_to_ndc_xform.m[2],
		mdl_to_ndc_xform.m[3], mdl_to_ndc_xform.m[4], mdl_to_ndc_xform.m[5],
		mdl_to_ndc_xform.m[6], mdl_to_ndc_xform.m[7], mdl_to_ndc_xform.m[8]
	};
	//shd_ref->second.SetUniform("uModel_to_NDC", glm_mdl_to_ndc_xform);
	//shd_ref->second.SetUniform("ourTexture", mdl_to_ndc_xform);
	GLuint tex_loc = glGetUniformLocation(shd_ref->second.GetHandle(), "ourTexture");
	glUniform1i(tex_loc, 0);

	GLboolean UniformModulate = glGetUniformLocation(shd_ref->second.GetHandle(), "modulatebool");
	glUniform1i(UniformModulate, modulate);

	GLboolean UniformTextures= glGetUniformLocation(shd_ref->second.GetHandle(), "texturebool");
	glUniform1i(UniformTextures, textures);

	// call glDrawElements with appropriate arguments
	glDrawElements(mdl_ref->second.getPrimitiveType(), mdl_ref->second.getDrawCnt(), GL_UNSIGNED_SHORT, NULL);

	// unbind VAO and unload shader program
	glBindVertexArray(0);
	//glDisable(GL_BLEND);
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
	glClearColor(0.3f, 1.f, 1.f, 1.f); // clear colorbuffer with RGBA value in glClearColor

	// Part 2: use the entire window as viewport ...
	GLint w = GLHelper::width, h = GLHelper::height;

	glViewport(0, 0, w, h);

	// Part 3: parse scene file $(SolutionDir)scenes/tutorial-4.scn
	// and store repositories of models of type GLModel in container
	// GLApp::models, store shader programs of type GLSLShader in
	// container GLApp::shdrpgms, and store repositories of objects of
	// type GLObject in container GLApp::objects
	GLApp::init_scene("../scenes/gam200.scn");

	Graphics::Texture::loadTexture("../images/duck-rgba-256.tex", texobj);

	// Part 4: initialize camera
	Graphics::camera2d.init(GLHelper::ptr_window, &GLApp::objects.at("Camera"));

	// Store physics related info to be printed in title bar
	currentCollision = 0;
	stepByStepCollision = false;
	collisionInfo[collisionType::NIL] = "NIL";
	collisionInfo[collisionType::SAT] = "SAT";
	collisionInfo[collisionType::DIAG] = "DIAG";
	collisionInfo[collisionType::AABBSTATIC] = "AABBSTATIC";
	collisionInfo[collisionType::SNAPDIAGSTATIC] = "SNAPDIAGSTATIC";
	collisionInfo[collisionType::AABBSTATIC] = "AABBSTATIC";
	collisionInfo[collisionType::AABBDYNAMIC] = "AABBDYNAMIC";

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
	Graphics::camera2d.update(GLHelper::ptr_window);
	objects["Camera"].update(GLHelper::delta_time);

	// update other inputs for physics

	if (GLHelper::keystateP)
	{
		stepByStepCollision = !stepByStepCollision;
		GLHelper::keystateP = false;
	}

	if (GLHelper::keystateC)
	{
		currentCollision = ++currentCollision % 6;
		GLHelper::keystateC = false;
	}
	if (GLHelper::keystateM)
	{
		modulate = !modulate;
		GLHelper::keystateM = GL_FALSE;
	}
	if (GLHelper::keystateB)
	{
		alphablend = !alphablend;
		GLHelper::keystateB = GL_FALSE;
	}
	if (GLHelper::keystateT)
	{
		textures = !textures;
		GLHelper::keystateT = GL_FALSE;
	}
	if (GLHelper::keystateQ)
	{
		for (int j = 0; j < 100; j++)
		{
			

			std::string tmpobjname = "Banana";
			tmpobjcounter++;
			std::stringstream tmpstream;
			tmpstream << tmpobjname << tmpobjcounter;
			std::string finalobjname = tmpstream.str();
			std::cout << "Final obj name " << finalobjname << std::endl;
			//using uniform_distribution_type = typename uniform_distribution_selector<std::is_integral<T>::value, T>::type;
			unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
			// create default engine as source of randomness
			std::default_random_engine generator(seed);

			std::uniform_int_distribution<int> posrandom(-500, 500);
			int randx = posrandom(generator);
			int randy = posrandom(generator);

		std::uniform_int_distribution<int> sizerandom(50, 150);
		float randwidth = (float)sizerandom(generator);
		float randheight = (float)sizerandom(generator);
		//std::cout << "Values " << randx << ", " << randy << std::endl;
		if (GLHelper::keystateQ)
		{
			GLApp::GLObject::gimmeObject("circle", finalobjname, vector2D::vec2D(randwidth, randwidth), vector2D::vec2D(-randx, -randy));
			GLHelper::keystateQ = false;
		}
		else
		{
			GLApp::GLObject::gimmeObject("square", finalobjname, vector2D::vec2D(randwidth, randheight), vector2D::vec2D(-randx, -randy));
			GLHelper::keystateE = false;
		}
	}
	
	//check for movement
	for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
	{
		if (obj1->first == "Banana1")
		{
			vector2D::vec2D velocity = movement(obj1->second.modelCenterPos, obj1->second.speed);
			obj1->second.body.move(velocity);
			obj1->second.modelCenterPos = obj1->second.body.getPos();
			//obj1->second.body.setPos(obj1->second.modelCenterPos);

		}
	}

#if false
	// Check for circle circle collision with push (WORKING CODE)
	for (int i{ 0 }; i < 8; ++i)
	{
		for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
		{
			if (obj1->first != "Camera")
			{
				for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
				{
					if (obj2->first != "Camera" && obj1->first != obj2->first)
					{
						float depth;
						vector2D::vec2D velocity{ 0.f, 0.f };
						if (physics::CollisionPushResponseCircleCircle(obj1->second.modelCenterPos, obj1->second.scaling.x,
																		obj2->second.modelCenterPos, obj2->second.scaling.x,
																		velocity, depth))
						{
							obj1->second.body.move(velocity);
							obj1->second.body.move(velocity);
							obj1->second.modelCenterPos = obj1->second.body.getPos();
							obj2->second.modelCenterPos = obj2->second.body.getPos();
							//obj1->second.body.setPos(obj1->second.modelCenterPos);
							//obj2->second.body.setPos(obj2->second.modelCenterPos);
						}
					}
				}
			}
		}
	}

#endif
#if false
	// Check for circle circle collision with block (WORKING CODE)
	for (int i{ 0 }; i < 8; ++i)
	{
		for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
		{
			//if (obj1->first != "Camera")
			if (obj1->first != "Banana1" && obj1->first != "Camera")
			{
				for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
				{
					if (obj2->first == "Banana1")
					{
						float depth;

						if (physics::CollisionBlockResponseCircleCircle(obj1->second.modelCenterPos, obj1->second.scaling.x,
							obj2->second.modelCenterPos, obj2->second.scaling.x,
							obj2->second.directionVec, depth))
						{
							obj2->second.body.setPos(obj2->second.modelCenterPos);
						}
					}
				}
			}
		}
	}

#endif

	bool GraceObj{ true };
	for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
	{
		for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
		{

		//if (obj1->first != "Camera")
		//{
			if (obj1 != obj2)
			{

				obj1->second.update(GLHelper::delta_time);
				//update phsyics according to input
				if (GraceObj && (obj1->first != "Camera"))
				{
					GraceObj = false;
					//movement(obj1->second, objects["Camera"], stepByStepCollision);
					//movement(obj1->second, obj2->second, stepByStepCollision);
				}
				//check for physics collision after update
				switch (currentCollision)
				{
				case 1: //collisionType::SAT
					if (physics::shapeOverlapSAT(obj2->second, obj1->second))
					{
						obj1->second.overlap = true;
						obj2->second.overlap = true;
					}
					break;
				case 2: //collisionType::DIAG
					if (physics::shapeOverlapDIAGONAL(obj2->second, obj1->second))
					{
						obj1->second.overlap = true;
						obj2->second.overlap = true;
					}
					break;
				case 3: //collisionType::SNAPDIAGSTATIC
					physics::shapeOverlapSnapStaticDIAGONAL(obj2->second, obj1->second);
					break;
				case 4: //collisionType::AABBSTATIC
					if (physics::shapeOverlapStaticAABB(obj2->second, obj1->second))
					{
						obj1->second.overlap = true;
						obj2->second.overlap = true;
					}
					break;
				case 5: //collisionType::AABBDYNAMIC
					physics::shapeOverlapDynamicAABB(obj2->second, obj1->second);
					break;
				default:
					break;
			}
			}

		}
	}
	//}
	for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end() ; ++obj1)
	{
		for (GLuint i = 0; i < obj1->second.mdl_ref->second.posvtx_cnt; i++)
		{
			obj1->second.ndc_coords[i] = obj1->second.world_to_ndc_xform * obj1->second.worldVertices[i], 1.f;
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
	title << " | Camera Position (" << Graphics::camera2d.getCameraObject().modelCenterPos.x << ", " << Graphics::camera2d.getCameraObject().modelCenterPos.y << ")";
	title << " | Orientation: " << std::setprecision(0) << (Graphics::camera2d.getCameraObject().orientation.x / M_PI * 180) << " degrees";
	title << " | Window height: " << Graphics::camera2d.getHeight();
	title << " | Collision Type: " << collisionInfo[static_cast< collisionType>(currentCollision)];
	title << std::setprecision(2) << " | FPS " << int(GLHelper::fps*100)/100.0;

	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());

	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	if (alphablend)
	{
		//std::cout << "Nani\n";
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	if (textures)
	{
		//std::cout << "Enabled\n";
		glBindTextureUnit(0, (texobj.getTexid()));
	}
	if (!textures)
	{
		//std::cout << "Disabled\n";
		glBindTextureUnit(0, 0);
		//glBindTextureUnit(0, (texobj.getTexid()));
	}	
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
		case 4: //collisionType::AABBSTATIC
			//collisionDebug(obj->second);
			if (obj->first != "Camera")
			{
				obj->second.draw();
				obj->second.color = green;
			}
			else
			{ 
				obj->second.color = blue;
			}

			if (obj->second.overlap)
			{
				obj->second.color = red;
			}
			break;
		case 5: //collisionType::AABBDYNAMIC
			//collisionDebug(obj->second);
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
		case 6: //collisionType::AABBDYNAMIC
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

	//objects["Camera"].draw();
	glDisable(GL_BLEND);
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

void GLApp::GLObject::gimmeObject(std::string modelname, std::string objname, vector2D::vec2D scale, vector2D::vec2D pos)
{
	GLObject tmpObj;
	std::string hi;
	std::cout << "hi\n";
	tmpObj.body.createCircleBody(150.f, vector2D::vec2D(-19350.f, -19800.f), 0.f, false, 0.f, &tmpObj.body, hi);
	tmpObj.body.setRad(scale.x);
	tmpObj.body.setPos(pos);

	unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	// create default engine as source of randomness
	std::default_random_engine generator(seed);

	std::uniform_real_distribution<float> colour(0.f, 1.f);

	float randr = colour(generator);
	float randg = colour(generator);
	float randb = colour(generator);
	tmpObj.color = glm::vec3(randr, randg, randb);
	tmpObj.scaling = vector2D::vec2D(tmpObj.body.getRad(), tmpObj.body.getRad());
	//tmpObj.scaling = scale;
	tmpObj.scaling = vector2D::vec2D(tmpObj.body.getRad(), tmpObj.body.getRad());
	//std::cout << "this is scale in obj and scale in scale in body: " << tmpObj.scaling.x << " " << tmpObj.scaling.y << " " << tmpObj.body.getRad() << std::endl;
	tmpObj.orientation = vector2D::vec2D(0, 0);
	tmpObj.modelCenterPos = pos;
	tmpObj.speed = 200.f;

	if (shdrpgms.find("gam200-shdrpgm") != shdrpgms.end())
	{
		tmpObj.shd_ref = shdrpgms.find("gam200-shdrpgm");
	}
	else
	{
		insert_shdrpgm("gam200-shdrpgm", "../shaders/gam200.vert", "../shaders/gam200.frag");
		tmpObj.shd_ref = shdrpgms.find("gam200-shdrpgm");
	}

	if (models.find(modelname) != models.end())
	{
		tmpObj.mdl_ref = models.find(modelname);
	}
	else
	{
		Graphics::Model Model;
		Model = Model.init(modelname);
		models[modelname] = Model;
		tmpObj.mdl_ref = models.find(modelname);
	}

	objects[objname] = tmpObj;

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
		getline(ifs, line); // 8th parameter: velocity
		std::istringstream velocity{ line };
		velocity >> Object.speed;
		Object.overlap = false;
		Object.untravelledDistance.first = vector2D::vec2D{ 0.f, 0.f };
		Object.untravelledDistance.second = 0.f;

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
			Graphics::Model Model;
			
			Model = Model.init(model_name);

			//std::cout << "Model " << Model.getPosvtxCnt() << std::endl;
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
/*! GLObject::init

@param none
@return none

This function is currently empty
*/
void GLApp::GLObject::init()
{

}
