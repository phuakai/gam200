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


#include <app.h>
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
#include <transform.h>
#include <random>
#include <physicsRigidBody.h>
#include <physicsPartition.h>
#include "ECS.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdint.h>
#include <camera.h>
#include <iomanip>
#include "physicsPartition.h"
#include "serialization.h"
#include "pathfinding.h"

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
std::map<std::string, GLSLShader> GLApp::shdrpgms; // define shaders

std::map<std::string, Graphics::Model> models; // define models

std::map<std::string, GLApp::GLObject> GLApp::objects; // define objects

std::unordered_map<GLApp::collisionType, std::string> GLApp::collisionInfo;

Graphics::BatchRenderer basicbatch; // Batch render object
Graphics::BatchRenderer debugbatch; // Batch render object for collision debug
Graphics::BatchRenderer debuglinebatch; // Batch render object for collision debug


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

ECS ecs;

Entity player1;
std::vector<Entity> walls(30);
std::vector<Entity> enemyUnits(2500);
//std::vector<Entity> createdUnits(100); // precreated empty entities

System<Texture> textureSystem(ecs, 1);
System<Movement, Render> system1(ecs, 2);

extern int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
float timer;
float animationTimer;

bool show_demo_window;
bool show_another_window;
ImVec4 clear_color;

RTTR_REGISTRATION{
	rttr::registration::class_<vector2D::vec2D>("vector2D::vec2D")
		.constructor<vector2D::vec2D>()
		.property("m", &vector2D::vec2D::m)
		.property("x", &vector2D::vec2D::x)
		.property("y", &vector2D::vec2D::y);

	rttr::registration::class_<vector3D::vec3D>("vector3D::vec3D")
		.constructor<vector3D::vec3D>()
		.property("m", &vector3D::vec3D::m)
		.property("x", &vector3D::vec3D::x)
		.property("y", &vector3D::vec3D::y)
		.property("z", &vector3D::vec3D::z)
		.property("r", &vector3D::vec3D::r)
		.property("g", &vector3D::vec3D::g)
		.property("b", &vector3D::vec3D::b);


	rttr::registration::class_<Render>("Render")
		.property("name", &Render::name)
		.property("type", &Render::type)
		.property("position", &Render::position)
		.property("color", &Render::color)
		.property("dimension", &Render::dimension)
		.property("vaoID", &Render::vaoID)
		.property("vboID", &Render::vboID)
		.property("eboID", &Render::eboID)
		.property("shaderName", &Render::shaderName);

	rttr::registration::class_<Texture>("Texture")
		.property("textureID", &Texture::textureID)
		.property("textureName", &Texture::textureName)
		.property("spriteStep", &Texture::spriteStep)
		.property("numberOfSprites", &Texture::numberOfSprites);

	rttr::registration::class_<Movement>("Movement")
		.property("velocity", &Movement::velocity)
		.property("target", &Movement::target)
		.property("force", &Movement::force)
		.property("speed", &Movement::speed)
		.property("collisionFlag", &Movement::collisionFlag);

	//rttr::registration::class_<Sprite>("Sprite")

	rttr::registration::class_<Stats>("Stats")
		.property("health", &Stats::health);
}

