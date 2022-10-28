
#include "mainHeader.h"
#include "ECS.h"
#include "eventManager.h"
#include "physicsPartition.h"
#include "serialization.h"
#include "pathfinding.h"
#include "physics.h"
#include "vec2D.h"
#include "vec3D.h"
#include "Font.h"
#include "AudioEngine.h"
#include <app.h>
#include <collision.h>

#include <random>

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
		.property("type", &Render::type)
		.property("color", &Render::color)
		.property("vaoID", &Render::vaoID)
		.property("vboID", &Render::vboID)
		.property("eboID", &Render::eboID)
		.property("shaderName", &Render::shaderName);

	rttr::registration::class_<BaseInfo>("BaseInfo")
		.property("name", &BaseInfo::name)
		.property("position", &BaseInfo::position)
		.property("dimension", &BaseInfo::dimension);

	rttr::registration::class_<Texture>("Texture")
		.property("textureID", &Texture::textureID)
		.property("textureName", &Texture::textureName)
		.property("spriteStep", &Texture::spriteStep)
		.property("numberOfSprites", &Texture::numberOfSprites);

	rttr::registration::class_<Physics>("Physics")
		.property("velocity", &Physics::velocity)
		.property("target", &Physics::target)
		.property("formationTarget", &Physics::formationTarget)
		.property("force", &Physics::force)
		.property("speed", &Physics::speed)
		.property("collisionFlag", &Physics::collisionFlag)
		.property("collisionResponse", &Physics::collisionResponse)
		.property("radius", &Physics::radius)
		.property("reached", &Physics::reached)
		.property("formationManagerID", &Physics::formationManagerID);

	rttr::registration::class_<Stats>("Stats")
		.property("health", &Stats::getHealth, &Stats::setHealth);
}

ECS ecs;

extern Entity player1;
extern std::vector<Entity> walls;

Entity player1;
std::vector<Entity> walls(0);
std::vector<Entity> enemyUnits(100);

// for checking
Entity enemyManagerEntity;
extern Entity enemyManagerEntity;

//std::vector<Entity> createdUnits(100); // precreated empty entities

System<Texture> textureSystem(ecs, 1);
System<BaseInfo, Physics> system1(ecs, 2);

extern EventManager eventManager;
EventManager eventManager;

extern std::vector<FormationManager> formationManagers;
std::vector<FormationManager> formationManagers;

extern int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
float timer;

quadTree mainTree;

spooky::CAudioEngine audioEngine;

