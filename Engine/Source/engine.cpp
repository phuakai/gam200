
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
#include "Font.h"
#include "AudioEngine.h"
#include "camera.h"
#include <app.h>
#include "uiManager.h"
#include "fowMap.h"

#include "app.h"
#include "collision.h"
#include <ctime>
#include <ratio>
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
		.property("vaoID", &Render::vaoID)
		.property("vboID", &Render::vboID)
		.property("eboID", &Render::eboID)
		.property("shaderName", &Render::shaderName)
		.property("render", &Render::render);

	rttr::registration::class_<BaseInfo>("BaseInfo")
		.property("type", &BaseInfo::type)
		.property("name", &BaseInfo::name)
		.property("position", &BaseInfo::position)
		.property("dimension", &BaseInfo::dimension)
		.property("textureID", &BaseInfo::textureID)
		.property("spriteStep", &BaseInfo::spriteStep)
		.property("numberOfSprites", &BaseInfo::numberOfSprites);

	rttr::registration::class_<Texture>("Texture")
		.property("textureID", &Texture::textureID)
		.property("textureName", &Texture::textureName)
		.property("spriteStep", &Texture::spriteStep)
		.property("numberOfSprites", &Texture::numberOfSprites);

	rttr::registration::class_<Collision>("Collision")
		.property("rigidBody", &Collision::rigidBody)
		.property("collisionFlag", &Collision::collisionFlag);

	rttr::registration::class_<Physics>("Physics")
		.property("velocity", &Physics::velocity)
		.property("target", &Physics::target)
		.property("formationManagerID", &Physics::formationManagerID)
		(
			rttr::metadata("NO_SERIALIZE", "TRUE")
		)
		.property("formationTarget", &Physics::formationTarget)
		.property("force", &Physics::force)
		.property("speed", &Physics::speed)
		.property("reached", &Physics::reached);

	rttr::registration::class_<HealthBar>("HealthBar")
		.property("health", &HealthBar::health)
		.property("position", &HealthBar::position)
		.property("dimension", &HealthBar::dimension)
		.property("textureID", &HealthBar::textureID)
		.property("spriteStep", &HealthBar::spriteStep)
		.property("numberOfSprites", &HealthBar::numberOfSprites)
		.property("color", &HealthBar::color);

	rttr::registration::class_<Script>("Script")
		.property("faction", &Script::faction)
		.property("type", &Script::type);

	rttr::registration::class_<Attack>("Attack")
		.property("attack", &Attack::attack)
		.property("attackTimer", &Attack::attackTimer);

	rttr::registration::class_<Building>("Building")
		.property("buildingType", &Building::buildingType)
		.property("buildTime", &Building::buildTime)
		.property("spawnTimer", &Building::spawnTimer);

	rttr::registration::class_<UIIcon>("UIIcon")
		.property("group", &UIIcon::group)
		(
			rttr::metadata("NO_SERIALIZE", "TRUE")
		)
		.property("uiType", &UIIcon::uiType)		// is it uibg/uibutton? 
		(
			rttr::metadata("NO_SERIALIZE", "TRUE")
		)
		.property("location", &UIIcon::location)	// is it in hud/map/action panel? store into the respective list
		(
			rttr::metadata("NO_SERIALIZE", "TRUE")
		);

	rttr::registration::class_<Button>("Button")
		.property("buttonType", &Button::buttonType)
		.property("buttonFunction", &Button::buttonFunction);

}
ECS ecs;

extern Entity player1;

Entity bg;
Entity player1;
Entity playbutton;
Entity exitbutton;
//std::vector<Entity> cloud(fow::fowMap.getDims());

System<Building> system2(ecs, 1);
System<BaseInfo, Physics> system1(ecs, 2);

extern EventManager eventManager;
EventManager eventManager;

std::vector<FormationManager> formationManagers;

float timer;
float walktimer;

extern CameraNS::Camera2D camera2d;

quadTree mainTree;
fow::fogOfWarMap fowMap(1600, 900, 20, 20, vector2D::vec2D(0.f, 0.f));

//-------------profiler test
std::chrono::steady_clock::time_point t1;
std::chrono::steady_clock::time_point t2;
std::chrono::duration <double> engineDrawTime;
std::chrono::duration <double> physicsTime;
std::chrono::duration <double> ecsSystemsTime;
std::chrono::duration <double> totalTime;
//------------------------

