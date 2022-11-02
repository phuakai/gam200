#pragma once

#include <vector>
#include <string>
#include "vec2D.h"
#include "vec3D.h"
#include "mat3x3.h"
#include <rttr/type>
#include <chrono>
#include "ECS.h"
//#include "behaviourTree.h"
#include "../../GAM200/Include/behaviourTree.h"

#define MAX_GRID_X 25
#define MAX_GRID_Y 25
#define WALL 255

enum unitType {
	PLAYER = 1,
	ENEMY
};

enum buildingType {
	RECRUITMENT = 1


};

struct Render // Sprite
{
	std::string type;

	vector3D::vec3D color{ 0 , 0 , 0 };

	unsigned int vaoID;
	unsigned int vboID;
	unsigned int eboID;
	std::string shaderName;
	bool render;
};

struct BaseInfo
{
	std::string type;
	std::string name;
	vector2D::vec2D position{ 0 , 0 };
	vector2D::vec2D dimension{ 0 , 0 };
	vector2D::vec2D orientation{ 0 , 0 };
};

struct Texture
{
	int textureID;
	int spriteStep;
	int numberOfSprites;
	std::string textureName;
};

struct Physics
{
	vector2D::vec2D velocity;
	vector2D::vec2D target;
	vector2D::vec2D formationTarget;
	float force;
	float speed;
	bool collisionFlag;
	vector2D::vec2D collisionResponse;
	float radius;
	bool reached;
	int formationManagerID;
};

struct Stats 
{
	int health;
	float attackTimer;
	EntityID unitLink;
};

struct Unit 
{
	int faction;
	int type;
	//EntityID target;
	BehaviourTree* aiTree;

};

struct Building 
{
	float buildTime;
	float spawnTimer;
	std::string buildingType;
};


struct ui 
{

	int group;
	int uiType;
	int location;
};


rttr::instance addComponentByName(rttr::type& componentName, const EntityID& entityID);
rttr::instance GetComponentByName(rttr::type& componentName, const EntityID& entityID);

void engineInit();
void engineUpdate();
void engineDraw();
void engineFree();
void swapBuffer();

extern bool imguiShow;
extern double imguiMouseX;
extern double imguiMouseY;

extern bool imguiCameraCheck;

extern std::vector<EntityID> prefabs;

extern EntityID selection;
extern EntityID enemyPrefab;
extern EntityID playerPrefab;
extern EntityID buildingPrefab;

void addHealthBar(EntityID id);
void updateHealthBars();

extern std::chrono::duration <double> engineDrawTime;
extern std::chrono::duration <double> physicsTime;
extern std::chrono::duration <double> ecsSystemsTime;
extern std::chrono::duration <double> totalTime;
