
//#if false

#include "physicsPartition.h"
#include <iostream>
#include <iomanip>

/******************************************************************************/
/*!
	Default constructor for bounding box
	Min is the bottom left and max is the top right of the box
*/
/******************************************************************************/
AABB::AABB()
{
	min.x = 0.f;
	min.y = 0.f;
	max.x = 0.f;
	max.y = 0.f;
}

/******************************************************************************/
/*!
	Constructor for bounding box
	Min is the bottom left and max is the top right of the box
*/
/******************************************************************************/
AABB::AABB(float minX, float minY, float maxX, float maxY)
{
	min.x = minX;
	min.y = minY;
	max.x = maxX;
	max.y = maxY;
}

/******************************************************************************/
/*!
	This function returns true if the input param pos is found within the
	boundary of the bouding box, else it returns false
*/
/******************************************************************************/
bool AABB::withinBoundary(vector2D::vec2D const& pos)
{
	if (pos.x >= min.x && pos.y >= min.y &&
		pos.x <= max.x && pos.y <= max.y)
		return true;
	return false;
}

/******************************************************************************/
/*!
	Default constructor for quadtree object
	Position is the center position of the object (circle/box)
	Key is the objectID
*/
/******************************************************************************/
quadObj::quadObj()
{
	position.x = 0.f;
	position.y = 0.f;
	key = -1;
}

/******************************************************************************/
/*!
	Cconstructor for quadtree object
	Position is the center position of the object (circle/box)
	Key is the objectID
*/
/******************************************************************************/
quadObj::quadObj(int id, vector2D::vec2D pos)
{
	position.x = pos.x;
	position.y = pos.y;
	key = id;
}

/******************************************************************************/
/*!
	Default deconstructor for quadtree object
*/
/******************************************************************************/
quadObj::~quadObj(){}

/******************************************************************************/
/*!
	Default constructor for the tree's node
	Position is the center of the node
	HalfWidth and halfHeight refers to the x and y distance between the center 
	and the corners of the box
*/
/******************************************************************************/
node::node()
{
	position.x = 0.f;
	position.y = 0.f;
	halfWidth = 0.f;
	halfHeight = 0.f;
}

/******************************************************************************/
/*!
	Constructor for the tree's node
	Position is the center of the node
	HalfWidth and halfHeight refers to the x and y distance between the center
	and the corners of the box
*/
/******************************************************************************/
node::node(vector2D::vec2D pos, float nodeWidth, float nodeHeight)
{
	position.x = pos.x;
	position.y = pos.y;
	halfWidth = nodeWidth;
	halfHeight = nodeHeight;
}

/******************************************************************************/
/*!
	Default deconstructor for the tree's snode
*/
/******************************************************************************/
node::~node(){}

/******************************************************************************/
/*!
	This function sets the node's position, halfwidth and halfheight
	Position is the center of the node
	HalfWidth and halfHeight refers to the x and y distance between the center
	and the corners of the box
*/
/******************************************************************************/
void node::createNode(vector2D::vec2D pos, float nodeWidth, float nodeHeight)
{
	position = pos;
	halfWidth = nodeWidth;
	halfHeight = nodeHeight;
}

/******************************************************************************/
/*!
	Default constructor for quadtree
*/
/******************************************************************************/
quadTree::quadTree()
{
	boundary.min.x = 0.5f;
	boundary.max.x = 0.5f;
	boundary.min.y = 0.5f;
	boundary.max.y = 0.5f;
	capacity = quadTreeCap;
	parent = nullptr;
	nw = nullptr;
	ne = nullptr;
	sw = nullptr;
	se = nullptr;
	plsDivide = true;
}

/******************************************************************************/
/*!
	Default constructor for quadtree
	CenterPos is the tree's bounding box's center position
	HalfWidth and halfHeight refers to the tree's bounding box's x and y 
	distance between the center and the corners of the box
*/
/******************************************************************************/
quadTree::quadTree(vector2D::vec2D const& centerPos, float const& halfWidth, float const& halfHeight)
{
	box.createNode(centerPos, halfWidth, halfHeight);
	boundary.min.x = box.position.x - box.halfWidth;
	boundary.max.x = box.position.x + box.halfWidth;
	boundary.min.y = box.position.y - box.halfHeight;
	boundary.max.y = box.position.y + box.halfHeight;
	capacity = quadTreeCap;
	parent = nullptr;
	nw = nullptr;
	ne = nullptr;
	sw = nullptr;
	se = nullptr;
	plsDivide = true;
}

