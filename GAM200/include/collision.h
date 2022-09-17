#pragma once
#ifndef COLLISION_H
#define COLLISION_H

#include "vec2D.h"
#include "mat3x3.h"
#include "glapp.h"
//#include "rigidBody.h"

#include <vector>

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
		
	*/
	/******************************************************************************/
	struct AABB
	{
		vector2D::vec2D	min;																// Min point of bounding box
		vector2D::vec2D	max;																// Max point of bounding box
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

	bool CollisionPushResponseCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad,
		vector2D::vec2D& dirNorm, float& depth);

	bool CollisionBlockResponseCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad,
		vector2D::vec2D& dirNorm, float& depth);
		
	bool CollisionDetectionCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad,
		float& distanceBtnCenters, float& sumOfRad);

	bool shapeOverlapStaticAABB(GLApp::GLObject & polygon1, GLApp::GLObject & polygon2);

	void shapeOverlapDynamicAABB(GLApp::GLObject& staticPolygon, GLApp::GLObject& dynamicPolygon);

	bool shapeOverlapSAT(GLApp::GLObject const& polygon1,
						 GLApp::GLObject const& polygon2);

	bool shapeOverlapDIAGONAL(GLApp::GLObject const& polygon1, 
							  GLApp::GLObject const& polygon2);

	bool shapeOverlapSnapStaticDIAGONAL(GLApp::GLObject & polygon1,
										GLApp::GLObject & polygon2);

	void computeBoundingBox(GLApp::GLObject& polygon);

}
#endif /* PHYSICS_H */
