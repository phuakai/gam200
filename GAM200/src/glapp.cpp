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
#include <random>
#include "ECS.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdint.h>
#include "pathfinding.h"

#include <registration.h>
//#include <instance.h>
#include <type>


struct Position
{
	float x;
	float y;
};

struct Velocity
{
	float x;
	float y;
};

struct Randomness
{
	float a;
};

struct he {
	int a;
};
struct Stats {
	std::string name;
	int health;
};

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
//				if (str2 == "Position") {
//					//ecs.GetComponent<Position>(i)->x;
//					//ImGui::SameLine();
//					ImGui::InputScalar("Pos x", ImGuiDataType_Float, &ecs.GetComponent<Position>(i)->x, inputs_step ? &f32_one : NULL);
//					//ImGui::SameLine(); 
//					ImGui::InputScalar("Pos y", ImGuiDataType_Float, &ecs.GetComponent<Position>(i)->y, inputs_step ? &f32_one : NULL);
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
short GLApp::currentCollision;
bool GLApp::stepByStepCollision;

int tmpobjcounter{};

ECS ecs;

Entity player1;
std::vector<Entity> enemyUnits(1000);
System<Movement, Position> system1(ecs, 1);

extern int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
std::vector<vector2D::vec2D> walls;
float timer;


class test_class {
public:
	test_class(int value) : m_value(value) {}
	void print_value() const { std::cout << m_value; }
private:
	int m_value;
	RTTR_ENABLE();

};

