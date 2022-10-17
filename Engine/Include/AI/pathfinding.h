/*!
@file    pathfinding.h
@author  ruoyan.go@digipen.edu
@date    20/8/2022

This file includes the function declarations for pathfinding functions.

*//*__________________________________________________________________________*/

#include "../mainHeader.h"
#include "ECS.h"
#include "physicsPartition.h"

#define MAX_GRID_X 25
#define MAX_GRID_Y 25
#define WALL 255


// make these parameters instead of global (pointers)
extern int dijkstraField[MAX_GRID_Y][MAX_GRID_X];
// empty grid of vectors
extern vector2D::vec2D flowField[MAX_GRID_Y][MAX_GRID_X];

extern std::vector<Entity> enemyUnits;

/*  _________________________________________________________________________*/
/*! movementFlocking

@param id				Entity ID of the unit to calculate
@param destination		Target of the unit
@param allVelocity		The 3 vectors, separation, cohesion and alignment vectors
@param maintree			reference to main branch in quadTree
@return none

This function calculates the 3 vectors that allows the units to have a 
flocking behaviour with the surrounding units.
*/
void movementFlocking(EntityID id, vector2D::vec2D destination, std::vector<vector2D::vec2D>& allVelocity, quadTree& maintree = mainTree);

/*  _________________________________________________________________________*/
/*! generateDijkstraCost

@param endingPosition	Target position
@param walls			Vector of entities for the walls
@return none

This function calculates the 2D array of the dijkstra grid, calculating the
shortest part of all the nodes to the target position.
*/
void generateDijkstraCost(vector2D::vec2D & endingPosition, std::vector<Entity>&walls);

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
void generateFlowField(vector2D::vec2D& endingPosition);
