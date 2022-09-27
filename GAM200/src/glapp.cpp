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
#include <transform.h>
#include <random>
#include <physicsRigidBody.h>
#include <physicsPartition.h>
#include "ECS.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdint.h>
#include "pathfinding.h"
#include <camera.h>
#include <iomanip>
#include "physicsPartition.h"


//-----------------------------------RANDOM IMGUI FUNCTION LOL
//if (ImGui::TreeNode("Entities")) {
//	for (int i = 1; i < ecs.getEntities().size() + 1; ++i) {
//		std::string str = ecs.getEntityName(i);
//		const char* c = str.c_str();
//		if (ImGui::TreeNode((void*)(intptr_t)i, c)) {
//			std::vector<std::string> names = ecs.getEntityComponents(i);
//			//--------------------------------------------GET THIS HARDCODE SHIT AWAY FROM ME
//			for (int j = 0; j < names.size(); ++j) {
//				std::string str2 = names[j];
//				const char* c2 = str2.c_str();
//				ImGui::Text(c2);
//
//				if (str2 == "Object") {
//					//ecs.GetComponent<Object>(i)->x;
//					//ImGui::SameLine();
//					ImGui::InputScalar("Pos x", ImGuiDataType_Float, &ecs.GetComponent<Object>(i)->x, inputs_step ? &f32_one : NULL);
//					//ImGui::SameLine(); 
//					ImGui::InputScalar("Pos y", ImGuiDataType_Float, &ecs.GetComponent<Object>(i)->y, inputs_step ? &f32_one : NULL);
//				}
//			}
//			ImGui::TreePop();
//		}
//	}
//	ImGui::TreePop();
//}

//void imguiComponent(EntityID& id)
//{
//	std::vector<std::string> componentNames = ecs.getEntityComponents(id);
//
//
//}

//Entity createEntity()
//{
//
//}

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
std::vector<Entity> enemyUnits(100);
std::vector<Entity> createdUnits(100); // precreated empty entities
std::vector<Entity> walls(3);

System<Texture> textureSystem(ecs, 1);
System<Movement, Render> system1(ecs, 2);

extern int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
float timer;

bool show_demo_window;
bool show_another_window;
ImVec4 clear_color;


class test_class {
public:
	test_class(int value) : m_value(value) {}
	void print_value() const { std::cout << m_value; }
private:
	int m_value;
	RTTR_ENABLE();

};

