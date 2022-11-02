/*!
@file    pathfinding.cpp
@author  ruoyan.go@digipen.edu
@date    20/8/2022

This file includes the function definitions for pathfinding functions.

*//*__________________________________________________________________________*/

#include "../mainHeader.h"
#include "pathfinding.h"
#include <input.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include "eventManager.h"

extern EventManager eventManager;

vector2D::vec2D directionToCheck[4]{ vector2D::vec2D(-1,0), vector2D::vec2D(1,0),vector2D::vec2D(0,-1),vector2D::vec2D(0,1) }; //,vector2D::vec2D(-1,-1),vector2D::vec2D(1,-1),vector2D::vec2D(-1,1),vector2D::vec2D(1,1)};

// MAIN FUNCTIONS ========================================================================

bool pathfindingCalculation(EntityID& id)
{
	BaseInfo* baseInfo = ecs.GetComponent<BaseInfo>(id);
	Physics* physics = ecs.GetComponent<Physics>(id);

	vector2D::vec2D changedVelocity(0, 0);
	vector2D::vec2D offsetVector(0, 0);

	//if (physics->collisionFlag)
	//{
	//	offsetVector = baseInfo->position - physics->collisionResponse;
	//	physics->collisionFlag = false;
	//}

	// if the agent is in range of the target
	if ((baseInfo->position.x >= physics->target.x - 5 && baseInfo->position.x <= physics->target.x + 5) &&
		(baseInfo->position.y >= physics->target.y - 5 && baseInfo->position.y <= physics->target.y + 5) || formationManagers[physics->formationManagerID].reached)
	{
		// last update of the formation position
		if (!physics->reached)
		{
			formationManagers[physics->formationManagerID].updateSlot(id);
			physics->reached = true;
		}

		// if the agent is not in range of the formation center, move to the its formation position
		if (!((baseInfo->position.x >= physics->formationTarget.x - 2 && baseInfo->position.x <= physics->formationTarget.x + 2) &&
			(baseInfo->position.y >= physics->formationTarget.y - 2 && baseInfo->position.y <= physics->formationTarget.y + 2)))
		{
			changedVelocity = physics->formationTarget - baseInfo->position;
		}
	}
	// target continues pathfinding and flocking
	else
	{
		physics->reached = false;

		vector2D::vec2D nodePosition = (baseInfo->position - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

		if ((int)nodePosition.x < 0 || (int)nodePosition.y < 0 || (int)nodePosition.x >= MAX_GRID_X || (int)nodePosition.x >= MAX_GRID_Y)
		{
			changedVelocity = physics->target - baseInfo->position;
		}
		else
		{
			changedVelocity = formationManagers[physics->formationManagerID].flowField[(int)nodePosition.y][(int)nodePosition.x].direction + offsetVector;
		}

		changedVelocity += (physics->formationTarget - baseInfo->position) * 0.2f;

		changedVelocity += (physics->target - baseInfo->position) * 0.7f;

		std::vector<vector2D::vec2D> allVelocity{ vector2D::vec2D(0.f,0.f), vector2D::vec2D(0.f,0.f),vector2D::vec2D(0.f,0.f) };

		formationManagers[physics->formationManagerID].movementFlocking(id, allVelocity);

		changedVelocity += (allVelocity[0] * 6 + (allVelocity[1] * 0.1f) + allVelocity[2] * 0.2f) * 2; // *flockingModifier;
	}

	vector2D::Vector2DNormalize(changedVelocity, changedVelocity);

	// capping speed
	if (vector2D::Vector2DLength(changedVelocity) > physics->speed)
	{
		changedVelocity *= physics->speed / vector2D::Vector2DLength(changedVelocity);
	}
	changedVelocity *= physics->speed;

	physics->velocity = changedVelocity;

	return true;
}

void formationManagerUpdate()
{
	std::vector<Event>& eventQueue = eventManager.findQueue(Systems::Pathfinding);
	std::vector<int> formationUpdate;

	while (eventQueue.size())
	{
		Event currentEvent = eventManager.dequeue(Systems::Pathfinding);
		EntityID entity = currentEvent.id;
		Physics* physicsComponent = ecs.GetComponent<Physics>(entity);

		// Check if target is within formation manager already
		int formationManagerID = 0;
		for (; formationManagerID < formationManagers.size(); ++formationManagerID)
		{
			if (physicsComponent->target == formationManagers[formationManagerID].target)
			{
				break;
			}
		}

		// Means formation manager found
		if (formationManagerID < formationManagers.size())
		{
			// Already in a formation manager
			if (physicsComponent->formationManagerID != -1)
			{
				// Switch formation managers
				formationManagers[physicsComponent->formationManagerID].removeCharacter(entity);
			}
		}
		// Adding new formation manager / using inactive formation managers
		else
		{
			// Finding inactive formation manager
			for (int i = 0; i < formationManagers.size(); ++i)
			{
				// means inactive
				if (formationManagers[i].slotAssignment.size() == 0)
				{
					formationManagerID = i;
					break;
				}
			}

			if (formationManagerID == formationManagers.size())
			{
				FormationManager newManager;
				formationManagers.push_back(newManager);
			}

			formationManagers[formationManagerID].target = physicsComponent->target;
		}

		formationManagers[formationManagerID].addCharacter(entity);

		if (std::find(formationUpdate.begin(), formationUpdate.end(), formationManagerID) == formationUpdate.end())
		{
			formationUpdate.push_back(formationManagerID);
		}

		physicsComponent->formationManagerID = formationManagerID;
	}

	for (int i : formationUpdate)
	{
		formationManagers[i].generateDijkstraCost();
		formationManagers[i].generateFlowField();
		formationManagers[i].updateReached();
	}
}


// MOVEMENT ==============================================================================
/*  _________________________________________________________________________*/
/*! movementFlocking

@param id				Entity ID of the unit to calculate
@param allVelocity		The 3 vectors, separation, cohesion and alignment vectors
@param maintree			reference to main branch in quadTree
@return none

This function calculates the 3 vectors that allows the units to have a
flocking behaviour with the surrounding units.
*/
void FormationManager::movementFlocking(EntityID id, std::vector<vector2D::vec2D>& allVelocity, quadTree& maintree)
{
	Physics* movement = ecs.GetComponent<Physics>(id);
	BaseInfo* entity = ecs.GetComponent<BaseInfo>(id);

	// make these not hardcoded please
	float agentRadius = ((entity->dimension.x + entity->dimension.y) / 2) / 10;
	float minimumSeparation = ((entity->dimension.x + entity->dimension.y) / 2);			// used for Separation
	float maximumCohesion = ((entity->dimension.x + entity->dimension.y) / 2);				// used for Cohesion and Alignment

	vector2D::vec2D totalForce = { 0 , 0 };
	// 1 count for each part of flocking -> separation, cohesion, and alignment
	int neighbourCount[3] = { 0 , 0 , 0 };

	vector2D::vec2D centerForCohesion = entity->position;
	vector2D::vec2D averageDirection{ 0,0 };

	std::list<EntityID*> myList;
	AABB range(entity->position.x - maximumCohesion,
		entity->position.y - maximumCohesion,
		entity->position.x + maximumCohesion,
		entity->position.y + maximumCohesion);
		maintree.query(range, myList);

	for (std::list <EntityID*>::iterator obj2 = myList.begin(); obj2 != myList.end(); ++obj2)
	{
		// skip if it is the input agent
		if ((**obj2) == id)
			continue;

		// Checking if it is in the same formation manager
		bool check = true;
		for (int i = 0; i < slotAssignment.size(); ++i)
		{
			if ((**obj2) == slotAssignment[i])
			{
				check = false;
				break;
			}
		}
		if (check)
		{
			continue;
		}

		BaseInfo* agentPosition = ecs.GetComponent<BaseInfo>((**obj2));
		Physics* agentMovement = ecs.GetComponent<Physics>((**obj2));
			
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

		// the 2 agents are in range of cohesion
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
				totalForce /= (float)neighbourCount[i];
				allVelocity[i] = totalForce * movement->force;
				break;

			case 1:
				// COHESION
				centerForCohesion /= (float)neighbourCount[i];

				allVelocity[i] = centerForCohesion - entity->position;
				vector2D::Vector2DNormalize(allVelocity[i], allVelocity[i] * movement->speed);
				force = allVelocity[i] - movement->velocity;
				force *= movement->force / movement->speed;
				break;

			case 2:
				// ALIGNMENT
				averageDirection /= (float)neighbourCount[i];

				allVelocity[i] = averageDirection * movement->speed;
				force = allVelocity[i] - movement->velocity;
				force *= movement->force / movement->speed;
				break;
			}
		}
	}
}