EntityID enemyPrefab;
EntityID playerPrefab;
EntityID buildingPrefab;

//spooky::CAudioEngine audioEngine;

// PUT IN INPUT SYSTEM -> DRAG SELECT
bool drag;
int selected;
int selectedEntityID;
int currentFormationManagerID;
vector2D::vec2D dragSelectStartPosition;
vector2D::vec2D dragSelectEndPosition;
std::vector<EntityID> formationManagerUpdateEntities;
void dragSelect();

EntityID selection;
std::vector<Entity> enemyUnits(2500);
bool pause;

rttr::instance GetComponentByName(rttr::type& componentName, const EntityID& entityID)
{
	if (componentName == rttr::type::get<Render>())
		return *(ecs.GetComponent<Render>(entityID));
	else if (componentName == rttr::type::get<BaseInfo>())
		return *(ecs.GetComponent<BaseInfo>(entityID));
	else if (componentName == rttr::type::get<Texture>())
		return *(ecs.GetComponent<Texture>(entityID));
	else if (componentName == rttr::type::get<Collision>())
		return *(ecs.GetComponent<Collision>(entityID));
	else if (componentName == rttr::type::get<Physics>())
		return *(ecs.GetComponent<Physics>(entityID));
	else if (componentName == rttr::type::get<HealthBar>())
		return *(ecs.GetComponent<HealthBar>(entityID));
	else if (componentName == rttr::type::get<Script>())
		return *(ecs.GetComponent<Script>(entityID));
	else if (componentName == rttr::type::get<Attack>())
		return *(ecs.GetComponent<Attack>(entityID));
	else if (componentName == rttr::type::get<Building>())
		return *(ecs.GetComponent<Building>(entityID));
	else if (componentName == rttr::type::get<UIIcon>())
		return *(ecs.GetComponent<UIIcon>(entityID));
	else if (componentName == rttr::type::get<Button>())
		return *(ecs.GetComponent<Button>(entityID));
}

