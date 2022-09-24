#include "ECS.h"
#include "vec2D.h"
#include "vec3D.h"
#include <vector>
#include <string>

#define MAX_GRID_X 25
#define MAX_GRID_Y 25
#define WALL 255

struct Render
{
	std::string name;
	std::string type;
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
};

struct Texture
{
	int textureID;
	std::string textureName;
};

struct Movement
{
	vector2D::vec2D velocity;
	vector2D::vec2D target;
	float force;
	float speed;
};

//struct Sprite
//{
//};

struct Stats {
	int health;
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