// PATHFINDING ===========================================================================
/*  _________________________________________________________________________*/
/*! generateDijkstraCost

@param endingPosition	Target position
@param walls			Vector of entities for the walls
@return none

This function calculates the 2D array of the dijkstra grid, calculating the
shortest part of all the nodes to the target position.
*/
void FormationManager::generateDijkstraCost(std::vector<EntityID>* walls)
{
	vector2D::vec2D endingNode = (target - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			flowField[i][j].cost = -1;
			flowField[i][j].lineOfSight = -1;
		}
	}

	for (int i = 0; walls != nullptr && i < (*walls).size(); ++i)
	{
		BaseInfo* pointer = ecs.GetComponent<BaseInfo>((*walls)[i]);
		vector2D::vec2D wallNode = (pointer->position - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

		flowField[(int)wallNode.y][(int)wallNode.x].cost = WALL;
		flowField[(int)wallNode.y][(int)wallNode.x].lineOfSight = 0;
	}

	// setting ending node to 0
	flowField[(int)endingNode.y][(int)endingNode.x].cost = 0;
	flowField[(int)endingNode.y][(int)endingNode.x].lineOfSight = true;

	// array to store nodes that needs to be calculated
	std::vector<vector2D::vec2D> toCalculate = { endingNode };

	for (int i = 0; i < toCalculate.size(); ++i)
	{
		if (toCalculate[i].x != endingNode.x || toCalculate[i].y != endingNode.y)
		{
			vector2D::vec2D startingNode = toCalculate[i];
			calculateLOS(startingNode, target);
		}

		// for each neighbour
		for (int e = 0; e < sizeof(directionToCheck) / sizeof(vector2D::vec2D); ++e) {
			int j = (int)directionToCheck[e].y;
			int k = (int)directionToCheck[e].x;

			// position of the neighbour
			vector2D::vec2D neighbour = toCalculate[i] + vector2D::vec2D((float)k, (float)j);

			if ((neighbour.x >= 0 && neighbour.x < MAX_GRID_X &&					// checking if neighbour is in the grid
				neighbour.y >= 0 && neighbour.y < MAX_GRID_Y) &&
				flowField[(int)neighbour.y][(int)neighbour.x].cost == -1)		// finding nodes that are not calculated yet
			{
				// setting neighbour node to current toCalculate node + 1
				flowField[(int)neighbour.y][(int)neighbour.x].cost = flowField[(int)toCalculate[i].y][(int)toCalculate[i].x].cost + 1;
				toCalculate.push_back(neighbour);
			}

		}
	}
}