void engineInit()
{
	mainTree.createQuadTree(vector2D::vec2D(0, 0), 500, 500, nullptr);

	// ======================================================================================================================================
	// ECS: Register structs as components 
	ecs.RegisterComponent<BaseInfo>("BaseInfo");
	ecs.RegisterComponent<Texture>("Texture");
	ecs.RegisterComponent<Physics>("Physics");
	//ecs.RegisterComponent<Sprite>("Sprite");
	ecs.RegisterComponent<Stats>("Stats");
	ecs.RegisterComponent<Render>("Render");

	// ECS: Adding components into Entities
	// Render: name, type, position, color, dimension, vaoID, vboID, eboID, shaderName(?)
	player1.Add<Render>("square", vector3D::vec3D(0.3f, 0.3f, 0.7f), 0, 0, 0, "gam200-shdrpgm");
	player1.Add<BaseInfo>("player1", vector2D::vec2D(-200.f, 0.f), vector2D::vec2D(20.f, 20.f));
	// velocity, target, force, speed
	player1.Add<Texture>(0, 1, 1, "none");
	//player1.Add<Stats>(100);
	ecs.setEntityName(player1.GetID(), "Mouse Click");												// may not need this after rttr

	EntityID playerID = player1.GetID();

	FormationManager enemyManager;
	enemyManager.target = ecs.GetComponent<BaseInfo>(playerID)->position;

	unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	// create default engine as source of randomness
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> colour(0.f, 1.f);

	for (int i = 0; i < enemyUnits.size(); ++i)
	{
		float randr = colour(generator);
		float randg = colour(generator);
		float randb = colour(generator);

		enemyUnits[i].Add<Render>("square", vector3D::vec3D(randr, randg, randb), 0, 0, 0, "gam200-shdrpgm");
		enemyUnits[i].Add<BaseInfo>("enemy" + std::to_string(i + 1), vector2D::vec2D(-450.f + (i % 45 * 20), 400.f - ((int)i / 30 * 10)), vector2D::vec2D(10, 10));
		enemyUnits[i].Add<Texture>(6, 1, 1, "Enemy");
		enemyUnits[i].Add<Physics>(vector2D::vec2D(0, 0), ecs.GetComponent<BaseInfo>(playerID)->position, vector2D::vec2D(0, 0), 1, 2, 0, vector2D::vec2D(0, 0), 10, false, 0);
		//enemyUnits[i].Add<Stats>(100);
		ecs.setEntityName(enemyUnits[i].GetID(), "enemy" + std::to_string(i + 1));

		EntityID enemyID = enemyUnits[i].GetID();

		enemyManager.addCharacter(enemyID);
		mainTree.insertSuccessfully(enemyID, ecs.GetComponent<BaseInfo>(enemyID)->position);
	}
	formationManagers.push_back(enemyManager);

	enemyManagerEntity.Add<Render>("square", vector3D::vec3D(0, 0, 0), 0, 0, 0, "gam200-shdrpgm");
	enemyManagerEntity.Add<BaseInfo>("enemyManager", vector2D::vec2D(0, 0), vector2D::vec2D(20, 20));
	enemyManagerEntity.Add<Texture>(6, 1, 1, "Enemy");
	enemyManagerEntity.Add<Physics>(vector2D::vec2D(0, 0), ecs.GetComponent<BaseInfo>(playerID)->position, vector2D::vec2D(0, 0), 1, 2, 0, vector2D::vec2D(0, 0), 10, false);
	ecs.setEntityName(enemyManagerEntity.GetID(), "enemyManager");

	timer = 4;

	generateDijkstraCost(ecs.GetComponent<BaseInfo>(playerID)->position, walls);
	generateFlowField(ecs.GetComponent<BaseInfo>(playerID)->position);
	
	eventManager.subscribe(Systems::Collision);
	eventManager.subscribe(Systems::Physics);

	system1.Action([](const float elapsedMilliseconds, const std::vector<EntityID>& entities, BaseInfo* p, Physics* m)
	{
		for (auto i : entities)
		{
			if (i == enemyManagerEntity.GetID())
			{
				continue;
			}

			MoveEvent entityToMove;

			entityToMove.id = i;
			entityToMove.message = (1UL << Systems::Physics);

			eventManager.post(entityToMove);
		}
		//std::cout << eventManager.findQueue(Systems::Physics).size() << std::endl;

		// Check with walls
		///for (int i = 0; i < walls.size(); ++i)
		///{
		///	BaseInfo* pointer = ecs.GetComponent<BaseInfo>(walls[i].GetID());
		///	std::list<EntityID*> myList;
		///	AABB range(pointer->position.x - pointer->dimension.x,
		///		pointer->position.y - pointer->dimension.y,
		///		pointer->position.x + pointer->dimension.x,
		///		pointer->position.y + pointer->dimension.y);
		///	mainTree.query(range, myList);

		///	vector2D::vec2D wallPos = ecs.GetComponent<BaseInfo>(walls[i].GetID())->position;
		///	vector2D::vec2D wallDims = ecs.GetComponent<BaseInfo>(walls[i].GetID())->dimension;
		///	wallDims /= 2.f;

		///	// Create vertices for wall
		///	std::vector<vector2D::vec2D> wallVtx;
		///	for (int j = -1; j < 2; j += 2)
		///	{
		///		for (int k = -1; k < 2; k += 2)
		///		{
		///			wallVtx.emplace_back(vector2D::vec2D(wallPos.x + k * wallDims.x, wallPos.y + j * wallDims.y));
		///		}
		///	}
		
		///	for (std::list <EntityID*>::iterator enemyUnit = myList.begin(); enemyUnit != myList.end(); ++enemyUnit)
		///	{
		///		bool check = false;
		///		for (int j = 0; j < walls.size(); ++j)
		///		{
		///			if ((**enemyUnit) == walls[j].GetID())
		///			{
		///				check = true;
		///				break;
		///			}
		///		}
		///		if (check)
		///			continue;

		///		vector2D::vec2D enemyPos = ecs.GetComponent<BaseInfo>(**enemyUnit)->position;
		///		vector2D::vec2D enemyDims = ecs.GetComponent<BaseInfo>(**enemyUnit)->dimension;
		///		enemyDims /= 2.f;

		///		// Create vertices for enemy
		///		std::vector<vector2D::vec2D> enemyVtx;
		///		for (int j = -1; j < 2; j += 2)
		///		{
		///			for (int k = -1; k < 2; k += 2)
		///			{
		///				enemyVtx.emplace_back(vector2D::vec2D(enemyPos.x + k * enemyDims.x, enemyPos.y + j * enemyDims.y));
		///			}
		///		}
		///		if (physics::CollisionDetectionPolygonPolygon(wallVtx, enemyVtx))
		///		{
		///			Physics* collided = ecs.GetComponent<Physics>(**enemyUnit);
		///			collided->collisionFlag = true;
		///			collided->collisionResponse = pointer->position;
		///		}

		///	}
		///}

		//vector2D::vec2D oldPosition = baseInfo->position;
		//vector2D::vec2D changedVelocity(0, 0);
		//vector2D::vec2D offsetVector(0, 0);

		//if ((p[i].position.x >= m[i].target.x - 10 && p[i].position.x <= m[i].target.x + 10) &&
		//	(p[i].position.y >= m[i].target.y - 10 && p[i].position.y <= m[i].target.y + 10) || enemyManager.reached)
		//{
		//	if (!m[i].reached)
		//	{
		//		enemyManager.updateSlot(i);
		//		m[i].reached = true;
		//	}

		//	if (!((p[i].position.x >= m[i].formationTarget.x - 2 && p[i].position.x <= m[i].formationTarget.x + 2) &&
		//		(p[i].position.y >= m[i].formationTarget.y - 2 && p[i].position.y <= m[i].formationTarget.y + 2)))
		//	{
		//		changedVelocity = m[i].formationTarget - p[i].position;
		//	}
		//}
		//else
		//{
		//	m[i].reached = false;

		//	vector2D::vec2D nodePosition = (p[i].position - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

		//	if ((int)nodePosition.x < 0 || (int)nodePosition.y < 0 || (int)nodePosition.x >= MAX_GRID_X || (int)nodePosition.x >= MAX_GRID_Y)
		//	{
		//		changedVelocity = m[i].target - p[i].position;
		//	}
		//	else
		//	{
		//		changedVelocity = flowField[(int)nodePosition.y][(int)nodePosition.x] + offsetVector;
		//	}

		//	vector2D::Vector2DNormalize(m[i].formationTarget, m[i].formationTarget);

		//	changedVelocity += m[i].formationTarget * 0.1;

		//	std::vector<vector2D::vec2D> allVelocity{ vector2D::vec2D(0.f,0.f), vector2D::vec2D(0.f,0.f),vector2D::vec2D(0.f,0.f) };

		//	movementFlocking(entities[i], allVelocity);

		//	changedVelocity += (allVelocity[0] * 6 + (allVelocity[1] * 0.1f) + allVelocity[2]); // *flockingModifier;
		//}

		//if (!((p[i].position.x >= m[i].target.x - 5 && p[i].position.x <= m[i].target.x + 5) &&
		//	(p[i].position.y >= m[i].target.y - 5 && p[i].position.y <= m[i].target.y + 5) || enemyManager.reached))
		//{
		//	changedVelocity = m[i].formationTarget - p[i].position;
		//}

		//vector2D::Vector2DNormalize(changedVelocity, changedVelocity);

		//// capping speed
		//if (vector2D::Vector2DLength(changedVelocity) > m[i].speed)
		//{
		//	changedVelocity *= m[i].speed / vector2D::Vector2DLength(changedVelocity);
		//}
		//changedVelocity *= m[i].speed;

		//p[i].position += changedVelocity * (static_cast<float>(Graphics::Input::delta_time) > 1 / 60.f ? 1 / 60.f : static_cast<float>(Graphics::Input::delta_time)) * 100;

		//m[i].velocity = changedVelocity;
		//mainTree.updatePoint(entities[i], oldPosition, p[i].position, mainTree);
	});

	GLApp::init();

	//spooky::CAudioEngine audioEngine;
	audioEngine.Init();
	
	//audioEngine.LoadSound("../asset/sounds/StarWars60.wav", false);
	audioEngine.PlaySound("../asset/sounds/StarWars60.wav", spooky::Vector2{ 0, 0 }, audioEngine.VolumeTodb(1.0f));

	Font::init();
	Font::shader.CompileShaderFromFile(GL_VERTEX_SHADER, "../asset/shaders/Font.vert");
	Font::shader.CompileShaderFromFile(GL_FRAGMENT_SHADER, "../asset/shaders/Font.frag");
	if (false == Font::shader.Link() || false == Font::shader.IsLinked())
	{
		assert("ERROR: Unable to link shaders!");
	}
	if (false == Font::shader.Validate() || false == Font::shader.Validate())
	{
		assert("ERROR: Unable to validate shaders!");
	}
}

