#include "vec2D.h"
#include <vector>
#include <string>

#define MAX_GRID_X 25
#define MAX_GRID_Y 25
#define WALL 255

struct Unit {
	std::string unitName;
	int unitID;
	// Position of the enemy
	vector2D::vec2D position;
	// Position of the enemy
	vector2D::vec2D nodePosition;
	// Velocity of enemy
	vector2D::vec2D velocity;
	// Target
	Unit* target;
	// Maximum speed of enemy
	float maxSpeed;
	// Accelration of enemy
	float maxForce;

	void Move();
	void Print();

	int pathIndex;
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

void movementFlocking(Unit& unit, vector2D::vec2D destination, std::vector<vector2D::vec2D>& allVelocity);
void generateDijkstraCost(vector2D::vec2D& endingPosition, std::vector<vector2D::vec2D> walls);
void calculateLOS(vector2D::vec2D& startingNode, vector2D::vec2D& endingPosition);
void generateFlowField();
