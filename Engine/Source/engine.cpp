
#include "mainHeader.h"
#include <window.h>
#include "ECS.h"
#include "eventManager.h"
#include "physicsPartition.h"
#include "serialization.h"
#include "pathfinding.h"
#include "physics.h"
#include "vec2D.h"
#include "vec3D.h"
#include "camera.h"
#include <app.h>
#include <fowMap.h>
#include "UI/uiManager.h"

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
		.property("shaderName", &Render::shaderName)
		.property("name", &Render::render);

	rttr::registration::class_<BaseInfo>("BaseInfo")
		.property("type", &BaseInfo::type)
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


Entity bg;
Entity player1;
std::vector<Entity> walls(0);
std::vector<Entity> enemyUnits(100);
//std::vector<Entity> cloud(fow::fowMap.getDims());
std::vector<Entity> uiEntity(113);

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

extern CameraNS::Camera2D camera2d;

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
	App::init();

	// ======================================================================================================================================
	// ECS: Register structs as components 
	ecs.RegisterComponent<BaseInfo>("BaseInfo");
	ecs.RegisterComponent<Texture>("Texture");
	ecs.RegisterComponent<Physics>("Physics");
	ecs.RegisterComponent<Render>("Render");

	// ======================================================================================================================================
	// PREFABS
	const EntityID& enemyPrefab = ecs.GetNewID();
	ecs.AddComponent<BaseInfo>(enemyPrefab, "Prefab", "Enemy", vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f));
	ecs.AddComponent<Render>(enemyPrefab, "", vector3D::vec3D(0.f, 0.f, 0.f), 0, 0, 0, "", true);
	ecs.AddComponent<Texture>(enemyPrefab, 0, 0, 0, "");
	ecs.AddComponent<Physics>(enemyPrefab, vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), 0, 0, 0, vector2D::vec2D(0.f, 0.f), 0, false, 0);
	prefabs.push_back(enemyPrefab);

	const EntityID& playerPrefab = ecs.GetNewID();
	ecs.AddComponent<BaseInfo>(playerPrefab, "Prefab", "Player", vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f));
	ecs.AddComponent<Render>(playerPrefab, "", vector3D::vec3D(0.f, 0.f, 0.f), 0, 0, 0, "", true);
	ecs.AddComponent<Texture>(playerPrefab, 0, 0, 0, "");
	ecs.AddComponent<Physics>(playerPrefab, vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), 0, 0, 0, vector2D::vec2D(0.f, 0.f), 0, false, 0);
	prefabs.push_back(playerPrefab);

	const EntityID& buildingPrefab = ecs.GetNewID();
	ecs.AddComponent<BaseInfo>(buildingPrefab, "Prefab", "Building", vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f));
	ecs.AddComponent<Render>(buildingPrefab, "", vector3D::vec3D(0.f, 0.f, 0.f), 0, 0, 0, "", true);
	ecs.AddComponent<Texture>(buildingPrefab, 0, 0, 0, "");
	prefabs.push_back(buildingPrefab);
	
	// ======================================================================================================================================

	mainTree.createQuadTree(vector2D::vec2D(0, 0), Graphics::Input::screenwidth, Graphics::Input::screenheight, nullptr);

	bg.Add<Render>("square", vector3D::vec3D(1.f, 1.0f, 1.0f), 0, 0, 0, "instanceshader", true);
	bg.Add<BaseInfo>("Background", "background", vector2D::vec2D(0.f, 0.f), vector2D::vec2D(Graphics::Input::screenwidth, Graphics::Input::screenheight));
	// velocity, target, force, speed
	bg.Add<Texture>(1, 1, 1, "Background");

	// ECS: Adding components into Entities
	// Render: name, type, position, color, dimension, vaoID, vboID, eboID, shaderName(?)
	player1.Add<Render>("square", vector3D::vec3D(0.3f, 0.3f, 0.7f), 0, 0, 0, "gam200-shdrpgm", true);
	player1.Add<BaseInfo>("Player", "player1", vector2D::vec2D(-200.f, 0.f), vector2D::vec2D(20.f, 20.f));
	// velocity, target, force, speed
	player1.Add<Texture>(0, 1, 1, "none");
	//player1.Add<Stats>(100);
	ecs.setEntityName(player1.GetID(), "Mouse Click");												// may not need this after rttr

	EntityID playerID = player1.GetID();

	// Create fow
	//fow::fowMap.createFow();
	//int inc = 0;
	//std::list<fow::fowTile> fowTileList = fow::fowMap.getFowTileMap();
	//for (std::list<fow::fowTile>::iterator it = fow::fowMap.getFowTileMap().begin(); it != fow::fowMap.getFowTileMap().end(); ++it, ++inc)
	//{
	//	//std::cout << "this is pos: " << it->getWorldPos().x << " " << it->getWorldPos().y << std::endl;
	//	//cloud[inc].Add<Render>("cloud" + std::to_string(inc + 1), "square", it->getWorldPos(), vector3D::vec3D(0.5f, 0.5f, 0.5f), vector2D::vec2D(it->getWdith(), it->getHeight()), 0, 0, 0, "gam200-shdrpgm", true);
	//	cloud[inc].Add<Render>("square", vector3D::vec3D(0.3f, 0.3f, 0.7f), 0, 0, 0, "gam200-shdrpgm", true);
	//	cloud[inc].Add<BaseInfo>("cloud" + std::to_string(inc + 1), it->getWorldPos(), vector2D::vec2D(it->getWdith(), it->getHeight()));
	//	cloud[inc].Add<Texture>(2, 1, 1, "Cloud");
	//	it->setid(cloud[inc].GetID());
	//	//std::cout << "this is cloud id :" << it->getid() << " " << cloud[inc].GetID() << " " << it->getWorldPos().x << " " << it->getWorldPos().y << std::endl;
	//}

	// Create ui entity
	UI::UIMgr.createGroupList();
	// 0 = base hud
	uiEntity[0].Add<Render>("square", vector3D::vec3D(0.2f, 0.2f, 0.2f), 0, 0, 0, "gam200-shdrpgm", true);
	uiEntity[0].Add<BaseInfo>("Entity", "uiEntity" + std::to_string(0), vector2D::vec2D(camera2d.getCamPosition().x, ( -camera2d.getHeight() + camera2d.getHeight() / 4.f) / 2.f), vector2D::vec2D(camera2d.getWidth() * 0.7f, camera2d.getHeight() /4.f));
	uiEntity[0].Add<Texture>(6, 1, 1, "UIEntity");
	UI::UIMgr.addUiToActionGroup(UI::uiBg(uiEntity[0].GetID(), ecs.GetComponent<BaseInfo>(uiEntity[0].GetID())->position, ecs.GetComponent<BaseInfo>(uiEntity[0].GetID())->dimension / 2.f), UI::groupName::base);
	
	// 1 = info panel (bottom right of screen)
	uiEntity[1].Add<Render>("square", vector3D::vec3D(0.7f, 0.7f, 0.7f), 0, 0, 0, "gam200-shdrpgm", true);
	uiEntity[1].Add<BaseInfo>("Entity", "uiEntity" + std::to_string(1), vector2D::vec2D(camera2d.getWidth() / 2.f * 0.83f /*17% screenwidth*/, -camera2d.getHeight() / 2.f + camera2d.getWidth() / 2.f * 0.17f), vector2D::vec2D(camera2d.getWidth() * 0.17f/*17% screenwidth*/, camera2d.getWidth() * 0.17f));
	uiEntity[1].Add<Texture>(5, 1, 1, "UIEntity");
	UI::UIMgr.addUiToActionGroup(UI::uiBg(uiEntity[1].GetID(), ecs.GetComponent<BaseInfo>(uiEntity[1].GetID())->position, vector2D::vec2D(300.f, ecs.GetComponent<BaseInfo>(uiEntity[1].GetID())->dimension.y)), UI::groupName::base);

	// 2 = minimap (bottom left of screen)
	uiEntity[2].Add<Render>("square", vector3D::vec3D(0.7f, 0.7f, 0.7f), 0, 0, 0, "gam200-shdrpgm", true);
	uiEntity[2].Add<BaseInfo>("Entity", "uiEntity" + std::to_string(1), vector2D::vec2D(-camera2d.getWidth() / 2.f * 0.83f /*17% screenwidth*/, -camera2d.getHeight() / 2.f + camera2d.getWidth() / 2.f * 0.17f), vector2D::vec2D(camera2d.getWidth() * 0.17f/*17% screenwidth*/, camera2d.getWidth() * 0.17f));
	uiEntity[2].Add<Texture>(5, 1, 1, "UIEntity");
	UI::UIMgr.addUiToActionGroup(UI::uiBg(uiEntity[2].GetID(), ecs.GetComponent<BaseInfo>(uiEntity[2].GetID())->position, vector2D::vec2D(300.f, ecs.GetComponent<BaseInfo>(uiEntity[2].GetID())->dimension.y)), UI::groupName::base);

	UI::UIMgr.addActionGroupToDisplay(&UI::UIMgr.getUiActionGroupList()[UI::groupName::base]);

	// 3 - 9 = buttons for building1
	BaseInfo* ptr = ecs.GetComponent<BaseInfo>(uiEntity[1].GetID());
	vector2D::vec2D dimensions = ptr->dimension / 5.f;
	vector2D::vec2D position = { ptr->position.x - ptr->dimension.x / 2.f + dimensions.x * 1.5f,// / 2.f,
								 ptr->position.y + ptr->dimension.y / 2.f - dimensions.y / 2.f };
	//vector2D::vec2D position = { ptr->position.x - ptr->dimension.x / 2.f + dimensions.x. / 2.f,
	//						 ptr->position.y + ptr->dimension.y / 2.f + dimensions.y / 2.f };

	
	for (int i = 3, colTracker = 0; i < 10; ++i, ++colTracker)
	{
		static vector2D::vec2D startingPos{ position };
		colTracker % 5 == 0 ? startingPos.x = position.x : startingPos.x += dimensions.x;
		colTracker % 5 == 0 ? startingPos.y -= dimensions.y : startingPos.y = startingPos.y;
		uiEntity[i].Add<Render>("square", vector3D::vec3D(0.f, 0.f, 1.f), 0, 0, 0, "gam200-shdrpgm", false);
		uiEntity[i].Add<BaseInfo>("Entity", "uiEntity" + std::to_string(i), startingPos, dimensions);
		uiEntity[i].Add<Texture>(6, 1, 1, "UIEntity");
		UI::UIMgr.addUiToActionGroup(UI::uiButton(uiEntity[i].GetID(), ecs.GetComponent<BaseInfo>(uiEntity[i].GetID())->position, ecs.GetComponent<BaseInfo>(uiEntity[i].GetID())->dimension / 2.f, 2), UI::groupName::building1);
	}

	// 10 - 12 = buttons for unit1
	for (int i = 10, colTracker = 0; i < 13; ++i, ++colTracker)
	{
		static vector2D::vec2D startingPos{ position };
		colTracker % 5 == 0 ? startingPos.x = position.x : startingPos.x += dimensions.x;
		colTracker % 5 == 0 ? startingPos.y -= dimensions.y : startingPos.y = startingPos.y;
		uiEntity[i].Add<Render>("square", vector3D::vec3D(0.f, 0.f, 1.f), 0, 0, 0, "gam200-shdrpgm", false);
		uiEntity[i].Add<BaseInfo>("Entity", "uiEntity" + std::to_string(i), startingPos, dimensions);
		uiEntity[i].Add<Texture>(6, 1, 1, "UIEntity");
		UI::UIMgr.addUiToActionGroup(UI::uiButton(uiEntity[i].GetID(), ecs.GetComponent<BaseInfo>(uiEntity[i].GetID())->position, ecs.GetComponent<BaseInfo>(uiEntity[i].GetID())->dimension / 2.f, 2), UI::groupName::unit1);
	}

	// 13 - 112 = icons of enemy
	for (int i = 13, colTracker = 0; i < 113; ++i, ++colTracker)
	{
		uiEntity[i].Add<Render>("square", vector3D::vec3D(0.f, 1.f, 0.f), 0, 0, 0, "gam200-shdrpgm", false);
		uiEntity[i].Add<BaseInfo>("Entity", "uiEntity" + std::to_string(i), position, dimensions);
		uiEntity[i].Add<Texture>(4, 1, 1, "UIEntity");
		UI::UIMgr.addUiToInfoList(UI::uiButton(uiEntity[i].GetID(), ecs.GetComponent<BaseInfo>(uiEntity[i].GetID())->position, ecs.GetComponent<BaseInfo>(uiEntity[i].GetID())->dimension / 2.f, 2), UI::groupName::unit1);
	}


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

		enemyUnits[i].Add<Render>("square", vector3D::vec3D(randr, randg, randb), 0, 0, 0, "gam200-shdrpgm", true);
		enemyUnits[i].Add<BaseInfo>("Enemy", "enemy" + std::to_string(i + 1), vector2D::vec2D(-450.f + (i % 45 * 20), 400.f - ((int)i / 30 * 10)), vector2D::vec2D(10, 10));
		enemyUnits[i].Add<Texture>(4, 1, 1, "Enemy");
		enemyUnits[i].Add<Physics>(vector2D::vec2D(0, 0), ecs.GetComponent<BaseInfo>(playerID)->position, vector2D::vec2D(0, 0), 1, 2, 0, vector2D::vec2D(0, 0), 10, false, 0);
		//enemyUnits[i].Add<Stats>(100);
		ecs.setEntityName(enemyUnits[i].GetID(), "enemy" + std::to_string(i + 1));

		EntityID enemyID = enemyUnits[i].GetID();

		enemyManager.addCharacter(enemyID);
		mainTree.insertSuccessfully(enemyID, ecs.GetComponent<BaseInfo>(enemyID)->position);
		//fow::fowMap.addObjToFow(fow::fowObj(enemyID, ecs.GetComponent<BaseInfo>(enemyID)->position, fow::fowMap.worldToMap(ecs.GetComponent<BaseInfo>(enemyID)->position), fow::fowMap.getCol(), fow::fowMap.getRow()));
	}
	formationManagers.push_back(enemyManager);

	//enemyManagerEntity.Add<Render>("square", vector3D::vec3D(0, 0, 0), 0, 0, 0, "gam200-shdrpgm");
	//enemyManagerEntity.Add<BaseInfo>("enemyManager", vector2D::vec2D(0, 0), vector2D::vec2D(20, 20));
	//enemyManagerEntity.Add<Texture>(6, 1, 1, "Enemy");
	//enemyManagerEntity.Add<Physics>(vector2D::vec2D(0, 0), ecs.GetComponent<BaseInfo>(playerID)->position, vector2D::vec2D(0, 0), 1, 2, 0, vector2D::vec2D(0, 0), 10, false);
	//ecs.setEntityName(enemyManagerEntity.GetID(), "enemyManager");

	formationManager.Add<Render>("square", vector3D::vec3D(0, 0, 0), 0, 0, 0, "gam200-shdrpgm", true);
	formationManager.Add<BaseInfo>("", "formationManager", vector2D::vec2D(0, 0), vector2D::vec2D(0, 0));
	formationManager.Add<Texture>(0, 1, 1, "FM");
	ecs.setEntityName(formationManager.GetID(), "formationManager");

	timer = 4;

	//fromJsonECS("data.json");

	formationManagers[0].generateDijkstraCost(walls);
	formationManagers[0].generateFlowField();
	
	eventManager.subscribe(Systems::Collision);
	eventManager.subscribe(Systems::Physics);

	system1.Action([](const float elapsedMilliseconds, const std::vector<EntityID>& entities, BaseInfo* p, Physics* m)
	{
		for (int i = 0; i < entities.size(); ++i)
		{
			//if (i == enemyManagerEntity.GetID())
			//{
			//	continue;
			//}
			//if (m->reached)
			//{
			//	continue;
			//}

			if (p[i].type == "Prefab")
			{
				continue;
			}

			MoveEvent entityToMove;

			entityToMove.id = entities[i];
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

	imguiShow = true;

	imguiMouseX = 0.0;
	imguiMouseY = 0.0;

}

void engineUpdate()
{
	double mousePosX = 0.0, mousePosY = 0.0;

	if (imguiShow)
	{
		mousePosX = imguiMouseX;
		mousePosY = imguiMouseY;
	}
	else
	{
		Graphics::Input::getCursorPos(&mousePosX, &mousePosY);
	}

	if (Graphics::Input::mousestateLeft) // just clicked
	{
		if (!drag)
		{
			dragSelectStartPosition.x = mousePosX;
			dragSelectStartPosition.y = mousePosY;
			drag = true;
			
			// Clear info from hud
			UI::UIMgr.removeInfoFromDisplay();

			// Remove actions from panel
			UI::UIMgr.removeActionGroupFromDisplay(&UI::UIMgr.getUiActionGroupList()[UI::unit1]);
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
					if ((**obj2) == player1.GetID()  /* || (**obj2) == enemyManagerEntity.GetID() */ || (**obj2) == formationManager.GetID() || ecs.GetComponent<Physics>((**obj2)) == nullptr)
					{
						continue;
					}

					int& formationManagerID = ecs.GetComponent<Physics>((**obj2))->formationManagerID;

					if (formationManagerID != -1)
					{
						auto initialFormationManagerSlot = std::find(formationManagers[formationManagerID].slotAssignment.begin(), formationManagers[formationManagerID].slotAssignment.end(), (**obj2));

						if (initialFormationManagerSlot == formationManagers[formationManagerID].slotAssignment.end())
						{
							formationManagerID = -1;
						}
						else
						{
							formationManagers[formationManagerID].slotAssignment.erase(initialFormationManagerSlot);
						}
					}

					newManager.addCharacter((**obj2));
					formationManagerID = newFormationManagerID;
					ecs.GetComponent<Physics>((**obj2))->target = newManager.target;

					// Add unit to info
					UI::UIMgr.addInfoDisplay(&UI::UIMgr.getUiInfoList()[**obj2 - 3]); // 3 objects bef ui

					if (selected == 2)	// Choosing only 1 entity for clicking and not dragging
					{
						break;
					}
				}

				// Add actions to panel
				UI::UIMgr.addActionGroupToDisplay(&UI::UIMgr.getUiActionGroupList()[UI::unit1]);

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

	//fow::fowMap.updateFow();
	UI::UIMgr.UIUpdate();

	Graphics::Input::update_time(1.0);

	App::update();						// graphics system
}

void engineDraw()
{
	App::draw();
}

void engineFree()
{
	App::cleanup();

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