void engineUpdate()
{
	if (timer > 0)
		timer -= (float)Graphics::Input::delta_time;

	else
	{
		ecs.RunSystems(2, 100);

		for (int i = 0; i < formationManagers.size(); ++i)
		{
			vector2D::vec2D anchorNode = (formationManagers[i].getAnchorPosition() - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);
			//std::cout << formationManagers[i].target.x << "\t" << formationManagers[i].target.y << std::endl;

			if ((formationManagers[i].getAnchorPosition().x >= formationManagers[i].target.x - 20 && formationManagers[i].getAnchorPosition().x <= formationManagers[i].target.x + 20) &&
				(formationManagers[i].getAnchorPosition().y >= formationManagers[i].target.y - 20 && formationManagers[i].getAnchorPosition().y <= formationManagers[i].target.y + 20))
			{
				formationManagers[i].reached = true;
			}

			if (!formationManagers[i].reached)
			{
				formationManagers[i].updateAnchorPosition();
				ecs.GetComponent<BaseInfo>(enemyManagerEntity.GetID())->position = formationManagers[i].getAnchorPosition();
				formationManagers[i].updateSlots();
			}
		}
		physicsUpdate();					// physics system
	}				

	glfwPollEvents();

	Graphics::Input::update_time(1.0);

	GLApp::update();						// graphics system

	if (Graphics::Input::mousestateLeft)
	{
		audioEngine.PlaySound("../asset/sounds/vine-boom.wav", spooky::Vector2{ 0,0 }, audioEngine.VolumeTodb(1.0f));
	}

	Font::RenderFont(Font::shader, "Text Renderer Testing", 100.f, 450.f, 1.f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void engineDraw()
{
	GLApp::draw();
}

void engineFree()
{
	GLApp::cleanup();

	Graphics::Input::cleanup();
}

void swapBuffer()
{
	// Swap buffers: front <-> back
	glfwSwapBuffers(Graphics::Input::ptr_to_window);
}

int Stats::getHealth () const
{
	return health;
}

void Stats::setHealth(const int h)
{
	health = h;
}
