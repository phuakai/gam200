
#include "mainHeader.h"
#include "ECS.h"
#include "physicsPartition.h"
#include "serialization.h"
#include "pathfinding.h"
#include "vec2D.h"
#include "vec3D.h"
#include <app.h>
#include <collision.h>
#include <fowMap.h>

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
		.property("name", &Render::name)
		.property("type", &Render::type)
		.property("position", &Render::position)
		.property("color", &Render::color)
		.property("dimension", &Render::dimension)
		.property("vaoID", &Render::vaoID)
		.property("vboID", &Render::vboID)
		.property("eboID", &Render::eboID)
		.property("shaderName", &Render::shaderName)
		.property("name", &Render::render);

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

ECS ecs;

extern Entity player1;
extern std::vector<Entity> walls;

Entity player1;
std::vector<Entity> walls(0);
std::vector<Entity> enemyUnits(5);
std::vector<Entity> cloud(fow::fowMap.getDims());
//std::vector<Entity> createdUnits(100); // precreated empty entities

System<Texture> textureSystem(ecs, 1);
System<Movement, Render> system1(ecs, 2);

extern int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
float timer;

void engineInit()
{
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
	player1.Add<Render>("player1", "square", vector2D::vec2D(-200.f, 0.f), vector3D::vec3D(0.3f, 0.3f, 0.7f), vector2D::vec2D(20.f, 20.f), 0, 0, 0, "gam200-shdrpgm", true);
	// velocity, target, force, speed
	//player1.Add<Movement>(vector2D::vec2D(0, 0), vector2D::vec2D(0, 0), 10, 2);
	//player1.Add<Texture>(0, 1, 1, "none");
	player1.Add<Stats>(100);
	ecs.setEntityName(player1.GetID(), "Mouse Click");												// may not need this after rttr

	EntityID playerID = player1.GetID();

	// Create fow
	fow::fowMap.createFow();
	int inc = 0;
	std::list<fow::fowTile> fowTileList = fow::fowMap.getFowTileMap();
	//std::cout << "this is size of tile list: " << fowTileList.size() << std::endl;
	for (std::list<fow::fowTile>::iterator it = fow::fowMap.getFowTileMap().begin(); it != fow::fowMap.getFowTileMap().end(); ++it, ++inc)
	{
		//std::cout << "this is pos: " << it->getWorldPos().x << " " << it->getWorldPos().y << std::endl;
		cloud[inc].Add<Render>("cloud" + std::to_string(inc + 1), "square", it->getWorldPos() , vector3D::vec3D(0.5f, 0.5f, 0.5f), vector2D::vec2D(it->getWdith(), it->getHeight()), 0, 0, 0, "gam200-shdrpgm", true);
		cloud[inc].Add<Texture>(1, 1, 1, "Cloud");
		it->setid(cloud[inc].GetID());
		//std::cout << "this is cloud id :" << it->getid() << " " << cloud[inc].GetID() << " " << it->getWorldPos().x << " " << it->getWorldPos().y << std::endl;
	}

	unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	// create default engine as source of randomness
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> colour(0.f, 1.f);

	for (int i = 0; i < enemyUnits.size(); ++i)
	{
		float randr = colour(generator);
		float randg = colour(generator);
		float randb = colour(generator);

		enemyUnits[i].Add<Render>("enemy" + std::to_string(i + 1), "square", vector2D::vec2D(-450.f + (i % 45 * 20), 400.f - ((int)i / 30 * 10)), vector3D::vec3D(randr, randg, randb), vector2D::vec2D(10, 10), 0, 0, 0, "gam200-shdrpgm", true);
		enemyUnits[i].Add<Texture>(2, 1, 1, "Enemy");
		// velocity, target, force, speed
		enemyUnits[i].Add<Movement>(vector2D::vec2D(0, 0), ecs.GetComponent<Render>(playerID)->position, 1, 2, 0, vector2D::vec2D(0, 0));
		//enemyUnits[i].Add<Sprite>();
		enemyUnits[i].Add<Stats>(100);
		ecs.setEntityName(enemyUnits[i].GetID(), "enemy" + std::to_string(i + 1));

		EntityID enemyID = enemyUnits[i].GetID();
		//GLApp::GLObject::gimmeObject(ecs.GetComponent<Render>(enemyID)->type, ecs.GetComponent<Render>(enemyID)->name, ecs.GetComponent<Render>(enemyID)->dimension, ecs.GetComponent<Render>(enemyID)->position, vector3D::vec3D(randr, randg, randb));

		//quadObj entity;
		//entity.key = enemyUnits[i].GetID();
		//entity.position = ecs.GetComponent<Render>(enemyUnits[i].GetID())->position;

		mainTree.insertSuccessfully(enemyID, ecs.GetComponent<Render>(enemyID)->position);
		//EntityID _id, vector2D::vec2D _worldPos, vector2D::vec2D _gridPos, int col, int row
		fow::fowMap.addObjToFow(fow::fowObj(enemyID, ecs.GetComponent<Render>(enemyID)->position, fow::fowMap.worldToMap(ecs.GetComponent<Render>(enemyID)->position), fow::fowMap.getCol(), fow::fowMap.getRow()));
	}
	timer = 4;

	generateDijkstraCost(ecs.GetComponent<Render>(playerID)->position, walls);
	generateFlowField(ecs.GetComponent<Render>(playerID)->position);

	system1.Action([](const float elapsedMilliseconds, const std::vector<EntityID>& entities, Movement* m, Render* p)
	{
		// Check with walls
		for (int i = 0; i < walls.size(); ++i)
		{
			Render* pointer = ecs.GetComponent<Render>(walls[i].GetID());
			std::list<EntityID*> myList;
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

			for (std::list <EntityID*>::iterator enemyUnit = myList.begin(); enemyUnit != myList.end(); ++enemyUnit)
			{
				bool check = false;
				for (int j = 0; j < walls.size(); ++j)
				{
					if ((**enemyUnit) == walls[j].GetID())
					{
						check = true;
						break;
					}
				}
				if (check)
					continue;

				vector2D::vec2D enemyPos = ecs.GetComponent<Render>(**enemyUnit)->position;
				vector2D::vec2D enemyDims = ecs.GetComponent<Render>(**enemyUnit)->dimension;
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
					Movement* collided = ecs.GetComponent<Movement>(**enemyUnit);
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

			std::vector<vector2D::vec2D> allVelocity{ vector2D::vec2D(0.f,0.f), vector2D::vec2D(0.f,0.f),vector2D::vec2D(0.f,0.f) };

			movementFlocking(entities[i], m[i].target, allVelocity);

			changedVelocity += (allVelocity[0] * 6 + (allVelocity[1] * 0.1f) + allVelocity[2]); // *flockingModifier;

			vector2D::Vector2DNormalize(changedVelocity, changedVelocity);

			// capping speed
			//if (vector2D::Vector2DLength(changedVelocity) > m[i].speed)
			//{
			//	changedVelocity *= m[i].speed / vector2D::Vector2DLength(changedVelocity);
			//}
			changedVelocity *= m[i].speed;

			p[i].position += changedVelocity * (static_cast<float>(Graphics::Input::delta_time) > 1 / 60.f ? 1 / 60.f : static_cast<float>(Graphics::Input::delta_time)) * 100;

			m[i].velocity = changedVelocity;
			mainTree.updatePoint(entities[i], oldPosition, p[i].position, mainTree);
		}
		fow::fowMap.updateFow();

	});


	GLApp::init();
}

void engineUpdate()
{
	Render* player = ecs.GetComponent<Render>(player1.GetID());

	if (timer > 0)
		timer -= (float)Graphics::Input::delta_time;

	else
		ecs.RunSystems(2, 100);

	glfwPollEvents();

	Graphics::Input::update_time(1.0);

	GLApp::update();
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
