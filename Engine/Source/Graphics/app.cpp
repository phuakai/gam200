/*!
@file    app.cpp
@author  a.weiren@digipen.edu, ruoyan.go@digipen.edu, p.jiankai@digipen.edu
@date    10/08/2022

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and to extract function pointers
to OpenGL implementations.

*//*__________________________________________________________________________*/

/*                                                                   includes
----------------------------------------------------------------------------- */

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

#include <app.h>
#include <collision.h>
#include <physics.h>
#include <collisiondebug.h>
#include <buffer.h>
#include <model.h>
#include <texture.h>
#include <transform.h>

#include <random>
#include <stdint.h>
#include <camera.h>
#include <iomanip>
#include "../mainHeader.h"
#include "ECS.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "pathfinding.h"

extern Entity player1;
extern std::vector<Entity> walls;

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
std::map<std::string, GLSLShader> GLApp::shdrpgms; // define shaders

std::map<std::string, Graphics::Model> models; // define models

std::map<std::string, GLApp::GLObject> GLApp::objects; // define objects

std::unordered_map<GLApp::collisionType, std::string> GLApp::collisionInfo;

RenderNS::BatchRenderer basicbatch; // Batch render object
RenderNS::BatchRenderer debugbatch; // Batch render object for collision debug
RenderNS::BatchRenderer debuglinebatch; // Batch render object for collision debug

RenderNS::InstancedRenderer basicinstance; // Instance render object for collision debug

//Graphics::Texture texobj;

//std::vector<Graphics::Texture> Graphics::textureobjects;

Graphics::Camera2D camera2d;

GLApp::collisionType GLApp::currentCollision;
bool GLApp::movableShape;
//std::list <std::pair<int, partitionObj>> partitionStorage;

bool GLApp::modulate;
bool GLApp::alphablend;
bool GLApp::textures;
bool GLApp::coldebug;
bool GLApp::velocitydirectiondebug;
bool GLApp::graphicsmode;

int GLApp::objectcounter;

int entitycounter;

/*! GLApp::init

@param none
@return none

This function is called once on initialization to initialize values and call setup functions
It uses OpenGL functions such as:
glClearColor and glViewport to initialize the app
*/

