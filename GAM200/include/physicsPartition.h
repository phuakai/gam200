
#pragma once
//#if false
#include <list>
#include "vec2D.h"
#include <memory>
#include <array>
#include <string>
#include<iterator>
#include "mainHeader.h"

#define quadTreeCap 4
enum class dir
{
	nw = 0,
	ne, 
	sw, 
	se
};

struct AABB
{
public:
	vector2D::vec2D	min;										// Min point of bounding box
	vector2D::vec2D	max;										// Max point of bounding box

	/******************************************************************************/
	/*!
		Default constructor for bounding box
		Min is the bottom left and max is the top right of the box
	*/
	/******************************************************************************/
	AABB();

	/******************************************************************************/
	/*!
		Constructor for bounding box
		Min is the bottom left and max is the top right of the box
	*/
	/******************************************************************************/
	AABB(float minX, float minY, float maxX, float maxY);

	/******************************************************************************/
	/*!
		This function returns true if the input param pos is found within the
		boundary of the bouding box, else it returns false
	*/
	/******************************************************************************/
	bool withinBoundary(vector2D::vec2D const& pos);
};

struct quadObj
{
private:
public:
	int key;																// Object's id
	vector2D::vec2D position;												// Object's center pos

	/******************************************************************************/
	/*!
		Default constructor for quadtree object
		Position is the center position of the object (circle/box)
		Key is the objectID
	*/
	/******************************************************************************/
	quadObj();

	/******************************************************************************/
	/*!
		Cconstructor for quadtree object
		Position is the center position of the object (circle/box)
		Key is the objectID
	*/
	/******************************************************************************/
	quadObj(int id, vector2D::vec2D pos);

	/******************************************************************************/
	/*!
		Default deconstructor for quadtree object
	*/
	/******************************************************************************/
	~quadObj();
};

struct node
{
private:
public:
	vector2D::vec2D position;												// Node's center pos
	float halfWidth;														// Node's half width
	float halfHeight;														// Node's half height

	/******************************************************************************/
	/*!
		Default constructor for the tree's node
		Position is the center of the node
		HalfWidth and halfHeight refers to the x and y distance between the center
		and the corners of the box
	*/
	/******************************************************************************/
	node();

	/******************************************************************************/
	/*!
		Constructor for the tree's node
		Position is the center of the node
		HalfWidth and halfHeight refers to the x and y distance between the center
		and the corners of the box
	*/
	/******************************************************************************/
	node(vector2D::vec2D pos, float nodeWidth, float nodeHeight);

	/******************************************************************************/
	/*!
		Default deconstructor for the tree's snode
	*/
	/******************************************************************************/
	~node();
	/******************************************************************************/
	/*!
		This function sets the node's position, halfwidth and halfheight
		Position is the center of the node
		HalfWidth and halfHeight refers to the x and y distance between the center
		and the corners of the box
	*/
	/******************************************************************************/
	void createNode(vector2D::vec2D pos, float nodeWidth, float nodeHeight);
};

struct quadTree
{
private:
public:
	node box;																// Tree's node
	AABB boundary;															// Tree's bounding box
	size_t capacity;														// Maximum number of points
	std::list<quadObj> points;												// List of points
	std::shared_ptr<quadTree> parent;										// Pointer to parent
	std::shared_ptr<quadTree> nw;											// Pointer to north-west child
	std::shared_ptr<quadTree> ne;											// Pointer to north-east child
	std::shared_ptr<quadTree> sw;											// Pointer to south-west child
	std::shared_ptr<quadTree> se;											// Pointer to south-east child
	std::array <std::shared_ptr<quadTree>, 4> children{nw, ne, sw, se};		// Array of the children
	bool plsDivide;															// Indicates if tree has/does not have children
	
	/******************************************************************************/
	/*!
		Default constructor for quadtree
	*/
	/******************************************************************************/
	quadTree();

	/******************************************************************************/
	/*!
		Default constructor for quadtree
		CenterPos is the tree's bounding box's center position
		HalfWidth and halfHeight refers to the tree's bounding box's x and y
		distance between the center and the corners of the box
	*/
	/******************************************************************************/
	quadTree(vector2D::vec2D const& centerPos, float const& halfWidth, float const& halfHeight);

	/******************************************************************************/
	/*!
		Default deconstructor for the tree
	*/
	/******************************************************************************/
	~quadTree();

	/******************************************************************************/
	/*!
		This function sets the tree's center position, halfwidth, halfheight,
		capacity and pointer to the parent tree
		Position is the center of the node/tree
		HalfWidth and halfHeight refers to the x and y distance between the center
		and the corners of the box
		Boundary stores the bottom left and top right coordinates of the tree
		Parent is a pointer to the parent tree
		Capacity is the number of points the tree can store
		Nw/ne/sw/se are the pointers to the children
		PlsDivide indicates if the tree is at max capacity and branch out to
		the children
	*/
	/******************************************************************************/
	void createQuadTree(vector2D::vec2D const& centerPos, float const& halfWidth, float const& halfHeight, std::shared_ptr<quadTree>tree);

	/******************************************************************************/
	/*!
		This function returns true if the quadObj is within the tree's boundary
		and is successfully inserted into the tree. If the quadObj is out of the
		boundary, it returns false
	*/
	/******************************************************************************/
	bool insertSuccessfully(quadObj const& obj);

	/******************************************************************************/
	/*!
		This function divides the tree into 4 sections (nw/ne/sw/se)
	*/
	/******************************************************************************/
	void divideQuad();

	/******************************************************************************/
	/*!
		This function returns true if the input param pos is found within the tree's
		boudning box
	*/
	/******************************************************************************/
	bool withinBoundary(vector2D::vec2D const& pos);

	/******************************************************************************/
	/*!
		This function returns true if the tree's bounding box is intersecting with
		the queryRange (AABB detection)
	*/
	/******************************************************************************/
	bool AABBIntersect(AABB const& queryRange);

	/******************************************************************************/
	/*!
		This function checks for all points found witin a the queryRange and
		stores the into objList
	*/
	/******************************************************************************/
	bool query(AABB & queryRange, std::list<quadObj*>& objList);

	/******************************************************************************/
	/*!
		This function finds the location of the point and checks if the point's
		new position is still within the boundary. If it is, no action is required.
		Else, the point will be swapped with a suitable point at the top of the tree.
		If no suitable point is found, the point will be removed from the tree and
		re-inserted into the tree
	*/
	/******************************************************************************/
	bool updatePoint(quadObj const& obj, vector2D::vec2D const& newPos, quadTree& maintree);

	/******************************************************************************/
	/*!
		This function prints the quadTree's children
	*/
	/******************************************************************************/
	static void printQuadTree(quadTree tree);
	
};

static quadTree mainTree;
//static std::shared_ptr<quadTree> mainTree;
//#endif