/******************************************************************************/
/*!
	Default deconstructor for the tree
*/
/******************************************************************************/
quadTree::~quadTree() { }

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
void quadTree::createQuadTree(vector2D::vec2D const& centerPos, float const& halfWidth, float const& halfHeight, std::shared_ptr<quadTree>tree)
{
	box.createNode(centerPos, halfWidth, halfHeight);
	boundary.min.x = box.position.x - box.halfWidth;
	boundary.max.x = box.position.x + box.halfWidth;
	boundary.min.y = box.position.y - box.halfHeight;
	boundary.max.y = box.position.y + box.halfHeight;
	parent = std::shared_ptr<quadTree>();
	parent = tree;
	capacity = quadTreeCap;
	nw = nullptr;
	ne = nullptr;
	sw = nullptr;
	se = nullptr;
	plsDivide = true;
}

/******************************************************************************/
/*!
	This function returns true if the quadObj is within the tree's boundary 
	and is successfully inserted into the tree. If the quadObj is out of the
	boundary, it returns false
*/
/******************************************************************************/
bool quadTree::insertSuccessfully(EntityID const& id, vector2D::vec2D pos)
{
	if (!withinBoundary(pos))
		return false;	// Point is not in the quadTree boundary
	if (points.size() < capacity)
	{
		// QaudTree has space
		points.push_front(id);
		return true;
	}
	else if (plsDivide)
		divideQuad();	// QuadTree has no space, divide into children if it isnt divided

	if (nw->withinBoundary(pos))
	{
		// Point is in north-west boundary, insert it into the tree
		if (nw->insertSuccessfully(id, pos))
		{
			// Point has been successfully inserted into tree
			return true;
		}
		//std::cout << "enter top left boundary: (" << nw->boundary.min.x << ", " << nw->boundary.min.y << ") (" << nw->boundary.max.x << ", " << nw->boundary.max.y << ")\n";
		//std::cout << "this is position of point: (" << obj.position.x << ", " << obj.position.y << ")\n";
	}
	if (ne->withinBoundary(pos))
	{
		// Point is in north-east boundary, insert it into the tree
		if (ne->insertSuccessfully(id, pos))
		{
			// Point has been successfully inserted into tree
			return true;
		}
		//std::cout << "enter top right boundary: (" << ne->boundary.min.x << ", " << ne->boundary.min.y << ") (" << ne->boundary.max.x << ", " << ne->boundary.max.y << ")\n";
		//std::cout << "this is position of point: (" << obj.position.x << ", " << obj.position.y << ")\n";
	}
	if (sw->withinBoundary(pos))
	{
		// Point is in south-west boundary, insert it into the tree
		if (sw->insertSuccessfully(id, pos))
		{
			// Point has been successfully inserted into tree
			return true;
		}
		//std::cout << "enter bottom left boundary: (" << sw->boundary.min.x << ", " << sw->boundary.min.y << ") (" << sw->boundary.max.x << ", " << sw->boundary.max.y << ")\n";
		//std::cout << "this is position of point: (" << obj.position.x << ", " << obj.position.y << ")\n";
	}
	if (se->withinBoundary(pos))
	{
		// Point is in south-east boundary, insert it into the tree
		if (se->insertSuccessfully(id, pos))
		{
			// Point has been successfully inserted into tree
			return true;
		}
		//std::cout << "enter bottom right boundary: (" << se->boundary.min.x << ", " << se->boundary.min.y << ") (" << se->boundary.max.x << ", " << se->boundary.max.y << ")\n";
		//std::cout << "this is position of point: (" << obj.position.x << ", " << obj.position.y << ")\n";
	}
	return true;
}
//bool quadTree::insertSuccessfully(quadObj const& obj)
//{
//	if (!withinBoundary(obj.position))
//		return false;	// Point is not in the quadTree boundary
//	if (points.size() < capacity)
//	{
//		// QaudTree has space
//		points.push_front(obj);
//		return true;
//	}
//	else if (plsDivide)
//		divideQuad();	// QuadTree has no space, divide into children if it isnt divided
//
//	if (nw->withinBoundary(obj.position))
//	{
//		// Point is in north-west boundary, insert it into the tree
//		if (nw->insertSuccessfully(obj))
//		{
//			// Point has been successfully inserted into tree
//			return true;
//		}
//		//std::cout << "enter top left boundary: (" << nw->boundary.min.x << ", " << nw->boundary.min.y << ") (" << nw->boundary.max.x << ", " << nw->boundary.max.y << ")\n";
//		//std::cout << "this is position of point: (" << obj.position.x << ", " << obj.position.y << ")\n";
//	}
//	if (ne->withinBoundary(obj.position))
//	{
//		// Point is in north-east boundary, insert it into the tree
//		if (ne->insertSuccessfully(obj))
//		{
//			// Point has been successfully inserted into tree
//			return true;
//		}
//		//std::cout << "enter top right boundary: (" << ne->boundary.min.x << ", " << ne->boundary.min.y << ") (" << ne->boundary.max.x << ", " << ne->boundary.max.y << ")\n";
//		//std::cout << "this is position of point: (" << obj.position.x << ", " << obj.position.y << ")\n";
//	}
//	if (sw->withinBoundary(obj.position))
//	{
//		// Point is in south-west boundary, insert it into the tree
//		if (sw->insertSuccessfully(obj))
//		{
//			// Point has been successfully inserted into tree
//			return true;
//		}
//		//std::cout << "enter bottom left boundary: (" << sw->boundary.min.x << ", " << sw->boundary.min.y << ") (" << sw->boundary.max.x << ", " << sw->boundary.max.y << ")\n";
//		//std::cout << "this is position of point: (" << obj.position.x << ", " << obj.position.y << ")\n";
//	}
//	if (se->withinBoundary(obj.position))
//	{
//		// Point is in south-east boundary, insert it into the tree
//		if (se->insertSuccessfully(obj))
//		{
//			// Point has been successfully inserted into tree
//			return true;
//		}
//		//std::cout << "enter bottom right boundary: (" << se->boundary.min.x << ", " << se->boundary.min.y << ") (" << se->boundary.max.x << ", " << se->boundary.max.y << ")\n";
//		//std::cout << "this is position of point: (" << obj.position.x << ", " << obj.position.y << ")\n";
//	}
//	return true;
//}

