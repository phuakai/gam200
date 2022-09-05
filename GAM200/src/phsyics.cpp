#include "physics.h"
#include <iostream>
#include <array>
#include <algorithm>
#include <math.h>

#include "physics.h"

namespace physics
{
	/******************************************************************************/
	/*!
		Builds a line segment to be used for collision check
	 */
	 /******************************************************************************/
	void BuildLineSegment(LineSegmentWNormal& lineSegment,
		const vector2D::vec2D& pos,
		float scale,
		float dir)
	{
		//create the points of the line and rotates the points
		lineSegment.pt0.x = pos.x - scale / 2.f * cos(dir);
		lineSegment.pt0.y = pos.y - scale / 2.f * sin(dir);
		lineSegment.pt1.x = pos.x + scale / 2.f * cos(dir);
		lineSegment.pt1.y = pos.y + scale / 2.f * sin(dir);

		//create the normal to the line
		lineSegment.normal.x = lineSegment.pt1.y - lineSegment.pt0.y;
		lineSegment.normal.y = -(lineSegment.pt1.x - lineSegment.pt0.x);

		//normalise the normal
		vector2D::vec2D normal{ lineSegment.normal };
		Vector2DNormalize(lineSegment.normal, normal);
	}

	/******************************************************************************/
	/*!
		Checks for collision between a circule and a line segment
	 */
	 /******************************************************************************/
	int CollisionIntersection_CircleLineSegment(const Circle& circle, //start pos, rad
		const vector2D::vec2D& ptEnd, //circle pos at the end of curr frame
		const LineSegmentWNormal& lineSeg, //border: start/end pos, normal
		vector2D::vec2D& interPt, //intersection pt btn circle & wall
		vector2D::vec2D& normalAtCollision, //vec normal at collision pt
		float& interTime) //value of intersection time
	{
		if (Vector2DDotProduct(lineSeg.normal, (circle.m_center - lineSeg.pt0)) <= -circle.m_radius)
		{
			vector2D::vec2D tmpPO{ lineSeg.pt0.x - circle.m_radius * lineSeg.normal.x,
									lineSeg.pt0.y - circle.m_radius * lineSeg.normal.y };
			vector2D::vec2D tmpP1{ lineSeg.pt1.x - circle.m_radius * lineSeg.normal.x,
									lineSeg.pt1.y - circle.m_radius * lineSeg.normal.y };

			vector2D::vec2D V{ ptEnd.x - circle.m_center.x, ptEnd.y - circle.m_center.y };
			vector2D::vec2D M;

			vector2D::Vector2DNormalize(M, V);

			matrix3x3::mat3x3 rot;
			matrix3x3::Mtx33RotDeg(rot, -90.f);
			M = rot * M;
			if (vector2D::Vector2DDotProduct(M, circle.m_center - tmpPO) *
				vector2D::Vector2DDotProduct(M, circle.m_center - tmpP1) < 0)
			{
				interTime = (vector2D::Vector2DDotProduct(lineSeg.normal, lineSeg.pt0) -
					vector2D::Vector2DDotProduct(lineSeg.normal, circle.m_center) - circle.m_radius) /
					vector2D::Vector2DDotProduct(lineSeg.normal, V);

				if (interTime >= 0 && interTime <= 1)
				{
					interPt = circle.m_center + V * interTime;
					normalAtCollision = -lineSeg.normal;
					return 1; // intersection
				}
			}
		}
		else if (vector2D::Vector2DDotProduct(lineSeg.normal, (circle.m_center - lineSeg.pt0)) >= circle.m_radius)
		{
			vector2D::vec2D tmpPO{ lineSeg.pt0.x + circle.m_radius * lineSeg.normal.x,
									lineSeg.pt0.y + circle.m_radius * lineSeg.normal.y };
			vector2D::vec2D tmpP1{ lineSeg.pt1.x + circle.m_radius * lineSeg.normal.x,
									lineSeg.pt1.y + circle.m_radius * lineSeg.normal.y };

			vector2D::vec2D V{ ptEnd.x - circle.m_center.x, ptEnd.y - circle.m_center.y };
			vector2D::vec2D M;

			vector2D::Vector2DNormalize(M, V);

			matrix3x3::mat3x3 rot;
			matrix3x3::Mtx33RotDeg(rot, -90.f);
			M = rot * M;
			if (vector2D::Vector2DDotProduct(M, circle.m_center - tmpPO) *
				vector2D::Vector2DDotProduct(M, circle.m_center - tmpP1) < 0)
			{
				interTime = (vector2D::Vector2DDotProduct(lineSeg.normal, lineSeg.pt0) -
					vector2D::Vector2DDotProduct(lineSeg.normal, circle.m_center) + circle.m_radius) /
					vector2D::Vector2DDotProduct(lineSeg.normal, V);

				if (interTime >= 0 && interTime <= 1)
				{
					interPt = circle.m_center + V * interTime;
					normalAtCollision = -lineSeg.normal;
					return 1; // intersection
				}
			}
		}
		return 0; // no intersection
	}

