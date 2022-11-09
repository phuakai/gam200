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

enum EntityType {
	PLAYER = 1,
	ENEMY,
	PREFAB,
	SELECT,
	UI,
	BUTTON,
	BUILDING,
	BACKGROUND,
	FOW
};

enum BuildingType {
	RECRUITMENT = 1
};

enum ButtonType {
	ATTACK = 1
};

struct Render // TAKE THIS OUT
{
	std::string type;

	vector3D::vec3D color{ 0 , 0 , 0 };

	unsigned int vaoID;
	unsigned int vboID;
	unsigned int eboID;
	std::string shaderName;
	bool render;
};

struct BaseInfo								// One of the components that can be passed to Graphics to draw
{
	EntityType type;						// Type of entity
	std::string name;						// Name of entity

	vector2D::vec2D position{ 0 , 0 };		// Position of entity
	vector2D::vec2D dimension{ 0 , 0 };		// Dimension of entity
	vector2D::vec2D orientation{ 0 , 0 };	// Orientation of entity
	
	int textureID;							// Texture ID of entity
	int spriteStep;							// Sprite Step of entity
	int numberOfSprites;					// Total number of sprites in the texture of entity
};

struct Texture
{
	int textureID;
	int spriteStep;
	int numberOfSprites;
	std::string textureName;
};

struct Collision
{
	vector2D::vec2D rigidBody;				// Dimension of entity that checks with collision
	bool collisionFlag;						// Flag for collision check, true if collided (Not sure if this is needed)
};

struct Physics
{
	vector2D::vec2D velocity;				// Velocity of entity
	vector2D::vec2D target;					// Target of entity
	int formationManagerID;					// Formation Manager the entity is in
	vector2D::vec2D formationTarget;		// Formation target of entity
	float force;							// Force of entity
	float speed;							// Speed of entity
	bool reached;							// Boolean to check that entity reach its target
};

struct HealthBar 
{
	float health;							// Health of entity
	vector2D::vec2D position{ 0 , 0 };		// Position of healthbar
	vector2D::vec2D dimension{ 0 , 0 };		// Dimension of healthbar
	int textureID;							// Texture ID of healthbar
	int spriteStep;							// Sprite Step of healthbar
	int numberOfSprites;					// Total number of sprites in the texture of healthbar (Not sure if we shld have this)

	vector3D::vec3D color{ 0 , 0 , 0 };		// Color of healthbar (Not sure if we shld have this)
}; 

struct Script
{
	int faction;
	int type;
	BehaviourTree* aiTree;
};

struct Attack
{
	float attack;							// Attack of entity
	float attackTimer;						// Attack timer of entity
};

struct Building
{
	BuildingType buildingType;				// Type of building
	float buildTime;						// Time needed to build up/reclaim the building
	float spawnTimer;						// Timer to spawn unit entities
};

struct UIIcon
{
	int group;
	int uiType;
	int location;

	vector2D::vec2D position{ 0 , 0 };		// Position of UI of entity
	vector2D::vec2D dimension{ 0 , 0 };		// Dimension of UI of entity
	int textureID;							// Texture ID of UI of entity
	int spriteStep;							// Sprite Step of UI of entity
	int numberOfSprites;					// Total number of sprites in the texture of UI of entity
};

struct Button
{
	ButtonType buttonType;					// Button type of button entity
	void (*buttonFunction)(void);			// Use this function pointer to run onClick (will not show up in ImGui)
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

extern std::chrono::duration <double> engineDrawTime;
extern std::chrono::duration <double> physicsTime;
extern std::chrono::duration <double> ecsSystemsTime;
extern std::chrono::duration <double> totalTime;