void engineInit()
{
	App::init();

	// ======================================================================================================================================
	// ECS: Register structs as components 
	ecs.RegisterComponent<Render>("Render");
	ecs.RegisterComponent<BaseInfo>("BaseInfo");
	ecs.RegisterComponent<Texture>("Texture");
	ecs.RegisterComponent<Collision>("Collision");
	ecs.RegisterComponent<Physics>("Physics");
	ecs.RegisterComponent<HealthBar>("HealthBar");
	ecs.RegisterComponent<Script>("Script");
	ecs.RegisterComponent<Attack>("Attack");
	ecs.RegisterComponent<Building>("Building");
	ecs.RegisterComponent<UIIcon>("UIIcon");
	ecs.RegisterComponent<Button>("Button");

	// ======================================================================================================================================
	// PREFABS
	enemyPrefab = ecs.GetNewID();
	ecs.AddComponent<BaseInfo>(enemyPrefab, EntityType::PREFAB, "Enemy", vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), 4, 1, 1);
	ecs.AddComponent<Render>(enemyPrefab, "square", vector3D::vec3D(0.3f, 0.3f, 0.7f), 0, 0, 0, "instanceshader", true);
	ecs.AddComponent<Texture>(enemyPrefab, 4, 1, 1, "Enemy");
	ecs.AddComponent<Physics>(enemyPrefab, vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), 0, vector2D::vec2D(0.f, 0.f), 0, 0, false);
	ecs.AddComponent<Script>(enemyPrefab, 0, 0);
	prefabs.push_back(enemyPrefab);

	playerPrefab = ecs.GetNewID();
	ecs.AddComponent<BaseInfo>(playerPrefab, EntityType::PREFAB, "Player", vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), 12, 0, 0);
	ecs.AddComponent<Render>(playerPrefab, "square", vector3D::vec3D(0.3f, 0.3f, 0.7f), 0, 0, 0, "instanceshader", true);
	ecs.AddComponent<Texture>(playerPrefab, 12, 0, 0, "");
	ecs.AddComponent<Physics>(playerPrefab, vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), 0, vector2D::vec2D(0.f, 0.f), 0, 0, false);
	prefabs.push_back(playerPrefab);

	buildingPrefab = ecs.GetNewID();
	ecs.AddComponent<BaseInfo>(buildingPrefab, EntityType::PREFAB, "Building", vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), 0, 0, 0);
	ecs.AddComponent<Render>(buildingPrefab, "square", vector3D::vec3D(0.3f, 0.3f, 0.7f), 0, 0, 0, "instanceshader", true);
	ecs.AddComponent<Texture>(buildingPrefab, 0, 0, 0, "");
	ecs.AddComponent<Building>(buildingPrefab, BuildingType::RECRUITMENT, 0, 0);
	prefabs.push_back(buildingPrefab);
	
	// ======================================================================================================================================

	mainTree.createQuadTree(vector2D::vec2D(0, 0), static_cast<float>(camera2d.getWidth()), static_cast<float>(camera2d.getHeight()), nullptr);

	bg.Add<Render>("square", vector3D::vec3D(1.f, 1.0f, 1.0f), 0, 0, 0, "instanceshader", true);
	bg.Add<BaseInfo>(EntityType::BACKGROUND, "background", vector2D::vec2D(0.f, 0.f), vector2D::vec2D((float)Graphics::Input::screenwidth, (float)Graphics::Input::screenheight), vector2D::vec2D(0.f, 0.f), 1, 1, 1);
	// velocity, target, force, speed
	bg.Add<Texture>(1, 1, 1, "Background");

	// ECS: Adding components into Entities
	// Render: name, type, position, color, dimension, vaoID, vboID, eboID, shaderName(?)
	player1.Add<Render>("square", vector3D::vec3D(0.3f, 0.3f, 0.7f), 0, 0, 0, "instanceshader", true);
	player1.Add<BaseInfo>(EntityType::PLAYER, "player1", vector2D::vec2D(-200.f, 0.f), vector2D::vec2D(20.f, 20.f), vector2D::vec2D(0.f, 0.f), 12, 1, 1);
	// velocity, target, force, speed
	player1.Add<Texture>(12, 1, 1, "none");
	//player1.Add<Stats>(100);

	playbutton.Add<Render>("square", vector3D::vec3D(1.f, 1.0f, 1.0f), 0, 0, 0, "instanceshader", true);
	vector2D::vec2D playdimension{ 150.f, 75.f };
	playbutton.Add<BaseInfo>(EntityType::UI, "play", vector2D::vec2D(((float)Graphics::Input::screenwidth / 2.f) - (playdimension.x / 2.f), 100.f), playdimension, vector2D::vec2D(0.f, 0.f), 8, 1, 1);
	// velocity, target, force, speed
	playbutton.Add<Texture>(8, 1, 1, "Play");

	exitbutton.Add<Render>("square", vector3D::vec3D(1.f, 1.0f, 1.0f), 0, 0, 0, "instanceshader", true);
	vector2D::vec2D exitdimension{ 150.f, 75.f };
	exitbutton.Add<BaseInfo>(EntityType::UI, "exit", vector2D::vec2D(((float)Graphics::Input::screenwidth / 2.f) - (exitdimension.x / 2.f), 0.f), exitdimension, vector2D::vec2D(0.f, 0.f), 10, 1, 1);
	// velocity, target, force, speed
	exitbutton.Add<Texture>(10, 1, 1, "Exit");


	//FormationManager enemyManager;

	//unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
	//// create default engine as source of randomness
	//std::default_random_engine generator(seed);
	//std::uniform_real_distribution<float> colour(0.f, 1.f);

	//for (int i = 0; i < enemyUnits.size(); ++i)
	//{
	//	float randr = colour(generator);
	//	float randg = colour(generator);
	//	float randb = colour(generator);

	//	enemyUnits[i].Add<Render>("square", vector3D::vec3D(randr, randg, randb), 0, 0, 0, "gam200-shdrpgm", true);
	//	enemyUnits[i].Add<BaseInfo>("Enemy", "enemy" + std::to_string(i + 1), vector2D::vec2D(-450.f + (i % 45 * 20), 400.f - ((int)i / 30 * 10)), vector2D::vec2D(10, 10), vector2D::vec2D(0.f, 0.f));
	//	enemyUnits[i].Add<Texture>(4, 1, 1, "Enemy");
	//	enemyUnits[i].Add<Physics>( vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), vector2D::vec2D(0.f, 0.f), 0, 0, 0, vector2D::vec2D(0.f, 0.f), 0, false, 0);
	//	enemyUnits[i].Add<Unit>( 0, 0);

	//	EntityID enemyID = enemyUnits[i].GetID();

	//	enemyManager.addCharacter(enemyID);
	//	mainTree.insertSuccessfully(enemyID, ecs.GetComponent<BaseInfo>(enemyID)->position);
	//	//fow::fowMap.addObjToFow(fow::fowObj(enemyID, ecs.GetComponent<BaseInfo>(enemyID)->position, fow::fowMap.worldToMap(ecs.GetComponent<BaseInfo>(enemyID)->position), fow::fowMap.getCol(), fow::fowMap.getRow()));
	//}
	//formationManagers.push_back(enemyManager);

	// Create fow
	//fow::fowMap.createFow();
	//int inc = 0;
	//std::list<fow::fowTile> fowTileList = fow::fowMap.getFowTileMap();
	//for (std::list<fow::fowTile>::iterator it = fow::fowMap.getFowTileMap().begin(); it != fow::fowMap.getFowTileMap().end(); ++it, ++inc)
	//{
	//	//std::cout << "this is pos: " << it->getWorldPos().x << " " << it->getWorldPos().y << std::endl;
	//	//cloud[inc].Add<Render>("cloud" + std::to_string(inc + 1), "square", it->getWorldPos(), vector3D::vec3D(0.5f, 0.5f, 0.5f), vector2D::vec2D(it->getWdith(), it->getHeight()), 0, 0, 0, "gam200-shdrpgm", true);
	//	cloud[inc].Add<Render>("square", vector3D::vec3D(0.3f, 0.3f, 0.7f), 0, 0, 0, "gam200-shdrpgm", true);
	//	cloud[inc].Add<BaseInfo>("Cloud", "cloud" + std::to_string(inc + 1), it->getWorldPos(), vector2D::vec2D(it->getWdith(), it->getHeight()), vector2D::vec2D(0.f, 0.f));
	//	cloud[inc].Add<Texture>(2, 1, 1, "Cloud");
	//	it->setid(cloud[inc].GetID());
	//	//std::cout << "this is cloud id :" << it->getid() << " " << cloud[inc].GetID() << " " << it->getWorldPos().x << " " << it->getWorldPos().y << std::endl;
	//}

	ecs.AddComponent<Render>(selected, "square", vector3D::vec3D(0, 0, 0), 0, 0, 0, "gam200-shdrpgm", true);
	ecs.AddComponent<BaseInfo>(selected, EntityType::SELECT, "selection", vector2D::vec2D(0, 0), vector2D::vec2D(0, 0), vector2D::vec2D(0.f, 0.f), 12, 1, 1);
	ecs.AddComponent<Texture>(selected, 12, 1, 1, "FM");

	timer = 4;
	walktimer = 4;

	//fromJsonECS("data.json");

	//formationManagers[0].generateDijkstraCost();
	//formationManagers[0].generateFlowField();
	
	eventManager.subscribe(Systems::Collision);
	eventManager.subscribe(Systems::Pathfinding);
	eventManager.subscribe(Systems::Physics);

	system1.Action([](const float elapsedMilliseconds, const std::vector<EntityID>& entities, BaseInfo* p, Physics* m)
	{
		for (int i = 0; i < entities.size(); ++i)
		{
			if (p[i].type == EntityType::PLAYER && m[i].formationManagerID != -1)
			{
				std::list<EntityID*>myList;
				AABB range(p[i].position.x - p[i].dimension.x,
					p[i].position.y - p[i].dimension.y,
					p[i].position.x + p[i].dimension.x,
					p[i].position.y + p[i].dimension.y);
				mainTree.query(range, myList);

				if (myList.size() != 0)
				{
					for (std::list <EntityID*>::iterator obj = myList.begin(); obj != myList.end(); ++obj)
					{
						if (ecs.GetComponent<BaseInfo>(**obj)->type == EntityType::ENEMY)
						{
							if (ecs.GetComponent<Attack>(entities[i]))
							{
								ecs.GetComponent<Attack>(entities[i])->attackTimer -= (float)Graphics::Input::delta_time;
								
								if (ecs.GetComponent<HealthBar>(**obj) && ecs.GetComponent<Attack>(entities[i])->attackTimer <= 0)
								{
									ecs.GetComponent<HealthBar>(**obj)->health -= 1;
								}

							}
							if (physics::CollisionDetectionCircleCircle(ecs.GetComponent<BaseInfo>(**obj)->position, ecs.GetComponent<BaseInfo>(**obj)->dimension.x, ecs.GetComponent<BaseInfo>(entities[i])->position, ecs.GetComponent<BaseInfo>(entities[i])->dimension.x))
							{
								m[i].target = p[i].position;
								formationManagers[m[i].formationManagerID].target = p[i].position;
								m[i].reached = true;

								std::cout << "collision?" << std::endl;
							}
						}
					}
				}
				if (ecs.GetComponent<Attack>(entities[i]) && ecs.GetComponent<Attack>(entities[i])->attackTimer <= 0)
				{
					ecs.GetComponent<Attack>(entities[i])->attackTimer = 5;
				}
				MoveEvent entityToMove;

				entityToMove.id = entities[i];
				entityToMove.message = (1UL << Systems::Physics);

				eventManager.post(entityToMove);
			}
			
		}
		//fow::fowMap.updateFow();
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
	//system2.Action([](const float elapsedMilliseconds, const std::vector<EntityID>& entities, Building* b)
	//{
	//	for (int i = 0; i < entities.size(); ++i)
	//	{
	//		b[i].spawnTimer -= (float)Graphics::Input::delta_time;
	//		if (b[i].spawnTimer <= 0)
	//		{

	//			b[i].spawnTimer = 3;
	//		}
	//	}
	//});

	drag = false;
	selected = -1;
	selectedEntityID = -1;
	currentFormationManagerID = -1;
	dragSelectStartPosition = vector2D::vec2D{ 0 , 0 };
	dragSelectEndPosition = vector2D::vec2D{ 0 , 0 };

	imguiShow = true;

	imguiMouseX = 0.0;
	imguiMouseY = 0.0;

	pause = false;

	spooky::CAudioEngine audioEngine;
	audioEngine.Init();
	
	audioEngine.LoadSound("../asset/sounds/lofistudy.wav", false);
	audioEngine.LoadSound("../asset/sounds/vine-boom.wav", false);
	audioEngine.PlaySound("../asset/sounds/lofistudy.wav", spooky::Vector2{ 0, 0 }, audioEngine.VolumeTodb(1.0f));
	//audioEngine.PlaySound("../asset/sounds/Star Wars The Imperial March Darth Vaders Theme.wav", spooky::Vector2{ 0,0 }, audioEngine.VolumeTodb(1.0f));

	/*Font::shader.CompileShaderFromFile(GL_VERTEX_SHADER, "../asset/shaders/Font.vert");
	Font::shader.CompileShaderFromFile(GL_FRAGMENT_SHADER, "../asset/shaders/Font.frag");
	if (false == Font::shader.Link() || false == Font::shader.IsLinked())
	{
		assert("ERROR: Unable to link shaders!");
	}
	if (false == Font::shader.Validate() || false == Font::shader.Validate())
	{
		assert("ERROR: Unable to validate shaders!");
	}*/

	//insert_shdrpgm(shdrnames, shdrpgms, "font", "../asset/shaders/Font.vert", "../asset/shaders/Font.frag");

	Font::init();
}

void engineUpdate()
{
	t1 = std::chrono::steady_clock::now();

	static FormationManager newManager;

	//double mousePosX = 0.0, mousePosY = 0.0;
	if (Graphics::Input::keystateSpacebar)
	{
		pause = pause ? false : true;
		Graphics::Input::keystateSpacebar = false;
	}

	if (!pause)
	{
		if (imguiCameraCheck)
		{
			dragSelect();
		}

		for (int i = 0; i < formationManagers.size(); ++i)
		{
			vector2D::vec2D anchorNode = (formationManagers[i].getAnchorPosition() - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

			if ((formationManagers[i].getAnchorPosition().x >= formationManagers[i].target.x - 10 && formationManagers[i].getAnchorPosition().x <= formationManagers[i].target.x + 10) &&
				(formationManagers[i].getAnchorPosition().y >= formationManagers[i].target.y - 10 && formationManagers[i].getAnchorPosition().y <= formationManagers[i].target.y + 10))
			{
				formationManagers[i].reached = true;
			}

			if (!formationManagers[i].reached)
			{
				formationManagers[i].updateAnchorPosition();
				formationManagers[i].updateSlots();
			}
		}

		ecs.RunSystems(2, 100);

		t2 = std::chrono::steady_clock::now();
		ecsSystemsTime = duration_cast<std::chrono::duration<double>>(t2 - t1);

		formationManagerUpdate();

		t1 = std::chrono::steady_clock::now();
		physicsUpdate();					// physics system

		//updateHealthBars();
		
		t2 = std::chrono::steady_clock::now();
		ecsSystemsTime = duration_cast<std::chrono::duration<double>>(t2 - t1);

		// Animation
		timer -= (float)Graphics::Input::delta_time;

		if (timer <= 0)
		{
			std::vector<EntityID> entities = ecs.getEntities();

			for (int i = 0; i < entities.size(); ++i)
			{
				if (ecs.GetComponent<Texture>(entities[i]) && ecs.GetComponent<Texture>(entities[i])->textureID == 16)
				{
					ecs.GetComponent<Texture>(entities[i])->spriteStep += 1;

					if (ecs.GetComponent<Texture>(entities[i])->spriteStep > 4)
					{
						ecs.GetComponent<Texture>(entities[i])->spriteStep = 1;
					}
				}
			}
			timer = 5.0f;
		}

		walktimer -= (float)Graphics::Input::delta_time;

		if (walktimer <= 0)
		{
			std::vector<EntityID> entities = ecs.getEntities();

			for (int i = 0; i < entities.size(); ++i)
			{
				if (!ecs.GetComponent<Texture>(entities[i]))
				{
					continue;
				}

				if (ecs.GetComponent<Physics>(entities[i]) != nullptr && ecs.GetComponent<Physics>(entities[i])->reached == false)
				{
					ecs.GetComponent<Texture>(entities[i])->textureID = 25;
				}
				if (ecs.GetComponent<Physics>(entities[i]) != nullptr && ecs.GetComponent<Physics>(entities[i])->reached == true && ecs.GetComponent<Texture>(entities[i])->textureID == 25)
				{
					ecs.GetComponent<Texture>(entities[i])->textureID = 16;
				}
				if (ecs.GetComponent<Texture>(entities[i])->textureID == 25)
				{
					ecs.GetComponent<Texture>(entities[i])->spriteStep += 1;

					if (ecs.GetComponent<Texture>(entities[i])->spriteStep > 6)
					{
						ecs.GetComponent<Texture>(entities[i])->spriteStep = 1;
					}
				}
			}
			walktimer = 0.2f;
		}
	}

	glfwPollEvents();

	//fow::fowMap.updateFow();
	//UI::UIMgr.UIUpdate();

	Graphics::Input::update_time(1.0);

	App::update();						// graphics system
}

void engineDraw()
{
	t1 = std::chrono::steady_clock::now();
	App::draw();
	t2 = std::chrono::steady_clock::now();
	engineDrawTime = duration_cast<std::chrono::duration<double>>(t2 - t1);

	totalTime = (ecsSystemsTime + physicsTime + engineDrawTime);
	
	//App::shdrpgms.find("font");
	//Font::RenderFont(GLApp::shdrpgms.find("font")->second , "Text Renderer Testing", Graphics::camera2d.getWinWidth() / 2, Graphics::camera2d.getWinHeight() / 2, 1.f, glm::vec3(1.0f, 1.0f, 1.0f));
	//Font::RenderFont(GLApp::shdrpgms.find("font")->second, "The Quick Brown Fox Jumps Over The Fence", Graphics::camera2d.getWinWidth() / 2, Graphics::camera2d.getWinHeight(), 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));
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
}

void dragSelect()
{
	double mousePosX = 0.0, mousePosY = 0.0;

	vector2D::vec2D mousepos;
	Graphics::Input::getCursorScreenPos(&mousepos);
	//std::cout << "This mouse pos " << mousepos.x << ", " << mousepos.y << std::endl;
	BaseInfo* playbuttoninfo = ecs.GetComponent<BaseInfo>(playbutton.GetID());
	Texture* playbuttontex = ecs.GetComponent<Texture>(playbutton.GetID());
	//std::cout << "This playbutton dimension " << playbuttoninfo->dimension.x << ", " << playbuttoninfo->dimension.y << std::endl;
	std::vector<vector2D::vec2D> playbuttonvtx
	{
		vector2D::vec2D(playbuttoninfo->position.x - (playbuttoninfo->dimension.x / 2.f), playbuttoninfo->position.y + (playbuttoninfo->dimension.y / 2.f)),
		vector2D::vec2D(playbuttoninfo->position.x + (playbuttoninfo->dimension.x / 2.f), playbuttoninfo->position.y + (playbuttoninfo->dimension.y / 2.f)),
		vector2D::vec2D(playbuttoninfo->position.x + (playbuttoninfo->dimension.x / 2.f), playbuttoninfo->position.y - (playbuttoninfo->dimension.y / 2.f)),
		vector2D::vec2D(playbuttoninfo->position.x - (playbuttoninfo->dimension.x / 2.f), playbuttoninfo->position.y - (playbuttoninfo->dimension.y / 2.f))
	};

	BaseInfo* exitbuttoninfo = ecs.GetComponent<BaseInfo>(exitbutton.GetID());
	//std::cout << "This playbutton dimension " << playbuttoninfo->dimension.x << ", " << playbuttoninfo->dimension.y << std::endl;
	std::vector<vector2D::vec2D> exitbuttonvtx
	{
		vector2D::vec2D(exitbuttoninfo->position.x - (exitbuttoninfo->dimension.x / 2.f), exitbuttoninfo->position.y + (exitbuttoninfo->dimension.y / 2.f)),
		vector2D::vec2D(exitbuttoninfo->position.x + (exitbuttoninfo->dimension.x / 2.f), exitbuttoninfo->position.y + (exitbuttoninfo->dimension.y / 2.f)),
		vector2D::vec2D(exitbuttoninfo->position.x + (exitbuttoninfo->dimension.x / 2.f), exitbuttoninfo->position.y - (exitbuttoninfo->dimension.y / 2.f)),
		vector2D::vec2D(exitbuttoninfo->position.x - (exitbuttoninfo->dimension.x / 2.f), exitbuttoninfo->position.y - (exitbuttoninfo->dimension.y / 2.f))
	};

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
		if (physics::CollisionDetectionCirclePolygon(mousepos, 1.f, playbuttonvtx))
		{
			std::cout << "Play button" << std::endl;
			pause = pause ? false : true;
			std::cout << "Pause state " << pause << std::endl;
			if (pause)
			{
				playbuttontex->textureID = 8;
			}
			else
			{
				playbuttontex->textureID = 8;
			}
			Graphics::Input::mousestateLeft = false;
		}
		if (physics::CollisionDetectionCirclePolygon(mousepos, 1.f, exitbuttonvtx))
		{
			std::cout << "Exit button" << std::endl;
			glfwSetWindowShouldClose(Graphics::Input::ptr_to_window, GLFW_TRUE);
			Graphics::Input::mousestateLeft = false;
		}

		if (!drag)
		{
			dragSelectStartPosition.x = (float)mousePosX;
			dragSelectStartPosition.y = (float)mousePosY;
			drag = true;

			// Clear info from hud
			//UI::UIMgr.removeInfoFromDisplay();

			// Remove actions from panel
			//UI::UIMgr.removeActionGroupFromDisplay(&UI::UIMgr.getUiActionGroupList()[static_cast<int>(UI::UIManager::groupName::unit1)]);
		}
		else // still dragging
		{
			dragSelectEndPosition.x = (float)mousePosX;
			dragSelectEndPosition.y = (float)mousePosY;

			BaseInfo* formationManagerInfo = ecs.GetComponent<BaseInfo>(selection);

			formationManagerInfo->dimension.x = (float)fabs(mousePosX - dragSelectStartPosition.x);
			formationManagerInfo->dimension.y = (float)fabs(mousePosY - dragSelectStartPosition.y);

			formationManagerInfo->position = (dragSelectEndPosition - dragSelectStartPosition) / 2 + dragSelectStartPosition;
		}
	}
	else if (drag) // released
	{
		BaseInfo* formationManagerInfo = ecs.GetComponent<BaseInfo>(selection);

		if (formationManagerInfo->dimension.x < 2 && formationManagerInfo->dimension.y < 2)	// click and not drag
		{
			formationManagerInfo->dimension.x = 30;
			formationManagerInfo->dimension.y = 30;

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

		// Checking for units in the selected area using quadTree
		vector2D::vec2D center = (dragSelectEndPosition - dragSelectStartPosition) / 2 + dragSelectStartPosition;
		vector2D::vec2D dimension;
		dimension.x = fabs(dragSelectEndPosition.x - dragSelectStartPosition.x);
		dimension.y = fabs(dragSelectEndPosition.y - dragSelectStartPosition.y);

		std::list<EntityID*>myList;
		formationManagerUpdateEntities.clear();
		AABB range(center.x - dimension.x / 2,
			center.y - dimension.y / 2,
			center.x + dimension.x / 2,
			center.y + dimension.y / 2);
		mainTree.query(range, myList);

		if (myList.size() != 0)
		{
			for (std::list <EntityID*>::iterator obj2 = myList.begin(); obj2 != myList.end(); ++obj2)
			{
				std::cout << "test" << std::endl;
				if (ecs.GetComponent<BaseInfo>(**obj2)->type == EntityType::PLAYER)
				{
					formationManagerUpdateEntities.push_back(**obj2);
				}

				// Add unit to info
				//UI::UIMgr.addInfoDisplay(&UI::UIMgr.getUiInfoList()[**obj2 - 3]); // 3 objects bef ui

				if (selected == 2)	// Choosing only 1 entity for clicking and not dragging
				{
					break;
				}
			}

			// Add actions to panel
			//UI::UIMgr.addActionGroupToDisplay(&UI::UIMgr.getUiActionGroupList()[static_cast<int>(UI::UIManager::groupName::unit1)]);

			selected = 0;
		}
		else
		{
			selected = -1;
		}

		formationManagerInfo->dimension.x = 0;
		formationManagerInfo->dimension.y = 0;

		drag = false;
	}

	if (Graphics::Input::mousestateRight)
	{
		if (selected != -1)
		{
			for (auto i : formationManagerUpdateEntities)
			{
				ecs.GetComponent<Physics>(i)->target = vector2D::vec2D((float)mousePosX, (float)mousePosY);

				MoveEvent entityToPathFind;

				entityToPathFind.id = i;
				entityToPathFind.message = (1UL << Systems::Pathfinding);

				eventManager.post(entityToPathFind);
			}
		}

		Graphics::Input::mousestateRight = false;
	}
}

//void updateHealthBars()
//{
//	for (auto i : ecs.getEntities())
//	{
//		BaseInfo* baseInfo = ecs.GetComponent<BaseInfo>(i);
//		if (baseInfo->type == "HealthBar")
//		{
//			if (ecs.GetComponent<Stats>(ecs.GetComponent<Stats>(i)->unitLink)->health <= 0)
//			{
//				ecs.GetComponent<BaseInfo>(ecs.GetComponent<Stats>(i)->unitLink)->type = "DeadEnemy";
//				ecs.GetComponent<Render>(ecs.GetComponent<Stats>(i)->unitLink)->render = false;
//				ecs.GetComponent<Render>(i)->render = false;
//			}
//			else
//			{
//				BaseInfo* otherBaseInfo = ecs.GetComponent<BaseInfo>(ecs.GetComponent<Stats>(i)->unitLink);
//				baseInfo->position = otherBaseInfo->position;
//				baseInfo->position.y += otherBaseInfo->dimension.y;
//				baseInfo->dimension = vector2D::vec2D((ecs.GetComponent<Stats>(ecs.GetComponent<Stats>(i)->unitLink)->health / 100.f) * otherBaseInfo->dimension.x, otherBaseInfo->dimension.x * 0.2f);
//			}
//		}
//	}
//}