/*  _________________________________________________________________________*/
/*! wallGeneration

@param positionX Node x position of the wall in the grid
@param positionY Node y position of the wall in the grid
@return none

This function is a helper function to generate walls in the game for 
collision and pathfinding checks.
*/
void wallGeneration(int positionX, int positionY)
{
	quadObj entity;
	static int count = 0;
	vector2D::vec2D position = vector2D::vec2D(positionX, positionY) * (1000 / MAX_GRID_X) + vector2D::vec2D(-500, -500) + vector2D::vec2D(1000 / MAX_GRID_X / 2, 1000 / MAX_GRID_Y / 2);
	walls[count].Add<Render>("wall" + std::to_string(count), "square", position, vector3D::vec3D(0, 0, 0), vector2D::vec2D((1000 / MAX_GRID_X), (1000 / MAX_GRID_Y)), 0, 0, 0, "gam200-shdrpgm");
	walls[count].Add<Movement>(vector2D::vec2D(0, 0), vector2D::vec2D(0, 0), 0, 0, 0, vector2D::vec2D(0, 0));
	ecs.setEntityName(walls[count].GetID(), "wall" + std::to_string(count));

	entity.key = walls[count].GetID();
	entity.position = ecs.GetComponent<Render>(walls[count].GetID())->position;
	mainTree.insertSuccessfully(entity);

	count++;
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

	Graphics::createTextureVector(Graphics::textureobjects, 9);

	glClearColor(0.3f, 1.f, 1.f, 1.f);						// clear colorbuffer with RGBA value in glClearColor
	glViewport(0, 0, Graphics::Input::screenwidth, Graphics::Input::screenheight);

	Graphics::Texture::loadTexture("../images/BaseTree.png", Graphics::textureobjects[0]); // BaseTree
	Graphics::Texture::loadTexture("../images/BaseTree.png", Graphics::textureobjects[1]); // 
	Graphics::Texture::loadTexture("../images/GrassMap.png", Graphics::textureobjects[2]); // Grass map
	Graphics::Texture::loadTexture("../images/BlueCircle.png", Graphics::textureobjects[3]); // Blue Circle
	Graphics::Texture::loadTexture("../images/YellowCircle.png", Graphics::textureobjects[4]); // Yellow Circle
	Graphics::Texture::loadTexture("../images/DragBox.png", Graphics::textureobjects[5]); // Drag Box
	Graphics::Texture::loadTexture("../images/Unit_tank_front.png", Graphics::textureobjects[6]); // Enemy unit
	Graphics::Texture::loadTexture("../images/Map_sprite1.png", Graphics::textureobjects[7]); // BG1
	Graphics::Texture::loadTexture("../images/Map_sprite2.png", Graphics::textureobjects[8]); // BG2


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

	// ======================================================================================================================================
	// IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(Graphics::Input::ptr_to_window, true);
	ImGui_ImplOpenGL3_Init(NULL);

	show_demo_window = true;
	show_another_window = false;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// quad tree stuff
	mainTree.createQuadTree(vector2D::vec2D(0, 0), 500, 500, nullptr);

	// ======================================================================================================================================
	// ECS: Register structs as components 
	ecs.RegisterComponent<Render>("Render");
	ecs.RegisterComponent<Texture>("Texture");
	ecs.RegisterComponent<Movement>("Movement");
	//ecs.RegisterComponent<Sprite>("Sprite");
	ecs.RegisterComponent<Stats>("Stats");

	// ECS: Adding components into Entities
	// Render: name, type, position, color, dimension, vaoID, vboID, eboID, shaderName(?)
	player1.Add<Render>("player1", "square", vector2D::vec2D(-200.f, 0.f), vector3D::vec3D(0.3f, 0.3f, 0.7f), vector2D::vec2D(20.f, 20.f), 0, 0, 0, "gam200-shdrpgm");
	// velocity, target, force, speed
	//player1.Add<Movement>(vector2D::vec2D(0, 0), vector2D::vec2D(0, 0), 10, 2);
	//player1.Add<Texture>(0, 1, 1, "none");
	player1.Add<Stats>(100);
	ecs.setEntityName(player1.GetID(), "Mouse Click");												// may not need this after rttr

	EntityID playerID = player1.GetID();
	GLApp::GLObject::gimmeObject(ecs.GetComponent<Render>(playerID)->type, ecs.GetComponent<Render>(playerID)->name, ecs.GetComponent<Render>(playerID)->dimension, ecs.GetComponent<Render>(playerID)->position, vector3D::vec3D(0.3f, 0.3f, 0.7f));
	//GLApp::GLObject::gimmeObject("square", playerList[i].unitName, playerList[i].size, vector2D::vec2D(playerList[i].position.x, playerList[i].position.y), vector3D::vec3D(0.3, 0.3, 0.7));

	wallGeneration(10, 10);
	wallGeneration(11, 10);
	wallGeneration(12, 10);
	wallGeneration(13, 10);
	wallGeneration(14, 10);
	wallGeneration(14, 9);
	wallGeneration(14, 8);
	wallGeneration(14, 7);
	wallGeneration(14, 6);
	wallGeneration(14, 5);

	wallGeneration(20, 19);
	wallGeneration(20, 20);
	wallGeneration(20, 21);
	wallGeneration(20, 22);
	wallGeneration(20, 23);
	wallGeneration(19, 23);
	wallGeneration(18, 23);
	wallGeneration(17, 23);
	wallGeneration(16, 23);
	wallGeneration(15, 23);

	wallGeneration(12, 15);
	wallGeneration(11, 15);
	wallGeneration(10, 15);
	wallGeneration(9, 15);
	wallGeneration(8, 15);
	wallGeneration(7, 15);
	wallGeneration(7, 14);
	wallGeneration(7, 13);
	wallGeneration(7, 12);
	wallGeneration(7, 11);


	unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	// create default engine as source of randomness
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> colour(0.f, 1.f);

	for (int i = 0; i < enemyUnits.size(); ++i)
	{
		float randr = colour(generator);
		float randg = colour(generator);
		float randb = colour(generator);

		enemyUnits[i].Add<Render>("enemy" + std::to_string(i + 1), "square", vector2D::vec2D(-450 + (i % 45 * 20), 400 - ((int)i / 30 * 10)), vector3D::vec3D(randr, randg, randb), vector2D::vec2D(10, 10), 0, 0, 0, "gam200-shdrpgm");
		enemyUnits[i].Add<Texture>(6, 1, 1, "Enemy");
		// velocity, target, force, speed
		enemyUnits[i].Add<Movement>(vector2D::vec2D(0, 0), ecs.GetComponent<Render>(player1.GetID())->position, 1, 2, 0, vector2D::vec2D(0, 0));
		//enemyUnits[i].Add<Sprite>();
		enemyUnits[i].Add<Stats>(100);
		ecs.setEntityName(enemyUnits[i].GetID(), "enemy" + std::to_string(i + 1));

		EntityID enemyID = enemyUnits[i].GetID();
		//GLApp::GLObject::gimmeObject(ecs.GetComponent<Render>(enemyID)->type, ecs.GetComponent<Render>(enemyID)->name, ecs.GetComponent<Render>(enemyID)->dimension, ecs.GetComponent<Render>(enemyID)->position, vector3D::vec3D(randr, randg, randb));

		quadObj entity;
		entity.key = enemyUnits[i].GetID();
		entity.position = ecs.GetComponent<Render>(enemyUnits[i].GetID())->position;

		mainTree.insertSuccessfully(entity);
	}

	timer = 4;
	animationTimer = 4;

	generateDijkstraCost(ecs.GetComponent<Render>(playerID)->position, walls);
	generateFlowField(ecs.GetComponent<Render>(playerID)->position);
	//enemyList[5].Print();

	vector2D::vec2D startingPoint{ 0 - 500 + 1000 / MAX_GRID_X / 2, 0 - 500 + 1000 / MAX_GRID_Y / 2 };

	int counter = 1;
	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			//GLApp::GLObject::gimmeObject("square", std::to_string(counter), vector2D::vec2D(1000 / MAX_GRID_X - 5, 1000 / MAX_GRID_Y - 5), vector2D::vec2D(startingPoint.x + (j * 1000 / MAX_GRID_X), startingPoint.y + (i * 1000 / MAX_GRID_X)), vector3D::vec3D(1, 1, 1));
			++counter;
		}
	}
	//GLApp::GLObject::gimmeObject("square", "0gridBackground", vector2D::vec2D(1010, 1010), vector2D::vec2D(0, 0), vector3D::vec3D(0, 0, 0));
	GLApp::GLObject::gimmeObject("square", "0Background", vector2D::vec2D((float)Graphics::Input::screenheight, (float)Graphics::Input::screenheight), vector2D::vec2D(0, 0), vector3D::vec3D(0, 0, 0), -1, 7, 3);
	GLApp::GLObject::gimmeObject("square", "0Background2", vector2D::vec2D((float)Graphics::Input::screenheight, (float)Graphics::Input::screenheight), vector2D::vec2D(0, 0), vector3D::vec3D(0, 0, 0), -1, 8, 3);
	//GLApp::GLObject::gimmeObject("square", "zDragBox", vector2D::vec2D(Graphics::Input::screenwidth, Graphics::Input::screenwidth), vector2D::vec2D(0, 0), vector3D::vec3D(0, 0, 0), -2, 5);

	textureSystem.Action([](const float elapsedMilliseconds,
		const std::vector<EntityID>& entities,
		Texture* t)
		{
			//static float renderTimer = 4;
			//std::cout << renderTimer << std::endl;
			//if (renderTimer <= 0)
			//{
			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				if (t[i].spriteStep == t[i].numberOfSprites)
				{
					t[i].spriteStep = 1;
				}
				else
					++t[i].spriteStep;
			}
			//renderTimer = 4;
		//}
		//else
			//renderTimer -= Graphics::Input::delta_time;
		});

	system1.Action([](const float elapsedMilliseconds,
		const std::vector<EntityID>& entities,
		Movement* m,
		Render* p)
		{
			// Check with walls
			for (int i = 0; i < walls.size(); ++i)
			{
				Render* pointer = ecs.GetComponent<Render>(walls[i].GetID());
				std::list<quadObj*> myList;
				AABB range(pointer->position.x - pointer->dimension.x,
					pointer->position.y - pointer->dimension.y,
					pointer->position.x + pointer->dimension.x,
					pointer->position.y + pointer->dimension.y);
				mainTree.query(range, myList);

				vector2D::vec2D wallPos = ecs.GetComponent<Render>(walls[i].GetID())->position;
				vector2D::vec2D wallDims = ecs.GetComponent<Render>(walls[i].GetID())->dimension;
				wallDims /= 2.f;

				// Create vertices for wall
				std::vector<vector2D::vec2D> wallVtx;
				for (int j = -1; j < 2; j += 2)
				{
					for (int k = -1; k < 2; k += 2)
					{
						wallVtx.emplace_back(vector2D::vec2D(wallPos.x + k * wallDims.x, wallPos.y + j * wallDims.y));
					}
				}

				for (std::list <quadObj*>::iterator enemyUnit = myList.begin(); enemyUnit != myList.end(); ++enemyUnit)
				{
					bool check = false;
					for (int j = 0; j < walls.size(); ++j)
					{
						if ((*enemyUnit)->key == walls[j].GetID())
						{
							check = true;
							break;
						}
					}
					if (check)
						continue;

					vector2D::vec2D enemyPos = ecs.GetComponent<Render>((*enemyUnit)->key)->position;
					vector2D::vec2D enemyDims = ecs.GetComponent<Render>((*enemyUnit)->key)->dimension;
					enemyDims /= 2.f;

						// Create vertices for enemy
						std::vector<vector2D::vec2D> enemyVtx;
						for (int j = -1; j < 2; j += 2)
						{
							for (int k = -1; k < 2; k += 2)
							{
								enemyVtx.emplace_back(vector2D::vec2D(enemyPos.x + k * enemyDims.x, enemyPos.y + j * enemyDims.y));
							}
						}
						if (physics::CollisionDetectionPolygonPolygon(wallVtx, enemyVtx))
						{
							Movement* collided = ecs.GetComponent<Movement>((*enemyUnit)->key);
							collided->collisionFlag = true;
						collided->collisionResponse = pointer->position;
						}
					
				}
			}


			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				//std::cout << "this is in glapp: " << entities[i] << " " << ecs.GetComponent<Render>(entities[i])->position.x << " " << ecs.GetComponent<Render>(entities[i])->position.y << std::endl;
				vector2D::vec2D oldPosition = p[i].position;
				vector2D::vec2D changedVelocity(0, 0);
				vector2D::vec2D offsetVector(0, 0);

				if (m[i].collisionFlag)
				{
					offsetVector = p[i].position - m[i].collisionResponse;
					m[i].collisionFlag = false;
				}

				vector2D::vec2D nodePosition = (p[i].position - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

				if ((int)nodePosition.x < 0 || (int)nodePosition.y < 0 || (int)nodePosition.x >= MAX_GRID_X || (int)nodePosition.x >= MAX_GRID_Y)
					changedVelocity = m[i].target - p[i].position;
				else
					changedVelocity = flowField[(int)nodePosition.y][(int)nodePosition.x] + offsetVector;

				std::vector<vector2D::vec2D> allVelocity{ vector2D::vec2D(0,0), vector2D::vec2D(0,0),vector2D::vec2D(0,0) };

				movementFlocking(entities[i], m[i].target, allVelocity);

				changedVelocity += (allVelocity[0] * 6 + (allVelocity[1] * 0.1) + allVelocity[2]); // *flockingModifier;

				vector2D::Vector2DNormalize(changedVelocity, changedVelocity);

				// capping speed
				//if (vector2D::Vector2DLength(changedVelocity) > m[i].speed)
				//{
				//	changedVelocity *= m[i].speed / vector2D::Vector2DLength(changedVelocity);
				//}
				changedVelocity *= m[i].speed;

				p[i].position += changedVelocity * (Graphics::Input::delta_time > 1 / 60.f ? 1 / 60.f : Graphics::Input::delta_time) * 100;

				m[i].velocity = changedVelocity;
				mainTree.updatePoint(quadObj((int)entities[i], oldPosition), p[i].position, mainTree);
			}

		});

	//std::string json_string;

	//ecsWriteToFile();

	//std::string fileLocation = "data/data" + ecs.getEntityName(player1.GetID()) + ".json";
	//to_json(*ecs.GetComponent<Render>(player1.GetID()), fileLocation);

	//fileLocation = "data/data" + ecs.getEntityName(enemyUnits[0].GetID()) + ".json";
	//to_json(*ecs.GetComponent<Render>(enemyUnits[0].GetID()), fileLocation);
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
	basicbatch.batchmodel = mdl_ref->second;
	basicbatch.batchshader = shd_ref->second;

	std::vector<vector3D::Vec3> clr_vtx
	{
		vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b),
		vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b)
	};
	int totalframes = 4;
	int curframe = 4;
	int textureid = texId; // TO REMOVE CONST
	if (textureid == 3 || textureid == 4)
	{
		totalframes = 1;
		curframe = 1;
		if (overlap)
		{
			if (textureid == 3)
			{
				textureid = 4;
			}
		}
		else
		{
			if (textureid == 4)
			{
				textureid = 3;
			}
		}
	}
	if (textureid == 7 || textureid == 8)
	{
		//std::cout << "Does it enter here\n";
		totalframes = totalsprites;
		curframe = 1;
	}
	std::vector<vector2D::Vec2> texcoord;
	texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe - 1) / float(totalframes), 0.f)); // Bottom left
	texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe) / float(totalframes), 0.f)); // Bottom right
	texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe) / float(totalframes), 1.f)); // Top right
	texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe - 1) / float(totalframes), 1.f)); // Top left

	std::vector<Graphics::vertexData> vertexData;
	for (int i = 0; i < ndc_coords.size(); ++i)
	{
		Graphics::vertexData tmpVtxData;
		tmpVtxData.posVtx = ndc_coords[i];
		if (mdl_ref->first == "circle")
		{
			tmpVtxData.clrVtx = vector3D::Vec3(color.r, color.g, color.b);
		}
		else
		{
			tmpVtxData.clrVtx = clr_vtx[i];
		}
		tmpVtxData.txtVtx = texcoord[i];
		tmpVtxData.txtIndex = (float)textureid;
		vertexData.emplace_back(tmpVtxData);
	}
	basicbatch.batchdata.insert(basicbatch.batchdata.end(), vertexData.begin(), vertexData.end());
	basicbatch.ebodata.insert(basicbatch.ebodata.end(), mdl_ref->second.primitive.begin(), mdl_ref->second.primitive.end());
	basicbatch.totalindicesize += mdl_ref->second.getPrimitiveCnt();
	basicbatch.vaoid = mdl_ref->second.getVAOid();
	basicbatch.vboid = mdl_ref->second.getVBOid();
	basicbatch.eboid = mdl_ref->second.getEBOid();
	basicbatch.totalsize += (int)vertexData.size();
	basicbatch.primtype = mdl_ref->second.getPrimitiveType();
	basicbatch.totaldrawcnt += mdl_ref->second.getDrawCnt();

	if (coldebug == true)
	{
		debugbatch.batchmodel = mdl_ref->second;
		debugbatch.batchshader = shd_ref->second;
		std::vector<vector3D::vec3D> clr_vtxcoldebug;
		for (int i = 0; i < 4; i++)
		{
			if (overlap == true)
			{
				clr_vtxcoldebug.emplace_back(vector3D::vec3D(0.8f, 0.f, 0.2f)); // red if overlapping
			}
			else
			{
				clr_vtxcoldebug.emplace_back(vector3D::vec3D(0.f, 0.2f, 0.7f)); // blue if not overlapping
			}
		}
		std::vector<Graphics::vertexData> colDebugData;
		for (int i = 0; i < ndc_coords.size(); ++i)
		{
			Graphics::vertexData tmpVtxData;
			tmpVtxData.posVtx = ndc_coords[i];
			if (mdl_ref->first == "circle")
			{
				tmpVtxData.clrVtx = vector3D::vec3D(1.f, 1.f, 1.f);
			}
			else
			{
				tmpVtxData.clrVtx = clr_vtxcoldebug[i];
			}
			tmpVtxData.txtVtx = texcoord[i];
			tmpVtxData.txtIndex = (float)texId; // notexture for coldebug
			colDebugData.emplace_back(tmpVtxData);
		}

		debugbatch.batchdata.insert(debugbatch.batchdata.end(), colDebugData.begin(), colDebugData.end());
		debugbatch.ebodata.insert(debugbatch.ebodata.end(), mdl_ref->second.primitive.begin(), mdl_ref->second.primitive.end());
		debugbatch.totalindicesize += mdl_ref->second.getPrimitiveCnt();
		debugbatch.vaoid = mdl_ref->second.getVAOid();
		debugbatch.vboid = mdl_ref->second.getVBOid();
		debugbatch.eboid = mdl_ref->second.getEBOid();
		debugbatch.totalsize += (int)colDebugData.size();
		debugbatch.primtype = mdl_ref->second.getPrimitiveType();
		debugbatch.totaldrawcnt += mdl_ref->second.getDrawCnt();
	}
	texcoord.clear();
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

	if (timer > 0)
		timer -= (float)Graphics::Input::delta_time;

	else
		ecs.RunSystems(2, 100);

	if (animationTimer > 0)
		animationTimer -= Graphics::Input::delta_time;
	else
	{
		ecs.RunSystems(1, 100);
		animationTimer = 4;
	}

	bool test{ true };
	for (std::map <std::string, GLObject> ::iterator obj = objects.begin(); obj != objects.end(); ++obj)
	{
		if (player->name == obj->first && mouseClick)
		{
			player->position = vector2D::vec2D(mousePosX, mousePosY);
			obj->second.modelCenterPos = player->position;

			generateDijkstraCost(player->position, walls);
			generateFlowField(player->position);
		}

		//for (int i = 0; i < enemyUnits.size(); ++i)
		//{
		//	if (ecs.GetComponent<Render>(enemyUnits[i].GetID())->name == obj->first)
		//	{
		//		obj->second.modelCenterPos = ecs.GetComponent<Render>(enemyUnits[i].GetID())->position;
		//		break;
		//	}
		//}

		if (obj->first != "Camera")
		{
			obj->second.update(Graphics::Input::delta_time);

			for (GLuint i = 0; i < obj->second.mdl_ref->second.getPosvtxCnt(); i++)
			{

				obj->second.ndc_coords[i] = obj->second.world_to_ndc_xform * obj->second.worldVertices[i], 1.f;
			}
		}
	}
	//-----------------------------------------extra imgui stuff here  -- we are so moving this out
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.

	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.


	std::vector<EntityID> entities = ecs.getEntities();


	// -----------------------------------------------I'm pretty sure this should be checked with messaging system for when created/ destroyed or itll lag and explode later on

	static bool inputs_step = true;
	static float  f32_v = 0.123f;
	const float f32_one = 1.f;
	
	//for (auto i : ecs.getEntities()) {

	if (ImGui::TreeNode("Entities")) {
		for (int i = 1; i < ecs.getEntities().size() + 1; ++i) {
			std::string str = ecs.getEntityName(i);

			if (str == "")
				continue;

			const char* c = str.c_str();
			if (ImGui::TreeNode((void*)(intptr_t)i, c)) {
				std::vector<std::string> names = ecs.getEntityComponents(i);
				//--------------------------------------------GET THIS HARDCODE SHIT AWAY FROM ME
				for (int j = 0; j < names.size(); ++j) {
					std::string str2 = names[j];
					const char* c2 = str2.c_str();
					ImGui::Text(c2);

					if (str2 == "Render") {
						//ecs.GetComponent<Object>(i)->x;
						//ImGui::SameLine();
						ImGui::InputScalar("Pos x", ImGuiDataType_Float, &ecs.GetComponent<Render>(i)->position.x, inputs_step ? &f32_one : NULL);
						//ImGui::SameLine(); 
						ImGui::InputScalar("Pos y", ImGuiDataType_Float, &ecs.GetComponent<Render>(i)->position.y, inputs_step ? &f32_one : NULL);
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	// button to create entity
	static char name[100]{ '\0' };
	static bool checks[100] = { false };
	static EntityID temp;
	static Render* render;
	static Texture* texture;
	static Movement* movement;
	static Stats* stats;
	static std::vector<int> componentCheck(ecs.getAllRegisteredComponents().size(), 0);

	if (ImGui::Button("Create Entity"))
	{
		if (counter & 1)
		{
			//if (name[0] != '\0')
			// creating entity
			//ecs.RegisterEntity(ecs.GetNewID(), name);

			//for (int i = 0; i < 1; ++i)
			//{
			//	if (checks[i])
			//		ecs.AddComponent<rttr::type::get_by_name(ecs.getAllRegisteredComponents()[i])>();
			//}
			if (name[0] == '\0' || render == nullptr)
			{
				checks[99] = true;
			}

			else
			{
				std::string nameToString(name);
				render->name = nameToString;
				ecs.setEntityName(temp, nameToString);

				for (int i = 0; i < ecs.getAllRegisteredComponents().size(); ++i)
					componentCheck[i] = 0;

				memset(name, 0, sizeof(name));
				checks[99] = false;
				counter++;
			}
		}
		else
		{
			temp = ecs.GetNewID();
			ecs.RegisterEntity(temp, "In IMGUI");
			std::cout << temp;
			counter++;
		}
	}

	if (counter & 1)
	{
		if (checks[99])
			ImGui::Text("Please Input Entity Name");

		ImGui::InputText("Entity Name", name, 100);
		
		for (int i = 0; i < ecs.getAllRegisteredComponents().size(); ++i)
		{
			std::string componentName = ecs.getAllRegisteredComponents()[i];
			//rttr::instance instance = rttr::instance::instance(componentList[i]);
			rttr::type component = rttr::type::get_by_name(componentName);
			
			// odd numbers, to join the 2nd checkbox in the same line
			//if (i & 1)
			//	ImGui::SameLine();

			ImGui::Checkbox(ecs.getAllRegisteredComponents()[i].c_str(), &checks[i]);

			if (checks[i])
			{
				// setting all of the components for the 1st time
				if (!componentCheck[i])
				{
					std::cout << "test" << std::endl;
					// i wanna make this not hardcoded :((
					if (componentName == "Render")
						render = ecs.AddComponent<Render>(temp);
					else if (componentName == "Texture")
						texture = ecs.AddComponent<Texture>(temp);
					else if (componentName == "Movement")
						movement = ecs.AddComponent<Movement>(temp);
					else if (componentName == "Stats")
						stats = ecs.AddComponent<Stats>(temp);

					componentCheck[i] = 1;
				}

				if (componentName == "Render")
				{
					//static const char* types[] = { "square", "circle" };
					//static int typeCurrent = 0;
					//ImGui::Combo("Type", &typeCurrent, types, IM_ARRAYSIZE(types));
					//render->type = types[typeCurrent];

					ImGui::DragFloat2("Position", render->position.m, 1.0f, -500.0f, 500.0f);
					ImGui::DragFloat3("Color", render->color.m, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat2("Dimension", render->dimension.m, 0.5f, 0.0f, 500.0f);
				}

				else if (componentName == "Movement")
				{
					//static const char* types[] = { "square", "circle" };
					//static int typeCurrent = 0;
					//ImGui::Combo("Type", &typeCurrent, types, IM_ARRAYSIZE(types));
					//render->type = types[typeCurrent];
					
/*					static std::vector<std::string> entities = ecs.getEntityNames();
					static int currentItem = 0;*/ // Here we store our selection data as an index.
					//	ImGui::ListBox("Target", &currentItem, 1, entities.data(), entities.size());

					//static ImGuiComboFlags flags = 0;

					//if (ImGui::BeginCombo("combo",combo_preview_value,flags)) {
					//	for (int i = 0; i < entities.size(); ++i) {
					//		const bool isSelected = (currentItem == i);
					//		if (ImGui::Selectable(entities[i], isSelected)) {
					//			currentItem = i;
					//		}
					//	}
					//}
					//ImGui::EndCombo;
					//const char* list = VectorOfStringGetter(entities, entities.size());
					//ImGui::Combo("Target", &currentItem, list, 3);
					//ImGui::ListBox("Symbols", currentItem, VectorOfStringGetter((void*)&entities, (int)entities.size()), entities.size(), 3);
					//ImGui::Combo("Example", &currentItem, entities);

					//ImGui::DragFloat2("Target", render->position.m, 1.0f, -500.0f, 500.0f);
					ImGui::DragFloat("Force", &movement->force, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Speed", &movement->speed, 0.5f, 0.0f, 500.0f);
				}


				//std::string comp = ecs.getAllRegisteredComponents()[i];
				//rttr::type theComp = rttr::type::get_by_name(ecs.getAllRegisteredComponents()[i]);
			
				//rttr::instance IR = r.create();
				
				//auto e =r.get_properties();
				//auto q = *e.begin();
			
				//std::cout << prop.get_name();
				//std::cout << VR.get_type().get_name().to_string();
				//rttr::instance instance = .get_type().get_raw_type().is_wrapper() ? componentList[i].get_wrapped_instance() : componentList[i];

				//for (auto& property : component.get_properties())
				//{
					//std::cout << typeid(property.get_type().get_name().to_string()) << std::endl;
					//rttr::property propertyType = instance.get_type().get_by_name(property.get_name()).get_property();

					//if (property.get_type() == rttr::type::get<vector2D::vec2D>())
					//{
						//static vector2D::vec2D temp;
						//ImGui::DragFloat2(property.get_name().data(), temp.m, 1.0f, -500.0f, 500.0f);
						
						//instance.get_type().get_by_name(property.get_name()).set_property_value(property.get_name(), temp);
						
						//std::cout << instance.get_type().get_property_value("position").to_string() << "test";
						//std::cout << instance.get_type().get_name().to_string() << std::endl;
						
					//}
					//ImGui::InputText(property.get_name().data(), name, 100);
				//}
			}

			else if (componentCheck[i])
			{
				// i wanna make this not hardcoded :((
				if (componentName == "Render")
					ecs.RemoveComponent<Render>(temp);
				else if (componentName == "Texture")
					ecs.RemoveComponent<Texture>(temp);
				else if (componentName == "Movement")
					ecs.RemoveComponent<Movement>(temp);
				else if (componentName == "Stats")
					ecs.RemoveComponent<Stats>(temp);

				componentCheck[i] = 0;
			}
		}

		if (ImGui::Button("Cancel"))
		{
			ecs.RemoveEntity(temp);
			memset(name, 0, sizeof(name));
			counter++;
		}
	}
	
	//ImGui::TreeNode("THE REST");
	/*
	for (auto i : ecs.getEntities()) {
		std::string str = ecs.getEntityName(i);
		const char* c = str.c_str();
		ImGui::Text(c);	
	for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
	{
		if (obj1->first != "Camera")
		{
			obj1->second.update(Graphics::Input::delta_time);
		}
	}
	
	for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end() ; ++obj1)
	{
		for (GLuint i = 0; i < obj1->second.mdl_ref->second.posvtx_cnt; i++)
		{
			obj1->second.ndc_coords[i] = obj1->second.world_to_ndc_xform * obj1->second.worldVertices[i], 1.f;
		}
	}
	*/
	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Text("Test");
	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//	counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	ImGui::Render();
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
			obj->second.draw(); // Comment to stop drawing from object map
		}
	}
	GLApp::entitydraw(); // Comment to stop drawing from ecs

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	basicbatch.BatchRender(Graphics::textureobjects); // Renders all objects at once
	glLineWidth(2.f);
	debuglinebatch.BatchRender(Graphics::textureobjects);
	glLineWidth(1.f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	debugbatch.BatchRender(Graphics::textureobjects); // For collision debug
	basicbatch.BatchClear();
	debuglinebatch.BatchClear();
	debugbatch.BatchClear();
	glDisable(GL_BLEND);

	objects["Camera"].draw();
	//-----------------------------------------extra imgui stuff here
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
/*  _________________________________________________________________________*/
/*! cleanup

@param none
@return none

This function is empty for now
*/
void GLApp::cleanup() 
{
	basicbatch.BatchDelete();
	debuglinebatch.BatchDelete();
	debugbatch.BatchDelete();
	Graphics::Texture::deleteTexture(Graphics::textureobjects[0]);
	Graphics::Texture::deleteTexture(Graphics::textureobjects[1]);
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
		tmpObj.body.createCircleBody(scale.x/2.f, pos, 0.f, false, 0.f, &tmpObj.body, hi);
	}
	else if (modelname == "square")
		tmpObj.body.createBoxBody(scale.x, scale.x, pos, 0.f, false, 0.f, &tmpObj.body, hi);

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
		insert_shdrpgm("gam200-shdrpgm", "../shaders/gam200.vert", "../shaders/gam200.frag");
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

		Render* curobj = ecs.GetComponent<Render>(entities[i]);
		int texid = 0;
		Texture* curobjTexture = ecs.GetComponent<Texture>(entities[i]);
		if (ecs.GetComponent<Texture>(entities[i]) != nullptr)
		{
			texid = curobjTexture->textureID;
		}
		basicbatch.batchmodel = models["square"];
		GLSLShader shaderid;
		if (shdrpgms.find(curobj->shaderName) != shdrpgms.end())
		{
			shaderid = shdrpgms.find(curobj->shaderName)->second;
		}
		else
		{
			insert_shdrpgm(curobj->shaderName, "../shaders/gam200.vert", "../shaders/gam200.frag");
			shaderid = shdrpgms.find(curobj->shaderName)->second;
		}
		basicbatch.batchshader = shaderid;

		std::vector<vector3D::vec3D> clr_vtx
		{
			vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b), vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b),
			vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b), vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b)
		};

		int numofsprites = 0;
		int spritestep = 0;
		if (texid)
		{
			numofsprites = curobjTexture->numberOfSprites;
			spritestep = curobjTexture->spriteStep;
			if (curobjTexture->textureID == 3 || curobjTexture->textureID == 4)
			{
				//if (overlapobj->overlap)
				//{
				//	if (curobj->textureID == 3)
				//	{
				//		curobj->textureID = 4;
				//	}
				//}
				//else
				//{
				//	if (curobj->textureID == 4)
				//	{
				//		curobj->textureID = 3;
				//	}
				//}
			}
		}
		std::vector<vector2D::vec2D> texcoord;
		texcoord.emplace_back(vector2D::vec2D(0.f + float(spritestep - 1) / float(numofsprites), 0.f));
		texcoord.emplace_back(vector2D::vec2D(0.f + float(spritestep) / float(numofsprites), 0.f));
		texcoord.emplace_back(vector2D::vec2D(0.f + float(spritestep) / float(numofsprites), 1.f));
		texcoord.emplace_back(vector2D::vec2D(0.f + float(spritestep - 1) / float(numofsprites), 1.f));

		std::vector<vector2D::vec2D> poscoord; // CALCULATE POSITION FROM CENTER
		float halfwidth = curobj->dimension.x / 2.f;
		float halfheight = curobj->dimension.y / 2.f;
		poscoord.emplace_back(vector2D::vec2D(curobj->position.x - halfwidth, curobj->position.y - halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobj->position.x + halfwidth, curobj->position.y - halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobj->position.x + halfwidth, curobj->position.y + halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobj->position.x - halfwidth, curobj->position.y + halfheight));

		matrix3x3::mat3x3 world_to_ndc_notglm = Graphics::camera2d.getWorldtoNDCxForm();
		matrix3x3::mat3x3 world_to_ndc_xform = matrix3x3::mat3x3
		(
			world_to_ndc_notglm.m[0], world_to_ndc_notglm.m[1], world_to_ndc_notglm.m[2],
			world_to_ndc_notglm.m[3], world_to_ndc_notglm.m[4], world_to_ndc_notglm.m[5],
			world_to_ndc_notglm.m[6], world_to_ndc_notglm.m[7], world_to_ndc_notglm.m[8]
		);

		std::vector <vector2D::vec2D> ndccoord;
		for (int i = 0; i < poscoord.size(); i++)
		{
			ndccoord.emplace_back(world_to_ndc_xform * poscoord[i]);
		}
		std::vector<Graphics::vertexData> vertexData;
		for (int i = 0; i < 4; ++i)
		{
			Graphics::vertexData tmpVtxData;
			tmpVtxData.posVtx = ndccoord[i];
			tmpVtxData.clrVtx = curobj->color;

			tmpVtxData.txtVtx = texcoord[i];
			vertexData.emplace_back(tmpVtxData);
		}

		basicbatch.batchdata.insert(basicbatch.batchdata.end(), vertexData.begin(), vertexData.end());
		basicbatch.ebodata.insert(basicbatch.ebodata.end(), models["square"].primitive.begin(), models["square"].primitive.end());
		basicbatch.totalindicesize += models["square"].getPrimitiveCnt();
		basicbatch.vaoid = models["square"].getVAOid();
		basicbatch.vboid = models["square"].getVBOid();
		basicbatch.eboid = models["square"].getEBOid();
		basicbatch.totalsize += (int)vertexData.size();
		basicbatch.primtype = models["square"].getPrimitiveType();
		basicbatch.totaldrawcnt += models["square"].getDrawCnt();

		texcoord.clear();
		ndccoord.clear();
		vertexData.clear();

		if (velocitydirectiondebug == true)
		{


			if (ecs.GetComponent<Movement>(entities[i]) == nullptr) // Added check for NIL objects
			{
				continue;
			}
			Movement* objmovement = ecs.GetComponent<Movement>(entities[i]);
			debuglinebatch.batchmodel = models["line"];
			debuglinebatch.batchshader = shaderid;

			std::vector<vector3D::vec3D> debugline_clrvtx
			{
				//vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b), vector3D::vec3D(curobj->color.r, curobj->color.g, curobj->color.b)
				vector3D::vec3D(0.2f, 0.f, 0.f), vector3D::vec3D(0.2f, 0.f, 0.f)
			};
			std::vector<vector2D::vec2D> debugline_texcoord
			{
				vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f)
			};

			std::vector<vector2D::vec2D> debugline_poscoord
			{
				vector2D::vec2D(curobj->position.x, curobj->position.y),
				vector2D::vec2D(curobj->position.x + (objmovement->velocity.x * 25), curobj->position.y + (objmovement->velocity.y * 25))
			}; // CALCULATE POSITION FROM CENTER

			std::vector <vector2D::vec2D> debugline_ndccoord;
			for (int i = 0; i < poscoord.size(); i++)
			{
				debugline_ndccoord.emplace_back(world_to_ndc_xform * debugline_poscoord[i]);

			}
			std::vector<Graphics::vertexData> debugline_vertexData;
			for (int i = 0; i < 2; ++i)
			{
				Graphics::vertexData tmpVtxData;
				tmpVtxData.posVtx = debugline_ndccoord[i];
				tmpVtxData.clrVtx = debugline_clrvtx[i];

				tmpVtxData.txtVtx = debugline_texcoord[i];
				tmpVtxData.txtIndex = 0;
				debugline_vertexData.emplace_back(tmpVtxData);
			}

			debuglinebatch.batchdata.insert(debuglinebatch.batchdata.end(), debugline_vertexData.begin(), debugline_vertexData.end());
			debuglinebatch.ebodata.insert(debuglinebatch.ebodata.end(), models["line"].primitive.begin(), models["line"].primitive.end());
			debuglinebatch.totalindicesize += models["line"].getPrimitiveCnt();
			debuglinebatch.vaoid = models["line"].getVAOid();
			debuglinebatch.vboid = models["line"].getVBOid();
			debuglinebatch.eboid = models["line"].getEBOid();
			debuglinebatch.totalsize += (int)debugline_vertexData.size();
			debuglinebatch.primtype = GL_LINES;
			debuglinebatch.totaldrawcnt += models["line"].getDrawCnt();

			debugline_ndccoord.clear();
			debugline_vertexData.clear();
		}
	}
}