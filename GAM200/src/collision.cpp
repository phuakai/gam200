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

		//vector2D::vec2D tmpNorm{ dirNorm * (depth / 2.f) };
		//vector2D::vec2D reverseTmpNorm{ tmpNorm * -1};

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
		dirNorm *= -1;

		//vector2D::vec2D tmpNorm{ dirNorm * depth };
		//vector2D::vec2D reverseTmpNorm{ tmpNorm * -1 };

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
		CollisionResponse_CircleCircle checks for collision between two circcles
		and returns true on collision else it returns false
	*/
	/**************************************************************************/
	bool CollisionDetectionCircleCircle(
		vector2D::vec2D& staticCenter, float& staticRad,
		vector2D::vec2D& kineticCenter, float& kineticRad)
	{
		float distanceBtnCenters{ vector2D::Vector2DSquareDistance(staticCenter, kineticCenter) };
		float sumOfRad{ (staticRad + kineticRad) * (staticRad + kineticRad) };

		// No collision
		if (distanceBtnCenters >= sumOfRad)
			return false;

		// There is collision
		return true;
	}

#if false
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

#endif
#if false
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
#endif
#if false
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
#endif
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
				if (!(shape2Max >= shape1Min && shape1Max >= shape2Min))
					return false; //shape is not in collision
			}
		}
		return true;
	}

	bool CollisionDetectionPolygonPolygon(std::vector < vector2D::vec2D> staticVtx, std::vector < vector2D::vec2D> kineticVtx)
	{

		vector2D::vec2D staticMinMax{ 0.f, 0.f }, kineticMinMax{ 0.f, 0.f };
		for (size_t i{ 0 }; i < staticVtx.size(); ++i)
		{
			vector2D::vec2D staticStart{ staticVtx[i] };
			vector2D::vec2D staticEnd{ staticVtx[(i + 1) % staticVtx.size()] };

			vector2D::vec2D edge{ staticEnd - staticStart };
			vector2D::vec2D projAxis{-edge.y, edge.x};					//clockwise normal
			
			staticMinMax = projectVtx(staticVtx, projAxis) ;
			kineticMinMax = projectVtx(kineticVtx, projAxis) ;

			//if (!(kineticMinMax.y >= staticMinMax.x && staticMinMax.y >= kineticMinMax.x))
			if (staticMinMax.x >= kineticMinMax.y || kineticMinMax.x >= staticMinMax.y)
				return false;
		}

		for (size_t i{ 0 }; i < kineticVtx.size(); ++i)
		{
			vector2D::vec2D kineticStart{ kineticVtx[i] };
			vector2D::vec2D kineticEnd{ kineticVtx[(i + 1) % kineticVtx.size()] };

			vector2D::vec2D edge{ kineticEnd - kineticStart };
			vector2D::vec2D projAxis{ -edge.y, edge.x };				//clockwise normal

			staticMinMax = projectVtx(staticVtx, projAxis);
			kineticMinMax = projectVtx(kineticVtx, projAxis);

			if (staticMinMax.x >= kineticMinMax.y || kineticMinMax.x >= staticMinMax.y)
				return false;
		}

		//std::cout << "--------------------\nthis is static vtx: \n"
		//	<< staticVtx[0].x << " " << staticVtx[0].y << std::endl
		//	<< staticVtx[1].x << " " << staticVtx[1].y << std::endl
		//	<< staticVtx[2].x << " " << staticVtx[2].y << std::endl
		//	<< staticVtx[3].x << " " << staticVtx[3].y << std::endl;
		//std::cout << "--------------------\nthis is kinetic vtx: \n"
		//	<< kineticVtx[0].x << " " << kineticVtx[0].y << std::endl
		//	<< kineticVtx[1].x << " " << kineticVtx[1].y << std::endl
		//	<< kineticVtx[2].x << " " << kineticVtx[2].y << std::endl
		//	<< kineticVtx[3].x << " " << kineticVtx[3].y << std::endl;

		//std::cout << "there is polygon polygon collision detection\n";
		return true;
	}

	bool CollisionPushPolygonPolygon(std::vector < vector2D::vec2D> staticVtx, std::vector < vector2D::vec2D> kineticVtx, 
										vector2D::vec2D& norm, float &depth)
	{

		vector2D::vec2D staticMinMax{ 0.f, 0.f }, kineticMinMax{ 0.f, 0.f };
		for (size_t i{ 0 }; i < staticVtx.size(); ++i)
		{
			vector2D::vec2D staticStart{ staticVtx[i] };
			vector2D::vec2D staticEnd{ staticVtx[(i + 1) % staticVtx.size()] };

			vector2D::vec2D edge{ staticEnd - staticStart };
			vector2D::vec2D projAxis{ -edge.y, edge.x };					//clockwise normal
			//vector2D::Vector2DNormalize(projAxis, projAxis);

			staticMinMax = projectVtx(staticVtx, projAxis);
			kineticMinMax = projectVtx(kineticVtx, projAxis);

			//if (!(kineticMinMax.y >= staticMinMax.x && staticMinMax.y >= kineticMinMax.x))
			// No collision
			if (staticMinMax.x >= kineticMinMax.y || kineticMinMax.x >= staticMinMax.y)
				return false;

			// There is collision, provide response
			float axisDepth = std::min(kineticMinMax.y - staticMinMax.x, staticMinMax.y - kineticMinMax.x);
			if (axisDepth < depth)
			{
				depth = axisDepth;
				norm = projAxis;
			}
		}

		for (size_t i{ 0 }; i < kineticVtx.size(); ++i)
		{
			vector2D::vec2D kineticStart{ kineticVtx[i] };
			vector2D::vec2D kineticEnd{ kineticVtx[(i + 1) % kineticVtx.size()] };

			vector2D::vec2D edge{ kineticEnd - kineticStart };
			vector2D::vec2D projAxis{ -edge.y, edge.x };				//clockwise normal
			//vector2D::Vector2DNormalize(projAxis, projAxis);

			staticMinMax = projectVtx(staticVtx, projAxis);
			kineticMinMax = projectVtx(kineticVtx, projAxis);

			// No collision
			if (staticMinMax.x >= kineticMinMax.y || kineticMinMax.x >= staticMinMax.y)
				return false;

			// There is collision, provide response
			float axisDepth = std::min(kineticMinMax.y - staticMinMax.x, staticMinMax.y - kineticMinMax.x);
			if (axisDepth < depth)
			{
				depth = axisDepth;
				norm = projAxis;
			}
		}

		depth /= vector2D::Vector2DLength(norm);
		vector2D::Vector2DNormalize(norm, norm);

		vector2D::vec2D staticCenter{ meanOfVertices(staticVtx) };
		vector2D::vec2D kineticCenter{ meanOfVertices(kineticVtx) };

		vector2D::vec2D direction{ staticCenter - kineticCenter };

		if (vector2D::Vector2DDotProduct(direction, norm) < 0.f)
			norm = -norm;

		std::cout << "there is polygon polygon collision and push response\n";
		return true;
	}

	bool CollisionBlockPolygonPolygon(std::vector < vector2D::vec2D> staticVtx, std::vector < vector2D::vec2D> kineticVtx,
		vector2D::vec2D& norm, float& depth)
	{
		vector2D::vec2D staticMinMax{ 0.f, 0.f }, kineticMinMax{ 0.f, 0.f };
		for (size_t i{ 0 }; i < staticVtx.size(); ++i)
		{
			vector2D::vec2D staticStart{ staticVtx[i] };
			vector2D::vec2D staticEnd{ staticVtx[(i + 1) % staticVtx.size()] };

			vector2D::vec2D edge{ staticEnd - staticStart };
			vector2D::vec2D projAxis{ -edge.y, edge.x };					//clockwise normal
			vector2D::Vector2DNormalize(projAxis, projAxis);

			staticMinMax = projectVtx(staticVtx, projAxis);
			kineticMinMax = projectVtx(kineticVtx, projAxis);

			//if (!(kineticMinMax.y >= staticMinMax.x && staticMinMax.y >= kineticMinMax.x))
			// No collision
			if (staticMinMax.x >= kineticMinMax.y || kineticMinMax.x >= staticMinMax.y)
				return false;

			// There is collision, provide response
			float axisDepth = std::min(kineticMinMax.y - staticMinMax.x, staticMinMax.y - kineticMinMax.x);
			if (axisDepth < depth)
			{
				depth = axisDepth;
				norm = projAxis;
			}
		}

		for (size_t i{ 0 }; i < kineticVtx.size(); ++i)
		{
			vector2D::vec2D kineticStart{ kineticVtx[i] };
			vector2D::vec2D kineticEnd{ kineticVtx[(i + 1) % kineticVtx.size()] };

			vector2D::vec2D edge{ kineticEnd - kineticStart };
			vector2D::vec2D projAxis{ -edge.y, edge.x };				//clockwise normal
			vector2D::Vector2DNormalize(projAxis, projAxis);

			staticMinMax = projectVtx(staticVtx, projAxis);
			kineticMinMax = projectVtx(kineticVtx, projAxis);

			// No collision
			if (staticMinMax.x >= kineticMinMax.y || kineticMinMax.x >= staticMinMax.y)
				return false;

			// There is collision, provide response
			float axisDepth = std::min(kineticMinMax.y - staticMinMax.x, staticMinMax.y - kineticMinMax.x);
			if (axisDepth < depth)
			{
				depth = axisDepth;
				norm = projAxis;
			}
		}

		depth /= vector2D::Vector2DLength(norm);
		vector2D::Vector2DNormalize(norm, norm);

		vector2D::vec2D staticCenter{ meanOfVertices(staticVtx) };
		vector2D::vec2D kineticCenter{ meanOfVertices(kineticVtx) };

		vector2D::vec2D direction{ staticCenter - kineticCenter };

		if (vector2D::Vector2DDotProduct(direction, norm) < 0.f)
			norm = -norm;

		std::cout << "there is polygon polygon collision and block response\n";
		return true;
	}

	bool CollisionDetectionCirclePolygon(vector2D::vec2D circleCenter, float rad, std::vector < vector2D::vec2D> boxVtx)
	{
		vector2D::vec2D boxMinMax{ 0.f, 0.f }, circleMinMax{ 0.f, 0.f }, projAxis{ 0.f, 0.f };
		float axisDepth{ 0.f };

		for (size_t i{ 0 }; i < boxVtx.size(); ++i)
		{
			vector2D::vec2D boxStart{ boxVtx[i] };
			vector2D::vec2D boxEnd{ boxVtx[(i + 1) % boxVtx.size()] };

			vector2D::vec2D edge{ boxEnd - boxStart };
			projAxis = vector2D::vec2D(-edge.y, edge.x);					//clockwise normal
			vector2D::Vector2DNormalize(projAxis, projAxis);

			boxMinMax = projectVtx(boxVtx, projAxis);
			circleMinMax = projectCircle(circleCenter, rad, projAxis);

			// No collision
			if (boxMinMax.x >= circleMinMax.y || circleMinMax.x >= boxMinMax.y)
				return false;
		}

		int closestPtIdx{ closestPointOfCircleToPolygon(circleCenter, boxVtx) };
		vector2D::vec2D closestPt{ boxVtx[closestPtIdx] };

		projAxis = closestPt - circleCenter;

		boxMinMax = projectVtx(boxVtx, projAxis);
		circleMinMax = projectCircle(circleCenter, rad, projAxis);

		// No collision
		if (boxMinMax.x >= circleMinMax.y || circleMinMax.x >= boxMinMax.y)
			return false;

		std::cout << "there is circle polygon collision detection\n";

		return true;
	}

	bool CollisionBlockCirclePolygon(vector2D::vec2D circleCenter, float rad, std::vector < vector2D::vec2D> boxVtx,
		vector2D::vec2D& norm, float& depth)
	{
		vector2D::vec2D boxMinMax{ 0.f, 0.f }, circleMinMax{ 0.f, 0.f }, projAxis{ 0.f, 0.f };
		float axisDepth{ 0.f };

		for (size_t i{ 0 }; i < boxVtx.size(); ++i)
		{
			vector2D::vec2D boxStart{ boxVtx[i] };
			vector2D::vec2D boxEnd{ boxVtx[(i + 1) % boxVtx.size()] };

			vector2D::vec2D edge{ boxEnd - boxStart };
			projAxis = vector2D::vec2D (-edge.y, edge.x);					//clockwise normal
			vector2D::Vector2DNormalize(projAxis, projAxis);

			boxMinMax = projectVtx(boxVtx, projAxis);
			circleMinMax = projectCircle(circleCenter, rad, projAxis);

			// No collision
			if (boxMinMax.x >= circleMinMax.y || circleMinMax.x >= boxMinMax.y)
				return false;

			// There is collision, provide response
			axisDepth = std::min(circleMinMax.y - boxMinMax.x, boxMinMax.y - circleMinMax.x);
			if (axisDepth < depth)
			{
				depth = axisDepth;
				norm = projAxis;
			}
		}

		int closestPtIdx{ closestPointOfCircleToPolygon(circleCenter, boxVtx) };
		vector2D::vec2D closestPt{ boxVtx[closestPtIdx] };

		projAxis = closestPt - circleCenter;

		boxMinMax = projectVtx(boxVtx, projAxis);
		circleMinMax = projectCircle(circleCenter, rad, projAxis);

		// No collision
		if (boxMinMax.x >= circleMinMax.y || circleMinMax.x >= boxMinMax.y)
			return false;

		// There is collision, provide response
		axisDepth = std::min(circleMinMax.y - boxMinMax.x, boxMinMax.y - circleMinMax.x);
		if (axisDepth < depth)
		{
			depth = axisDepth;
			norm = projAxis;
		}

		depth /= vector2D::Vector2DLength(norm);
		vector2D::Vector2DNormalize(norm, norm);

		vector2D::vec2D boxCenter{ meanOfVertices(boxVtx) };

		vector2D::vec2D direction{ boxCenter - circleCenter };

		if (vector2D::Vector2DDotProduct(direction, norm) < 0.f)
			norm = -norm;

		std::cout << "there is circle polygon collision\n";

		return true;
	}

	int closestPointOfCircleToPolygon(vector2D::vec2D circleCenter, std::vector<vector2D::vec2D> boxVtx)
	{
		int closestPoint { 0 }; //the vector index
		float minDist{ FLT_MAX };

		for (int i{ 0 }; i < boxVtx.size(); ++i)
		{
			//vector2D::vec2D vtx{  };
			float dist{ vector2D::Vector2DDistance(boxVtx[i], circleCenter) };
			if (dist < minDist)
			{
				minDist = dist;
				closestPoint = i;
			}
		}
		return closestPoint;
	}

	vector2D::vec2D projectVtx(std::vector<vector2D::vec2D> const& vtx, vector2D::vec2D const& axis)
	{
		vector2D::vec2D minMax{ FLT_MAX , -FLT_MAX };

		for (size_t i{ 0 }; i < vtx.size(); ++i)
		{
			float length{ vector2D::Vector2DDotProduct(axis, vtx[i]) };
			minMax.x = std::min(minMax.x, length);
			minMax.y = std::max(minMax.y, length);
		}
		return minMax;
	}

	vector2D::vec2D projectCircle(vector2D::vec2D circleCenter, float rad, vector2D::vec2D projAxis)
	{
		vector2D::vec2D dir;
		vector2D::Vector2DNormalize(dir, projAxis);
		vector2D::vec2D radVec(dir * rad); //scale the directional vec by the rad to get a point on the circle

		vector2D::vec2D point1{ circleCenter + radVec };
		vector2D::vec2D point2{ circleCenter - radVec };

		vector2D::vec2D circleMinMax{ vector2D::Vector2DDotProduct(point1, projAxis), vector2D::Vector2DDotProduct(point2, projAxis) };

		if (circleMinMax.x > circleMinMax.y)
		{
			float tmp{ circleMinMax.x };
			circleMinMax.x = circleMinMax.y;
			circleMinMax.y = tmp;
		}

		return circleMinMax;
	}

	vector2D::vec2D meanOfVertices(std::vector<vector2D::vec2D> vtx)
	{
		vector2D::vec2D sum{ 0.f, 0.f };

		for (int i{ 0 }; i < vtx.size(); ++i)
			sum += vtx[i];
		return (sum / (float)vtx.size());
	}

}