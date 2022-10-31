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
#include "Font.h";

extern std::vector<FormationManager> formationManagers;
extern Entity player1;
extern std::vector<Entity> walls;

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
std::map<std::string, GLSLShader> App::shdrpgms; // define shaders

std::map<std::string, ModelNS::Model> models; // define models

std::unordered_map<App::collisionType, std::string> App::collisionInfo;

std::vector<RenderNS::InstancedRenderer> InstanceContainer; // Instance render object for collision debug

FrameBufferNS::frameBuffer mainFrame; // Texture obj

CameraNS::Camera2D camera2d;

App::collisionType App::currentCollision;
bool App::movableShape;
//std::list <std::pair<int, partitionObj>> partitionStorage;

bool App::coldebug;
bool App::velocitydirectiondebug;
bool App::graphicsmode;

vector2D::vec2D readConfig(std::string path);

/*! App::init

@param none
@return none

This function is called once on initialization to initialize values and call setup functions
It uses OpenGL functions such as:
glClearColor and glViewport to initialize the app
*/

void App::init()
{
	InstanceContainer.resize(2);
	vector2D::vec2D screensize = readConfig("config.xml");  // Read from config
	if (!Graphics::Input::init((GLint)screensize.x, (GLint)screensize.y, "Bloom")) // Screensize.x is width, Screensize.y is height
	{
		std::cout << "Unable to create OpenGL context" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	mainFrame.createFrameBuffer(); // Create frame buffer

	ModelNS::initModels(models); // Initialize line and square models

	App::coldebug = false;

	glClearColor(0.2f, 1.f, 0.3f, 1.f);	// clear colorbuffer with RGBA value in glClearColor
	glViewport(0, 0, Graphics::Input::screenwidth, Graphics::Input::screenheight);
	std::vector<std::string> tmp{};
	TextureNS::Texture::CreateandLoadTexture(TextureNS::textureobjects, tmp); // Create and load textures

	camera2d.init(Graphics::Input::ptr_to_window, vector2D::vec2D(0, 0), vector2D::vec2D(0, 0)); // Initialize camera

	insertallshdrs(shdrpgms);
	mainFrame.frameshader = shdrpgms["framebuffershader"];

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
/*! App::update

@param none
@return none

This function updates the polygon rasterization mode when 'P' is pressed,
it also spawns new objects until reaching the maximum object limit, where it will
despawn the oldest objects, and respawn objects again once no objects are left.
*/
void App::update()
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
		//Graphics::Input::mousestateLeft = false;
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
		std::cout << "M pressed\n";
		Graphics::Input::keystateM = GL_FALSE;
	}
	if (Graphics::Input::keystateB)
	{
		Graphics::Input::keystateB = GL_FALSE;
	}
	if (Graphics::Input::keystateT)
	{
		std::cout << "T pressed\n";
		Graphics::Input::keystateT = GL_FALSE;
	}

	if (Graphics::Input::keystateG)
	{
		graphicsmode = !graphicsmode;
		Graphics::Input::keystateG = GL_FALSE;
	}
	BaseInfo* player = ecs.GetComponent<BaseInfo>(player1.GetID());
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
			if (player->dimension.x <= 150.f && player->dimension.y <= 150.f)
			{
				player->dimension += vector2D::vec2D(5.f, 5.f);
			}
			std::cout << "INCREASING" << std::endl;
		}
		if (Graphics::Input::keystateMinus)
		{
			if (player->dimension.x >= 20.f && player->dimension.y >= 20.f)
			{
				player->dimension -= vector2D::vec2D(5.f, 5.f);
			}
			std::cout << "DECREASING" << std::endl;
		}
		if (Graphics::Input::keystateSquareBracketLeft)
		{
			//if (player->orientation.y <= (5.f * Graphics::Input::delta_time)) // Uncomment to set limit
			{
				player->orientation.y -= (120.f * (float)Graphics::Input::delta_time); // 1.f * dt for constant rotation, 120.f * dt for fixed
			}
			std::cout << "ROT LEFT" << std::endl;
		}
		if (Graphics::Input::keystateSquareBracketRight)
		{
			//if (player->orientation.y >= (-5.f * Graphics::Input::delta_time)) // Uncomment to set limit
			{
				player->orientation.y += (120.f * (float)Graphics::Input::delta_time); // 1.f * dt for constant rotation, 120.f * dt for fixed
			}
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

	//bool test{ true };

		//	for (int i = 0; i < formationManagers.size(); ++i)
		//	{
		//		formationManagers[i].target = player->position;
		//		formationManagers[i].updateReached();
		//	}

		//	generateDijkstraCost(player->position, walls);
		//	generateFlowField(player->position);
		//}


	//if (mouseClick)
	//{
	//	player->position = vector2D::vec2D((float)mousePosX, (float)mousePosY);

	//	for (int i = 0; i < formationManagers.size(); ++i)
	//	{
	//		formationManagers[i].target = player->position;
	//		formationManagers[i].updateReached();
	//	}

	//	generateDijkstraxCost(player->position, walls);
	//	generateFlowField(player->position);
	//}
}

/*  _________________________________________________________________________*/
/*! App::draw

@param none
@return none

This function is called once per frame to set the title of the window,
update the rendering mode and renders all objects out onto the viewport
It uses OpenGL functions such as:
glClear to set the color buffer bit, glfwSetWindowTitle to set the window title
*/
void App::draw()
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindFramebuffer(GL_FRAMEBUFFER, mainFrame.framebuffer);
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	std::vector<GLenum> primtypestorage;
	primtypestorage.emplace_back(GL_TRIANGLES);
	if (coldebug == true)
	{
		primtypestorage.emplace_back(GL_LINE_STRIP);
	}

	RenderNS::DrawFunc(InstanceContainer, mainFrame, shdrpgms["instanceshader"], models, TextureNS::textureobjects, primtypestorage);
	//glBindFramebuffer(GL_FRAMEBUFFER, mainFrame.framebuffer);
	//RenderNS::DrawFunc(InstanceContainer[1], mainFrame, shdrpgms["instanceshader"], models, TextureNS::textureobjects, GL_LINE_STRIP);

	glDisable(GL_BLEND);
}
/*  _________________________________________________________________________*/
/*! cleanup

@param none
@return none

This function is empty for now
*/
void App::cleanup()
{
	mainFrame.delFrameBuffer();
	TextureNS::Texture::deleteTexture(textureobjects);
	//Graphics::Texture::deleteTexture(Graphics::textureobjects[1]);
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