#ifndef PHYSICS_H
#define PHYSICS_H

#include "vec2D.h"
#include "mat3x3.h"
#include "glapp.h"
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

	/******************************************************************************/
	/*!
		Builds a line segment to be used for collision check
	*/
	/******************************************************************************/
	void BuildLineSegment(LineSegmentWNormal& lineSegment,									// Line segment reference - input/output
						  const vector2D::vec2D& pos,										// Position - input
						  float scale,														// Scale - input
						  float dir);														// Direction - input


	// Intersection functions
	/******************************************************************************/
	/*!
		Checks for collision between a circule and a line segment
	*/
	/******************************************************************************/
	int CollisionIntersection_CircleLineSegment(const Circle& circle,						// Circle data - input
												const vector2D::vec2D& ptEnd,				// End circle position - input
												const LineSegmentWNormal& lineSeg,			// Line segment - input
												vector2D::vec2D& interPt,					// Intersection point - output
												vector2D::vec2D& normalAtCollision,			// Normal vector at collision time - output
												float& interTime);							// Intersection time ti - output

	// Response functions
	/******************************************************************************/
	/*!
		Computes the new circle position if there is collision between the circle
		and line segment
	*/
	/******************************************************************************/
	void CollisionResponse_CircleLineSegment(const vector2D::vec2D& ptInter,				// Intersection position of the circle - input
											 const vector2D::vec2D& normal,					// Normal vector of reflection on collision time - input
											 vector2D::vec2D& ptEnd,						// Final position of the circle after reflection - output
											 vector2D::vec2D& reflected);					// Normalized reflection vector direction - output

	bool shapeOverlapStaticAABB(GLApp::GLObject & polygon1, GLApp::GLObject & polygon2);

	//bool shapeOverlapDynamicAABB(const AABB& aabb1, const vector2D::vec2D& vel1,
	//	const AABB& aabb2, const vector2D::vec2D& vel2);
	void shapeOverlapDynamicAABB(GLApp::GLObject& polygon1, GLApp::GLObject& polygon2);

	bool shapeOverlapSAT(GLApp::GLObject const& polygon1,
						 GLApp::GLObject const& polygon2);

	bool shapeOverlapDIAGONAL(GLApp::GLObject const& polygon1, 
							  GLApp::GLObject const& polygon2);

	bool shapeOverlapSnapStaticDIAGONAL(GLApp::GLObject & polygon1,
										GLApp::GLObject & polygon2);

}
#endif /* PHYSICS_H */