/******************************************************************************/
/*!
	This function divides the tree into 4 sections (nw/ne/sw/se)
*/
/******************************************************************************/
void quadTree::divideQuad()
{
	// Compute children's half width and half height
	float halfWidth = box.halfWidth / 2.f;
	float halfHeight = box.halfHeight / 2.f;

	// Make current tree a pointer
	std::shared_ptr<quadTree>convert = std::make_shared<quadTree>();
	convert->boundary = this->boundary;
	convert->capacity = this->capacity;
	convert->points = this->points;
	convert->parent = this->parent;
	convert->nw = this->nw;
	convert->ne = this->ne;
	convert->sw = this->sw;
	convert->se = this->se;
	convert->children = this->children;
	convert->plsDivide = this->plsDivide;

	// Create child tree: north-west
	nw = std::make_shared<quadTree>();
	nw->createQuadTree(vector2D::vec2D(box.position.x - halfWidth, box.position.y + halfHeight),
										halfWidth, halfHeight, convert);
	
	// Create child tree: north-east
	ne = std::make_shared<quadTree>();
	ne->createQuadTree(vector2D::vec2D(box.position.x + halfWidth, box.position.y + halfHeight),
										halfWidth, halfHeight, convert);
	
	// Create child tree: south-west
	sw = std::make_shared<quadTree>();
	sw->createQuadTree(vector2D::vec2D(box.position.x - halfWidth, box.position.y - halfHeight),
										halfWidth, halfHeight, convert);
	
	// Create child tree: south-east
	se = std::make_shared<quadTree>();
	se->createQuadTree(vector2D::vec2D(box.position.x + halfWidth, box.position.y - halfHeight),
										halfWidth, halfHeight, convert);

	// Change to false since tree now has children
	plsDivide = false;
}

