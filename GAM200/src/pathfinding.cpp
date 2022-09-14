#include "pathfinding.h"
#include <glhelper.h>
#include <iostream>
#include <iomanip>


std::vector<Unit> agentList;
// make these parameters instead of global (pointers)
int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
// empty grid of vectors
vector2D::vec2D flowField[MAX_GRID_Y][MAX_GRID_X];
bool LOSgrid[MAX_GRID_Y][MAX_GRID_X];

extern std::vector<Unit> agentList;
// make these parameters instead of global (pointers)
extern int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
// empty grid of vectors
extern vector2D::vec2D flowField[MAX_GRID_Y][MAX_GRID_X];

// MOVEMENT ==============================================================================

void Unit::Move()
{
	nodePosition = (position - vector2D::vec2D(-19800, -20250)) / (1000 / MAX_GRID_X);

	if (LOSgrid[(int)nodePosition.y][(int)nodePosition.x])
	{
		vector2D::Vector2DNormalize(direction, target->position - position);
	}
	else
	{
		vector2D::Vector2DNormalize(direction, flowField[(int)nodePosition.y][(int)nodePosition.x]);

	}

	//std::cout << (int)nodePosition.x << "\t" <<  (int)nodePosition.y << std::endl;
	position += direction; // *GLHelper::delta_time;
	//std::cout << position.x << "\t" << position.y << std::endl;
}

void Unit::Print()
{
	nodePosition = (target->position - vector2D::vec2D(-19800, -20250)) / (1000 / MAX_GRID_X);
	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			if ((int)nodePosition.x == j && (int)nodePosition.y == i)
			{
				std::cout << " HI ";
				continue;
			}

			std::cout << std::setw(3) << LOSgrid[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

vector2D::vec2D movementFlocking(Unit* unit, vector2D::vec2D destination)
{
	vector2D::vec2D desiredVelocity;

	// make these not hardcoded please
	float agentRadius = 2.0f;
	float minimumSeparation = 10.0f;		// used for Separation
	float maximumCohesion = 10.0f;			// used for Cohesion and Alignment

	vector2D::vec2D totalForce = { 0 , 0 };
	// 1 count for each part of flocking -> separation, cohesion, and alignment
	int neighbourCount[3] = { 0 , 0 , 0 };

	// Direction to destination
	desiredVelocity = destination - unit->position;
	// Moving at maximum speed
	Vector2DNormalize(desiredVelocity, desiredVelocity * unit->maxSpeed);

	// for each agent
	for (int i = 0; i < agentList.size(); ++i)
	{
		// skip if it is the input agent
		if (agentList[i].unitID == unit->unitID)
			continue;

		float distance = Vector2DDistance(agentList[i].position, unit->position);

		// SEPARATION --------------------------------------------------------------------

		// the 2 agents are too close to each other
		if (distance < minimumSeparation && distance > 0)
		{
			vector2D::vec2D separationForce = unit->position - agentList[i].position;
			totalForce += separationForce / agentRadius;
			++neighbourCount[0];
		}

		// COHESION ----------------------------------------------------------------------

		// the 2 agents are too close to each other
		if (distance < maximumCohesion)
		{
			++neighbourCount[1];
		}

		// ALIGNMENT ---------------------------------------------------------------------


	}
}

// PATHFINDING ===========================================================================

void generateDijkstraCost(Node& endingNode, std::vector<Node> walls)
{
	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			dijkstraField[i][j] = -1;
			LOSgrid[i][j] = false;
		}
	}

	for (int i = 0; i < walls.size(); ++i)
	{
		dijkstraField[(int)walls[i].position.y][(int)walls[i].position.x] = WALL;
	}

	// setting ending node to 0
	dijkstraField[(int)endingNode.position.y][(int)endingNode.position.x] = 0;
	LOSgrid[(int)endingNode.position.y][(int)endingNode.position.x] = true;

	// array to store nodes that needs to be calculated
	std::vector<vector2D::vec2D> toCalculate = { endingNode.position };

	for (int i = 0; i < toCalculate.size(); ++i)
	{
		if (toCalculate[i].x != endingNode.position.x || toCalculate[i].y != endingNode.position.y)
		{
			Node startingNode;
			startingNode.position = toCalculate[i];
			calculateLOS(startingNode, endingNode);
		}

		// for each neighbour
		for (int j = -1; j <= 1; ++j)
		{
			for (int k = -1; k <= 1; ++k)
			{
				// position of the neighbour
				vector2D::vec2D neighbour = toCalculate[i] + vector2D::vec2D(k, j);

				if ((neighbour.x >= 0 && neighbour.x < MAX_GRID_X &&					// checking if neighbour is in the grid
					neighbour.y >= 0 && neighbour.y < MAX_GRID_Y) &&
					dijkstraField[(int)neighbour.y][(int)neighbour.x] == -1)		// finding nodes that are not calculated yet
				{
					// setting neighbour node to current toCalculate node + 1
					dijkstraField[(int)neighbour.y][(int)neighbour.x] = dijkstraField[(int)toCalculate[i].y][(int)toCalculate[i].x] + 1;
					toCalculate.push_back(neighbour);
				}
			}
		}
	}
}

