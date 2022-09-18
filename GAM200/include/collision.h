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

	bool CollisionDetectionCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad);

	bool shapeOverlapStaticAABB(GLApp::GLObject & polygon1, GLApp::GLObject & polygon2);

	void shapeOverlapDynamicAABB(GLApp::GLObject& staticPolygon, GLApp::GLObject& dynamicPolygon);

	bool shapeOverlapSAT(GLApp::GLObject const& polygon1,
						 GLApp::GLObject const& polygon2);

	bool CollisionDetectionPolygonPolygon(std::vector < vector2D::vec2D> staticVtx, std::vector < vector2D::vec2D> kineticVtx);
	bool CollisionPushPolygonPolygon(std::vector < vector2D::vec2D> staticVtx, std::vector < vector2D::vec2D> kineticVtx, vector2D::vec2D &norm, float& depth);
	bool CollisionBlockPolygonPolygon(std::vector < vector2D::vec2D> staticVtx, std::vector < vector2D::vec2D> kineticVtx, vector2D::vec2D& norm, float& depth);

	bool CollisionDetectionCirclePolygon(vector2D::vec2D circleCenter, float rad, std::vector < vector2D::vec2D> boxVtx);

	bool CollisionBlockCirclePolygon(vector2D::vec2D circleCenter, float rad, std::vector < vector2D::vec2D> boxVtx,
		vector2D::vec2D& norm, float& depth);

	vector2D::vec2D projectVtx(std::vector<vector2D::vec2D> const& vtx, vector2D::vec2D const& axis);
	vector2D::vec2D projectCircle(vector2D::vec2D circleCenter, float rad, vector2D::vec2D projAxis);
	int closestPointOfCircleToPolygon(vector2D::vec2D circleCenter, std::vector<vector2D::vec2D> boxVtx);
	vector2D::vec2D meanOfVertices(std::vector<vector2D::vec2D> vtx);

	void computeBoundingBox(GLApp::GLObject& polygon);

}
#endif /* PHYSICS_H */