/******************************************************************************/
/*!
	This function returns true if the input param pos is found within the tree's
	boudning box
*/
/******************************************************************************/
bool quadTree::withinBoundary(vector2D::vec2D const& pos)
{
	if (pos.x >= boundary.min.x && pos.y >= boundary.min.y &&
		pos.x <= boundary.max.x && pos.y <= boundary.max.y)
		return true;	// Point is within the tree's boundary

	return false;	// Point is outside the tree's boundary
}

/******************************************************************************/
/*!
	This function returns true if the tree's bounding box is intersecting with 
	the queryRange (AABB detection)
*/
/******************************************************************************/
bool quadTree::AABBIntersect(AABB const& queryRange)
{
	if (queryRange.min.x < boundary.max.x &&
		queryRange.min.y < boundary.max.y &&
		queryRange.max.x > boundary.min.x &&
		queryRange.max.y > boundary.min.y)
	{
		return true;	// Query Range is intersecting with tree's boundary
	}
	
	return false;	// Query Range is not intersecting with tree's boundary
	//std::cout << "no it did not intersect\n";
}

/******************************************************************************/
/*!
	This function checks for all points found witin a the queryRange and 
	stores the into objList
*/
/******************************************************************************/
bool quadTree::query(AABB & queryRange, std::list<EntityID*> & objList)
{
	if (AABBIntersect(queryRange))
	{
		// Query range and tree's boundary intersect
		for (std::list<EntityID>::iterator it = points.begin(); it != points.end(); ++it)
		{
			if (queryRange.withinBoundary(ecs.GetComponent<Render>(*it)->position))
			{
				// Point in tree is within the query range, add to list of obj to be tested for pathfinding/collision
				objList.push_front(&*it);
			}
		}
	}
	else
		return false;	// Query Range is not intersecting with tree's boundary

	if (!plsDivide) //if true, it has children
	{
		/* Check all childrenand include them in the list if they are in query range
		 Do not return true after each branch as you want to add all points within range*/
		// Tree has children
		nw->query(queryRange, objList);
			//return true;
		ne->query(queryRange, objList);
			//return true;
		sw->query(queryRange, objList);
			//return true;
		se->query(queryRange, objList);
			//return true;
	}
	return true;
}

/******************************************************************************/
/*!
	This function finds the location of the point and checks if the point's 
	new position is still within the boundary. If it is, no action is required.
	Else, the point will be swapped with a suitable point at the top of the tree.
	If no suitable point is found, the point will be removed from the tree and
	re-inserted into the tree
*/
/******************************************************************************/
bool quadTree::updatePoint(EntityID const& id, vector2D::vec2D const& prevPos, vector2D::vec2D const& currPos, quadTree & maintree)
{
	if (withinBoundary(prevPos))
	{
		for (std::list<EntityID>::iterator it = points.begin(); it != points.end(); ++it)
		{
			if (*it == id)
			{
				//std::cout << "KEY FOUND!!! " << obj.key << std::endl;
				// Obj found!!! Now, update the obj's pos
				//std::cout << "old pos: " << it->position.x << " " << it->position.y << std::endl;
				//it->position = newPos;
				//std::cout << "new pos: " << it->position.x << " " << it->position.y << std::endl;

				if (withinBoundary(currPos))
				{
					//std::cout << "point is within boundary\n";
					return true;	// Point's new pos is still within boundary, no update required
				}

				if (parent != nullptr && parent->withinBoundary(currPos))
				{
					if (parent->points.size() < (quadTreeCap -1))
					{
						// Parent has empty slot in std::list<quadObj> points, slot into parent
						if (!parent->insertSuccessfully(*it, currPos))
							std::cout << "failed\n";
						// Erase prev instance of obj
						std::cout << "bye\n";
						points.erase(it);
						return true;
					}
					
					// Check if children has empty slot instead
					for (size_t i{ 0 } ; i < parent->children.size() ; ++i)
					{
						if (parent->children[i] != nullptr && parent->children[i]->points.size() < (quadTreeCap-1))
						{
							// Child has space
							if (parent->children[i]->withinBoundary(currPos))
							{
								// Point is within child's boundary
								if (parent->children[i]->insertSuccessfully(*it, currPos))
									std::cout << "failed\n";

								// Erase prev instance of obj
								std::cout << "bye\n";
								points.erase(it);
								return true;
							}
						}
					}
				}
				maintree.insertSuccessfully(*it, currPos);
				points.erase(it);
				//std::cout << "point siz after erase: " << points.size() << std::endl;
				return true;
			}
		}
	}
	else
		return false;

	if (!plsDivide) //if true, it has children
	{
		if (nw->updatePoint(id, prevPos, currPos, maintree))
			return true;
		if (ne->updatePoint(id, prevPos, currPos, maintree))
			return true;
		if (sw->updatePoint(id, prevPos, currPos, maintree))
			return true;
		if (se->updatePoint(id, prevPos, currPos, maintree))
			return true;
	}

	return true;
}

