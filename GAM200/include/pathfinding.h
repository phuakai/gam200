#include "ECS.h"
#include "vec2D.h"
#include "vec3D.h"
#include <vector>
#include <string>

#define MAX_GRID_X 25
#define MAX_GRID_Y 25
#define WALL 255

struct Object
{
	std::string name;
	vector2D::vec2D position;

	vector3D::vec3D color;
	int textureID;

	vector2D::vec2D dimension;
	int spriteStep;
	int numberOfSprites;

	unsigned int vaoID;
	unsigned int vboID;
	unsigned int eboID;
	std::string shaderName;

	//RTTR_ENABLE();
};

struct Texture
{
	int textureID;
	std::string textureName;

	//RTTR_ENABLE();
};

struct Movement
{
	vector2D::vec2D velocity;
	vector2D::vec2D target;
	float force;
	float speed;

	//RTTR_ENABLE();
};

struct Sprite
{
	std::string type;
	vector2D::vec2D size;

	//RTTR_ENABLE();
};

struct Stats {
	int health;

	//RTTR_ENABLE();
};

struct Node {
	// Position of node
	vector2D::vec2D position;
	// Direction of node to destination
	vector2D::vec2D direction;

};

// make these parameters instead of global (pointers)
extern int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
// empty grid of vectors
extern vector2D::vec2D flowField[MAX_GRID_Y][MAX_GRID_X];

extern std::vector<Entity> enemyUnits;

void movementFlocking(EntityID id, vector2D::vec2D destination, std::vector<vector2D::vec2D>& allVelocity);
void generateDijkstraCost(vector2D::vec2D& endingPosition, std::vector<vector2D::vec2D> walls);
void calculateLOS(vector2D::vec2D& startingNode, vector2D::vec2D& endingPosition);
void generateFlowField(vector2D::vec2D& endingPosition);