	/******************************************************************************/
	/*!
		Computes the new circle position if there is collision between the circle
		and line segment
	 */
	 /******************************************************************************/
	void CollisionResponse_CircleLineSegment(const vector2D::vec2D& ptInter,
		const vector2D::vec2D& normal,
		vector2D::vec2D& ptEnd,
		vector2D::vec2D& reflected)
	{
		reflected = (ptEnd - ptInter) - 2 * (vector2D::Vector2DDotProduct(ptEnd - ptInter, normal)) * normal;
		vector2D::vec2D reflection{ reflected };
		vector2D::Vector2DNormalize(reflected, reflection);
		ptEnd = ptInter + reflected;
	}

	bool shapeOverlapSAT(GLApp::GLObject const& polygon1, GLApp::GLObject const& polygon2)
	{
		GLApp::GLObject const* shape1 = &polygon1;
		GLApp::GLObject const* shape2 = &polygon2;
		static float myInf{ std::numeric_limits<float>::infinity() };

		for (int i = 0; i < 2; ++i)
		{
			if (i == 1)									// swap both shapes
			{
				shape1 = &polygon2;
				shape2 = &polygon1;
			}

			for (size_t j = 0; j < shape1->worldVertices.size(); ++j)
			{
				int k = (j + 1) % shape1->worldVertices.size();

				//create normal on the left of the edge (ccw rot by 90 deg)
				vector2D::vec2D normalToEdge = { -(shape1->worldVertices[k].y - shape1->worldVertices[j].y),
													shape1->worldVertices[k].x - shape1->worldVertices[j].x };

				/***********************************
				* Check for min and max points for 
				* shape 1
				***********************************/
				float shape1Min{ FLT_MAX }, shape1Max{ -FLT_MAX };
				for (size_t l{ 0 }; l < shape1->worldVertices.size(); ++l)
				{
					/***********************************
					* Dot prod of the shape's point and 
					* the normal
					***********************************/
					float length = shape1->worldVertices[l].x * normalToEdge.x + shape1->worldVertices[l].y * normalToEdge.y;
					shape1Min = std::min(shape1Min, length);
					shape1Max = std::max(shape1Max, length);
				}

				/***********************************
				* Check for min and max points for 
				* shape 2
				***********************************/
				float shape2Min{ myInf }, shape2Max{ -myInf };
				for (size_t l{ 0 }; l < shape2->worldVertices.size(); ++l)
				{
					//dot prod of the shape's point and the normal
					float length = shape2->worldVertices[l].x * normalToEdge.x + shape2->worldVertices[l].y * normalToEdge.y;
					shape2Min = std::min(shape2Min, length);
					shape2Max = std::max(shape2Max, length);
				}

				/***********************************
				* Check AABB collision
				***********************************/
				//
				if (!(shape2Max >= shape1Min && shape1Max >= shape2Min))
					return false; //shape is not in collision
			}
		}
		return true;
	}

	bool shapeOverlapDIAGONAL(GLApp::GLObject const& polygon1, GLApp::GLObject const& polygon2)
	{
		GLApp::GLObject const* shape1 = &polygon1;
		GLApp::GLObject const* shape2 = &polygon2;
		static float myInf{ std::numeric_limits<float>::infinity() };

		for (int i = 0; i < 2; ++i)
		{
			if (i == 1)									// swap both shapes
			{
				shape1 = &polygon2;
				shape2 = &polygon1;
			}

			/***********************************
			* Check diagonals of polygon 1
			***********************************/
			for (size_t j = 0; j < shape1->worldVertices.size(); ++j)
			{
				physics::LineSegment lineSeg1{ vector2D::vec2D(shape1->worldCenterPos.x, shape1->worldCenterPos.y),					// Start point
											   vector2D::vec2D(shape1->worldVertices[j].x, shape1->worldVertices[j].y) };			// End point

				/***********************************
				* Check edges of polygon 2
				***********************************/
				for (size_t k = 0; k < shape2->worldVertices.size(); ++k)
				{
					int l = (k + 1) % shape2->worldVertices.size();
					physics::LineSegment lineSeg2{ vector2D::vec2D(shape2->worldVertices[k].x, shape2->worldVertices[k].y),					// Start point
												   vector2D::vec2D(shape2->worldVertices[l].x, shape2->worldVertices[l].y) };				// End point

					/***********************************
					* Check for line segments
					* intersection
					***********************************/
					float h{ (lineSeg2.pt1.x - lineSeg2.pt0.x) * (lineSeg1.pt0.y - lineSeg1.pt1.y) - (lineSeg1.pt0.x - lineSeg1.pt1.x) * (lineSeg2.pt1.y - lineSeg2.pt0.y) };
					float t1{ ((lineSeg2.pt0.y - lineSeg2.pt1.y) * (lineSeg1.pt0.x - lineSeg2.pt0.x) + (lineSeg2.pt1.x - lineSeg2.pt0.x) * (lineSeg1.pt0.y - lineSeg2.pt0.y)) / h};
					float t2{ ((lineSeg1.pt0.y - lineSeg1.pt1.y) * (lineSeg1.pt0.x - lineSeg2.pt0.x) + (lineSeg1.pt1.x - lineSeg1.pt0.x) * (lineSeg1.pt0.y - lineSeg2.pt0.y)) / h};

					if (t1 >= 0.f && t1 < 1.f && t2 >= 0.f && t2 < 1.f)
						return true;
				}
			}
		}
		return false;
	}