/******************************************************************************/
/*!
	This function prints the quadTree's children
*/
/******************************************************************************/
void quadTree::printQuadTree(quadTree tree)
{
	static int x = 0;
	static int width = 20;
	if (tree.nw != nullptr)
	{
		if (tree.nw->points.size() >= quadTreeCap)
		{
			std::cout << std::setfill(' ') << std::setw(x) << " " << std::right << "-------------\n" << std::setw(x) << " " << "Size of child tree\n"
				<< std::setw(x) << " " << "top left: " << tree.nw->points.size() << std::endl;
			x += width;
			printQuadTree(*tree.nw);
			x -= width;
		}
		else
		{
			std::cout << std::setfill(' ') << std::setw(x) << " " << std::right << "-------------\n" << std::setw(x) << " " << "Size of child tree\n"
				<< std::setw(x) << " " << "top left: " << tree.nw->points.size() << std::endl;
		}
	}
	if (tree.ne != nullptr)
	{
		if (tree.ne->points.size() >= quadTreeCap)
		{
			std::cout << std::setfill(' ') << std::setw(x) << " " << std::right << "-------------\n" << std::setw(x) << " " << "Size of child tree\n"
				<< std::setw(x) << " " << "top right: " << tree.ne->points.size() << std::endl;
			x += width;
			printQuadTree(*tree.ne);
			x -= width;
		}
		else
		{
			std::cout << std::setfill(' ') << std::setw(x) << " " << std::right << "-------------\n" << std::setw(x) << " " << "Size of child tree\n"
				<< std::setw(x) << " " << "top right: " << tree.ne->points.size() << std::endl;
		}
	}
	if (tree.sw != nullptr)
	{
		if (tree.sw->points.size() >= quadTreeCap)
		{
			std::cout << std::setfill(' ') << std::setw(x) << " " << std::right << "-------------\n" << std::setw(x) << " " << "Size of child tree\n"
				<< std::setw(x) << " " << "bottom left: " << tree.sw->points.size() << std::endl;
			x += width;
			printQuadTree(*tree.sw);
			x -= width;
		}
		else
		{
			std::cout << std::setfill(' ') << std::setw(x) << " " << std::right << "-------------\n" << std::setw(x) << " " << "Size of child tree\n"
				<< std::setw(x) << " " << "bottom left: " << tree.sw->points.size() << std::endl;
		}
	}
	if (tree.se != nullptr)
	{
		if (tree.se->points.size() >= quadTreeCap)
		{
			std::cout << std::setfill(' ') << std::setw(x) << " " << std::right << "-------------\n" << std::setw(x) << " " << "Size of child tree\n"
				<< std::setw(x) << " " << "bottom right: " << tree.se->points.size() << std::endl;
			x += width;
			printQuadTree(*tree.se);
			x -= width;
		}
		else
		{
			std::cout << std::setfill(' ') << std::setw(x) << " " << std::right << "-------------\n" << std::setw(x) << " " << "Size of child tree\n"
				<< std::setw(x) << " " << "bottom right: " << tree.se->points.size() << std::endl;
		}
	}
}

//#endif