/*  _________________________________________________________________________*/
/*! calculateLOS

@param startingNode		Node position
@param endingPosition	Target position
@return none

This function calculates whether the stating position has line of sight to
the target position.
*/
void FormationManager::calculateLOS(vector2D::vec2D& startingNode, vector2D::vec2D& endingPosition)
{
	vector2D::vec2D startingPosition = startingNode * (1000 / MAX_GRID_X) + vector2D::vec2D(-500, -500);
	vector2D::Vec2 difference = endingPosition - startingPosition;


	float differenceAbsX = difference.x < 0 ? -difference.x : difference.x;
	float differenceAbsY = difference.y < 0 ? -difference.y : difference.y;

	int differenceSignX = (int)differenceAbsX ? (int)(difference.x / differenceAbsX): 0;
	int differenceSignY = (int)differenceAbsY ? (int)(difference.y / differenceAbsY) : 0;

	bool haveLos = false;

	if (differenceAbsX >= differenceAbsY)
	{
		if (flowField[(int)(startingNode.y)][(int)(startingNode.x + differenceSignX)].lineOfSight) {
			haveLos = true;
		}
	}
	//Check in the y direction
	else if (differenceAbsX < differenceAbsY)
	{
		if (flowField[(int)(startingNode.y + differenceSignY)][(int)(startingNode.x)].lineOfSight) {
			haveLos = true;
		}
	}

	if (differenceAbsX > 0 && differenceAbsY > 0) {
		if (!flowField[(int)(startingNode.y + differenceSignY)][(int)(startingNode.x + differenceSignX)].lineOfSight)
		{
			haveLos = false;
		}
		else if (differenceSignX == differenceSignY)
		{
			if (flowField[(int)startingNode.y][(int)(startingNode.x + differenceSignX)].cost == WALL ||
				flowField[(int)(startingNode.y + differenceSignY)][(int)startingNode.x].cost == WALL)
			{
				haveLos = false;
			}
		}

	}

	flowField[(int)(startingNode.y)][(int)(startingNode.x)].lineOfSight = haveLos;
}