	bool shapeOverlapSnapStaticDIAGONAL(GLApp::GLObject & polygon1, GLApp::GLObject & polygon2)
	{
		GLApp::GLObject * shape1 = &polygon1;
		GLApp::GLObject * shape2 = &polygon2;
		static float myInf{ std::numeric_limits<float>::infinity() };

		for (int i = 0; i < 2; ++i)
		{
			if (i == 1)									// swap both shapes
			{
				shape1 = &polygon2;
				shape2 = &polygon1;
			}

			/***********************************
			* Check diagonals of polygon 1
			***********************************/
			for (size_t j = 0; j < shape1->worldVertices.size(); ++j)
			{
				physics::LineSegment lineSeg1{ vector2D::vec2D(shape1->worldCenterPos.x, shape1->worldCenterPos.y),					// Start point
											   vector2D::vec2D(shape1->worldVertices[j].x, shape1->worldVertices[j].y) };			// End point
				vector2D::vec2D displacement{ 0.f, 0.f };

				/***********************************
				* Check edges of polygon 2
				***********************************/
				for (size_t k = 0; k < shape2->worldVertices.size(); ++k)
				{
					int l = (k + 1) % shape2->worldVertices.size();
					physics::LineSegment lineSeg2{ vector2D::vec2D(shape2->worldVertices[k].x, shape2->worldVertices[k].y),					// Start point
												   vector2D::vec2D(shape2->worldVertices[l].x, shape2->worldVertices[l].y) };				// End point

					/***********************************
					* Check for line segments
					* intersection
					***********************************/
					float h{ (lineSeg2.pt1.x - lineSeg2.pt0.x) * (lineSeg1.pt0.y - lineSeg1.pt1.y) - (lineSeg1.pt0.x - lineSeg1.pt1.x) * (lineSeg2.pt1.y - lineSeg2.pt0.y) };
					float t1{ ((lineSeg2.pt0.y - lineSeg2.pt1.y) * (lineSeg1.pt0.x - lineSeg2.pt0.x) + (lineSeg2.pt1.x - lineSeg2.pt0.x) * (lineSeg1.pt0.y - lineSeg2.pt0.y)) / h };
					float t2{ ((lineSeg1.pt0.y - lineSeg1.pt1.y) * (lineSeg1.pt0.x - lineSeg2.pt0.x) + (lineSeg1.pt1.x - lineSeg1.pt0.x) * (lineSeg1.pt0.y - lineSeg2.pt0.y)) / h };

					if (t1 >= 0.f && t1 < 1.f && t2 >= 0.f && t2 < 1.f)
					{
						displacement.x += (1.f - t1) * (lineSeg1.pt1.x - lineSeg1.pt0.x);
						displacement.y += (1.f - t1) * (lineSeg1.pt1.y - lineSeg1.pt0.y);
						shape1->overlap = true;
					}
					//std::cout << displacement.x << " " << displacement.y << std::endl;

				}
				for (size_t m{ 0 }; m < shape1->worldVertices.size(); ++m)
				{
					shape1->worldVertices[m].x += displacement.x * (i == 0 ? -1 : +1);
					shape1->worldVertices[m].y += displacement.y * (i == 0 ? -1 : +1);
				}
				shape1->worldCenterPos.x += displacement.x * (i == 0 ? -1 : +1);
				shape1->worldCenterPos.y += displacement.y * (i == 0 ? -1 : +1);
			}
		}
		return false;
	}


}