RTTR_REGISTRATION{
	//rttr::registration::class_<Entity>("Entity")
	//	.constructor<Entity>()
	//	.method("Add", &Entity::Add);

	//rttr::registration::class_<ECS>("ECS")
	//	.constructor<ECS>()
	//	.method("AddComponent", &ECS::AddComponent);

	rttr::registration::class_<vector2D::vec2D>("vector2D::vec2D")
		.constructor<vector2D::vec2D>()
		.property("m", &vector2D::vec2D::m)
		.property("x", &vector2D::vec2D::x)
		.property("y", &vector2D::vec2D::y);

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
		.property("speed", &Movement::speed);

	//rttr::registration::class_<Sprite>("Sprite")

	rttr::registration::class_<Stats>("Stats")
		.property("health", &Stats::health);

	//rttr::registration::class_<test_class>("test_class")
	//.constructor<int>()
	//.method("print_value", &test_class::print_value);

	//i think we cant register private vars anymore, just use setters /getters and register the method
	//.property("value", &test_class::m_value)
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
	GLApp::alphablend = false;
	GLApp::textures = true;
	GLApp::coldebug = false;

	Graphics::createTextureVector(Graphics::textureobjects, 6);
	//std::cout << "Texture units " << Graphics::textureobjects.size() << std::endl;
	//Graphics::textureobjects.resize(2);
	// Part 1: initialize OpenGL state ...
	glClearColor(0.3f, 1.f, 1.f, 1.f);						// clear colorbuffer with RGBA value in glClearColor

	// Part 2: use the entire window as viewport ...
	glViewport(0, 0, GLHelper::width, GLHelper::height);

	// Part 3: parse scene file $(SolutionDir)scenes/tutorial-4.scn
	// and store repositories of models of type GLModel in container
	// GLApp::models, store shader programs of type GLSLShader in
	// container GLApp::shdrpgms, and store repositories of objects of
	// type GLObject in container GLApp::objects
	//GLApp::init_scene("../scenes/gam200.scn");

	Graphics::Texture::loadTexture("../images/BaseTree.png", Graphics::textureobjects[0]); // BaseTree
	Graphics::Texture::loadTexture("../images/BaseTree.png", Graphics::textureobjects[1]); // 
	Graphics::Texture::loadTexture("../images/GrassMap.png", Graphics::textureobjects[2]); // Grass map
	Graphics::Texture::loadTexture("../images/BlueCircle.png", Graphics::textureobjects[3]); // Blue Circle
	Graphics::Texture::loadTexture("../images/YellowCircle.png", Graphics::textureobjects[4]); // Yellow Circle
	Graphics::Texture::loadTexture("../images/DragBox.png", Graphics::textureobjects[5]); // Yellow Circle


	// Part 4: initialize camera (NEED TO CHANGE THIS PLEASE)
	GLApp::GLObject::gimmeObject("square", "Camera", vector2D::vec2D(1, 1), vector2D::vec2D(0, 0), vector3D::vec3D(1, 1, 1));
	Graphics::camera2d.init(GLHelper::ptr_window, &GLApp::objects.at("Camera"));

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
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO(); (void)io;

	//ImGui::StyleColorsDark();
	//ImGui_ImplGlfw_InitForOpenGL(GLHelper::ptr_window, true);
	//ImGui_ImplOpenGL3_Init(NULL);

	//show_demo_window = true;
	//show_another_window = false;
	//clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

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
	player1.Add<Render>("player1", "square", vector2D::vec2D(-200, 0), vector3D::vec3D(0.3, 0.3, 0.7), vector2D::vec2D(20, 20), 0, 0, 0, "gam200-shdrpgm");
	// velocity, target, force, speed
	//player1.Add<Movement>(vector2D::vec2D(0, 0), vector2D::vec2D(0, 0), 10, 2);
	//player1.Add<Texture>(0, 1, 1, "none");
	player1.Add<Stats>(100);
	ecs.setEntityName(player1.GetID(), "player1");													// may not need this after rttr

	EntityID playerID = player1.GetID();
	GLApp::GLObject::gimmeObject(ecs.GetComponent<Render>(playerID)->type, ecs.GetComponent<Render>(playerID)->name, ecs.GetComponent<Render>(playerID)->dimension, ecs.GetComponent<Render>(playerID)->position, vector3D::vec3D(0.3, 0.3, 0.7));
	//GLApp::GLObject::gimmeObject("square", playerList[i].unitName, playerList[i].size, vector2D::vec2D(playerList[i].position.x, playerList[i].position.y), vector3D::vec3D(0.3, 0.3, 0.7));



	quadObj entity;
	int count = 1;
	vector2D::vec2D position = vector2D::vec2D(10, 10) * (1000 / MAX_GRID_X) + vector2D::vec2D(-500, -500) + vector2D::vec2D(1000 / MAX_GRID_X / 2, 1000 / MAX_GRID_Y / 2);
	walls[0].Add<Render>("wall" + std::to_string(count++), "square", position, vector3D::vec3D(0, 0, 0), vector2D::vec2D(50, 50), 0, 0, 0, "gam200-shdrpgm");

	entity.key = walls[0].GetID();
	std::cout << "thsi is wall: " << walls[0].GetID() << std::endl;
	entity.position = ecs.GetComponent<Render>(walls[0].GetID())->position;
	mainTree.insertSuccessfully(entity);

	position = vector2D::vec2D(11, 10) * (1000 / MAX_GRID_X) + vector2D::vec2D(-500, -500) + vector2D::vec2D(1000 / MAX_GRID_X / 2, 1000 / MAX_GRID_Y / 2);
	walls[1].Add<Render>("wall" + std::to_string(count++), "square", position, vector3D::vec3D(0, 0, 0), vector2D::vec2D(50, 50), 0, 0, 0, "gam200-shdrpgm");

	entity.key = walls[1].GetID();
	entity.position = ecs.GetComponent<Render>(walls[1].GetID())->position;
	mainTree.insertSuccessfully(entity);

	position = vector2D::vec2D(12, 10) * (1000 / MAX_GRID_X) + vector2D::vec2D(-500, -500) + vector2D::vec2D(1000 / MAX_GRID_X / 2, 1000 / MAX_GRID_Y / 2);
	walls[2].Add<Render>("wall" + std::to_string(count), "square", position, vector3D::vec3D(0, 0, 0), vector2D::vec2D(50, 50), 0, 0, 0, "gam200-shdrpgm");
	mainTree.insertSuccessfully(entity);

	entity.key = walls[2].GetID();
	entity.position = ecs.GetComponent<Render>(walls[2].GetID())->position;
	mainTree.insertSuccessfully(entity);


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
		// velocity, target, force, speed
		enemyUnits[i].Add<Movement>(vector2D::vec2D(0, 0), ecs.GetComponent<Render>(player1.GetID())->position, 1, 2);
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

	generateDijkstraCost(ecs.GetComponent<Render>(playerID)->position, walls);
	generateFlowField(ecs.GetComponent<Render>(playerID)->position);
	//enemyList[5].Print();

	vector2D::vec2D startingPoint{ 0 - 500 + 1000 / MAX_GRID_X / 2, 0 - 500 + 1000 / MAX_GRID_Y / 2 };

	int counter = 1;
	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			GLApp::GLObject::gimmeObject("square", std::to_string(counter), vector2D::vec2D(1000 / MAX_GRID_X - 5, 1000 / MAX_GRID_Y - 5), vector2D::vec2D(startingPoint.x + (j * 1000 / MAX_GRID_X), startingPoint.y + (i * 1000 / MAX_GRID_X)), vector3D::vec3D(1, 1, 1));

			++counter;
		}
	}
	GLApp::GLObject::gimmeObject("square", "0gridBackground", vector2D::vec2D(1010, 1010), vector2D::vec2D(0, 0), vector3D::vec3D(0, 0, 0));
	GLApp::GLObject::gimmeObject("square", "0Background", vector2D::vec2D(GLHelper::width, GLHelper::width), vector2D::vec2D(0, 0), vector3D::vec3D(0, 0, 0), -1, 2);
	//GLApp::GLObject::gimmeObject("square", "zDragBox", vector2D::vec2D(GLHelper::width, GLHelper::width), vector2D::vec2D(0, 0), vector3D::vec3D(0, 0, 0), -2, 5);

	// Part 5: Print OpenGL context and GPU specs
	//GLHelper::print_specs();


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
			//renderTimer -= GLHelper::delta_time;
		});

	system1.Action([](const float elapsedMilliseconds,
		const std::vector<EntityID>& entities,
		Movement* m,
		Render* p)
		{
			// Check with walls
			for (int i = 0; i < 3; ++i)
			{
				Render* pointer = ecs.GetComponent<Render>(walls[i].GetID());
				std::list<quadObj*> myList;
				AABB range(pointer->position.x - pointer->dimension.x * 2,
					pointer->position.y - pointer->dimension.y * 2,
					pointer->position.x + pointer->dimension.x * 2,
					pointer->position.y + pointer->dimension.y * 2);
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
					if ((*enemyUnit)->key != walls[0].GetID() && (*enemyUnit)->key != walls[1].GetID() && (*enemyUnit)->key != walls[2].GetID())
					{
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
							m[(*enemyUnit)->key].collisionFlag = true;
						}
					}
				}
			}


			for (std::size_t i = 0; i < entities.size(); ++i)
			{
				//std::cout << "this is in glapp: " << entities[i] << " " << ecs.GetComponent<Render>(entities[i])->position.x << " " << ecs.GetComponent<Render>(entities[i])->position.y << std::endl;
				vector2D::vec2D oldPosition = p[i].position;
				vector2D::vec2D changedVelocity = m[i].velocity;

				vector2D::vec2D nodePosition = (p[i].position - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

				vector2D::Vector2DNormalize(changedVelocity, flowField[(int)nodePosition.y][(int)nodePosition.x]);

				std::vector<vector2D::vec2D> allVelocity{ vector2D::vec2D(0,0), vector2D::vec2D(0,0),vector2D::vec2D(0,0) };

				movementFlocking(entities[i], m[i].target, allVelocity);

				changedVelocity += allVelocity[0] * 4 + (allVelocity[1] * 0.1) + allVelocity[2];

				// capping speed
				if (vector2D::Vector2DLength(changedVelocity) > m[i].speed)
				{
					changedVelocity *= m[i].speed / vector2D::Vector2DLength(changedVelocity);
				}

				p[i].position += changedVelocity * (GLHelper::delta_time > 1 / 60.f ? 1 / 60.f : GLHelper::delta_time) * 100;

				m[i].velocity = changedVelocity;
				mainTree.updatePoint(quadObj((int)entities[i], oldPosition), p[i].position, mainTree);
			}

		});
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
	//if (mdl_ref->first == "circle")
	//{																														  // load shader program in use by this object
	//	shd_ref->second.Use();
	//	// bind VAO of this object's model
	//	glBindVertexArray(mdl_ref->second.getVAOid()); // Rebind VAO
	//	std::vector<vector2D::Vec2> tex_coord
	//	{
	//		vector2D::Vec2(1.f, 1.f), vector2D::Vec2(0.f, 1.f),
	//		vector2D::Vec2(0.f, 0.f), vector2D::Vec2(1.f, 0.f)
	//	};
	//	std::vector<vector3D::Vec3> clr_vtx
	//	{
	//		vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b),
	//		vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b)
	//	};


	//	std::vector<Graphics::vertexData> vertexData;
	//	for (int i = 0; i < ndc_coords.size(); ++i)
	//	{
	//		Graphics::vertexData tmpVtxData;
	//		tmpVtxData.posVtx = ndc_coords[i];
	//		if (mdl_ref->first == "circle")
	//		{
	//			tmpVtxData.clrVtx = vector3D::Vec3(color.r, color.g, color.b);
	//			tmpVtxData.txtVtx = vector2D::Vec2(1.f, 1.f);
	//		}
	//		else
	//		{
	//			tmpVtxData.clrVtx = clr_vtx[i];
	//			tmpVtxData.txtVtx = tex_coord[i];
	//		}
	//		//tmpVtxData.txtVtx = tex_coord[i];
	//		vertexData.emplace_back(tmpVtxData);
	//	}
	//	glNamedBufferSubData(mdl_ref->second.getVBOid(), 0, sizeof(Graphics::vertexData) * vertexData.size(), vertexData.data()); // Set new buffer index with subdata


	//	// copy object's model-to-NDC matrix to vertex shader's
	//	// uniform variable uModelToNDC
	//	glm::mat3 glm_mdl_to_ndc_xform
	//	{
	//		mdl_to_ndc_xform.m[0], mdl_to_ndc_xform.m[1], mdl_to_ndc_xform.m[2],
	//		mdl_to_ndc_xform.m[3], mdl_to_ndc_xform.m[4], mdl_to_ndc_xform.m[5],
	//		mdl_to_ndc_xform.m[6], mdl_to_ndc_xform.m[7], mdl_to_ndc_xform.m[8]
	//	};
	//	//shd_ref->second.SetUniform("uModel_to_NDC", glm_mdl_to_ndc_xform);
	//	//shd_ref->second.SetUniform("ourTexture", mdl_to_ndc_xform);
	//	//std::cout << "Shdr handle " << shd_ref->second.GetHandle() << std::endl;
	//	GLuint tex_loc = glGetUniformLocation(shd_ref->second.GetHandle(), "ourTexture");
	//	glUniform1i(tex_loc, 0);

	//	GLboolean UniformModulate = glGetUniformLocation(shd_ref->second.GetHandle(), "modulatebool");
	//	glUniform1i(UniformModulate, modulate);

	//	GLboolean UniformTextures = glGetUniformLocation(shd_ref->second.GetHandle(), "texturebool");
	//	glUniform1i(UniformTextures, textures);

	//	// call glDrawElements with appropriate arguments
	//	glDrawElements(mdl_ref->second.getPrimitiveType(), mdl_ref->second.getDrawCnt(), GL_UNSIGNED_SHORT, NULL);

	//	// unbind VAO and unload shader program
	//	glBindVertexArray(0);

	//	if (coldebug == true)
	//	{


	//		glBindVertexArray(mdl_ref->second.getVAOid()); // Rebind VAO
	//		//glGetVertexArrayIndexediv(mdl_ref->second.getVAOid(), 0, GL_VERTEX_BINDING_BUFFER, reinterpret_cast<GLint*>(&buffer));

	//		std::vector<vector2D::Vec2> tex_coord2
	//		{
	//			vector2D::Vec2(1.f, 1.f), vector2D::Vec2(0.f, 1.f),
	//			vector2D::Vec2(0.f, 0.f), vector2D::Vec2(1.f, 0.f)
	//		};


	//		std::vector<vector3D::Vec3> clr_vtx2
	//		{
	//			vector3D::Vec3(1.f, 1.f, 0.f), vector3D::Vec3(1.f, 1.f, 0.f),
	//			vector3D::Vec3(1.f, 1.f, 0.f), vector3D::Vec3(1.f, 1.f, 0.f)
	//		};
	//		if (overlap == true)
	//		{
	//			//std::cout << "Overlapping" << std::endl;
	//			for (int i = 0; i < 4; i++)
	//			{
	//				clr_vtx2[i] = vector3D::Vec3(1.f, 0.f, 0.f);
	//			}
	//		}

	//		std::vector<Graphics::vertexData> vertexData2;
	//		for (int i = 0; i < ndc_coords.size(); ++i)
	//		{
	//			Graphics::vertexData tmpVtxData;
	//			tmpVtxData.posVtx = ndc_coords[i];
	//			if (mdl_ref->first == "circle")
	//			{
	//				if (overlap == true)
	//				{
	//					tmpVtxData.clrVtx = vector3D::Vec3(1.f, 0.f, 0.f);
	//					tmpVtxData.txtVtx = vector2D::Vec2(1.f, 1.f);
	//				}
	//				else
	//				{
	//					tmpVtxData.clrVtx = vector3D::Vec3(1.f, 1.f, 0.f);
	//					tmpVtxData.txtVtx = vector2D::Vec2(1.f, 1.f);
	//				}
	//			}
	//			else
	//			{
	//				tmpVtxData.clrVtx = clr_vtx2[i];
	//				tmpVtxData.txtVtx = tex_coord2[i];
	//			}
	//			vertexData2.emplace_back(tmpVtxData);
	//		}
	//		glNamedBufferSubData(mdl_ref->second.getVBOid(), 0, sizeof(Graphics::vertexData) * vertexData2.size(), vertexData2.data()); // Set new buffer index with subdata

	//		std::cout << "Buffer size " << vertexData2.size() << " Draw count " << mdl_ref->second.getDrawCnt() << std::endl;
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//		glDrawElements(mdl_ref->second.getPrimitiveType(), mdl_ref->second.getDrawCnt(), GL_UNSIGNED_SHORT, NULL);
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//		glBindVertexArray(0);
	//	}
	//	//glDisable(GL_BLEND);
	//	shd_ref->second.UnUse();
	//}

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

	std::vector<vector2D::Vec2> texcoord;
	texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe - 1) / float(totalframes), 0.f)); // Bottom left
	texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe) / float(totalframes), 0.f)); // Bottom right
	texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe) / float(totalframes), 1.f)); // Top right
	texcoord.emplace_back(vector2D::Vec2(0.f + float(curframe - 1) / float(totalframes), 1.f)); // Top left
	//std::cout << "Texture coords " << std::endl
	//	<< texcoord[0].x << ", " << texcoord[0].y << std::endl
	//	<< texcoord[1].x << ", " << texcoord[1].y << std::endl
	//	<< texcoord[2].x << ", " << texcoord[2].y << std::endl
	//	<< texcoord[3].x << ", " << texcoord[3].y << std::endl;

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
		tmpVtxData.txtIndex = textureid;
		vertexData.emplace_back(tmpVtxData);
	}
	basicbatch.batchdata.insert(basicbatch.batchdata.end(), vertexData.begin(), vertexData.end());
	basicbatch.ebodata.insert(basicbatch.ebodata.end(), mdl_ref->second.primitive.begin(), mdl_ref->second.primitive.end());
	basicbatch.totalindicesize += mdl_ref->second.getPrimitiveCnt();
	basicbatch.vaoid = mdl_ref->second.getVAOid();
	basicbatch.vboid = mdl_ref->second.getVBOid();
	basicbatch.eboid = mdl_ref->second.getEBOid();
	basicbatch.totalsize += vertexData.size();
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
				//std::cout << "Colour " << clr_vtxcoldebug[i].x << ", " << clr_vtxcoldebug[i].y << ", " << clr_vtxcoldebug[i].z << std::endl;
				tmpVtxData.clrVtx = clr_vtxcoldebug[i];
			}
			tmpVtxData.txtVtx = texcoord[i];
			tmpVtxData.txtIndex = texId; // notexture for coldebug
			colDebugData.emplace_back(tmpVtxData);
		}

		debugbatch.batchdata.insert(debugbatch.batchdata.end(), colDebugData.begin(), colDebugData.end());
		debugbatch.ebodata.insert(debugbatch.ebodata.end(), mdl_ref->second.primitive.begin(), mdl_ref->second.primitive.end());
		debugbatch.totalindicesize += mdl_ref->second.getPrimitiveCnt();
		debugbatch.vaoid = mdl_ref->second.getVAOid();
		debugbatch.vboid = mdl_ref->second.getVBOid();
		debugbatch.eboid = mdl_ref->second.getEBOid();
		debugbatch.totalsize += colDebugData.size();
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
	Graphics::camera2d.update(GLHelper::ptr_window);
	objects["Camera"].update(GLHelper::delta_time);

	// update other inputs for physics

	double mousePosX, mousePosY;
	bool mouseClick = false;
	if (GLHelper::mousestateLeft)
	{
		GLHelper::mousestateLeft = false;

		Graphics::Input::getCursorPos(&mousePosX, &mousePosY);
		mouseClick = true;

		std::cout << "this is my mouse pos: " << mousePosX << " " << mousePosY << std::endl;

		//obj->second.modelCenterPos.x = (float)mousePosx;
		//obj->second.modelCenterPos.y = (float)mousePosy;
	}

	if (GLHelper::keystateP)
	{
		movableShape = !movableShape;
		GLHelper::keystateP = false;
	}

	if (GLHelper::keystateC)
	{
		int tmp = (int)(currentCollision);
		currentCollision = (collisionType)(++tmp % 9);
		GLHelper::keystateC = false;
	}
	if (GLHelper::keystateM)
	{
		modulate = !modulate;
		std::cout << "M pressed\n";
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
		std::cout << "T pressed\n";
		GLHelper::keystateT = GL_FALSE;
	}

	if (GLHelper::keystateG)
	{
		graphicsmode = !graphicsmode;
		GLHelper::keystateG = GL_FALSE;
	}
	if (graphicsmode)
	{
		if (GLHelper::keystateX)
		{
			coldebug = !coldebug;
			GLHelper::keystateX = GL_FALSE;
		}
		if (GLHelper::keystateO)
		{
			velocitydirectiondebug = !velocitydirectiondebug;
			GLHelper::keystateX = GL_FALSE;
		}
		if (GLHelper::keystatePlus)
		{
			std::cout << "INCREASING" << std::endl;
			objects["Banana1"].scaling *= 1.1f;

		}
		if (GLHelper::keystateMinus)
		{
			std::cout << "DECREASING" << std::endl;
			objects["Banana1"].scaling /= 1.1f;
		}
		if (GLHelper::keystateSquareBracketLeft)
		{
			std::cout << "ROT LEFT" << std::endl;
			//objects["Banana1"].orientation.x += -1.5f;
			objects["Banana1"].orientation.x += (-1.5f * float(M_PI / 180));
			std::cout << "Orientation " << objects["Banana1"].orientation.x << ", " << objects["Banana1"].orientation.y << std::endl;
		}
		if (GLHelper::keystateSquareBracketRight)
		{
			std::cout << "ROT RIGHT" << std::endl;
			//objects["Banana1"].orientation.x += 1.5f;
			objects["Banana1"].orientation.x += (1.5f * float(M_PI / 180));
			std::cout << "Orientation " << objects["Banana1"].orientation.x << ", " << objects["Banana1"].orientation.y << std::endl;
		}
	}

	if (GLHelper::keystateQ || GLHelper::keystateE)
	{
		std::string modelname;
		if (GLHelper::keystateQ)
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
			GLApp::GLObject::gimmeObject(modelname, finalobjname, vector2D::vec2D(randwidth, randwidth), vector2D::vec2D(static_cast<float>(randx), static_cast<float>(randy)), tmpcolor, objectcounter, randindex);


			//GLApp::GLObject::gimmeObject(modelname, finalobjname, vector2D::vec2D(randwidth, randheight), vector2D::vec2D(static_cast<float>(randx), static_cast<float>(randy)), tmpcolor, objectcounter, randindex);
			//

		//EntityID entid = ecs.GetNewID();
		//ecs.RegisterEntity(entid, finalobjname);
		//ecs.AddComponent<Texture>(entid);
		//createdUnits.resize(objectcounter);


		// Name, type, pos, color, texid, dimension, spritestep, numofsprites, vao, vbo, ebo, shadername
			createdUnits[objectcounter].Add<Render>(finalobjname, "square", vector2D::vec2D(static_cast<float>(randx), static_cast<float>(randy)), tmpcolor, vector2D::vec2D(randwidth, randheight), models.find(modelname)->second.vaoid, models.find(modelname)->second.vboid, models.find(modelname)->second.eboid, "gam200-shdrpgm");
			createdUnits[objectcounter].Add<Texture>(randindex, 1, 4, "");
			//createdUnits[objectcounter].Add<Sprite>("square", vector2D::vec2D(randwidth, randheight));
			createdUnits[objectcounter].Add<Stats>(100);
			//createdUnits[objectcounter].Add<Texture>(1, "tree");
			ecs.setEntityName(createdUnits[objectcounter].GetID(), finalobjname);
			//EntityID testid = createdUnits[objectcounter].GetID();
			//std::cout << "Position " << ecs.GetComponent<Object>(testid)->position.x << "," << ecs.GetComponent<Object>(testid)->position.y << std::endl;

			//createdUnits[objectcounter].Add<Object>(vector2D::vec2D(-200, 0), vector3D::vec3D(0.f, 0.2f, 0.8f), 1, vector2D::vec2D(100, 100), 1, 4, 0, 0, 0, "test");
			//createdUnits[objectcounter].Add<Object>(entid, vector2D::vec2D(static_cast<float>(randx), static_cast<float>(randy)), tmpcolor, randindex, vector2D::vec2D(randwidth, randheight), 1, 4, models.find(modelname)->second.vaoid, models.find(modelname)->second.vboid, models.find(modelname)->second.eboid, "gam200-shdrpgm");
			//createdUnits[objectcounter].Add<Texture>(entid, 1, "tree");
			// Position, Colour, Texture id, Dimension, Current frame, Number of frames, vao, vbo, ebo, shadername
			//std::cout << "Shader program " << tmpObj.shd_ref->second << std::endl;
			// ecs.AddComponent<Object>(entid, vector2D::vec2D(static_cast<float>(randx), static_cast<float>(randy)), tmpcolor, randindex, vector2D::vec2D(randwidth, randheight), 1, 4, models.find(modelname)->second.vaoid, models.find(modelname)->second.vboid, models.find(modelname)->second.eboid, "gam200-shdrpgm");
			//ecs.AddComponent<Object>(entid, vector2D::vec2D(static_cast<float>(randx), static_cast<float>(randy)), tmpcolor, randindex, vector2D::vec2D(randwidth, randheight), 1, 4, models.find(modelname)->second.vaoid, models.find(modelname)->second.vboid, models.find(modelname)->second.eboid, "gam200-shdrpgm");
			GLHelper::keystateQ = false;
			GLHelper::keystateE = false;
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
		timer -= GLHelper::delta_time;

	else
	{
		ecs.RunSystems(2, 100);
		//timer = 0.1;
	}

	// next, iterate through each element of container objects
	// for each object of type GLObject in container objects
	// call update function GLObject::update(delta_time) except on
	// object which has camera embedded in it - this is because
	// the camera has already updated the object's orientation
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
			obj->second.update(GLHelper::delta_time);


			switch (currentCollision)
			{
			case collisionType::CircleDetection:
				// Check for circle circle collision detection (WORKING CODE)
				for (int i{ 0 }; i < 1; ++i)
				{
					//std::cout << "I " << i << std::endl;
					for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
					{
						if (obj1->first[0] == 'B')
						{
							for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
							{
								if (obj2->first[0] == 'B' && obj1->first != obj2->first)
								{
									float radius = obj1->second.body.getRad();
									float radius2 = obj2->second.body.getRad();
									if (physics::CollisionDetectionCircleCircle(obj1->second.modelCenterPos, radius,
																				obj2->second.modelCenterPos, radius2))
									{
										//std::cout << "IM COLLIDING\n";
										obj1->second.overlap = true;
										//createdUnits[obj1->second.objId].Add<Stats>(obj1->first, 100, 1);
										obj2->second.overlap = true;
										//createdUnits[obj2->second.objId].Add<Stats>(obj2->first, 100, 1);
									}
								}
							}
						}
					}
				}
				break;
			}
			/*
			case collisionType::CirclePushResolution:
			//#if false
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
										velocity *= (depth / 2.f);
										obj1->second.body.move(velocity);
										velocity *= -1;
										obj2->second.body.move(velocity);
										obj1->second.modelCenterPos = obj1->second.body.getPos();
										obj2->second.modelCenterPos = obj2->second.body.getPos();
									}
								}
							}
						}
					}
				}
			//#endif
			case collisionType::CircleBlockResolution:
				//#if false
				// Check for circle circle collision with block (WORKING CODE)
				for (int i{ 0 }; i < 8; ++i)
				{
					for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
					{
						if (obj1->first != "Banana1")
						{
							for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
							{
								if (obj2->second.body.getShape() == ShapeType::circle && obj1->first != "Camera" && obj1->first != obj2->first)
								{
									float depth;
									vector2D::vec2D velocity{ 0.f, 0.f };
									if (physics::CollisionBlockResponseCircleCircle(obj1->second.modelCenterPos, obj1->second.scaling.x,
										obj2->second.modelCenterPos, obj2->second.scaling.x,
										velocity, depth))
									{
										velocity *= depth;
										obj2->second.body.move(velocity);
										obj2->second.modelCenterPos = obj2->second.body.getPos();
									}
								}
							}
						}
					}
				}
				//#endif
				break;
			case collisionType::PolygonDetection:
				//#if false
				// Check for polygon polygon collision detection (WORKING CODE)
				for (int i{ 0 }; i < 8; ++i)
				{
					for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
					{
						//if (obj1->first != "Camera")
						if (obj1->second.body.getShape() == ShapeType::box && obj1->first != "Camera")
						{
							for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
							{
								if (obj2->second.body.getShape() == ShapeType::box && obj2->first != "Camera" && obj1->first != obj2->first)
								{
									if (physics::CollisionDetectionPolygonPolygon(obj1->second.body.getTfmVtx(), obj2->second.body.getTfmVtx()))
									{
										obj1->second.overlap = true;
										obj2->second.overlap = true;
									}
								}
							}
						}
						else if (obj1->second.body.getShape() == ShapeType::circle && obj1->first != "Camera")
						{
							for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
							{
								if (obj2->second.body.getShape() == ShapeType::box && obj2->first != "Camera" && obj1->first != obj2->first)
								{
									if (physics::CollisionDetectionPolygonPolygon(obj1->second.body.getTfmVtx(), obj2->second.body.getTfmVtx()))
									{
										obj1->second.overlap = true;
										obj2->second.overlap = true;
									}
								}
							}
						}
					}
				}
				//#endif
				break;
			case collisionType::PolygonPushResolution:
				//#if false
				// Check for polygon polygon collision with push (WORKING CODE)
				for (int i{ 0 }; i < 8; ++i)
				{
					for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
					{
						if (obj1->first == "Banana1")
						{
							for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
							{
								if (obj2->second.body.getShape() == ShapeType::box && obj2->first != "Camera" && obj1->first != obj2->first)
								{
									float depth{ FLT_MAX };
									vector2D::vec2D velocity{ 0.f, 0.f };
									if (physics::CollisionPushPolygonPolygon(obj1->second.body.getTfmVtx(), obj2->second.body.getTfmVtx(), velocity, depth))
									{
										velocity *= (depth / 2.f);
										obj1->second.body.move(velocity);
										velocity *= -1;
										obj2->second.body.move(velocity);
										obj1->second.body.transformVertices();
										obj2->second.body.transformVertices();
										obj1->second.modelCenterPos = obj1->second.body.getPos();
										obj2->second.modelCenterPos = obj2->second.body.getPos();
									}
								}
							}
						}
					}
				}
				//#endif
				break;
			case collisionType::PolygonBlockResolution:
				//#if false
				// Check for polygon polygon collision with block (WORKING CODE)
				for (int i{ 0 }; i < 8; ++i)
				{
					for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
					{
						if (obj1->first != "Banana1")
						{
							for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
							{
								if (obj2->second.body.getShape() == ShapeType::box && obj2->first != "Camera" && obj1->first != obj2->first)
								{
									float depth{ FLT_MAX };
									vector2D::vec2D velocity{ 0.f, 0.f };
									if (physics::CollisionBlockPolygonPolygon(obj1->second.body.getTfmVtx(), obj2->second.body.getTfmVtx(), velocity, depth))
									{
										velocity *= depth;
										velocity *= -1;
										obj2->second.body.move(velocity);
										obj1->second.body.transformVertices();
										obj2->second.body.transformVertices();
										obj2->second.modelCenterPos = obj2->second.body.getPos();
									}
								}
							}
						}
					}
				}
				//#endif
				break;
			case collisionType::PolygonCircleDetection:
				//#if false
				// Check for circle polygon collision detection (WORKING CODE)
				for (int i{ 0 }; i < 8; ++i)
				{
					for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
					{
						if (obj1->second.body.getShape() == ShapeType::circle && obj1->first != "Camera")
						{
							for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
							{
								if (obj2->first != "Camera" && obj2->second.body.getShape() == ShapeType::box && obj1->first != obj2->first)
								{
									if (physics::CollisionDetectionCirclePolygon(obj1->second.body.getPos(), obj1->second.body.getRad(), obj2->second.body.getTfmVtx()))
									{
										obj1->second.overlap = true;
										obj2->second.overlap = true;
									}
								}
							}
						}
					}
				}
				//#endif
				break;
			case collisionType::PolygonCircleResolution:
				//#if false
				// Check for circle polygon collision block (WORKING CODE)
				for (int i{ 0 }; i < 8; ++i)
				{
					for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
					{
						if (obj1->second.body.getShape() == ShapeType::circle && obj1->first != "Camera")
						{
							for (std::map <std::string, GLObject>::iterator obj2 = objects.begin(); obj2 != objects.end(); ++obj2)
							{
								if (obj2->first != "Camera" && obj2->second.body.getShape() == ShapeType::box && obj1->first != obj2->first)
								{
									float depth{ FLT_MAX };
									vector2D::vec2D velocity{ 0.f, 0.f };
									if (physics::CollisionBlockCirclePolygon(obj1->second.body.getPos(), obj1->second.body.getRad(), obj2->second.body.getTfmVtx(), velocity, depth))
									{
										vector2D::Vector2DNormalize(velocity, velocity);
										velocity *= depth;
										obj2->second.body.move(velocity);
										velocity *= -1;
										obj2->second.body.transformVertices();
										obj2->second.modelCenterPos = obj2->second.body.getPos();
									}
								}
							}
						}
					}
				}
				//#endif
				break;
			default:
				break;
			}
			*/

			for (GLuint i = 0; i < obj->second.mdl_ref->second.getPosvtxCnt(); i++)
			{

				obj->second.ndc_coords[i] = obj->second.world_to_ndc_xform * obj->second.worldVertices[i], 1.f;
			}
		}
	}
	//-----------------------------------------extra imgui stuff here  -- we are so moving this out
	//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
	//ImGui::NewFrame();

	//ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.

	//static float f = 0.0f;
	//static int counter = 0;

	//ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.


	//std::vector<EntityID> entities = ecs.getEntities();


	// -----------------------------------------------I'm pretty sure this should be checked with messaging system for when created/ destroyed or itll lag and explode later on

	//if (ImGui::TreeNode("list")) {
	//	ImGui::TreeNode("test");
	//}
	//static bool inputs_step = true;
	//static float  f32_v = 0.123f;
	//const float f32_one = 1.f;
	/*
	for (auto i : ecs.getEntities()) {
		std::string str = ecs.getEntityName(i);
		const char* c = str.c_str();
		if (ImGui::TreeNode(c)) {
			ImGui::TreeNode("test");
			ImGui::TreePop();
		}

	}
	*/
	//for (auto i : ecs.getEntities()) {

