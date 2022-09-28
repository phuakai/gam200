#include "mainHeader.h"

#define MAX_GRID_X 25
#define MAX_GRID_Y 25
#define WALL 255


// make these parameters instead of global (pointers)
extern int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
// empty grid of vectors
extern vector2D::vec2D flowField[MAX_GRID_Y][MAX_GRID_X];

extern std::vector<Entity> enemyUnits;

void movementFlocking(EntityID id, vector2D::vec2D destination, std::vector<vector2D::vec2D>& allVelocity, quadTree& maintree = mainTree);
void generateDijkstraCost(vector2D::vec2D& endingPosition, std::vector<Entity>& walls);
void calculateLOS(vector2D::vec2D& startingNode, vector2D::vec2D& endingPosition);
void generateFlowField(vector2D::vec2D& endingPosition);
