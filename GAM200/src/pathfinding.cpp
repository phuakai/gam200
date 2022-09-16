#include "pathfinding.h"
#include <glhelper.h>
#include <iostream>
#include <iomanip>


extern std::vector<Unit> enemyList;
// make these parameters instead of global (pointers)
int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
// empty grid of vectors
vector2D::vec2D flowField[MAX_GRID_Y][MAX_GRID_X];
// line of sight grid
int LOSgrid[MAX_GRID_Y][MAX_GRID_X];

vector2D::vec2D directionToCheck[4]{ vector2D::vec2D(-1,0), vector2D::vec2D(1,0),vector2D::vec2D(0,-1),vector2D::vec2D(0,1) }; //,vector2D::vec2D(-1,-1),vector2D::vec2D(1,-1),vector2D::vec2D(-1,1),vector2D::vec2D(1,1)};

// make these parameters instead of global (pointers)
extern int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
// empty grid of vectors
extern vector2D::vec2D flowField[MAX_GRID_Y][MAX_GRID_X];

// MOVEMENT ==============================================================================

void Unit::Move()
{
	nodePosition = (position - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

	if (position.x < target->position.x + target->size.x + 10 && position.x > target->position.x - target->size.x - 10 &&
		position.y < target->position.y + target->size.y + 10 && position.y > target->position.y - target->size.y - 10)
	{
		velocity.x = 0;
		velocity.y = 0;
		//return;
	}
	else
	{
		if (LOSgrid[(int)nodePosition.y][(int)nodePosition.x])
		{
			vector2D::Vector2DNormalize(velocity, target->position - position);
		}
		else
		{
			vector2D::Vector2DNormalize(velocity, flowField[(int)nodePosition.y][(int)nodePosition.x]);
		}
	}

	std::vector<vector2D::vec2D> allVelocity{ vector2D::vec2D(0,0), vector2D::vec2D(0,0),vector2D::vec2D(0,0) };

	movementFlocking(*this, target->position, allVelocity);

	//if (unitName == "enemy1")
	//	std::cout << velocity.x << "\t" << velocity.y << "\t\t";

	velocity += allVelocity[0] + (allVelocity[1] * 0.05) + allVelocity[2];

	//if (unitName == "enemy1")
	//	std::cout << velocity.x << "\t" << velocity.y << std::endl;

	// capping speed
	if (vector2D::Vector2DLength(velocity) > maxSpeed)
	{
		velocity *= maxSpeed / vector2D::Vector2DLength(velocity);
	}

	//std::cout << (int)nodePosition.x << "\t" <<  (int)nodePosition.y << std::endl;
	position += velocity * GLHelper::delta_time * 100;
	//std::cout << position.x << "\t" << position.y << std::endl;
}

void Unit::Print()
{
	nodePosition = (target->position - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);
	for (int i = MAX_GRID_Y - 1; i >= 0; --i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			std::cout << std::setw(3) << dijkstraField[i][j] << " ";
		}
		std::cout << std::endl;
	}

	std::cout << std::endl;

	for (int i = MAX_GRID_Y - 1; i >= 0; --i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			std::cout << std::setw(3) << LOSgrid[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void movementFlocking(Unit& unit, vector2D::vec2D destination, std::vector<vector2D::vec2D>& allVelocity)
{
	vector2D::vec2D desiredVelocity;

	// make these not hardcoded please
	float agentRadius = ((unit.size.x + unit.size.y) / 2) / 10;
	float minimumSeparation = ((unit.size.x + unit.size.y) / 2) * 0.9;		// used for Separation
	float maximumCohesion = ((unit.size.x + unit.size.y) / 2) * 0.9;			// used for Cohesion and Alignment

	vector2D::vec2D totalForce = { 0 , 0 };
	// 1 count for each part of flocking -> separation, cohesion, and alignment
	int neighbourCount[3] = { 0 , 0 , 0 };

	vector2D::vec2D centerForCohesion = unit.position;
	vector2D::vec2D averageDirection{ 0,0 };

	// Direction to destination
	desiredVelocity = destination - unit.position;
	// Moving at maximum speed
	Vector2DNormalize(desiredVelocity, desiredVelocity * unit.maxSpeed);

	// for each agent
	for (int i = 0; i < enemyList.size(); ++i)
	{
		// skip if it is the input agent
		if (enemyList[i].unitID == unit.unitID)
			continue;

		float distance = Vector2DDistance(enemyList[i].position, unit.position);

		// SEPARATION --------------------------------------------------------------------

		// the 2 agents are too close to each other
		if (distance < minimumSeparation && distance > 0)
		{
			vector2D::vec2D separationForce = unit.position - enemyList[i].position;
			totalForce += separationForce / agentRadius;
			++neighbourCount[0];
		}

		// COHESION ----------------------------------------------------------------------

		// the 2 agents are too close to each other
		if (distance < maximumCohesion)
		{
			centerForCohesion += enemyList[i].position;
			++neighbourCount[1];

			// ALIGNMENT ---------------------------------------------------------------------
			if (vector2D::Vector2DLength(enemyList[i].velocity) > 0)
			{
				vector2D::vec2D temp;
				vector2D::Vector2DNormalize(temp, enemyList[i].velocity);
				averageDirection += temp;

				++neighbourCount[2];
			}
		}
	}

	vector2D::vec2D force;

	for (int i = 0; i < 3; ++i)
	{
		if (!neighbourCount[i])
		{
			allVelocity[i] = vector2D::vec2D{ 0 , 0 };
		}
		else 
		{
			switch (i)
			{
			case 0:
				//SEPARATION
				totalForce /= neighbourCount[i];
				allVelocity[i] = totalForce * unit.maxForce;
				break;

			case 1:
				// COHESION
				centerForCohesion /= neighbourCount[i];

				allVelocity[i] = centerForCohesion - unit.position;
				vector2D::Vector2DNormalize(allVelocity[i], allVelocity[i] * unit.maxSpeed);
				force = allVelocity[i] - unit.velocity;
				force *= unit.maxForce / unit.maxSpeed;
				break;

			case 2:
				// ALIGNMENT
				averageDirection /= neighbourCount[i];

				allVelocity[i] = averageDirection * unit.maxSpeed;
				force = allVelocity[i] - unit.velocity;
				force *= unit.maxForce / unit.maxSpeed;
				break;
			}
		}
	}
}

// PATHFINDING ===========================================================================

void generateDijkstraCost(vector2D::vec2D& endingPosition, std::vector<vector2D::vec2D> walls)
{
	//while (directionToCheck != NULL)
	vector2D::vec2D endingNode = (endingPosition - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);
	std::cout << endingNode.x << ", " << endingNode.y << std::endl;

	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			dijkstraField[i][j] = -1;
			LOSgrid[i][j] = -1;
		}
	}

	for (int i = 0; i < walls.size(); ++i)
	{
		dijkstraField[(int)walls[i].y][(int)walls[i].x] = WALL;
		LOSgrid[(int)walls[i].y][(int)walls[i].x] = 0;
	}

	// setting ending node to 0
	dijkstraField[(int)endingNode.y][(int)endingNode.x] = 0;
	LOSgrid[(int)endingNode.y][(int)endingNode.x] = true;

	// array to store nodes that needs to be calculated
	std::vector<vector2D::vec2D> toCalculate = { endingNode };

	for (int i = 0; i < toCalculate.size(); ++i)
	{
		if (toCalculate[i].x != endingNode.x || toCalculate[i].y != endingNode.y)
		{
			vector2D::vec2D startingNode = toCalculate[i];
			calculateLOS(startingNode, endingPosition);
		}

		// for each neighbour
		for (int e = 0; e < sizeof(directionToCheck) / sizeof(vector2D::vec2D); ++e) {
			int j = directionToCheck[e].y;
			int k = directionToCheck[e].x;

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

void calculateLOS(vector2D::vec2D& startingNode, vector2D::vec2D& endingPosition)
{
	vector2D::vec2D startingPosition = startingNode * (1000 / MAX_GRID_X) + vector2D::vec2D(-500, -500);
	vector2D::Vec2 difference = endingPosition - startingPosition;


	float differenceAbsX = difference.x < 0 ? -difference.x : difference.x;
	float differenceAbsY = difference.y < 0 ? -difference.y : difference.y;

	int differenceSignX = differenceAbsX ? difference.x / differenceAbsX : 0;
	int differenceSignY = differenceAbsY ? difference.y / differenceAbsY : 0;

	bool haveLos = false;

	if (differenceAbsX >= differenceAbsY) 
	{
		if (LOSgrid[(int)(startingNode.y)][(int)(startingNode.x + differenceSignX)]) {
			haveLos = true;
		}
	}
	//Check in the y direction
	else if (differenceAbsX < differenceAbsY) 
	{
		if (LOSgrid[(int)(startingNode.y + differenceSignY)][(int)(startingNode.x)]) {
			haveLos = true;
		}
	}

	if (differenceAbsX > 0 && differenceAbsY > 0) {
		if (!LOSgrid[(int)(startingNode.y + differenceSignY)][(int)(startingNode.x + differenceSignX)])
		{
			haveLos = false;
		}
		else if (differenceSignX == differenceSignY) 
		{
			if (dijkstraField[(int)startingNode.y][(int)(startingNode.x + differenceSignX)] == WALL ||
				dijkstraField[(int)(startingNode.y + differenceSignY)][(int)startingNode.x] == WALL)
			{
				haveLos = false;
			}
		}

	}

	// checking why line of sign does not work
	//if (haveLos == false)
	if ((int)startingNode.x == 6 && (int)startingNode.y == 11)
	{
		std::cout << "Position " << startingNode.x << "\t" << startingNode.y << std::endl;
		std::cout << "End " << endingPosition.x << "\t" << endingPosition.y << std::endl;

		std::cout << "Diff " << difference.x << "\t" << difference.y << std::endl;
		std::cout << "Abs " << differenceAbsX << "\t" << differenceAbsY << std::endl;
		std::cout << "Sign " << differenceSignX << "\t" << differenceSignY << std::endl;

		std::cout << "LOS from position + diffX " << LOSgrid[(int)(startingNode.y)][(int)(startingNode.x) + differenceSignX] << std::endl;
		std::cout << "LOS from position + diffY " << LOSgrid[(int)(startingNode.y) + differenceSignY][(int)(startingNode.x)] << std::endl;
		std::cout << "LOS from position + diffX + diffY " << LOSgrid[(int)(startingNode.y) + differenceSignY][(int)(startingNode.x) + differenceSignX] << std::endl;
	}

	LOSgrid[(int)(startingNode.y)][(int)(startingNode.x)] = haveLos;
}

void generateFlowField()
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
							// if digonal
							if (k != 0 && l != 0)
							{
								if (dijkstraField[i + k][j] == WALL || dijkstraField[i][j + l] == WALL)
								{
									continue;
								}
							}

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
