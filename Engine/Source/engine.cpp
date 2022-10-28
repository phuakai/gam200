
#include "mainHeader.h"
#include "ECS.h"
#include "eventManager.h"
#include "physicsPartition.h"
#include "serialization.h"
#include "pathfinding.h"
#include "physics.h"
#include "vec2D.h"
#include "vec3D.h"
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

	rttr::registration::class_<EntityID>("EntityID")
		.property("type", &Render::type)
		.property("color", &Render::color)
		.property("vaoID", &Render::vaoID)
		.property("vboID", &Render::vboID)
		.property("eboID", &Render::eboID)
		.property("shaderName", &Render::shaderName);

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
//std::vector<Entity> enemyUnits(100);

// for checking
//Entity enemyManagerEntity;
//extern Entity enemyManagerEntity;

//std::vector<Entity> createdUnits(100); // precreated empty entities

System<Texture> textureSystem(ecs, 1);
System<BaseInfo, Physics> system1(ecs, 2);

extern EventManager eventManager;
EventManager eventManager;

std::vector<FormationManager> formationManagers;

float timer;

quadTree mainTree;

// PUT IN INPUT SYSTEM -> DRAG SELECT
bool drag;
int selected;
int currentFormationManagerID;
vector2D::vec2D dragSelectStartPosition;
vector2D::vec2D dragSelectEndPosition;

Entity formationManager;

rttr::instance GetComponentByName(rttr::type& componentName, const EntityID& entityID)
{
	if (componentName == rttr::type::get<Render>())
		return *(ecs.GetComponent<Render>(entityID));
	else if (componentName == rttr::type::get<BaseInfo>())
		return *(ecs.GetComponent<BaseInfo>(entityID));
	else if (componentName == rttr::type::get<Texture>())
		return *(ecs.GetComponent<Texture>(entityID));
	else if (componentName == rttr::type::get<Physics>())
		return *(ecs.GetComponent<Physics>(entityID));
}

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

	//FormationManager enemyManager;
	//enemyManager.target = ecs.GetComponent<BaseInfo>(playerID)->position;

	//unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	//// create default engine as source of randomness
	//std::default_random_engine generator(seed);
	//std::uniform_real_distribution<float> colour(0.f, 1.f);

	//for (int i = 0; i < enemyUnits.size(); ++i)
	//{
	//	float randr = colour(generator);
	//	float randg = colour(generator);
	//	float randb = colour(generator);

	//	enemyUnits[i].Add<Render>("square", vector3D::vec3D(randr, randg, randb), 0, 0, 0, "gam200-shdrpgm");
	//	enemyUnits[i].Add<BaseInfo>("enemy" + std::to_string(i + 1), vector2D::vec2D(-450.f + (i % 45 * 20), 400.f - ((int)i / 30 * 10)), vector2D::vec2D(10, 10));
	//	enemyUnits[i].Add<Texture>(6, 1, 1, "Enemy");
	//	enemyUnits[i].Add<Physics>(vector2D::vec2D(0, 0), ecs.GetComponent<BaseInfo>(playerID)->position, vector2D::vec2D(0, 0), 1, 2, 0, vector2D::vec2D(0, 0), 10, false, 0);
	//	//enemyUnits[i].Add<Stats>(100);
	//	ecs.setEntityName(enemyUnits[i].GetID(), "enemy" + std::to_string(i + 1));

	//	EntityID enemyID = enemyUnits[i].GetID();

	//	enemyManager.addCharacter(enemyID);
	//	mainTree.insertSuccessfully(enemyID, ecs.GetComponent<BaseInfo>(enemyID)->position);
	//}
	//formationManagers.push_back(enemyManager);

	//enemyManagerEntity.Add<Render>("square", vector3D::vec3D(0, 0, 0), 0, 0, 0, "gam200-shdrpgm");
	//enemyManagerEntity.Add<BaseInfo>("enemyManager", vector2D::vec2D(0, 0), vector2D::vec2D(20, 20));
	//enemyManagerEntity.Add<Texture>(6, 1, 1, "Enemy");
	//enemyManagerEntity.Add<Physics>(vector2D::vec2D(0, 0), ecs.GetComponent<BaseInfo>(playerID)->position, vector2D::vec2D(0, 0), 1, 2, 0, vector2D::vec2D(0, 0), 10, false);
	//ecs.setEntityName(enemyManagerEntity.GetID(), "enemyManager");

	formationManager.Add<Render>("square", vector3D::vec3D(0, 0, 0), 0, 0, 0, "gam200-shdrpgm");
	formationManager.Add<BaseInfo>("formationManager", vector2D::vec2D(0, 0), vector2D::vec2D(0, 0));
	ecs.setEntityName(formationManager.GetID(), "formationManager");

	timer = 4;

	//fromJsonECS("data.json");

	//formationManagers[0].generateDijkstraCost(walls);
	//formationManagers[0].generateFlowField();
	
	eventManager.subscribe(Systems::Collision);
	eventManager.subscribe(Systems::Physics);

	system1.Action([](const float elapsedMilliseconds, const std::vector<EntityID>& entities, BaseInfo* p, Physics* m)
	{
		for (auto i : entities)
		{
			//if (i == enemyManagerEntity.GetID())
			//{
			//	continue;
			//}
			//if (m->reached)
			//{
			//	continue;
			//}

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
	});

	drag = false;
	selected = 0;
	currentFormationManagerID = -1;
	dragSelectStartPosition = vector2D::vec2D{ 0 , 0 };
	dragSelectEndPosition = vector2D::vec2D{ 0 , 0 };

	// Serialisation ========================================================================================================
	//std::vector<EntityID> enemyUnitsID;
	//for (int i = 0; i < enemyUnits.size(); ++i)
	//{
	//	enemyUnitsID.push_back(enemyUnits[i].GetID());
	//}
	
	//toJsonECS(enemyUnitsID, "data.json", true);

	GLApp::init();
}