/*  _________________________________________________________________________*/
/*! generateFlowField

@param endingPosition	Target position
@return none

This function calculates the flow field array based on the dijkstra array
and the line of sight array.
*/
void FormationManager::generateFlowField()
{
	vector2D::vec2D endingNode = (target - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);
	//generateDijkstraCost(endingNode);

	// resetting flow field to zero vectors
	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			flowField[i][j].direction = { 0 , 0 };
		}
	}

	// for each node
	for (int i = 0; i < MAX_GRID_Y; ++i)
	{
		for (int j = 0; j < MAX_GRID_X; ++j)
		{
			// continue if node is a wall
			if (flowField[i][j].cost == MAX_GRID_X * MAX_GRID_Y)
				continue;

			// checking if there is line of sight
			if (flowField[i][j].lineOfSight)
			{
				// line of sight = true
				Vector2DNormalize(flowField[i][j].direction, target - vector2D::vec2D(j * (1000.f / MAX_GRID_Y) - 500 + (1000.f / MAX_GRID_Y / 2), i * (1000.f / MAX_GRID_X) - 500 + (1000.f / MAX_GRID_X / 2)));
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
					vector2D::vec2D neighbour = vector2D::vec2D((float)j, (float)i) + vector2D::vec2D((float)l, (float)k);

					// checking if neighbour is in the grid
					if (neighbour.x >= 0 && neighbour.x < MAX_GRID_X &&
						neighbour.y >= 0 && neighbour.y < MAX_GRID_Y)
					{
						// distance of neighbour node
						int distance = flowField[(int)neighbour.y][(int)neighbour.x].cost;

						// finding the lowest distance among the neighbours
						if (minimumDistance > distance)
						{
							// if digonal
							if (k != 0 && l != 0)
							{
								if (flowField[i + k][j].cost == WALL || flowField[i][j + l].cost == WALL)
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
				Vector2DNormalize(flowField[i][j].direction, targetPosition - vector2D::vec2D((float)j, (float)i));
			}
		}
	}
}

bool FormationManager::addCharacter(EntityID& id)
{
	int occupiedSlots = (int)slotAssignment.size();

	// change this to an array / define
	if (occupiedSlots <= 100)
	{
		// Adding new entityID into slotAssignment
		slotAssignment.push_back(id);
		return true;
	}

	return false;
}

bool FormationManager::removeCharacter(EntityID& id)
{
	auto foundIterator = std::find(begin(slotAssignment), end(slotAssignment), id);

	if (foundIterator == end(slotAssignment))
	{
		return false;
	}

	slotAssignment.erase(foundIterator);

	return true;
}

void FormationManager::updateSlots()
{
	for (int i = 0; i < slotAssignment.size(); ++i)
	{
		Physics* movement = ecs.GetComponent<Physics>(slotAssignment[i]);

		// Change to implement orientation
		// vector2D::vec2D* velocity = &ecs.GetComponent<Movement>(slotAssignment[i])->velocity;
		
		// change 1 to orientation matrix
		movement->formationTarget = anchorPosition + 1 * formationPosition(i);

		// implement orientation here

		movement->formationTarget -= offsetPosition;
	}
}

void FormationManager::updateAnchorPosition()
{
	vector2D::vec2D totalPositions { 0 , 0 };

	for (int i = 0; i < slotAssignment.size(); ++i)
	{
		totalPositions += ecs.GetComponent<BaseInfo>(slotAssignment[i])->position;
	}

	if (slotAssignment.size() != 0)
	{
		anchorPosition = totalPositions / (float)slotAssignment.size();
	}

	vector2D::vec2D anchorNode = (anchorPosition - vector2D::vec2D(-500, -500)) / (1000 / MAX_GRID_X);

	//anchorPosition += flowField[(int)anchorNode.y][(int)anchorNode.x].direction * 5;
}

//void FormationManager::updateAnchorOrientation()
//{
//	vector2D::vec2D totalVelocities;
//
//	for (int i = 0; i < slotAssignment.size(); ++i)
//	{
//		totalVelocities += ecs.GetComponent<Movement>(slotAssignment[i])->velocity;
//	}
//
//	anchorOrientation = totalVelocities / slotAssignment.size();
//}

void FormationManager::updateOffsetPosition()
{
	vector2D::vec2D totalPositions { 0 , 0 };
	for (int i = 0; i < slotAssignment.size(); ++i)
	{
		totalPositions += formationPosition(i);
	}
	offsetPosition = totalPositions / (float)slotAssignment.size();
}

//void FormationManager::updateOffsetOrientation()
//{
//	
//}

vector2D::vec2D FormationManager::formationPosition(int index)
{
	vector2D::vec2D slotRelativePosition;
	int rows = (int)round(sqrt(slotAssignment.size()));
	int columns;
	if ((round(slotAssignment.size() / rows == (int)round(slotAssignment.size() / rows))) || ((int)round(slotAssignment.size() / rows) == (double)((int)sqrt(slotAssignment.size()))))
	{
		columns = (int)round(slotAssignment.size() / rows);
	}
	else
	{
		columns = (int)round(slotAssignment.size() / rows) == (int)round(slotAssignment.size() / rows) + 1 ? (int)round(slotAssignment.size() / rows) : (int)round(slotAssignment.size() / rows) + 1;
	}
	
	if (columns == rows + 1)
	{
		slotRelativePosition.x = (float)(index % (columns - 1)) - (columns - 1) / 2.f;
	}
	else
	{
		slotRelativePosition.x = (float)(index % columns) - (columns - 1) / 2.f;
	}
	slotRelativePosition.y = (float)(index / rows) - (rows - 1) / 2.f;

	slotRelativePosition *= ecs.GetComponent<Physics>(slotAssignment[index])->radius;

	return slotRelativePosition;
}

vector2D::vec2D FormationManager::getAnchorPosition()
{
	return anchorPosition;
}

// Pushing the reached units to the front of the vector (to sort them)
void FormationManager::updateSlot(const EntityID& id)
{
	for (int i = 0; i < slotAssignment.size(); ++i)
	{
		if (ecs.GetComponent<Physics>(slotAssignment[i])->reached)
		{
			continue;
		}
		else
		{
			auto found = std::find(slotAssignment.begin(), slotAssignment.end(), id);
			EntityID temp = *found;
			*found = slotAssignment[i];
			slotAssignment[i] = temp;
			return;
		}
	}
}

void FormationManager::updateReached()
{
	for (int i = 0; i < slotAssignment.size(); ++i)
	{
		Physics* physics = ecs.GetComponent<Physics>(slotAssignment[i]);

		physics->reached = false;
		physics->target = target;
	}
	flocking = true;
	reached = false;
}

int FormationManager::getNumberOfEntities()
{
	return (int)slotAssignment.size();
}
