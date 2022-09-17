#include <iostream>
#include <array>
#include <algorithm>
#include <math.h>

#include "physics.h"
#include "collision.h"

namespace physics
{

	/**************************************************************************/
	/*!
		CollisionDetection_CircleCircle checks for collision between two circcles
		and returns true on collision else it returns false
	*/
	/**************************************************************************/
	bool CollisionPushResponseCircleCircle(
		vector2D::vec2D &staticCenter, float &staticRad, 
		vector2D::vec2D &kineticCenter, float &kineticRad,
		vector2D::vec2D & dirNorm, float &depth)
	{
		float distanceBtnCenters, sumOfRad;

		// No collision
		if (!CollisionDetectionCircleCircle(staticCenter, staticRad, kineticCenter, kineticRad, distanceBtnCenters, sumOfRad))
			return false;

		// There is collision, provide response
		vector2D::Vector2DNormalize(dirNorm, staticCenter - kineticCenter);
		depth = sqrt(sumOfRad) - sqrt(distanceBtnCenters);

		vector2D::vec2D tmpNorm{ dirNorm * (depth / 2.f) };
		vector2D::vec2D reverseTmpNorm{ tmpNorm * -1};

		//move(tmpNorm, staticCenter);
		//move(reverseTmpNorm, kineticCenter);

		return true;
	}

