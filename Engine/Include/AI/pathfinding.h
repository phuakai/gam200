/*!
@file    pathfinding.h
@author  ruoyan.go@digipen.edu
@date    20/8/2022

This file includes the function declarations for pathfinding functions.

*//*__________________________________________________________________________*/

#include "ECS.h"
#include "physicsPartition.h"

#define MAX_GRID_X 25
#define MAX_GRID_Y 25
#define WALL 255

struct Node
{
	int cost;
	int lineOfSight;
	vector2D::vec2D direction;
};

// FORMATION MANAGER CLASS =====================================================

class FormationManager
{
public:
	bool addCharacter(EntityID& id);
	bool removeCharacter(EntityID& id);
	void updateSlots();
	void updateAnchorPosition();
	//void updateAnchorOrientation();
	void updateOffsetPosition();
	//void updateOffsetOrientation();
	vector2D::vec2D formationPosition(int index);
	vector2D::vec2D getAnchorPosition();
	void updateReached();

	void updateSlot(const EntityID& id);

	/*  _________________________________________________________________________*/
	/*! movementFlocking

	@param id				Entity ID of the unit to calculate
	@param destination		Target of the unit
	@param allVelocity		The 3 vectors, separation, cohesion and alignment vectors
	@param maintree			Reference to main branch in quadTree
	@return none

	This function calculates the 3 vectors that allows the units to have a
	flocking behaviour with the surrounding units.
	*/
	void movementFlocking(EntityID id, std::vector<vector2D::vec2D>& allVelocity, quadTree& maintree = mainTree);

	/*  _________________________________________________________________________*/
	/*! generateDijkstraCost

	@param endingPosition	Target position
	@param walls			Vector of entities for the walls
	@return none

	This function calculates the 2D array of the dijkstra grid, calculating the
	shortest part of all the nodes to the target position.
	*/
	void generateDijkstraCost(std::vector<EntityID>* walls = nullptr);

	/*  _________________________________________________________________________*/
	/*! calculateLOS

	@param startingNode		Node position
	@param endingPosition	Target position
	@return none

	This function calculates whether the stating position has line of sight to
	the target position.
	*/
	void calculateLOS(vector2D::vec2D& startingNode, vector2D::vec2D& endingPosition);

	/*  _________________________________________________________________________*/
	/*! generateFlowField

	@param endingPosition	Target position
	@return none

	This function calculates the flow field array based on the dijkstra array
	and the line of sight array.
	*/
	void generateFlowField();

	int getNumberOfEntities();

	bool reached = false;
	bool flocking = true;
	vector2D::vec2D target;
	Node flowField[MAX_GRID_Y][MAX_GRID_X];
	std::vector<EntityID> slotAssignment;

private:
	vector2D::vec2D anchorPosition;				// Actual center of mass
	//vector2D::vec2D anchorOrientation;			// Actual average orientation
	vector2D::vec2D offsetPosition;				// Formation center of mass
	//vector2D::vec2D offsetOrientation;			// Formation orientation
};

extern std::vector<FormationManager> formationManagers;

// =============================================================================
void formationManagerUpdate();
bool pathfindingCalculation(EntityID& id);
