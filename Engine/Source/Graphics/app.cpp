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
#include "framebuffer.h"

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

extern std::vector<FormationManager> formationManagers;
extern Entity player1;
extern std::vector<Entity> walls;

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
std::map<std::string, GLSLShader> GLApp::shdrpgms; // define shaders

std::map<std::string, ModelNS::Model> models; // define models

std::unordered_map<GLApp::collisionType, std::string> GLApp::collisionInfo;

RenderNS::BatchRenderer basicbatch; // Batch render object
RenderNS::BatchRenderer debugbatch; // Batch render object for collision debug
RenderNS::BatchRenderer debuglinebatch; // Batch render object for collision debug

RenderNS::InstancedRenderer basicinstance; // Instance render object for collision debug

FrameBufferNS::frameBuffer mainFrame; // Texture obj

//Graphics::Texture texobj;

//std::vector<Graphics::Texture> Graphics::textureobjects;

CameraNS::Camera2D camera2d;

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

vector2D::vec2D readConfig(std::string path);

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
	vector2D::vec2D screensize = readConfig("config.xml");  // Read from config
	if (!Graphics::Input::init(screensize.x, screensize.y, "Bloom")) // Screensize.x is width, Screensize.y is height
	{
		std::cout << "Unable to create OpenGL context" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	mainFrame.createFrameBuffer(); // Create frame buffer

	ModelNS::initModels(models); // Initialize line and square models

	GLApp::modulate = false;
	GLApp::alphablend = true;
	GLApp::textures = true;
	GLApp::coldebug = false;
	GLApp::objectcounter = 0;

	glClearColor(0.2f, 1.f, 0.3f, 1.f);	// clear colorbuffer with RGBA value in glClearColor
	glViewport(0, 0, Graphics::Input::screenwidth, Graphics::Input::screenheight);

	TextureNS::Texture::CreateandLoadTexture(TextureNS::textureobjects); // Create and load textures

	camera2d.init(Graphics::Input::ptr_to_window, vector2D::vec2D(0, 0), vector2D::vec2D(0, 0)); // Initialize camera

	if (shdrpgms.find("framebuffer-shdrpgm") != shdrpgms.end())
	{
		mainFrame.frameshader = shdrpgms.find("framebuffer-shdrpgm")->second;
	}
	else
	{
		insert_shdrpgm("framebuffer-shdrpgm", "../shaders/framebuffer.vert", "../shaders/framebuffer.frag");
		mainFrame.frameshader = shdrpgms.find("framebuffer-shdrpgm")->second;
	}

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
	//std::cout << "cam2d camworld bef update " << CameraNS::camera2d.camworld_to_ndc_xform.m[0] << std::endl;
	camera2d.update(Graphics::Input::ptr_to_window);
	//std::cout << "cam2d camworld aft update " << CameraNS::camera2d.camworld_to_ndc_xform.m[0] << std::endl;
	//objects["Camera"].update(Graphics::Input::delta_time);

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
		}
		if (Graphics::Input::keystateMinus)
		{
			std::cout << "DECREASING" << std::endl;
		}
		if (Graphics::Input::keystateSquareBracketLeft)
		{
			std::cout << "ROT LEFT" << std::endl;
		}
		if (Graphics::Input::keystateSquareBracketRight)
		{
			std::cout << "ROT RIGHT" << std::endl;
		}
	}

	if (Graphics::Input::keystateQ || Graphics::Input::keystateE)
	{

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
	BaseInfo* player = ecs.GetComponent<BaseInfo>(player1.GetID());

	bool test{ true };

	if (mouseClick)
	{
		player->position = vector2D::vec2D((float)mousePosX, (float)mousePosY);

		for (int i = 0; i < formationManagers.size(); ++i)
		{
			formationManagers[i].target = player->position;
			formationManagers[i].updateReached();
		}

		generateDijkstraCost(player->position, walls);
		generateFlowField(player->position);
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
	title << " | Camera Position (" << camera2d.position.x << ", " << camera2d.position.y << ")";
	title << " | Window height: " << camera2d.getHeight();
	title << " | Collision Type: " << collisionInfo[static_cast<collisionType>(currentCollision)];

	glfwSetWindowTitle(Graphics::Input::ptr_to_window, title.str().c_str());

	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	if (alphablend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	basicinstance.InstanceClear();
	GLApp::entitydraw(basicinstance); // Comment to stop drawing from ecs

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindFramebuffer(GL_FRAMEBUFFER, mainFrame.framebuffer);
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	basicinstance.InstanceRender(TextureNS::textureobjects, entitycounter);
	mainFrame.drawFrameBuffer();
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

	//objects["Camera"].draw();
}
/*  _________________________________________________________________________*/
/*! cleanup

@param none
@return none

This function is empty for now
*/
void GLApp::cleanup() 
{
	mainFrame.delFrameBuffer();
	TextureNS::Texture::deleteTexture(TextureNS::textureobjects);
	//Graphics::Texture::deleteTexture(Graphics::textureobjects[1]);
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


void GLApp::entitydraw(RenderNS::InstancedRenderer& instanceobj)
{
	std::vector<EntityID> entities = ecs.getEntities();
	for (int i = 0; i < entities.size(); i++)
	{
		if (ecs.GetComponent<Render>(entities[i]) == nullptr) // Added check for NIL objects
		{
			continue;
		}

		BaseInfo* curobjBaseInfo = ecs.GetComponent<BaseInfo>(entities[i]);

		// Below code (2 lines) is for fow
		if (!ecs.GetComponent<Render>(entities[i])->render)
			continue;


		int texid{};
		Texture* curobjTexture = ecs.GetComponent<Texture>(entities[i]);
		if (ecs.GetComponent<Texture>(entities[i]) != nullptr)
		{
			texid = curobjTexture->textureID;
			//std::cout << " this is texid: "<< ecs.GetComponent <Render>(entities[i])->name << " " << texid << std::endl;
		}

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

		instanceobj.instanceshader = shaderid;

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

		ModelNS::modelVtxData tmpHeaderData;
		std::vector<ModelNS::modelVtxData> vertexData;
		std::vector<matrix3x3::mat3x3> testdata;

		std::vector<vector2D::vec2D> poscoord; // CALCULATE POSITION FROM CENTER
		float halfwidth = curobjBaseInfo->dimension.x / 2.f;
		float halfheight = curobjBaseInfo->dimension.y / 2.f;
		poscoord.emplace_back(vector2D::vec2D(curobjBaseInfo->position.x - halfwidth, curobjBaseInfo->position.y - halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobjBaseInfo->position.x + halfwidth, curobjBaseInfo->position.y - halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobjBaseInfo->position.x + halfwidth, curobjBaseInfo->position.y + halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobjBaseInfo->position.x - halfwidth, curobjBaseInfo->position.y + halfheight));


		std::vector <vector2D::vec2D> ndccoord;
		for (int i = 0; i < poscoord.size(); i++)
		{
			ndccoord.emplace_back(poscoord[i]);
		}
		
		for (int j = 0; j < ndccoord.size(); ++j)
		{
			ModelNS::modelVtxData tmpVtxData;
			//tmpVtxData.posVtx = ndccoord[i];

			tmpVtxData.clrVtx = clr_vtx[j];
			tmpVtxData.posVtx = models["square"].model_coords[j];
			//std::cout << "Position " << tmpVtxData.posVtx.x << ", " << tmpVtxData.posVtx.y << std::endl;
			tmpVtxData.txtVtx = texcoord[j];
			tmpVtxData.txtIndex = texid;
			vertexData.emplace_back(tmpVtxData);
			//std::cout << "Start of position before matrix mult " << tmpVtxData.posVtx.x << ", " << tmpVtxData.posVtx.y << std::endl;
			//std::cout << "End NDC for entity draw " << testend.x << ", " << testend.y << std::endl;
		}

		matrix3x3::mat3x3 translate = Transform::createTranslationMat(vector2D::vec2D(curobjBaseInfo->position.x, curobjBaseInfo->position.y));
		matrix3x3::mat3x3 scale = Transform::createScaleMat(vector2D::vec2D(curobjBaseInfo->dimension.x * 2.5f, curobjBaseInfo->dimension.y * 2.5f));
		matrix3x3::mat3x3 rot = Transform::createRotationMat(0.f);

		matrix3x3::mat3x3 model_to_world = translate * rot * scale;


		matrix3x3::mat3x3 world_to_ndc_xform = camera2d.getWorldtoNDCxForm();

		matrix3x3::mat3x3 model_to_ndc_xformnotglm = world_to_ndc_xform * model_to_world;

		matrix3x3::mat3x3 model_to_ndc_xform = matrix3x3::mat3x3
		(
			//model_to_ndc_xformnotglm.m[0], model_to_ndc_xformnotglm.m[3], model_to_ndc_xformnotglm.m[6],
			//model_to_ndc_xformnotglm.m[1], model_to_ndc_xformnotglm.m[4], model_to_ndc_xformnotglm.m[7],
			//model_to_ndc_xformnotglm.m[2], model_to_ndc_xformnotglm.m[5], texid
			model_to_ndc_xformnotglm.m[0], curobj->color.r, curobj->color.g,
			curobj->color.b, model_to_ndc_xformnotglm.m[4], model_to_ndc_xformnotglm.m[7],
			model_to_ndc_xformnotglm.m[2], model_to_ndc_xformnotglm.m[5], texid

		);
		
		testdata.emplace_back(model_to_ndc_xform); // Emplace back a base 1, 1 translation

		instanceobj.headerdata.clear();
		//instanceobj.instancedata.clear(); // Instance stacks up
		instanceobj.ebodata.clear();
		instanceobj.headerdata.insert(instanceobj.headerdata.end(), vertexData.begin(), vertexData.end());
		instanceobj.instancedata.insert(instanceobj.instancedata.end(), testdata.begin(), testdata.end());
		instanceobj.ebodata.insert(instanceobj.ebodata.end(), models["square"].primitive.begin(), models["square"].primitive.end());
		instanceobj.vaoid = models["square"].getVAOid();
		entitycounter++;
		
		testdata.clear();

	}
}

vector2D::vec2D readConfig(std::string path)
{
	std::fstream myfile;
	myfile.open(path);
	vector2D::vec2D dimension{};
	myfile >> dimension.x;
	myfile >> dimension.y;
	return dimension;
}