	/**************************************************************************/
	/*!
		CollisionDetection_CircleCircle checks for collision between two circcles
		and returns true on collision else it returns false
	*/
	/**************************************************************************/
	bool CollisionBlockResponseCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad,
		vector2D::vec2D& dirNorm, float& depth)
	{
		float distanceBtnCenters, sumOfRad;

		// No collision
		if (!CollisionDetectionCircleCircle(staticCenter, staticRad, kineticCenter, kineticRad, distanceBtnCenters, sumOfRad))
			return false;

		// There is collision, provide response
		vector2D::Vector2DNormalize(dirNorm, staticCenter - kineticCenter);
		depth = sqrt(sumOfRad) - sqrt(distanceBtnCenters);

		vector2D::vec2D tmpNorm{ dirNorm * depth };
		vector2D::vec2D reverseTmpNorm{ tmpNorm * -1 };

		//move(reverseTmpNorm, kineticCenter);

		return true;
	}

	/**************************************************************************/
	/*!
		CollisionResponse_CircleCircle checks for collision between two circcles
		and returns true on collision else it returns false
	*/
	/**************************************************************************/
	bool CollisionDetectionCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad,
		float &distanceBtnCenters, float & sumOfRad)
	{
		distanceBtnCenters = vector2D::Vector2DSquareDistance(staticCenter, kineticCenter) ;
		sumOfRad = (staticRad + kineticRad) * (staticRad + kineticRad) ;

		// No collision
		if (distanceBtnCenters >= sumOfRad)
			return false;

		// There is collision
		return true;
	}


	/**************************************************************************/
	/*!
		CollisionIntersection_RectRect checks for collision between two rectangles
		and returns true on collision else it returns false
	*/
	/**************************************************************************/
	//bool shapeOverlapStaticAABB(const AABB& rect1, const AABB& rect2)
	bool shapeOverlapStaticAABB(GLApp::GLObject & polygon1, GLApp::GLObject & polygon2)
	{
		polygon1.boundingBoxWorldVertices.clear();
		polygon2.boundingBoxWorldVertices.clear();
		
		// Store min/max points
		//AABB poly1{ polygon1.boundingBoxWorldVertices[0], polygon1.boundingBoxWorldVertices[2] };
		//AABB poly2{ polygon2.boundingBoxWorldVertices[0], polygon2.boundingBoxWorldVertices[2] };

		AABB poly1{ vector2D::minPointsOfPolygonBoundingBox(polygon1.worldVertices), vector2D::maxPointsOfPolygonBoundingBox(polygon1.worldVertices) };
		AABB poly2{ vector2D::minPointsOfPolygonBoundingBox(polygon2.worldVertices), vector2D::maxPointsOfPolygonBoundingBox(polygon2.worldVertices) };

		//Store bounding box for rendering
		polygon1.boundingBoxWorldVertices.emplace_back(poly1.min);
		polygon1.boundingBoxWorldVertices.emplace_back(vector2D::vec2D(poly1.max.x, poly1.min.y));
		polygon1.boundingBoxWorldVertices.emplace_back(poly1.max);
		polygon1.boundingBoxWorldVertices.emplace_back(vector2D::vec2D(poly1.min.x, poly1.max.y));

		polygon2.boundingBoxWorldVertices.emplace_back(poly2.min);
		polygon2.boundingBoxWorldVertices.emplace_back(vector2D::vec2D(poly2.max.x, poly2.min.y));
		polygon2.boundingBoxWorldVertices.emplace_back(poly2.max);
		polygon2.boundingBoxWorldVertices.emplace_back(vector2D::vec2D(poly2.min.x, poly2.max.y));


		// Check for static collision detection between rectangles
		if (poly1.min.x <= poly2.max.x && poly1.max.x >= poly2.min.x &&
			poly1.min.y <= poly2.max.y && poly1.max.y >= poly2.min.y)
		{
			return true;
		}
		return false;

		//AABB poly1{ polygon1.worldVertices[0], polygon1.worldVertices[2] };
		//AABB poly2{ polygon2.worldVertices[0], polygon2.worldVertices[2] };

		//std::cout << "----------------------\nthis is polygon1 worldcoord: \n"
		//	<< "size of polygon1 world coords: " << polygon1.worldVertices.size() << std::endl
		//	<< polygon1.worldVertices[0].x << " " << polygon1.worldVertices[0].y << std::endl
		//	<< polygon1.worldVertices[1].x << " " << polygon1.worldVertices[1].y << std::endl
		//	<< polygon1.worldVertices[2].x << " " << polygon1.worldVertices[2].y << std::endl
		//	<< polygon1.worldVertices[3].x << " " << polygon1.worldVertices[3].y << std::endl;
		//std::cout << "----------------------\nthis is polygon2 worldcoord: \n"
		//	<< "size of polygon2 world coords: " << polygon2.worldVertices.size() << std::endl
		//	<< polygon2.worldVertices[0].x << " " << polygon2.worldVertices[0].y << std::endl
		//	<< polygon2.worldVertices[1].x << " " << polygon2.worldVertices[1].y << std::endl
		//	<< polygon2.worldVertices[2].x << " " << polygon2.worldVertices[2].y << std::endl
		//	<< polygon2.worldVertices[3].x << " " << polygon2.worldVertices[3].y << std::endl;

		//std::cout << "----------------------\nthis is polygon1 BB (min, max): \n"
		//	<< poly1.min.x << " " << poly1.min.y << std::endl
		//	<< poly1.max.x << " " << poly1.max.y << std::endl;

		//std::cout << "----------------------\nthis is polygon2 BB (min, max): \n"
		//	<< poly2.min.x << " " << poly2.min.y << std::endl
		//	<< poly2.max.x << " " << poly2.max.y << std::endl;

	}

	void computeBoundingBox(GLApp::GLObject& polygon)
	{
		polygon.boundingBoxWorldVertices.clear();

		// Compute min/max points
		AABB poly{ vector2D::minPointsOfPolygonBoundingBox(polygon.worldVertices), vector2D::maxPointsOfPolygonBoundingBox(polygon.worldVertices) };

		// Store bounding box for rendering
		polygon.boundingBoxWorldVertices.emplace_back(poly.min);
		polygon.boundingBoxWorldVertices.emplace_back(vector2D::vec2D(poly.max.x, poly.min.y));
		polygon.boundingBoxWorldVertices.emplace_back(poly.max);
		polygon.boundingBoxWorldVertices.emplace_back(vector2D::vec2D(poly.min.x, poly.max.y));

	}

	//bool shapeOverlapDynamicAABB(const AABB& aabb1, const vector2D::vec2D& vel1,
	//const AABB& aabb2, const vector2D::vec2D& vel2)
	void shapeOverlapDynamicAABB(GLApp::GLObject& staticPolygon, GLApp::GLObject& dynamicPolygon)
	{
		if (shapeOverlapStaticAABB(staticPolygon, dynamicPolygon)) //polygon1 is pushed back
		{
			//float tFirst(0), tLast(GLHelper::delta_time);
			//vector2D::vec2D relativeVel{ staticPolygon.vel.x - dynamicPolygon.vel.x, staticPolygon.vel.y - dynamicPolygon.vel.y };

			// Compute min/max points
			AABB staticPoly{ vector2D::minPointsOfPolygonBoundingBox(staticPolygon.worldVertices), vector2D::maxPointsOfPolygonBoundingBox(staticPolygon.worldVertices) };
			AABB dynamicPoly{ vector2D::minPointsOfPolygonBoundingBox(dynamicPolygon.worldVertices), vector2D::maxPointsOfPolygonBoundingBox(dynamicPolygon.worldVertices) };

			std::cout << "-----------------\nstatic poly bef response\n"
				<< "min points: (" << staticPoly.min.x << ", " << staticPoly.min.y << ")\n"
				<< "max points: (" << staticPoly.max.x << ", " << staticPoly.max.y << ")\n"
				<< "center points: (" << staticPolygon.modelCenterPos.x << ", " << staticPolygon.modelCenterPos.y << ")\n";
				
			std::cout << "-----------------\ndynamic poly bef response\n"
				<< "min points: (" << dynamicPoly.min.x << ", " << dynamicPoly.min.y << ")\n"
				<< "max points: (" << dynamicPoly.max.x << ", " << dynamicPoly.max.y << ")\n"
				<< "center points: (" << dynamicPolygon.modelCenterPos.x << ", " << dynamicPolygon.modelCenterPos.y << ")\n";

			
			// Check location of dynamicPolygon wrt staticPolygon
			//if (dynamicPoly.max.x > dynamicPoly.min.x) // dymanic is on left side of static

			// Find shorter length
			float lengthOfOverlap = (staticPoly.max.x - dynamicPoly.min.x) < (dynamicPoly.max.x - staticPoly.min.x) ? staticPoly.max.x - dynamicPoly.min.x : dynamicPoly.max.x - staticPoly.min.x;

			// Length btn both polygon along x-axis
			float distanceBtnCenters{ staticPolygon.modelCenterPos.x - dynamicPolygon.modelCenterPos.x };
			//std::cout << "this is distance btn centers: " << distanceBtnCenters << std::endl;
			distanceBtnCenters = distanceBtnCenters < 0.f ? -distanceBtnCenters : distanceBtnCenters;

			// Half width of polygon1 and polygon 2
			float poly1HalfWidth{ staticPoly.max.x - staticPolygon.modelCenterPos.x }, poly2HalfWidth{ dynamicPoly.max.x - dynamicPolygon.modelCenterPos.x };

			// Vector btn the center of polygon1 and polygon2
			//vector2D::vec2D directionalVec{ staticPolygon.modelCenterPos - dynamicPolygon.modelCenterPos };

			// Interpolate and negate the vector
			//vector2D::vec2D reverseDirectionalVec { -directionalVec * (lengthOfOverlap/distanceBtnCenters)};
			//std::cout << reverseDirectionalVec.x << " " << reverseDirectionalVec.y << std::endl;
			

			std::cout << "this is delta time: " << GLHelper::delta_time << std::endl;
			std::cout << "direction vec in col: " << dynamicPolygon.directionVec.x * GLHelper::delta_time * 350.f << " " << dynamicPolygon.directionVec.y * GLHelper::delta_time * 200.f << std::endl;
			vector2D::vec2D reverseDirectionalVec{ -dynamicPolygon.directionVec * GLHelper::delta_time * 400.f };// *(lengthOfOverlap / distanceBtnCenters) };
			std::cout << "this is reverse direction vec: " << reverseDirectionalVec.x << " " << reverseDirectionalVec.y << std::endl;

			// Add vector to modelcenterpos
			dynamicPolygon.modelCenterPos += reverseDirectionalVec ;
			//std::cout << "this is center after response: " << dynamicPolygon.modelCenterPos.x << " " << dynamicPolygon.modelCenterPos.y << std::endl;
			
			computeBoundingBox(dynamicPolygon);

			std::cout << "-----------------\nstatic poly aft response\n"
				<< "min points: (" << staticPoly.min.x << ", " << staticPoly.min.y << ")\n"
				<< "max points: (" << staticPoly.max.x << ", " << staticPoly.max.y << ")\n"
				<< "center points: (" << staticPolygon.modelCenterPos.x << ", " << staticPolygon.modelCenterPos.y << ")\n";

			std::cout << "-----------------\ndynamic poly aft response\n"
				<< "min points: (" << dynamicPoly.min.x << ", " << dynamicPoly.min.y << ")\n"
				<< "max points: (" << dynamicPoly.max.x << ", " << dynamicPoly.max.y << ")\n"
				<< "center points: (" << dynamicPolygon.modelCenterPos.x << ", " << dynamicPolygon.modelCenterPos.y << ")\n";


			dynamicPolygon.untravelledDistance.second = 0.f;
			dynamicPolygon.directionVec.x = 0.f;
			dynamicPolygon.directionVec.y = 0.f;
			staticPolygon.overlap = true;
			dynamicPolygon.overlap = true;
		}
	}

	/******************************************************************************/
	/*!

	*/
	/******************************************************************************/
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

	/******************************************************************************/
	/*!

	*/
	/******************************************************************************/
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

	/******************************************************************************/
	/*!

	*/
	/******************************************************************************/
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

	///******************************************************************************/
	///*!
	//	Check for both circle & pillar and circle & circle collision
	// */
	// /******************************************************************************/
	//int CollisionIntersection_CircleCircle(const Circle& circleA,		//ball
	//										const vector2D::vec2D& velA,  //ball vel
	//										const Circle& circleB,		//pillar
	//										const  vector2D::vec2D& velB,  //pillar vel
	//										vector2D::vec2D & interPtA,	//ball's intersection pt
	//										vector2D::vec2D & interPtB,	//pillar's intersection pt
	//										float& interTime)			//intersection time -> B(ti)
	//{
	//	// make circleB static
	//	float m{ 0.f };
	//	vector2D::vec2D velATmp{ velA };
	//	vector2D::Vector2DNormalize(velATmp, velA);
	//	m = vector2D::Vector2DDotProduct(circleB.m_center - circleA.m_center, velATmp);

	//	float radiusSum{ circleA.m_radius + circleB.m_radius };
	//	float distanceBtnCircles{ Vector2DLength(circleA.m_center - circleB.m_center) };
	//	float nSquare = distanceBtnCircles * distanceBtnCircles - m * m;

	//	if (m < 0 && distanceBtnCircles > radiusSum)
	//	{
	//		return 0;
	//	}
	//	else if (nSquare > radiusSum * radiusSum)
	//	{
	//		return 0;
	//	}
	//	else
	//	{
	//		float sSquare{ radiusSum * radiusSum - nSquare };
	//		float interTimeTmp0{ 0.f }, interTimeTmp1{ 0.f };
	//		interTimeTmp0 = (m - sqrt(sSquare)) / Vector2DLength(velA);
	//		interTimeTmp1 = (m + sqrt(sSquare)) / Vector2DLength(velA);
	//		interTime = std::min(interTimeTmp0, interTimeTmp1);
	//		if (interTime >= 0 && interTime <= 1)
	//		{
	//			interPtA = circleA.m_center + velA * interTime;
	//			return 1;
	//		}
	//	}
	//	return 0;
	//}

	///******************************************************************************/
	///*!
	//	Computes the new circle position if there is collision between the circle
	//	and the pillar
	// */
	// /******************************************************************************/
	//void CollisionResponse_CirclePillar(const vector2D::vec2D & normal,
	//									const float& interTime,
	//									const vector2D::vec2D& ptStart,
	//									const vector2D::vec2D& ptInter,
	//									vector2D::vec2D& ptEnd,
	//									vector2D::vec2D& reflectedVectorNormalized)
	//{
	//	reflectedVectorNormalized = 2 * (vector2D::Vector2DDotProduct(ptStart - ptInter, normal)) * normal - (ptStart - ptInter);
	//	vector2D::Vector2DNormalize(reflectedVectorNormalized, reflectedVectorNormalized);
	//	ptEnd = ptInter + vector2D::Vector2DLength(ptEnd - ptStart) * reflectedVectorNormalized * (1 - interTime);
	//}

	///******************************************************************************/
	///*!

	//*/
	// /******************************************************************************/
	//void CollisionResponse_CircleCircle(vector2D::vec2D & normal,
	//									const float interTime,
	//									vector2D::vec2D & velA,
	//									const float& massA,
	//									vector2D::vec2D & interPtA,
	//									vector2D::vec2D & velB,
	//									const float& massB,
	//									vector2D::vec2D & interPtB,
	//									vector2D::vec2D & reflectedVectorA,
	//									vector2D::vec2D & ptEndA,
	//									vector2D::vec2D & reflectedVectorB,
	//									vector2D::vec2D & ptEndB)
	//{
	//	float tmpVecA{ vector2D::Vector2DDotProduct(velA, normal) };
	//	float tmpVecB{ vector2D::Vector2DDotProduct(velB, normal) };

	//	reflectedVectorA = velA - (2 * (tmpVecA - tmpVecB) / (massA + massB)) * massB * normal;

	//	ptEndA = interPtA + (1 - interTime) * reflectedVectorA;

	//	reflectedVectorB = velB + (2 * (tmpVecA - tmpVecB) / (massA + massB)) * massA * normal;

	//	ptEndB = interPtB + (1 - interTime) * reflectedVectorB;
	//}

}