void calculateLOS(Node& startingNode, Node& endingNode) 
{
	vector2D::Vec2 difference = endingNode.position - startingNode.position;

	int differenceAbsX = (int)difference.x < 0 ? -difference.x : difference.x;
	int differenceAbsY = (int)difference.y < 0 ? -difference.y : difference.y;

	int differenceSignX = differenceAbsX ? difference.x / differenceAbsX : 0;
	int differenceSignY = differenceAbsY ? difference.y / differenceAbsY : 0;

	bool haveLos = false;

	if (differenceAbsX >= differenceAbsY) 
	{
		if (LOSgrid[(int)(startingNode.position.y)][(int)(startingNode.position.x + differenceSignX)]) {
			haveLos = true;
		}
	}
	//Check in the y direction
	else if (differenceAbsX < differenceAbsY) 
	{
		if (LOSgrid[(int)(startingNode.position.y + differenceSignY)][(int)(startingNode.position.x)]) {
			haveLos = true;
		}
	}

	if (differenceAbsX > 0 && differenceAbsY > 0) {
		if (!LOSgrid[(int)(startingNode.position.y + differenceSignY)][(int)(startingNode.position.x + differenceSignX)]) 
		{
			haveLos = false;
		}
		else if (differenceSignX == differenceSignY) 
		{
			if (dijkstraField[(int)startingNode.position.y][(int)(startingNode.position.x + differenceSignX)] == WALL ||
				dijkstraField[(int)(startingNode.position.y + differenceSignY)][(int)startingNode.position.x] == WALL)
			{
				haveLos = false;
			}
		}

	}

	LOSgrid[(int)(startingNode.position.y)][(int)(startingNode.position.x)] = haveLos;
}

void generateFlowField(Node& endingNode)
{
	//generateDijkstraCost(endingNode);

	// resetting flow field to zero vectors
	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			flowField[i][j] = { 0 , 0 };
		}
	}

	// for each node
	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			// continue if node is a wall
			if (dijkstraField[i][j] == MAX_GRID_X * MAX_GRID_Y)
				continue;

			vector2D::vec2D targetPosition{ MAX_GRID_X , MAX_GRID_Y };
			int minimumDistance = MAX_GRID_X * MAX_GRID_Y;

			// for each neighbour
			for (int k = -1; k <= 1; ++k)
			{
				for (int l = -1; l <= 1; ++l)
				{
					// position of the neighbour
					vector2D::vec2D neighbour = vector2D::vec2D(j, i) + vector2D::vec2D(l, k);

					// checking if neighbour is in the grid
					if (neighbour.x >= 0 && neighbour.x < MAX_GRID_X &&
						neighbour.y >= 0 && neighbour.y < MAX_GRID_Y)
					{
						// distance of neighbour node
						int distance = dijkstraField[(int)neighbour.y][(int)neighbour.x];

						// finding the lowest distance among the neighbours
						if (minimumDistance > distance)
						{
							minimumDistance = distance;
							targetPosition = neighbour;
						}
					}
				}
			}

			// found the neighbour with the shortest distance
			if (targetPosition.x != MAX_GRID_X || targetPosition.y != MAX_GRID_Y)
			{
				// direction vector of the node
				Vector2DNormalize(flowField[i][j], targetPosition - vector2D::vec2D(j , i));
			}
		}
	}
}
