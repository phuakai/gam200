#include "pathfinding.h"
#include <glhelper.h>
#include <iostream>
#include <iomanip>



// make these parameters instead of global (pointers)
int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
// empty grid of vectors
vector2D::vec2D flowField[MAX_GRID_Y][MAX_GRID_X];
// line of sight grid
int LOSgrid[MAX_GRID_Y][MAX_GRID_X];

vector2D::vec2D directionToCheck[4]{ vector2D::vec2D(-1,0), vector2D::vec2D(1,0),vector2D::vec2D(0,-1),vector2D::vec2D(0,1) }; //,vector2D::vec2D(-1,-1),vector2D::vec2D(1,-1),vector2D::vec2D(-1,1),vector2D::vec2D(1,1)};

// MOVEMENT ==============================================================================

void movementFlocking(EntityID id, vector2D::vec2D destination, std::vector<vector2D::vec2D>& allVelocity)
{
	Movement* movement = ecs.GetComponent<Movement>(id);
	Render* entity = ecs.GetComponent<Render>(id);

	vector2D::vec2D desiredVelocity;

	// make these not hardcoded please
	float agentRadius = ((entity->dimension.x + entity->dimension.y) / 2) / 10;
	float minimumSeparation = ((entity->dimension.x + entity->dimension.y) / 2) * 0.9;		// used for Separation
	float maximumCohesion = ((entity->dimension.x + entity->dimension.y) / 2) * 0.9;			// used for Cohesion and Alignment

	vector2D::vec2D totalForce = { 0 , 0 };
	// 1 count for each part of flocking -> separation, cohesion, and alignment
	int neighbourCount[3] = { 0 , 0 , 0 };

	vector2D::vec2D centerForCohesion = entity->position;
	vector2D::vec2D averageDirection{ 0,0 };

	// Direction to destination
	desiredVelocity = destination - entity->position;
	// Moving at maximum speed
	Vector2DNormalize(desiredVelocity, desiredVelocity * movement->speed);

	// for each agent
	for (int i = 0; i < enemyUnits.size(); ++i)
	{
		// skip if it is the input agent
		if (enemyUnits[i].GetID() == id)
			continue;

		Render* agentPosition = ecs.GetComponent<Render>(enemyUnits[i].GetID());
		Movement* agentMovement = ecs.GetComponent<Movement>(enemyUnits[i].GetID());

		float distance = Vector2DDistance(agentPosition->position, entity->position);

		// SEPARATION --------------------------------------------------------------------

		// the 2 agents are too close to each other
		if (distance < minimumSeparation && distance > 0)
		{
			vector2D::vec2D separationForce = entity->position - agentPosition->position;
			totalForce += separationForce / agentRadius;
			++neighbourCount[0];
		}

		// COHESION ----------------------------------------------------------------------

		// the 2 agents are too close to each other
		if (distance < maximumCohesion)
		{
			centerForCohesion += agentPosition->position;
			++neighbourCount[1];

			// ALIGNMENT ---------------------------------------------------------------------
			if (vector2D::Vector2DLength(agentMovement->velocity) > 0)
			{
				vector2D::vec2D temp;
				vector2D::Vector2DNormalize(temp, agentMovement->velocity);
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
				allVelocity[i] = totalForce * movement->force;
				break;

			case 1:
				// COHESION
				centerForCohesion /= neighbourCount[i];

				allVelocity[i] = centerForCohesion - entity->position;
				vector2D::Vector2DNormalize(allVelocity[i], allVelocity[i] * movement->speed);
				force = allVelocity[i] - movement->velocity;
				force *= movement->force / movement->speed;
				break;

			case 2:
				// ALIGNMENT
				averageDirection /= neighbourCount[i];

				allVelocity[i] = averageDirection * movement->speed;
				force = allVelocity[i] - movement->velocity;
				force *= movement->force / movement->speed;
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

	LOSgrid[(int)(startingNode.y)][(int)(startingNode.x)] = haveLos;
}

void generateFlowField(vector2D::vec2D& endingPosition)
{
	vector2D::vec2D endingNode = (endingPosition - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);
	//std::cout << endingNode.x << endingNode.y << std::endl;
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

			// checking if there is line of sight
			if (LOSgrid[i][j])
			{
				// line of sight = true
				Vector2DNormalize(flowField[i][j], endingPosition - vector2D::vec2D(j * (1000 / MAX_GRID_Y) - 500 + (1000 / MAX_GRID_Y / 2), i * (1000 / MAX_GRID_X) - 500 + (1000 / MAX_GRID_X / 2)));
				
				if (i == (int)endingNode.y && j == (int)endingNode.x)
					std::cout << flowField[i][j].x << "\t" << flowField[i][j].y << std::endl;
				
				continue;
			}

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
				Vector2DNormalize(flowField[i][j], targetPosition - vector2D::vec2D(j, i));
			}
		}
	}
}