//	if (ImGui::TreeNode("Entities")) {
//		for (int i = 1; i < ecs.getEntities().size()+1; ++i) {
//			std::string str = ecs.getEntityName(i);
//
//			if (str == "")
//				continue;
//
//			const char* c = str.c_str();
//			if (ImGui::TreeNode((void*)(intptr_t)i, c)) {	
//				std::vector<std::string> names = ecs.getEntityComponents(i);
//				//--------------------------------------------GET THIS HARDCODE SHIT AWAY FROM ME
//				for (int j = 0; j < names.size(); ++j) {
//					std::string str2 = names[j];
//					const char* c2 = str2.c_str();
//					ImGui::Text(c2);
//
//					if (str2 == "Render") {
//						//ecs.GetComponent<Object>(i)->x;
//						//ImGui::SameLine();
//						ImGui::InputScalar("Pos x", ImGuiDataType_Float, &ecs.GetComponent<Render>(i)->position.x, inputs_step ? &f32_one : NULL);
//						//ImGui::SameLine(); 
//						ImGui::InputScalar("Pos y", ImGuiDataType_Float, &ecs.GetComponent<Render>(i)->position.y, inputs_step ? &f32_one : NULL);
//					}
//				}
//				ImGui::TreePop();
//			}
//		}
//		ImGui::TreePop();
//	}
//
//	// button to create entity
//	static char name[100]{ '\0' };
//	static bool checks[100] = { false };
//	static EntityID temp;
//	static Render* render;
//	static Texture* texture;
//	static Movement* movement;
//	static Stats* stats;
//	static std::vector<int> componentCheck(ecs.getAllRegisteredComponents().size(), 0);
//
//	if (ImGui::Button("Create Entity"))
//	{
//		if (counter & 1)
//		{
//			//if (name[0] != '\0')
//			// creating entity
//			//ecs.RegisterEntity(ecs.GetNewID(), name);
//
//			//for (int i = 0; i < 1; ++i)
//			//{
//			//	if (checks[i])
//			//		ecs.AddComponent<rttr::type::get_by_name(ecs.getAllRegisteredComponents()[i])>();
//			//}
//			if (name[0] == '\0')
//			{
//				checks[99] = true;
//			}
//
//			else
//			{
//				//std::string nameToString = "";
//				//for (int i = 0; i < sizeof(name) / sizeof(char); i++) {
//				//	nameToString += name[i];
//				//}
//				//render->name = nameToString;
//
//				for (int i = 0; i < ecs.getAllRegisteredComponents().size(); ++i)
//					componentCheck[i] = 0;
//
//				memset(name, 0, sizeof(name));
//				counter++;
//			}
//		}
//		else
//		{
//			temp = ecs.GetNewID();
//			ecs.RegisterEntity(temp, "In IMGUI");
//			std::cout << temp;
//			counter++;
//		}
//	}
//
//	if (counter & 1)
//	{
//		if (checks[99])
//			ImGui::Text("Please Input Entity Name");
//
//		ImGui::InputText("Entity Name", name, 100);
//		
//		for (int i = 0; i < ecs.getAllRegisteredComponents().size(); ++i)
//		{
//			std::string componentName = ecs.getAllRegisteredComponents()[i];
//			//rttr::instance instance = rttr::instance::instance(componentList[i]);
//			rttr::type component = rttr::type::get_by_name(componentName);
//			
//			// odd numbers, to join the 2nd checkbox in the same line
//			//if (i & 1)
//			//	ImGui::SameLine();
//
//			ImGui::Checkbox(ecs.getAllRegisteredComponents()[i].c_str(), &checks[i]);
//
//			if (checks[i])
//			{
//				// setting all of the components for the 1st time
//				if (!componentCheck[i])
//				{
//					std::cout << "test" << std::endl;
//					// i wanna make this not hardcoded :((
//					if (componentName == "Render")
//						render = ecs.AddComponent<Render>(temp);
//					else if (componentName == "Texture")
//						texture = ecs.AddComponent<Texture>(temp);
//					else if (componentName == "Movement")
//						movement = ecs.AddComponent<Movement>(temp);
//					else if (componentName == "Stats")
//						stats = ecs.AddComponent<Stats>(temp);
//
//					componentCheck[i] = 1;
//				}
//
//				if (componentName == "Render")
//				{
//					//static const char* types[] = { "square", "circle" };
//					//static int typeCurrent = 0;
//					//ImGui::Combo("Type", &typeCurrent, types, IM_ARRAYSIZE(types));
//					//render->type = types[typeCurrent];
//
//					ImGui::DragFloat2("Position", render->position.m, 1.0f, -500.0f, 500.0f);
//					ImGui::DragFloat3("Color", render->color.m, 0.01f, 0.0f, 1.0f);
//					ImGui::DragFloat2("Dimension", render->dimension.m, 0.5f, 0.0f, 500.0f);
//				}
//
//				else if (componentName == "Movement")
//				{
//					//static const char* types[] = { "square", "circle" };
//					//static int typeCurrent = 0;
//					//ImGui::Combo("Type", &typeCurrent, types, IM_ARRAYSIZE(types));
//					//render->type = types[typeCurrent];
//					
///*					static std::vector<std::string> entities = ecs.getEntityNames();
//					static int currentItem = 0;*/ // Here we store our selection data as an index.
//					//	ImGui::ListBox("Target", &currentItem, 1, entities.data(), entities.size());
//
//					//static ImGuiComboFlags flags = 0;
//
//					//if (ImGui::BeginCombo("combo",combo_preview_value,flags)) {
//					//	for (int i = 0; i < entities.size(); ++i) {
//					//		const bool isSelected = (currentItem == i);
//					//		if (ImGui::Selectable(entities[i], isSelected)) {
//					//			currentItem = i;
//					//		}
//					//	}
//					//}
//					//ImGui::EndCombo;
//					//const char* list = VectorOfStringGetter(entities, entities.size());
//					//ImGui::Combo("Target", &currentItem, list, 3);
//					//ImGui::ListBox("Symbols", currentItem, VectorOfStringGetter((void*)&entities, (int)entities.size()), entities.size(), 3);
//					//ImGui::Combo("Example", &currentItem, entities);
//
//					//ImGui::DragFloat2("Target", render->position.m, 1.0f, -500.0f, 500.0f);
//					ImGui::DragFloat("Force", render->color.m, 0.01f, 0.0f, 1.0f);
//					ImGui::DragFloat("Speed", render->dimension.m, 0.5f, 0.0f, 500.0f);
//				}
//
//
//				//std::string comp = ecs.getAllRegisteredComponents()[i];
//				//rttr::type theComp = rttr::type::get_by_name(ecs.getAllRegisteredComponents()[i]);
//			
//				//rttr::instance IR = r.create();
//				
//				//auto e =r.get_properties();
//				//auto q = *e.begin();
//			
//				//std::cout << prop.get_name();
//				//std::cout << VR.get_type().get_name().to_string();
//				//rttr::instance instance = .get_type().get_raw_type().is_wrapper() ? componentList[i].get_wrapped_instance() : componentList[i];
//
//				//for (auto& property : component.get_properties())
//				//{
//					//std::cout << typeid(property.get_type().get_name().to_string()) << std::endl;
//					//rttr::property propertyType = instance.get_type().get_by_name(property.get_name()).get_property();
//
//					//if (property.get_type() == rttr::type::get<vector2D::vec2D>())
//					//{
//						//static vector2D::vec2D temp;
//						//ImGui::DragFloat2(property.get_name().data(), temp.m, 1.0f, -500.0f, 500.0f);
//						
//						//instance.get_type().get_by_name(property.get_name()).set_property_value(property.get_name(), temp);
//						
//						//std::cout << instance.get_type().get_property_value("position").to_string() << "test";
//						//std::cout << instance.get_type().get_name().to_string() << std::endl;
//						
//					//}
//					//ImGui::InputText(property.get_name().data(), name, 100);
//				//}
//			}
//
//			else if (componentCheck[i])
//			{
//				// i wanna make this not hardcoded :((
//				if (componentName == "Render")
//					ecs.RemoveComponent<Render>(temp);
//				else if (componentName == "Texture")
//					ecs.RemoveComponent<Texture>(temp);
//				else if (componentName == "Movement")
//					ecs.RemoveComponent<Movement>(temp);
//				else if (componentName == "Stats")
//					ecs.RemoveComponent<Stats>(temp);
//
//				componentCheck[i] = 0;
//			}
//		}
//
//		if (ImGui::Button("Cancel"))
//		{
//			ecs.RemoveEntity(temp);
//			memset(name, 0, sizeof(name));
//			counter++;
//		}
//	}
//	
//	//ImGui::TreeNode("THE REST");
//	/*
//	for (auto i : ecs.getEntities()) {
//		std::string str = ecs.getEntityName(i);
//		const char* c = str.c_str();
//		ImGui::Text(c);	
//	for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
//	{
//		if (obj1->first != "Camera")
//		{
//			obj1->second.update(GLHelper::delta_time);
//		}
//	}
//	
//	for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end() ; ++obj1)
//	{
//		for (GLuint i = 0; i < obj1->second.mdl_ref->second.posvtx_cnt; i++)
//		{
//			obj1->second.ndc_coords[i] = obj1->second.world_to_ndc_xform * obj1->second.worldVertices[i], 1.f;
//		}
//	}
//	*/
//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
//	ImGui::Text("Test");
//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
//	ImGui::Checkbox("Another Window", &show_another_window);
//
//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
//
//	//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//	//	counter++;
//	ImGui::SameLine();
//	ImGui::Text("counter = %d", counter);
//
//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//	ImGui::End();
//	
//	if (show_another_window)
//	{
//		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
//		ImGui::Text("Hello from another window!");
//		if (ImGui::Button("Close Me"))
//			show_another_window = false;
//		ImGui::End();
//	}
//
//	ImGui::Render();
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
	title << std::setprecision(2) << " | FPS " << int(GLHelper::fps * 100) / 100.0;
	title << " | Camera Position (" << Graphics::camera2d.getCameraObject().modelCenterPos.x << ", " << Graphics::camera2d.getCameraObject().modelCenterPos.y << ")";
	title << " | Orientation: " << std::setprecision(0) << (Graphics::camera2d.getCameraObject().orientation.x / M_PI * 180) << " degrees";
	title << " | Window height: " << Graphics::camera2d.getHeight();
	title << " | Collision Type: " << collisionInfo[static_cast<collisionType>(currentCollision)];

	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());

	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	if (alphablend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// Part 4: Render each object in container GLApp::objects
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
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
/*  _________________________________________________________________________*/
/*! cleanup

@param none
@return none

This function is empty for now
*/
void GLApp::cleanup() {
	//std::cout << "Cleaning" << std::endl;
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
	// add compiled, linked, and validated shader program to
	// std::map container GLApp::shdrpgms
	GLApp::shdrpgms[shdr_pgm_name] = shdr_pgm;
}

void GLApp::GLObject::gimmeObject(std::string modelname, std::string objname, vector2D::vec2D scale, vector2D::vec2D pos, vector3D::vec3D colour, int id, int texid)
{
	GLObject tmpObj;
	std::string hi;

	tmpObj.objId = id;
	tmpObj.texId = texid;
	if (modelname == "circle")
	{
		//std::cout << "Tex id set\n";
		tmpObj.texId = 3;
	}
	if (modelname == "circle")
	{
		tmpObj.body.createCircleBody(scale.x/2.f, pos, 0.f, false, 0.f, &tmpObj.body, hi);
		//std::cout << "Radius " << tmpObj.body.getRad() << std::endl;
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
	//std::cout << "Position " << pos.x << ", " << pos.y << std::endl;
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

			std::cout << "Shaderprog " << model_shader_pgm << std::endl;
		}
		else
		{
			insert_shdrpgm(model_shader_pgm, model_vert_shader, model_frag_shader);
			Object.shd_ref = shdrpgms.find(model_shader_pgm);
			std::cout << "Shaderprog " << model_shader_pgm << std::endl;
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


void GLApp::entitydraw()
{
	std::vector<EntityID> entities = ecs.getEntities();
	for (int i = 0; i < entities.size(); i++)
	{
		if (ecs.GetComponent<Render>(entities[i]) == nullptr) // Added check for NIL objects
		{
			continue;
		}

		//std::cout << "Integers " << i << ", " << entities[i] << std::endl;
		Render* curobj = ecs.GetComponent<Render>(entities[i]);
		int texid = 0;
		Texture* curobjTexture = ecs.GetComponent<Texture>(entities[i]);
		if (ecs.GetComponent<Texture>(entities[i]) != nullptr)
		{
			texid = curobjTexture->textureID;
		}
		//Stats* overlapobj = ecs.GetComponent<Stats>(entities[i]);
		//if (i > 100)
		//{
		//	std::cout << "Position " << curobj->position.x << ", " << curobj->position.y << std::endl;
		//	std::cout << "Colour " << curobj->color.r << ", " << curobj->color.g << ", " << curobj->color.b << std::endl;
		//	std::cout << "Shader prog " << curobj->shaderName << std::endl;
		//}
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

		//std::vector<vector3D::Vec3> clr_vtx
		//{
		//	vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b),
		//	vector3D::Vec3(color.r, color.g, color.b), vector3D::Vec3(color.r, color.g, color.b)
		//};
		//std::cout << "Texture id " << curobj->textureID << std::endl;
		int numofsprites = 0;
		int spritestep = 0;
		if (texid)
		{
			//curobjTexture->numberOfSprites = 4;
			//curobjTexture->spriteStep = 1;
			numofsprites = curobjTexture->numberOfSprites;
			spritestep = curobjTexture->spriteStep;
			//std::cout << "This current sprite " << spritestep << std::endl;
			if (curobjTexture->textureID == 3 || curobjTexture->textureID == 4)
			{
				//std::cout << "Circle\n";
				//std::cout << overlapobj->overlap << std::endl;
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

		//if (curobj->textureID == 3 || curobj->textureID == 4)
		//{
		//	for (int i = 0; i < 4; i++)
		//	{
		//		std::cout << "Tex coord " << texcoord[i].x << ", " << texcoord[i].y << std::endl;
		//	}
		// }
		//	std::cout << std::endl;

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
			tmpVtxData.txtIndex = texid;
			vertexData.emplace_back(tmpVtxData);
		}

		//	std::vector<Graphics::vertexData> vertexData;
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
		//	tmpVtxData.txtIndex = texId;
		//	vertexData.emplace_back(tmpVtxData);
		//}

		basicbatch.batchdata.insert(basicbatch.batchdata.end(), vertexData.begin(), vertexData.end());
		basicbatch.ebodata.insert(basicbatch.ebodata.end(), models["square"].primitive.begin(), models["square"].primitive.end());
		basicbatch.totalindicesize += models["square"].getPrimitiveCnt();
		basicbatch.vaoid = models["square"].getVAOid();
		basicbatch.vboid = models["square"].getVBOid();
		basicbatch.eboid = models["square"].getEBOid();
		basicbatch.totalsize += vertexData.size();
		basicbatch.primtype = models["square"].getPrimitiveType();
		basicbatch.totaldrawcnt += models["square"].getDrawCnt();

		texcoord.clear();
		ndccoord.clear();
		vertexData.clear();
		//basicbatch.ebodata.insert(basicbatch.ebodata.end(), mdl_ref->second.primitive.begin(), mdl_ref->second.primitive.end());
	//basicbatch.totalindicesize += mdl_ref->second.getPrimitiveCnt();
	//basicbatch.vaoid = mdl_ref->second.getVAOid();
	//basicbatch.vboid = mdl_ref->second.getVBOid();
	//basicbatch.eboid = mdl_ref->second.getEBOid();
	//basicbatch.totalsize += vertexData.size();
	//basicbatch.primtype = mdl_ref->second.getPrimitiveType();
	//basicbatch.totaldrawcnt += mdl_ref->second.getDrawCnt();
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
			//std::cout << "Lines " << i << std::endl;
			//std::cout << "Position " << curobj->position.x << ", " << curobj->position.y << std::endl;
			//std::cout << "Velocity " << objmovement->velocity.x << ", " << objmovement->velocity.y << std::endl;
			//std::cout << "Stored positions " << debugline_poscoord[0].x << ", " << debugline_poscoord[0].y << " "
			//			 << debugline_poscoord[1].x << ", " << debugline_poscoord[1].y << std::endl;
			//std::cout << "Target " << objmovement->target.x << ", " << objmovement->target.y << std::endl;

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
			debuglinebatch.totalsize += debugline_vertexData.size();
			debuglinebatch.primtype = GL_LINES;
			debuglinebatch.totaldrawcnt += models["line"].getDrawCnt();

			debugline_ndccoord.clear();
			debugline_vertexData.clear();
		}
	}
}