RTTR_REGISTRATION{
	rttr::registration::class_<test_class>("test_class")
	.constructor<int>()
	.method("print_value", &test_class::print_value);

	//i think we cant register private vars anymore, just use setters /getters and register the method
	//.property("value", &test_class::m_value)
}

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
	//GLuint buffer;
	//glGetVertexArrayIndexediv(mdl_ref->second.getVAOid(), 0, GL_VERTEX_BINDING_BUFFER, reinterpret_cast<GLint*>(&buffer));


	glNamedBufferSubData(mdl_ref->second.getVBOid(), 0, sizeof(vector2D::vec2D) * ndc_coords.size(), ndc_coords.data()); // Set new buffer index with subdata

	glVertexArrayAttribBinding(mdl_ref->second.getVAOid(), 0, 0);

	glBindVertexArray(mdl_ref->second.getVAOid()); // Rebind VAO

	// copy object's color to fragment shader uniform variable uColor
	shd_ref->second.SetUniform("uColor", color);

	// copy object's model-to-NDC matrix to vertex shader's
	// uniform variable uModelToNDC
	//shd_ref->second.SetUniform("uModel_to_NDC", mdl_to_ndc_xform);

	// call glDrawElements with appropriate arguments
	glDrawElements(mdl_ref->second.getPrimitiveType(), mdl_ref->second.getDrawCnt(), GL_UNSIGNED_SHORT, NULL);

	// unbind VAO and unload shader program
	glBindVertexArray(0);
	shd_ref->second.UnUse();
}
//-----------------------------------------extra imgui stuff here
bool show_demo_window;
bool show_another_window;
ImVec4 clear_color;
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
	//GLApp::init_scene("../scenes/gam200.scn");

	GLApp::GLObject::gimmeObject("square", "Camera", vector2D::vec2D(1, 1), vector2D::vec2D(0, 0), glm::vec3(1, 1, 1));

	// Part 4: initialize 
	Graphics::camera2d.init(GLHelper::ptr_window, &GLApp::objects.at("Camera"));

	// Store physics related info to be printed in title bar
	currentCollision = 0;
	stepByStepCollision = false;
	collisionInfo[collisionType::NIL] = "NIL";
	collisionInfo[collisionType::SAT] = "SAT";
	collisionInfo[collisionType::DIAG] = "DIAG";
	collisionInfo[collisionType::AABBSTATIC] = "AABBSTATIC";
	collisionInfo[collisionType::SNAPDIAGSTATIC] = "SNAPDIAGSTATIC";


	ecs.RegisterComponent<Position>("Position");
	ecs.RegisterComponent<Velocity>("Velocity");
	ecs.RegisterComponent<Randomness>("Randomness");
	ecs.RegisterComponent<he>("he");

	Entity ent1( ecs);

	ent1.Add<Position>({ 1.45,2 });
	ent1.Add<Velocity>({ .5f,.5f });
	ent1.Add<Randomness>({ .25f });
	ecs.setEntityName(ent1.GetID(), "enemy1");

	Entity ent2( ecs);

	ent2.Add<Position>({ 4,3 });
	ent2.Add<Velocity>({ 123.5f,123.5f });
	ent2.Add<Randomness>({ .25f });
	ecs.setEntityName(ent2.GetID(), "enemy2");


	//ecs.getEntityComponents(1);

	//std::cout << ent2.GetID() << std::endl;

	//for (auto i : ecs.getEntities()) {
	//	std::cout << "entity ID " << i.first << std::endl;
	//	
	//}
	//auto it = ecs.m_entityArchetypeMap.find(ent2.GetID());

	//std::cout << "archetype is "  << reinterpret_cast<Position*>(it->second.archetype->componentData[0]);
	// 
	//Velocity* dab = reinterpret_cast<Velocity*>(it->second.archetype->componentData[1]+8);
	//std::cout << dab->x << dab->y;
	//std::cout << "archetype is "  << (it->second.archetype->componentData[0]);

	//
	//std::unordered_map test = ecs.getComponents(); 
	//
	//for (auto i = test.begin(); i != test.end(); i++)
	//	std::cout << i->first << "      " << (i->second)->GetName()
	//	<< std::endl;
	//


	
	//for (int i : ecs.getEntities()) {
	//	std::cout << i << std::endl;
	//	std::cout << ecs.getEntityName(i);
	//}

	//-----------------------------------------extra imgui stuff here
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(GLHelper::ptr_window, true);
	ImGui_ImplOpenGL3_Init(NULL);

	show_demo_window = true;
	show_another_window = false;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ecs.RegisterComponent<Position>();
	ecs.RegisterComponent<Movement>();
	ecs.RegisterComponent<Sprite>();
	ecs.RegisterComponent<Stats>();


	EntityID id;

	player1.Add<Position>(vector2D::vec2D(-200, 0));
	// velocity, target, force, speed
	player1.Add<Movement>(vector2D::vec2D(0, 0), vector2D::vec2D(0, 0), 10, 2);
	player1.Add<Sprite>("square", vector2D::vec2D(20, 20));
	player1.Add<Stats>("player1");

	id = player1.GetID();
	GLApp::GLObject::gimmeObject(ecs.GetComponent<Sprite>(id)->type, ecs.GetComponent<Stats>(id)->name, ecs.GetComponent<Sprite>(id)->size, ecs.GetComponent<Position>(id)->position, glm::vec3(0.3, 0.3, 0.7));
	//GLApp::GLObject::gimmeObject("square", playerList[i].unitName, playerList[i].size, vector2D::vec2D(playerList[i].position.x, playerList[i].position.y), glm::vec3(0.3, 0.3, 0.7));



	unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	// create default engine as source of randomness
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> colour(0.f, 1.f);

	for (int i = 0; i < enemyUnits.size(); ++i)
	{
		float randr = colour(generator);
		float randg = colour(generator);
		float randb = colour(generator);

		enemyUnits[i].Add<Position>(vector2D::vec2D(-450 + (i % 45 * 20), 400 - ((int)i / 30 * 10)));
		// velocity, target, force, speed
		enemyUnits[i].Add<Movement>(vector2D::vec2D(0, 0), ecs.GetComponent<Position>(player1.GetID())->position, 1, 2);
		enemyUnits[i].Add<Sprite>("square", vector2D::vec2D(10, 10));
		enemyUnits[i].Add<Stats>("enemy" + std::to_string(i + 1), 100);

		id = enemyUnits[i].GetID();
		GLApp::GLObject::gimmeObject(ecs.GetComponent<Sprite>(id)->type, ecs.GetComponent<Stats>(id)->name, ecs.GetComponent<Sprite>(id)->size, ecs.GetComponent<Position>(id)->position, glm::vec3(randr, randg, randb));
	}




	//---------------------------------------------------------------------------------RTTR stuff

	//test_class obj(42);
	//rttr::type class_type = rttr::type::get("test_class");
	//class_type.invoke("print_value", obj, {});


	//std::vector<Entity> playerUnits(1000);

	// objects creation
	//for (int i = 0; i < 2500; ++i)
	//{
	//	Unit enemy;
	//	enemy.position = vector2D::vec2D(-450 + (i % 45 * 20), 400 - ((int)i/30 * 10));
	//	enemy.maxSpeed = 2;
	//	enemy.maxForce = 1;
	//	enemy.size = vector2D::vec2D(10, 10);
	//	enemy.unitID = i + 1;
	//	enemy.unitName = "enemy" + std::to_string(i + 1);
	//	enemyList.push_back(enemy);


	//for (int i = 0; i < enemyList.size(); ++i)
	//{

	//	float randr = colour(generator);
	//	float randg = colour(generator);
	//	float randb = colour(generator);

	//	enemyList[i].target = &playerList[0];
	//	GLApp::GLObject::gimmeObject("square", enemyList[i].unitName, enemyList[i].size, vector2D::vec2D(enemyList[i].position.x, enemyList[i].position.y), glm::vec3(randr, randg, randb));
	//	//GLApp::GLObject::gimmeObject("square", enemyList[i].unitName, enemyList[i].size, vector2D::vec2D(enemyList[i].position.x, enemyList[i].position.y), glm::vec3(0.7, 0.3, 0.3));
	//}

	// walls
	//walls.push_back(vector2D::vec2D(10, 11));
	//walls.push_back(vector2D::vec2D(10, 12));
	//walls.push_back(vector2D::vec2D(10, 13));
	//walls.push_back(vector2D::vec2D(10, 14));
	//walls.push_back(vector2D::vec2D(10, 15));

	timer = 4;

	generateDijkstraCost(ecs.GetComponent<Position>(id)->position, walls);
	generateFlowField(ecs.GetComponent<Position>(id)->position);
	//enemyList[5].Print();

	vector2D::vec2D startingPoint{ 0 - 500 + 1000 / MAX_GRID_X / 2, 0 - 500 + 1000 / MAX_GRID_Y / 2 };

	int counter = 1;
	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			// wall
			if (dijkstraField[i][j] == WALL)
				GLApp::GLObject::gimmeObject("square", std::to_string(counter), vector2D::vec2D(1000 / MAX_GRID_X - 5, 1000 / MAX_GRID_Y - 5), vector2D::vec2D(startingPoint.x + (j * 1000 / MAX_GRID_X), startingPoint.y + (i * 1000 / MAX_GRID_X)), glm::vec3(0.5, 0.5, 0.5));
			else
				GLApp::GLObject::gimmeObject("square", std::to_string(counter), vector2D::vec2D(1000 / MAX_GRID_X - 5, 1000 / MAX_GRID_Y - 5), vector2D::vec2D(startingPoint.x + (j * 1000 / MAX_GRID_X), startingPoint.y + (i * 1000 / MAX_GRID_X)), glm::vec3(1, 1, 1));

			++counter;
		}
	}
	GLApp::GLObject::gimmeObject("square", "0gridBackground", vector2D::vec2D(1010, 1010), vector2D::vec2D(0, 0), glm::vec3(0, 0, 0));

	// Part 5: Print OpenGL context and GPU specs
	//GLHelper::print_specs();


	system1.Action([](const float elapsedMilliseconds,
	const std::vector<EntityID>& entities,
	Movement* m,
	Position* p)
	{
		for (std::size_t i = 0; i < entities.size(); ++i)
		{
			vector2D::vec2D nodePosition = (p[i].position - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

			vector2D::Vector2DNormalize(m[i].velocity, flowField[(int)nodePosition.y][(int)nodePosition.x]);

			std::vector<vector2D::vec2D> allVelocity{ vector2D::vec2D(0,0), vector2D::vec2D(0,0),vector2D::vec2D(0,0) };

			movementFlocking(entities[i], m[i].target, allVelocity);

			//m[i].velocity += allVelocity[0] + (allVelocity[1] * 0.05) + allVelocity[2];

			//// capping speed
			//if (vector2D::Vector2DLength(m[i].velocity) > m[i].speed)
			//{
			//	m[i].velocity *= m[i].speed / vector2D::Vector2DLength(m[i].velocity);
			//}

			//p[i].position += m[i].velocity * GLHelper::delta_time * 100;

			p[i].position += vector2D::vec2D(-1, -1);
		}
	});
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
		stepByStepCollision = !stepByStepCollision;
		GLHelper::keystateP = false;
	}

	if (GLHelper::keystateC)
	{
		currentCollision = ++currentCollision % 5;
		
		GLHelper::keystateC = false;
	}
	if (GLHelper::keystateQ)
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

		std::uniform_int_distribution<int> posrandom(19000, 20000);
		int randx = posrandom(generator);
		int randy = posrandom(generator);

		std::uniform_int_distribution<int> sizerandom(50, 150);
		float randwidth = (float)sizerandom(generator);
		float randheight = (float)sizerandom(generator);
		//std::cout << "Values " << randx << ", " << randy << std::endl;
		GLApp::GLObject::gimmeObject("square", finalobjname, vector2D::vec2D(randwidth, randheight), vector2D::vec2D(-randx, -randy), glm::vec3(0, 0, 0));
		GLHelper::keystateQ = false;
	}

	Position* playerPosition = ecs.GetComponent<Position>(player1.GetID());
	Stats* playerInfo = ecs.GetComponent<Stats>(player1.GetID());

	if (timer > 0)
		timer -= GLHelper::delta_time;

	else
		ecs.RunSystems(1, 100);

	// next, iterate through each element of container objects
	// for each object of type GLObject in container objects
	// call update function GLObject::update(delta_time) except on
	// object which has camera embedded in it - this is because
	// the camera has already updated the object's orientation
	bool test{ true };
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
				//if (objects["Camera"].overlap)
				//{
					//objects["Camera"].modelCenterPos = objects["Camera"].worldToMdlXform * glm::vec3(objects["Camera"].worldCenterPos, 1.f);
				//}
				break;
			case 4: //AABB collision
				if (physics::shapeOverlapStaticAABB(objects["Camera"], obj->second))
				{
					obj->second.overlap = true;
					objects["Camera"].overlap = true;
				}
				break;
			default:
				break;
			}
			//if (GLHelper::mousestateLeft)
			//{
			//	GLHelper::mousestateLeft = false;
			//	double mousePosx, mousePosy;

			//	Graphics::Input::getCursorPos(&mousePosx, &mousePosy);

			//	std::cout << "this is my mouse pos: " << mousePosx << " " << mousePosy << std::endl;

			//	//obj->second.modelCenterPos.x = (float)mousePosx;
			//	//obj->second.modelCenterPos.y = (float)mousePosy;
			//}
			for (GLuint i = 0; i < obj->second.mdl_ref->second.getPosvtxCnt(); i++)
			{
				obj->second.ndc_coords[i] = obj->second.world_to_ndc_xform * obj->second.worldVertices[i], 1.f;
			}
		}
	}
	//-----------------------------------------extra imgui stuff here  -- we are so moving this out
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	
	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.


	//std::vector<EntityID> entities = ecs.getEntities();


	// -----------------------------------------------I'm pretty sure this should be checked with messaging system for when created/ destroyed or itll lag and explode later on
		
	//if (ImGui::TreeNode("list")) {
	//	ImGui::TreeNode("test");
	//}
	static bool inputs_step = true;
	//static float  f32_v = 0.123f;
	const float f32_one = 1.f;
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
	if (ImGui::TreeNode("Entities")) {
		for (int i = 1; i < ecs.getEntities().size()+1; ++i) {
			std::string str = ecs.getEntityName(i);
			const char* c = str.c_str();
			if (ImGui::TreeNode((void*)(intptr_t)i, c)) {	
				std::vector<std::string> names = ecs.getEntityComponents(i);
				//--------------------------------------------GET THIS HARDCODE SHIT AWAY FROM ME
				for (int j = 0; j < names.size(); ++j) {
					std::string str2 = names[j];
					const char* c2 = str2.c_str();
					ImGui::Text(c2);

					if (str2 == "Position") {
						//ecs.GetComponent<Position>(i)->x;
						//ImGui::SameLine();
						ImGui::InputScalar("Pos x", ImGuiDataType_Float, &ecs.GetComponent<Position>(i)->x, inputs_step ? &f32_one : NULL);
						//ImGui::SameLine(); 
						ImGui::InputScalar("Pos y", ImGuiDataType_Float, &ecs.GetComponent<Position>(i)->y, inputs_step ? &f32_one : NULL);
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	// button to create entity
	static char name[100]{ '\0' };
	static std::string createEntityButtonName = "Create Entity";
	const char* text = createEntityButtonName.c_str();
	if (ImGui::Button(text))
	{
		if (name[0] != '\0')
		{
			// creating entity
			ecs.RegisterEntity(ecs.GetNewID(), name);
			memset(name, 0, sizeof(name));
		}

		createEntityButtonName = "Create Entity";
		text = createEntityButtonName.c_str();
		counter++;
	}
	if (counter & 1)
	{
		ImGui::InputText("Entity Name", name, 100);

		createEntityButtonName = "Cancel";
		text = createEntityButtonName.c_str();

		if (ImGui::Button("Create Entity"))
		{
			if (name[0] != '\0')
			{
				// creating entity
				ecs.RegisterEntity(ecs.GetNewID(), name);
				memset(name, 0, sizeof(name));
			}
			createEntityButtonName = "Create Entity";
			text = createEntityButtonName.c_str();
			counter++;
		}
	}
	
	//ImGui::TreeNode("THE REST");
	/*
	for (auto i : ecs.getEntities()) {
		std::string str = ecs.getEntityName(i);
		const char* c = str.c_str();
		ImGui::Text(c);	
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

		if (playerInfo->name == obj->first && mouseClick)
		{
			playerPosition->position = vector2D::vec2D(mousePosX, mousePosY);
			obj->second.modelCenterPos = playerPosition->position;

			generateDijkstraCost(playerPosition->position, walls);
			generateFlowField(playerPosition->position);
		}

		for (int i = 0; i < enemyUnits.size(); ++i)
		{
			if (ecs.GetComponent<Stats>(enemyUnits[i].GetID())->name == obj->first)
			{
				obj->second.modelCenterPos = ecs.GetComponent<Position>(enemyUnits[i].GetID())->position;
				break;
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
	title << " | Camera Position (" << Graphics::camera2d.getCameraObject().modelCenterPos.x << ", " << Graphics::camera2d.getCameraObject().modelCenterPos.y << ")";
	title << " | Orientation: " << std::setprecision(0) << (Graphics::camera2d.getCameraObject().orientation.x / M_PI * 180) << " degrees";
	title << " | Window height: " << Graphics::camera2d.getHeight();
	title << " | Collision Type: " << collisionInfo[static_cast< collisionType>(currentCollision)];
	title << std::setprecision(2) << " | FPS " << int(GLHelper::fps*100)/100.0;

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
		case 4: //collisionType::AABBSTATIC
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
				collisionDebug(obj->second);
				
				obj->second.color = red;
			}
			break;
		default:
			break;
		}
	}

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

void GLApp::GLObject::gimmeObject(std::string modelname, std::string objname, vector2D::vec2D scale, vector2D::vec2D pos, glm::vec3 colour)
{
	GLObject tmpObj;
	unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	// create default engine as source of randomness
	std::default_random_engine generator(seed);

	tmpObj.color = colour;
	tmpObj.scaling = scale;
	tmpObj.orientation = vector2D::vec2D(0, 0);
	tmpObj.modelCenterPos = pos;
	tmpObj.vel = vector2D::vec2D(10, 10);

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
		velocity >> Object.vel.x >> Object.vel.y;
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