void GLApp::init()
{
	entitycounter = 0;
	std::fstream myfile;
	myfile.open("config.xml");
	int width{};
	int height{};
	myfile >> width;
	myfile >> height;

	if (!Graphics::Input::init(width, height, "Bloom"))
	{
		std::cout << "Unable to create OpenGL context" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	GLApp::shdrpgms.clear(); // clear shaders
	models.clear(); // clear models
	GLApp::objects.clear(); // clear objects

	basicbatch.BatchClear(); // Clear basic batch
	debugbatch.BatchClear(); // Clear debug batch
	debuglinebatch.BatchClear(); // Clear debug line batch

	Graphics::Model linemodel; // Init line model
	linemodel = linemodel.init("line");
	models["line"] = linemodel;

	GLApp::objectcounter = 0;
	GLApp::modulate = false;
	GLApp::alphablend = true;
	GLApp::textures = true;
	GLApp::coldebug = false;

	glClearColor(0.2f, 1.f, 0.3f, 1.f);						// clear colorbuffer with RGBA value in glClearColor
	glViewport(0, 0, Graphics::Input::screenwidth, Graphics::Input::screenheight);

	Graphics::Texture::createTexturePath("../images/DragBox_256x256.png", Graphics::textureobjects);
	Graphics::Texture::createTexturePath("../images/cloud_256x256.png", Graphics::textureobjects);
	Graphics::Texture::loadTexture(Graphics::textureobjects); // Load all textures
	//Graphics::Texture::loadTexture("../images/BaseTree.png", Graphics::textureobjects); // 
	//Graphics::Texture::loadTexture("../images/GrassMap.png", Graphics::textureobjects); // Grass map
	//Graphics::Texture::loadTexture("../images/BlueCircle.png", Graphics::textureobjects); // Blue Circle
	//Graphics::Texture::loadTexture("../images/YellowCircle.png", Graphics::textureobjects); // Yellow Circle
	//Graphics::Texture::loadTexture("../images/DragBox.png", Graphics::textureobjects); // Drag Box
	//Graphics::Texture::loadTexture("../images/Unit_tank_front.png", Graphics::textureobjects); // Enemy unit
	//Graphics::Texture::loadTexture("../images/Map_sprite1.png", Graphics::textureobjects); // BG1
	//Graphics::Texture::loadTexture("../images/Map_sprite2.png", Graphics::textureobjects); // BG2


	// Part 4: initialize camera (NEED TO CHANGE THIS PLEASE)
	GLApp::GLObject::gimmeObject("square", "Camera", vector2D::vec2D(1, 1), vector2D::vec2D(0, 0), vector3D::vec3D(1, 1, 1));
	Graphics::camera2d.init(Graphics::Input::ptr_to_window, &GLApp::objects.at("Camera"));

	// ======================================================================================================================================
	// Store physics related info to be printed in title bar
	currentCollision = collisionType::NIL;
	movableShape = false;
	collisionInfo[collisionType::NIL] = "NIL";
	collisionInfo[collisionType::CircleDetection] = "CircleDetection";
	collisionInfo[collisionType::CirclePushResolution] = "CirclePushResolution";
	collisionInfo[collisionType::CircleBlockResolution] = "CircleBlockResolution";
	collisionInfo[collisionType::PolygonDetection] = "PolygonDetection";
	collisionInfo[collisionType::PolygonPushResolution] = "PolygonPushResolution";
	collisionInfo[collisionType::PolygonBlockResolution] = "PolygonBlockResolution";
	collisionInfo[collisionType::PolygonCircleDetection] = "PolygonCircleDetection";
	collisionInfo[collisionType::PolygonCircleResolution] = "PolygonCircleResolution";
	coldebug = false;

	EntityID playerID = player1.GetID();
	GLApp::GLObject::gimmeObject(ecs.GetComponent<Render>(playerID)->type, ecs.GetComponent<Render>(playerID)->name, ecs.GetComponent<Render>(playerID)->dimension, ecs.GetComponent<Render>(playerID)->position, vector3D::vec3D(0.3f, 0.3f, 0.7f), true);

}


/*  _________________________________________________________________________*/
/*! GLObject::update

@param GLdouble delta_time

@return none

This function is called once per frame to update an object's scale, rotation and translation matrices
*/
void GLApp::GLObject::update(GLdouble delta_time)
{

	matrix3x3::mat3x3 scale
	(scaling.x, 0, 0,
		0, scaling.y, 0,
		0, 0, 1);

	if (mdl_ref->first != "triangle")	// check if is black triangle
	{
		orientation.x += orientation.y * float(delta_time);
	}

	//std::cout << "Orientation " << orientation.x << ", " << orientation.y << std::endl;
	
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
	// TO MOVE OUT IN THE FUTURE (is called every draw when it only needs to be called once)
	matrix3x3::mat3x3 controlxform = translation * rotation * scale;
	controlworldpos.clear();
	controlndcpos.clear();
	std::vector <vector2D::vec2D> controlmodelpos = mdl_ref->second.getModelCoords();
	for (GLuint i = 0; i < mdl_ref->second.posvtx_cnt; i++)
	{
		controlworldpos.emplace_back(controlxform * controlmodelpos[i]);
		controlndcpos.emplace_back(world_to_ndc_xform * controlworldpos[i]);
	}	

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
	basicinstance.instanceshader = shd_ref->second;

	std::vector<vector3D::Vec3> clr_vtx
	{
		vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b),
		vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b)
	};

	std::vector<vector2D::Vec2> texcoord;
	texcoord.emplace_back(vector2D::Vec2(0.f, 0.f)); // Bottom left
	texcoord.emplace_back(vector2D::Vec2(1.f, 0.f)); // Bottom right
	texcoord.emplace_back(vector2D::Vec2(1.f, 1.f)); // Top right
	texcoord.emplace_back(vector2D::Vec2(0.f, 1.f)); // Top left

	Graphics::vertexData tmpHeaderData;
	std::vector<Graphics::vertexData> vertexData;
	std::vector<matrix3x3::mat3x3> testdata;
	for (int i = 0; i < controlndcpos.size(); ++i)
	{
		Graphics::vertexData tmpVtxData;
		tmpVtxData.posVtx = controlndcpos[i];
		if (mdl_ref->first == "circle")
		{
			tmpVtxData.clrVtx = vector3D::Vec3(color.r, color.g, color.b);
		}
		else
		{
			tmpVtxData.clrVtx = clr_vtx[i];
		}
		tmpVtxData.txtVtx = texcoord[i];
		tmpVtxData.txtIndex = 6.f;
		vertexData.emplace_back(tmpVtxData);
		testdata.emplace_back(matrix3x3::mat3x3(1.f,0.f,0.f,  0.f, 1.f, 0.f,   0.f, 0.f, 1.f)); // Emplace back a base 1, 1 translation
	}


	basicinstance.headerdata.clear();
	//basicinstance.instancedata.clear(); // Instance stacks up
	basicinstance.ebodata.clear();
	basicinstance.headerdata.insert(basicinstance.headerdata.end(), vertexData.begin(), vertexData.end());
	basicinstance.instancedata.insert(basicinstance.instancedata.end(), testdata.begin(), testdata.end());
	basicinstance.ebodata.insert(basicinstance.ebodata.end(), mdl_ref->second.primitive.begin(), mdl_ref->second.primitive.end());
	basicinstance.vaoid = mdl_ref->second.getVAOid();

	//basicbatch.batchmodel = mdl_ref->second;
	//basicbatch.batchshader = shd_ref->second;

	//std::vector<vector3D::Vec3> clr_vtx
	//{
	//	vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b),
	//	vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b)
	//};
	//int totalframes = 4;
	//int curframe = 4;
	//int textureid = texId; // TO REMOVE CONST
	//if (textureid == 3 || textureid == 4)
	//{
	//	totalframes = 1;
	//	curframe = 1;
	//	if (overlap)
	//	{
	//		if (textureid == 3)
	//		{
	//			textureid = 4;
	//		}
	//	}
	//	else
	//	{
	//		if (textureid == 4)
	//		{
	//			textureid = 3;
	//		}
	//	}
	//}
	//if (textureid == 7 || textureid == 8)
	//{
	//	//std::cout << "Does it enter here\n";
	//	totalframes = totalsprites;
	//	curframe = 1;
	//}
	//std::vector<vector2D::Vec2> texcoord;
	//texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe - 1) / float(totalframes), 0.f)); // Bottom left
	//texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe) / float(totalframes), 0.f)); // Bottom right
	//texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe) / float(totalframes), 1.f)); // Top right
	//texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe - 1) / float(totalframes), 1.f)); // Top left

	//std::vector<Graphics::vertexData> vertexData;
	//for (int i = 0; i < ndc_coords.size(); ++i)
	//{
	//	Graphics::vertexData tmpVtxData;
	//	tmpVtxData.posVtx = ndc_coords[i];
	//	if (mdl_ref->first == "circle")
	//	{
	//		tmpVtxData.clrVtx = vector3D::Vec3(color.r, color.g, color.b);
	//	}
	//	else
	//	{
	//		tmpVtxData.clrVtx = clr_vtx[i];
	//	}
	//	tmpVtxData.txtVtx = texcoord[i];
	//	tmpVtxData.txtIndex = (float)textureid;
	//	vertexData.emplace_back(tmpVtxData);
	//}
	//basicbatch.batchdata.insert(basicbatch.batchdata.end(), vertexData.begin(), vertexData.end());
	//basicbatch.ebodata.insert(basicbatch.ebodata.end(), mdl_ref->second.primitive.begin(), mdl_ref->second.primitive.end());
	//basicbatch.totalindicesize += mdl_ref->second.getPrimitiveCnt();
	//basicbatch.vaoid = mdl_ref->second.getVAOid();
	//basicbatch.vboid = mdl_ref->second.getVBOid();
	//basicbatch.eboid = mdl_ref->second.getEBOid();
	//basicbatch.totalsize += (int)vertexData.size();
	//basicbatch.primtype = mdl_ref->second.getPrimitiveType();
	//basicbatch.totaldrawcnt += mdl_ref->second.getDrawCnt();

	//if (coldebug == true)
	//{
	//	debugbatch.batchmodel = mdl_ref->second;
	//	debugbatch.batchshader = shd_ref->second;
	//	std::vector<vector3D::vec3D> clr_vtxcoldebug;
	//	for (int i = 0; i < 4; i++)
	//	{
	//		if (overlap == true)
	//		{
	//			clr_vtxcoldebug.emplace_back(vector3D::vec3D(0.8f, 0.f, 0.2f)); // red if overlapping
	//		}
	//		else
	//		{
	//			clr_vtxcoldebug.emplace_back(vector3D::vec3D(0.f, 0.2f, 0.7f)); // blue if not overlapping
	//		}
	//	}
	//	std::vector<Graphics::vertexData> colDebugData;
	//	for (int i = 0; i < ndc_coords.size(); ++i)
	//	{
	//		Graphics::vertexData tmpVtxData;
	//		tmpVtxData.posVtx = ndc_coords[i];
	//		if (mdl_ref->first == "circle")
	//		{
	//			tmpVtxData.clrVtx = vector3D::vec3D(1.f, 1.f, 1.f);
	//		}
	//		else
	//		{
	//			tmpVtxData.clrVtx = clr_vtxcoldebug[i];
	//		}
	//		tmpVtxData.txtVtx = texcoord[i];
	//		tmpVtxData.txtIndex = (float)texId; // notexture for coldebug
	//		colDebugData.emplace_back(tmpVtxData);
	//	}

	//	debugbatch.batchdata.insert(debugbatch.batchdata.end(), colDebugData.begin(), colDebugData.end());
	//	debugbatch.ebodata.insert(debugbatch.ebodata.end(), mdl_ref->second.primitive.begin(), mdl_ref->second.primitive.end());
	//	debugbatch.totalindicesize += mdl_ref->second.getPrimitiveCnt();
	//	debugbatch.vaoid = mdl_ref->second.getVAOid();
	//	debugbatch.vboid = mdl_ref->second.getVBOid();
	//	debugbatch.eboid = mdl_ref->second.getEBOid();
	//	debugbatch.totalsize += (int)colDebugData.size();
	//	debugbatch.primtype = mdl_ref->second.getPrimitiveType();
	//	debugbatch.totaldrawcnt += mdl_ref->second.getDrawCnt();
	//}
	//texcoord.clear();
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
	Graphics::camera2d.update(Graphics::Input::ptr_to_window);
	objects["Camera"].update(Graphics::Input::delta_time);

	// update other inputs for physics

	double mousePosX, mousePosY;
	bool mouseClick = false;
	if (Graphics::Input::mousestateLeft)
	{
		Graphics::Input::mousestateLeft = false;
		Graphics::Input::getCursorPos(&mousePosX, &mousePosY);
		mouseClick = true;
	}

	if (Graphics::Input::keystateP)
	{
		movableShape = !movableShape;
		Graphics::Input::keystateP = false;
	}

	if (Graphics::Input::keystateC)
	{
		int tmp = (int)(currentCollision);
		currentCollision = (collisionType)(++tmp % 9);
		Graphics::Input::keystateC = false;
	}
	if (Graphics::Input::keystateM)
	{
		modulate = !modulate;
		std::cout << "M pressed\n";
		Graphics::Input::keystateM = GL_FALSE;
	}
	if (Graphics::Input::keystateB)
	{
		alphablend = !alphablend;
		Graphics::Input::keystateB = GL_FALSE;
	}
	if (Graphics::Input::keystateT)
	{
		textures = !textures;
		std::cout << "T pressed\n";
		Graphics::Input::keystateT = GL_FALSE;
	}

	if (Graphics::Input::keystateG)
	{
		graphicsmode = !graphicsmode;
		Graphics::Input::keystateG = GL_FALSE;
	}
	if (graphicsmode)
	{
		if (Graphics::Input::keystateX)
		{
			coldebug = !coldebug;
			Graphics::Input::keystateX = GL_FALSE;
		}
		if (Graphics::Input::keystateO)
		{
			velocitydirectiondebug = !velocitydirectiondebug;
			Graphics::Input::keystateX = GL_FALSE;
		}
		if (Graphics::Input::keystatePlus)
		{
			std::cout << "INCREASING" << std::endl;
			objects["Banana1"].scaling *= 1.1f;

		}
		if (Graphics::Input::keystateMinus)
		{
			std::cout << "DECREASING" << std::endl;
			objects["Banana1"].scaling /= 1.1f;
		}
		if (Graphics::Input::keystateSquareBracketLeft)
		{
			std::cout << "ROT LEFT" << std::endl;
			//objects["Banana1"].orientation.x += -1.5f;
			objects["Banana1"].orientation.x += (-1.5f * float(M_PI / 180));
			std::cout << "Orientation " << objects["Banana1"].orientation.x << ", " << objects["Banana1"].orientation.y << std::endl;
		}
		if (Graphics::Input::keystateSquareBracketRight)
		{
			std::cout << "ROT RIGHT" << std::endl;
			//objects["Banana1"].orientation.x += 1.5f;
			objects["Banana1"].orientation.x += (1.5f * float(M_PI / 180));
			std::cout << "Orientation " << objects["Banana1"].orientation.x << ", " << objects["Banana1"].orientation.y << std::endl;
		}
	}

	if (Graphics::Input::keystateQ || Graphics::Input::keystateE)
	{
		std::string modelname;
		if (Graphics::Input::keystateQ)
		{
			modelname = "circle";
		}
		else
		{
			modelname = "square";
		}
		for (int j = 0; j < 1; j++)
		{
			std::string tmpobjname = "Banana";
			objectcounter++;
			std::stringstream tmpstream;
			tmpstream << tmpobjname << objectcounter;
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

			std::uniform_real_distribution<float> colour(0.2f, 0.8f);
			float randr = colour(generator);
			float randg = colour(generator);
			float randb = colour(generator);
			vector3D::vec3D tmpcolor = vector3D::vec3D(randr, randg, randb);

			std::uniform_int_distribution<int> texrandom(1, 1);
			float randindex = float(texrandom(generator));
			if (modelname == "circle")
			{
				//std::cout << "Tex id set\n";
				randindex = 3;
				randwidth = 100;
				randheight = 100;
			}
			GLApp::GLObject::gimmeObject(modelname, finalobjname, vector2D::vec2D(randwidth, randwidth), vector2D::vec2D(static_cast<float>(randx), static_cast<float>(randy)), tmpcolor, objectcounter, (int)randindex);
			// Name, type, pos, color, texid, dimension, spritestep, numofsprites, vao, vbo, ebo, shadername
			//createdUnits[objectcounter].Add<Render>(finalobjname, "square", vector2D::vec2D(static_cast<float>(randx), static_cast<float>(randy)), tmpcolor, vector2D::vec2D(randwidth, randheight), models.find(modelname)->second.vaoid, models.find(modelname)->second.vboid, models.find(modelname)->second.eboid, "gam200-shdrpgm");
			//createdUnits[objectcounter].Add<Texture>(randindex, 1, 4, "");
			//createdUnits[objectcounter].Add<Stats>(100);
			//ecs.setEntityName(createdUnits[objectcounter].GetID(), finalobjname);

			Graphics::Input::keystateQ = false;
			Graphics::Input::keystateE = false;
		}
	}
	//check for movement

	//for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
	//{
	//	if (obj1->first == "Banana1")
	//	{
	//		if (!movableShape && obj1->first == "Banana1") // first shape drawn is a box
	//		{
	//			//obj1->second.body.rotate(45.f); // Change for rotation
	//			//float rad{ 45.f / 180.f * M_PI };
	//			//obj1->second.orientation.x = rad;

	//			double destX, destY;
	//			Graphics::Input::getCursorPos(&destX, &destY);

	//			vector2D::vec2D velocity = mouseMovement(obj1->second.modelCenterPos, vector2D::vec2D(static_cast<float>(destX), static_cast<float>(destY)), obj1->second.speed);
	//			//vector2D::vec2D velocity = keyboardMovement(obj1->second.modelCenterPos, obj1->second.speed, stepByStepCollision);
	//			//vector2D::vec2D velocity = keyboardMovement(obj1->second.modelCenterPos, obj1->second.speed, stepByStepCollision);
	//			obj1->second.body.move(velocity);
	//		}
	//		else if (movableShape && obj1->first == "Banana1") // first shape drawn is a circle
	//		{
	//			double destX, destY;
	//			Graphics::Input::getCursorPos(&destX, &destY);

	//			vector2D::vec2D velocity = mouseMovement(obj1->second.modelCenterPos, vector2D::vec2D(static_cast<float>(destX), static_cast<float>(destY)), obj1->second.speed);
	//			obj1->second.body.move(velocity);
	//		}

	//		//obj1->second.overlap = false;
	//		obj1->second.body.transformVertices();
	//		obj1->second.modelCenterPos = obj1->second.body.getPos();
	//	}
	//}
	Render* player = ecs.GetComponent<Render>(player1.GetID());

	bool test{ true };
	for (std::map <std::string, GLObject> ::iterator obj = objects.begin(); obj != objects.end(); ++obj)
	{
		if (player->name == obj->first && mouseClick)
		{
			player->position = vector2D::vec2D((float)mousePosX, (float)mousePosY);
			obj->second.modelCenterPos = player->position;

			generateDijkstraCost(player->position, walls);
			generateFlowField(player->position);
		}

		if (obj->first != "Camera")
		{
			obj->second.update(Graphics::Input::delta_time);

			for (GLuint i = 0; i < obj->second.mdl_ref->second.getPosvtxCnt(); i++)
			{

				obj->second.ndc_coords[i] = obj->second.world_to_ndc_xform * obj->second.worldVertices[i], 1.f;
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
	std::stringstream title;
	title << std::fixed;
	title << std::setprecision(2);
	title << "Bloom";
	title << std::setprecision(2) << " | FPS " << int(Graphics::Input::fps * 100) / 100.0;
	title << " | Camera Position (" << Graphics::camera2d.getCameraObject().modelCenterPos.x << ", " << Graphics::camera2d.getCameraObject().modelCenterPos.y << ")";
	title << " | Window height: " << Graphics::camera2d.getHeight();
	title << " | Collision Type: " << collisionInfo[static_cast<collisionType>(currentCollision)];

	glfwSetWindowTitle(Graphics::Input::ptr_to_window, title.str().c_str());

	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	if (alphablend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	for (std::map <std::string, GLObject>::iterator obj = objects.begin(); obj != objects.end(); ++obj)
	{
		if (obj->first != "Camera")
		{
			//obj->second.draw(); // Comment to stop drawing from object map
		}
	}
	basicinstance.InstanceClear();
	GLApp::entitydraw(); // Comment to stop drawing from ecs

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	basicinstance.InstanceRender(Graphics::textureobjects, entitycounter);
	basicinstance.InstanceClear();
	entitycounter = 0;
	//basicbatch.BatchRender(Graphics::textureobjects); // Renders all objects at once
	//glLineWidth(2.f);
	//debuglinebatch.BatchRender(Graphics::textureobjects);
	//glLineWidth(1.f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//debugbatch.BatchRender(Graphics::textureobjects); // For collision debug
	//basicbatch.BatchClear();
	//debuglinebatch.BatchClear();
	//debugbatch.BatchClear();
	glDisable(GL_BLEND);

	objects["Camera"].draw();
}
/*  _________________________________________________________________________*/
/*! cleanup

@param none
@return none

This function is empty for now
*/
void GLApp::cleanup() 
{
	//basicbatch.BatchDelete();
	//debuglinebatch.BatchDelete();
	//debugbatch.BatchDelete();
	//Graphics::Texture::deleteTexture(Graphics::textureobjects[0]);
	//Graphics::Texture::deleteTexture(Graphics::textureobjects[1]);
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

	GLApp::shdrpgms[shdr_pgm_name] = shdr_pgm;
}

void GLApp::GLObject::gimmeObject(std::string modelname, std::string objname, vector2D::vec2D scale, vector2D::vec2D pos, vector3D::vec3D colour, int id, int texid, int totalsprite)
{
	GLObject tmpObj;
	std::string hi;

	tmpObj.objId = id;
	tmpObj.texId = texid;
	tmpObj.totalsprites = totalsprite;
	if (modelname == "circle")
	{
		//std::cout << "Tex id set\n";
		tmpObj.texId = 3;
	}
	if (modelname == "circle")
	{
		tmpObj.body.createCircleBody(scale.x/2.f, pos, 0.f, false, 0.f, &tmpObj.body);
	}
	else if (modelname == "square")
		tmpObj.body.createBoxBody(scale.x, scale.x, pos, 0.f, false, 0.f, &tmpObj.body);

	tmpObj.color = colour;

	if (modelname == "circle")
		tmpObj.scaling = vector2D::vec2D(tmpObj.body.getRad(), tmpObj.body.getRad());
	else if (modelname == "square")
		tmpObj.scaling = vector2D::vec2D(tmpObj.body.getWidth(), tmpObj.body.getWidth());
	tmpObj.color = colour;
	tmpObj.scaling = scale;
	tmpObj.orientation = vector2D::vec2D(0, 0);
	tmpObj.modelCenterPos = pos;
	tmpObj.speed = 200.f;

	if (shdrpgms.find("gam200-shdrpgm") != shdrpgms.end())
	{
		tmpObj.shd_ref = shdrpgms.find("gam200-shdrpgm");
	}
	else
	{
		insert_shdrpgm("gam200-shdrpgm", "../shaders/instancing.vert", "../shaders/instancing.frag");
		tmpObj.shd_ref = shdrpgms.find("gam200-shdrpgm");
	}

	if (models.find("square") != models.end())
	{
		tmpObj.mdl_ref = models.find("square");
	}
	else
	{
		Graphics::Model Model;
		Model = Model.init("square");
		models["square"] = Model;
		tmpObj.mdl_ref = models.find("square");
	}
	objects[objname] = tmpObj;

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


void GLApp::entitydraw()
{
	std::vector<EntityID> entities = ecs.getEntities();
	for (int i = 0; i < entities.size(); i++)
	{
		if (ecs.GetComponent<Render>(entities[i]) == nullptr) // Added check for NIL objects
		{
			continue;
		}

		// Below code (2 lines) is for fow
		if (!ecs.GetComponent<Render>(entities[i])->render)
			continue;

		Render* curobj = ecs.GetComponent<Render>(entities[i]);

		GLSLShader shaderid;
		if (shdrpgms.find(curobj->shaderName) != shdrpgms.end())
		{
			shaderid = shdrpgms.find(curobj->shaderName)->second;
		}
		else
		{
			insert_shdrpgm(curobj->shaderName, "../shaders/instancing.vert", "../shaders/instancing.frag");
			shaderid = shdrpgms.find(curobj->shaderName)->second;
		}

		basicinstance.instanceshader = shaderid;

		std::vector<vector3D::Vec3> clr_vtx
		{
			vector3D::Vec3(curobj->color.r, curobj->color.g, curobj->color.b), vector3D::Vec3(curobj->color.r, curobj->color.g, curobj->color.b),
			vector3D::Vec3(curobj->color.r, curobj->color.g, curobj->color.b), vector3D::Vec3(curobj->color.r, curobj->color.g, curobj->color.b)
		};

		std::vector<vector2D::Vec2> texcoord;
		texcoord.emplace_back(vector2D::Vec2(0.f, 0.f)); // Bottom left
		texcoord.emplace_back(vector2D::Vec2(1.f, 0.f)); // Bottom right
		texcoord.emplace_back(vector2D::Vec2(1.f, 1.f)); // Top right
		texcoord.emplace_back(vector2D::Vec2(0.f, 1.f)); // Top left

		Graphics::vertexData tmpHeaderData;
		std::vector<Graphics::vertexData> vertexData;
		std::vector<matrix3x3::mat3x3> testdata;

		std::vector<vector2D::vec2D> poscoord; // CALCULATE POSITION FROM CENTER
		float halfwidth = curobj->dimension.x / 2.f;
		float halfheight = curobj->dimension.y / 2.f;
		poscoord.emplace_back(vector2D::vec2D(curobj->position.x - halfwidth, curobj->position.y - halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobj->position.x + halfwidth, curobj->position.y - halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobj->position.x + halfwidth, curobj->position.y + halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobj->position.x - halfwidth, curobj->position.y + halfheight));


		std::vector <vector2D::vec2D> ndccoord;
		for (int i = 0; i < poscoord.size(); i++)
		{
			ndccoord.emplace_back(poscoord[i]);
		}
		
		for (int j = 0; j < ndccoord.size(); ++j)
		{
			Graphics::vertexData tmpVtxData;
			//tmpVtxData.posVtx = ndccoord[i];

			tmpVtxData.clrVtx = clr_vtx[j];
			tmpVtxData.posVtx = models["square"].model_coords[j];
			//std::cout << "Position " << tmpVtxData.posVtx.x << ", " << tmpVtxData.posVtx.y << std::endl;
			tmpVtxData.txtVtx = texcoord[j];
			tmpVtxData.txtIndex = 6.f;
			vertexData.emplace_back(tmpVtxData);
			//std::cout << "Start of position before matrix mult " << tmpVtxData.posVtx.x << ", " << tmpVtxData.posVtx.y << std::endl;
			//std::cout << "End NDC for entity draw " << testend.x << ", " << testend.y << std::endl;
		}

		matrix3x3::mat3x3 translate = Transform::createTranslationMat(vector2D::vec2D(curobj->position.x, curobj->position.y));
		matrix3x3::mat3x3 scale = Transform::createScaleMat(vector2D::vec2D(curobj->dimension.x, curobj->dimension.y));
		matrix3x3::mat3x3 rot = Transform::createRotationMat(0.f);

		matrix3x3::mat3x3 model_to_world = translate * rot * scale;


		matrix3x3::mat3x3 world_to_ndc_xform = Graphics::camera2d.getWorldtoNDCxForm();

		matrix3x3::mat3x3 model_to_ndc_xformnotglm = world_to_ndc_xform * model_to_world;

		matrix3x3::mat3x3 model_to_ndc_xform = matrix3x3::mat3x3
		(
			model_to_ndc_xformnotglm.m[0], model_to_ndc_xformnotglm.m[3], model_to_ndc_xformnotglm.m[6],
			model_to_ndc_xformnotglm.m[1], model_to_ndc_xformnotglm.m[4], model_to_ndc_xformnotglm.m[7],
			model_to_ndc_xformnotglm.m[2], model_to_ndc_xformnotglm.m[5], model_to_ndc_xformnotglm.m[8]
		);
		
		testdata.emplace_back(model_to_ndc_xform); // Emplace back a base 1, 1 translation

		basicinstance.headerdata.clear();
		//basicinstance.instancedata.clear(); // Instance stacks up
		basicinstance.ebodata.clear();
		basicinstance.headerdata.insert(basicinstance.headerdata.end(), vertexData.begin(), vertexData.end());
		basicinstance.instancedata.insert(basicinstance.instancedata.end(), testdata.begin(), testdata.end());
		basicinstance.ebodata.insert(basicinstance.ebodata.end(), models["square"].primitive.begin(), models["square"].primitive.end());
		basicinstance.vaoid = models["square"].getVAOid();
		entitycounter++;
		
		testdata.clear();

		//if (ecs.GetComponent<Render>(entities[i]) == nullptr) // Added check for NIL objects
		//{
		//	continue;
		//}

		//Render* curobj = ecs.GetComponent<Render>(entities[i]);
		//int texid = 0;
		//Texture* curobjTexture = ecs.GetComponent<Texture>(entities[i]);
		//if (ecs.GetComponent<Texture>(entities[i]) != nullptr)
		//{
		//	texid = curobjTexture->textureID;
		//}
		//basicbatch.batchmodel = models["square"];
		//GLSLShader shaderid;
		//if (shdrpgms.find(curobj->shaderName) != shdrpgms.end())
		//{
		//	shaderid = shdrpgms.find(curobj->shaderName)->second;
		//}
		//else
		//{
		//	insert_shdrpgm(curobj->shaderName, "../shaders/gam200.vert", "../shaders/gam200.frag");
		//	shaderid = shdrpgms.find(curobj->shaderName)->second;
		//}
		//basicbatch.batchshader = shaderid;

		//std::vector<vector3D::vec3D> clr_vtx
		//{
		//	vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b), vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b),
		//	vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b), vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b)
		//};

		//int numofsprites = 0;
		//int spritestep = 0;
		//if (texid)
		//{
		//	numofsprites = curobjTexture->numberOfSprites;
		//	spritestep = curobjTexture->spriteStep;
		//	if (curobjTexture->textureID == 3 || curobjTexture->textureID == 4)
		//	{
		//		//if (overlapobj->overlap)
		//		//{
		//		//	if (curobj->textureID == 3)
		//		//	{
		//		//		curobj->textureID = 4;
		//		//	}
		//		//}
		//		//else
		//		//{
		//		//	if (curobj->textureID == 4)
		//		//	{
		//		//		curobj->textureID = 3;
		//		//	}
		//		//}
		//	}
		//}
		//std::vector<vector2D::vec2D> texcoord;
		//texcoord.emplace_back(vector2D::vec2D(0.f + float(spritestep - 1) / float(numofsprites), 0.f));
		//texcoord.emplace_back(vector2D::vec2D(0.f + float(spritestep) / float(numofsprites), 0.f));
		//texcoord.emplace_back(vector2D::vec2D(0.f + float(spritestep) / float(numofsprites), 1.f));
		//texcoord.emplace_back(vector2D::vec2D(0.f + float(spritestep - 1) / float(numofsprites), 1.f));

		//std::vector<vector2D::vec2D> poscoord; // CALCULATE POSITION FROM CENTER
		//float halfwidth = curobj->dimension.x / 2.f;
		//float halfheight = curobj->dimension.y / 2.f;
		//poscoord.emplace_back(vector2D::vec2D(curobj->position.x - halfwidth, curobj->position.y - halfheight));
		//poscoord.emplace_back(vector2D::vec2D(curobj->position.x + halfwidth, curobj->position.y - halfheight));
		//poscoord.emplace_back(vector2D::vec2D(curobj->position.x + halfwidth, curobj->position.y + halfheight));
		//poscoord.emplace_back(vector2D::vec2D(curobj->position.x - halfwidth, curobj->position.y + halfheight));

		//matrix3x3::mat3x3 world_to_ndc_notglm = Graphics::camera2d.getWorldtoNDCxForm();
		//matrix3x3::mat3x3 world_to_ndc_xform = matrix3x3::mat3x3
		//(
		//	world_to_ndc_notglm.m[0], world_to_ndc_notglm.m[1], world_to_ndc_notglm.m[2],
		//	world_to_ndc_notglm.m[3], world_to_ndc_notglm.m[4], world_to_ndc_notglm.m[5],
		//	world_to_ndc_notglm.m[6], world_to_ndc_notglm.m[7], world_to_ndc_notglm.m[8]
		//);

		//std::vector <vector2D::vec2D> ndccoord;
		//for (int i = 0; i < poscoord.size(); i++)
		//{
		//	ndccoord.emplace_back(world_to_ndc_xform * poscoord[i]);
		//}
		//std::vector<Graphics::vertexData> vertexData;
		//for (int i = 0; i < 4; ++i)
		//{
		//	Graphics::vertexData tmpVtxData;
		//	tmpVtxData.posVtx = ndccoord[i];
		//	tmpVtxData.clrVtx = curobj->color;

		//	tmpVtxData.txtVtx = texcoord[i];
		//	vertexData.emplace_back(tmpVtxData);
		//}

		//basicbatch.batchdata.insert(basicbatch.batchdata.end(), vertexData.begin(), vertexData.end());
		//basicbatch.ebodata.insert(basicbatch.ebodata.end(), models["square"].primitive.begin(), models["square"].primitive.end());
		//basicbatch.totalindicesize += models["square"].getPrimitiveCnt();
		//basicbatch.vaoid = models["square"].getVAOid();
		//basicbatch.vboid = models["square"].getVBOid();
		//basicbatch.eboid = models["square"].getEBOid();
		//basicbatch.totalsize += (int)vertexData.size();
		//basicbatch.primtype = models["square"].getPrimitiveType();
		//basicbatch.totaldrawcnt += models["square"].getDrawCnt();

		//texcoord.clear();
		//ndccoord.clear();
		//vertexData.clear();

		//if (velocitydirectiondebug == true)
		//{


		//	if (ecs.GetComponent<Movement>(entities[i]) == nullptr) // Added check for NIL objects
		//	{
		//		continue;
		//	}
		//	Movement* objmovement = ecs.GetComponent<Movement>(entities[i]);
		//	debuglinebatch.batchmodel = models["line"];
		//	debuglinebatch.batchshader = shaderid;

		//	std::vector<vector3D::vec3D> debugline_clrvtx
		//	{
		//		//vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b), vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b)
		//		vector3D::vec3D(0.2f, 0.f, 0.f), vector3D::vec3D(0.2f, 0.f, 0.f)
		//	};
		//	std::vector<vector2D::vec2D> debugline_texcoord
		//	{
		//		vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f)
		//	};

		//	std::vector<vector2D::vec2D> debugline_poscoord
		//	{
		//		vector2D::vec2D(curobj->position.x, curobj->position.y),
		//		vector2D::vec2D(curobj->position.x + (objmovement->velocity.x * 25), curobj->position.y + (objmovement->velocity.y * 25))
		//	}; // CALCULATE POSITION FROM CENTER

		//	std::vector <vector2D::vec2D> debugline_ndccoord;
		//	for (int i = 0; i < poscoord.size(); i++)
		//	{
		//		debugline_ndccoord.emplace_back(world_to_ndc_xform * debugline_poscoord[i]);

		//	}
		//	std::vector<Graphics::vertexData> debugline_vertexData;
		//	for (int i = 0; i < 2; ++i)
		//	{
		//		Graphics::vertexData tmpVtxData;
		//		tmpVtxData.posVtx = debugline_ndccoord[i];
		//		tmpVtxData.clrVtx = debugline_clrvtx[i];

		//		tmpVtxData.txtVtx = debugline_texcoord[i];
		//		tmpVtxData.txtIndex = 0;
		//		debugline_vertexData.emplace_back(tmpVtxData);
		//	}

		//	debuglinebatch.batchdata.insert(debuglinebatch.batchdata.end(), debugline_vertexData.begin(), debugline_vertexData.end());
		//	debuglinebatch.ebodata.insert(debuglinebatch.ebodata.end(), models["line"].primitive.begin(), models["line"].primitive.end());
		//	debuglinebatch.totalindicesize += models["line"].getPrimitiveCnt();
		//	debuglinebatch.vaoid = models["line"].getVAOid();
		//	debuglinebatch.vboid = models["line"].getVBOid();
		//	debuglinebatch.eboid = models["line"].getEBOid();
		//	debuglinebatch.totalsize += (int)debugline_vertexData.size();
		//	debuglinebatch.primtype = GL_LINES;
		//	debuglinebatch.totaldrawcnt += models["line"].getDrawCnt();

		//	debugline_ndccoord.clear();
		//	debugline_vertexData.clear();
		//}
	}
}