/* Start Header ************************************************************************/
/*!
\file		collision.h
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Sep 23, 2022
\brief		This file contains the function declarations and structs
			used by collision.cpp

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include "vec2D.h"
#include "mat3x3.h"
#include "app.h"
//#include "rigidBody.h"
#include <vector>

//void entityCollision()
//{
//
//}

namespace physics
{
	/******************************************************************************/
	/*!
		Struct that consist of the start and end point of a line, and the normal
		to the line
	*/
	/******************************************************************************/
	struct LineSegment
	{
		vector2D::vec2D	pt0;																// Start point
		vector2D::vec2D	pt1;																// end point
	};

	/******************************************************************************/
	/*!
		Struct that consist of the start and end point of a line, and the normal
		to the line
	*/
	/******************************************************************************/
	struct LineSegmentWNormal
	{
		vector2D::vec2D	pt0;																// Start point
		vector2D::vec2D	pt1;																// end point
		vector2D::vec2D	normal;																// Outward normalized normal
	};

	/******************************************************************************/
	/*!
		Struct that consist of the circle's center and its radius
	*/
	/******************************************************************************/
	struct Circle
	{
		vector2D::vec2D  m_center;
		float	m_radius;
	};

	/******************************************************************************/
	/*!
		This function checks for collision between static and dynamic circle objects. 
		If the objects are in collision, the static object will be pushed by the dynamic
		object
	*/
	/******************************************************************************/
	bool CollisionPushResponseCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad,
		vector2D::vec2D& dirNorm, float& depth);

	/******************************************************************************/
	/*!
		This function checks for collision between static and dynamic circle objects.
		If the objects are in collision, the dynamic object will be pushed back by the
		penetration amount
	*/
	/******************************************************************************/
	bool CollisionBlockResponseCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad,
		vector2D::vec2D& dirNorm, float& depth);
		
	/******************************************************************************/
	/*!
		This function checks for collision between static and dynamic circle objects
	*/
	/******************************************************************************/
	bool CollisionDetectionCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad,
		float& distanceBtnCenters, float& sumOfRad);

	/******************************************************************************/
	/*!
		This function checks for collision between static and dynamic circle objects
	*/
	/******************************************************************************/
	bool CollisionDetectionCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad);


	//bool shapeOverlapStaticAABB(GLApp::GLObject & polygon1, GLApp::GLObject & polygon2);

	//void shapeOverlapDynamicAABB(GLApp::GLObject& staticPolygon, GLApp::GLObject& dynamicPolygon);

	//bool shapeOverlapSAT(GLApp::GLObject const& polygon1,
	//					 GLApp::GLObject const& polygon2);

	/******************************************************************************/
	/*!
		This function checks for collision between static and dynamic convex polygon
		objects
	*/
	/******************************************************************************/
	bool CollisionDetectionPolygonPolygon(std::vector < vector2D::vec2D> staticVtx, std::vector < vector2D::vec2D> kineticVtx);
	
	/******************************************************************************/
	/*!
		This function checks for collision between static and dynamic convex polygon
		objects. If the objects are in collision, the static object will be pushed 
		by the dynamic object
	*/
	/******************************************************************************/
	bool CollisionPushPolygonPolygon(std::vector < vector2D::vec2D> staticVtx, std::vector < vector2D::vec2D> kineticVtx, vector2D::vec2D &norm, float& depth);
	
	/******************************************************************************/
	/*!
		This function checks for collision between static and dynamic convex polygon
		objects. If the objects are in collision, the dynamic object will be pushed 
		back by the penetration amount
	*/
	/******************************************************************************/
	bool CollisionBlockPolygonPolygon(std::vector < vector2D::vec2D> staticVtx, std::vector < vector2D::vec2D> kineticVtx, vector2D::vec2D& norm, float& depth);

	/******************************************************************************/
	/*!
		This function checks for collision between static and dynamic convex polygon
		and circle objects
	*/
	/******************************************************************************/
	bool CollisionDetectionCirclePolygon(vector2D::vec2D circleCenter, float rad, std::vector < vector2D::vec2D> boxVtx);

	/******************************************************************************/
	/*!
		This function checks for collision between static and dynamic convex polygon
		and circle objects. If the objects are in collision, the dynamic object will be pushed
		back by the penetration amount
	*/
	/******************************************************************************/
	bool CollisionBlockCirclePolygon(vector2D::vec2D circleCenter, float rad, std::vector < vector2D::vec2D> boxVtx,
		vector2D::vec2D& norm, float& depth);

	/******************************************************************************/
	/*!
		This helper function projects the vertices of a polygon on an axis for SAT
		collision
	*/
	/******************************************************************************/
	vector2D::vec2D projectVtx(std::vector<vector2D::vec2D> const& vtx, vector2D::vec2D const& axis);

	/******************************************************************************/
	/*!
		This helper function projects the ends of the circle on an axis for circle
		and polygon collision
	*/
	/******************************************************************************/
	vector2D::vec2D projectCircle(vector2D::vec2D circleCenter, float rad, vector2D::vec2D projAxis);

	/******************************************************************************/
	/*!
		This helper function checks for the closet point between the circle and
		polygon
	*/
	/******************************************************************************/
	int closestPointOfCircleToPolygon(vector2D::vec2D circleCenter, std::vector<vector2D::vec2D> boxVtx);

	/******************************************************************************/
	/*!
		This helper function gets the mean of the polygon's vertices
	*/
	/******************************************************************************/
	vector2D::vec2D meanOfVertices(std::vector<vector2D::vec2D> vtx);

	/******************************************************************************/
	/*!
		This helper function computes the bounding box of a convex polygon
	*/
	/******************************************************************************/
	void computeBoundingBox(GLApp::GLObject& polygon);

}
#endif /* PHYSICS_H */