void engineUpdate()
{
	double mousePosX, mousePosY;
	Graphics::Input::getCursorPos(&mousePosX, &mousePosY);

	if (Graphics::Input::mousestateLeft) // just clicked
	{
		if (!drag)
		{
			dragSelectStartPosition.x = mousePosX;
			dragSelectStartPosition.y = mousePosY;
			drag = true;
		}
		else // still dragging
		{
			dragSelectEndPosition.x = mousePosX;
			dragSelectEndPosition.y = mousePosY;

			BaseInfo* formationManagerInfo = ecs.GetComponent<BaseInfo>(formationManager.GetID());

			formationManagerInfo->dimension.x = fabs(mousePosX - dragSelectStartPosition.x);
			formationManagerInfo->dimension.y = fabs(mousePosY - dragSelectStartPosition.y);

			formationManagerInfo->position = (dragSelectEndPosition - dragSelectStartPosition) / 2 + dragSelectStartPosition;
		}
	}
	else if (drag) // released
	{
		BaseInfo* formationManagerInfo = ecs.GetComponent<BaseInfo>(formationManager.GetID());

		if (formationManagerInfo->dimension.x < 2 && formationManagerInfo->dimension.y < 2)	// click and not drag
		{
			formationManagerInfo->dimension.x = 10;
			formationManagerInfo->dimension.y = 10;

			formationManagerInfo->position = dragSelectStartPosition;

			dragSelectEndPosition.x = dragSelectStartPosition.x - 5;
			dragSelectEndPosition.y = dragSelectStartPosition.y - 5;
			dragSelectStartPosition.x += 5;
			dragSelectStartPosition.y += 5;

			selected = 2;
		}
		else
		{
			selected = 1;
		}
		drag = false;
	}

	if (Graphics::Input::mousestateRight)
	{
		if (selected)
		{
			// Checking for units in the selected area using quadTree
			vector2D::vec2D center = (dragSelectEndPosition - dragSelectStartPosition) / 2 + dragSelectStartPosition;
			vector2D::vec2D dimension;
			dimension.x = fabs(dragSelectEndPosition.x - dragSelectStartPosition.x);
			dimension.y = fabs(dragSelectEndPosition.y - dragSelectStartPosition.y);

			std::list<EntityID*> myList;
			AABB range(center.x - dimension.x / 2,
				center.y - dimension.y / 2,
				center.x + dimension.x / 2,
				center.y + dimension.y / 2);
			mainTree.query(range, myList);

			if (myList.size() != 0)
			{
				int newFormationManagerID = formationManagers.size();
				// Finding inactive formation manager
				for (int i = 0; i < formationManagers.size(); ++i)
				{
					// means inactive
					if (formationManagers[i].slotAssignment.size() == 0)
					{
						newFormationManagerID = i;
						break;
					}
				}
				currentFormationManagerID = newFormationManagerID;

				FormationManager newManager;
				newManager.target = vector2D::vec2D(mousePosX, mousePosY);

				for (std::list <EntityID*>::iterator obj2 = myList.begin(); obj2 != myList.end(); ++obj2)
				{
					if ((**obj2) == player1.GetID()  /* || (**obj2) == enemyManagerEntity.GetID() */ || (**obj2) == formationManager.GetID())
					{
						continue;
					}

					int& formationManagerID = ecs.GetComponent<Physics>((**obj2))->formationManagerID;

					if (formationManagerID != -1)
					{
						auto initialFormationManagerSlot = std::find(formationManagers[formationManagerID].slotAssignment.begin(), formationManagers[formationManagerID].slotAssignment.end(), (**obj2));

						if (initialFormationManagerSlot == formationManagers[formationManagerID].slotAssignment.end())
						{
							std::cout << "what how huh\t" << formationManagerID << std::endl;
						}

						formationManagers[formationManagerID].slotAssignment.erase(initialFormationManagerSlot);
					}

					newManager.addCharacter((**obj2));
					formationManagerID = newFormationManagerID;
					ecs.GetComponent<Physics>((**obj2))->target = newManager.target;

					if (selected == 2)	// Choosing only 1 entity for clicking and not dragging
					{
						break;
					}
				}
				std::cout << newManager.slotAssignment.size() << std::endl;

				newManager.generateDijkstraCost(walls);
				newManager.generateFlowField();
				newManager.updateReached();

				if (newFormationManagerID != formationManagers.size())
				{
					formationManagers[newFormationManagerID] = newManager;
				}
				else
				{
					formationManagers.push_back(newManager);
				}

				selected = 0;

				BaseInfo* formationManagerInfo = ecs.GetComponent<BaseInfo>(formationManager.GetID());

				formationManagerInfo->dimension.x = 0;
				formationManagerInfo->dimension.y = 0;
			}
		}
		else
		{
			formationManagers[currentFormationManagerID].target = vector2D::vec2D(mousePosX, mousePosY);
			formationManagers[currentFormationManagerID].generateDijkstraCost(walls);
			formationManagers[currentFormationManagerID].generateFlowField();
			formationManagers[currentFormationManagerID].updateReached();
		}
		Graphics::Input::mousestateRight = false;
	}

	if (timer > 0)
		timer -= (float)Graphics::Input::delta_time;

	else
	{
		ecs.RunSystems(2, 100);

		for (int i = 0; i < formationManagers.size(); ++i)
		{
			vector2D::vec2D anchorNode = (formationManagers[i].getAnchorPosition() - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

			if ((formationManagers[i].getAnchorPosition().x >= formationManagers[i].target.x - 5 && formationManagers[i].getAnchorPosition().x <= formationManagers[i].target.x + 5) &&
				(formationManagers[i].getAnchorPosition().y >= formationManagers[i].target.y - 5 && formationManagers[i].getAnchorPosition().y <= formationManagers[i].target.y + 5))
			{
				formationManagers[i].reached = true;
			}

			if (!formationManagers[i].reached)
			{
				formationManagers[i].updateAnchorPosition();
				//ecs.GetComponent<BaseInfo>(enemyManagerEntity.GetID())->position = formationManagers[i].getAnchorPosition();
				formationManagers[i].updateSlots();
			}
		}
		physicsUpdate();					// physics system
	}				

	glfwPollEvents();

	Graphics::Input::update_time(1.0);

	GLApp::update();						// graphics system
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

	//std::cout << "test" << std::endl;
}

int Stats::getHealth () const
{
	return health;
}

void Stats::setHealth(const int h)
{
	health = h;
}
