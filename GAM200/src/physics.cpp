#include "physics.h"
#define _USE_MATH_DEFINES //for M_PI
#include <iostream>
#include <math.h>
#include <input.h>
#include <iostream>

void movement(GLApp::GLObject& obj, GLApp::GLObject const& cam, bool const& stepByStep)
{

	float dt = GLHelper::delta_time;

	if (GLHelper::mousestateLeft)
{
		GLHelper::mousestateLeft = false;
		double destX, destY;

		// Get mouse pos in world
		Graphics::Input::getCursorPos(&destX, &destY);
		obj.untravelledDistance.first = vector2D::vec2D{ static_cast<float>(destX) , static_cast<float>(destY) }; // travel destination

		obj.directionVec = obj.untravelledDistance.first - obj.modelCenterPos;					// travel direction
		obj.untravelledDistance.second = vector2D::Vector2DLength(obj.directionVec);			// total travel distance
		vector2D::Vector2DNormalize(obj.directionVec, obj.directionVec);						// normalize travel direction
	}

	if (obj.untravelledDistance.second > 0.f)
	{
		vector2D::vec2D vecTravelled{ obj.directionVec * dt * 200.f };

		float distanceTravelled{ vector2D::Vector2DLength(vecTravelled) };						// distance travelled this frame
		
		obj.modelCenterPos = distanceTravelled < obj.untravelledDistance.second ? obj.modelCenterPos + vecTravelled : lerpVec(obj.modelCenterPos, obj.untravelledDistance.first, obj.untravelledDistance.second/ distanceTravelled);
		obj.untravelledDistance.second = distanceTravelled < obj.untravelledDistance.second ? obj.untravelledDistance.second - distanceTravelled : 0.f;
	}

	if (GLHelper::keystateI == true) //up
	{
		obj.modelCenterPos.y += obj.vel.y;
		obj.untravelledDistance.second = 0.f;
		if (stepByStep)
			GLHelper::keystateI = false;
	}
	
	if (GLHelper::keystateK == true) //down
	{
		obj.modelCenterPos.y -= obj.vel.y;
		obj.untravelledDistance.second = 0.f;
		if (stepByStep)
			GLHelper::keystateK = false;
	}

	if (GLHelper::keystateJ == true) //left
	{
		obj.modelCenterPos.x -= obj.vel.x;
		obj.untravelledDistance.second = 0.f;
		if (stepByStep)
			GLHelper::keystateJ = false;
	}

	if (GLHelper::keystateL == true) //right
	{
		obj.modelCenterPos.x += obj.vel.x;
		obj.untravelledDistance.second = 0.f;
		if (stepByStep)
			GLHelper::keystateL = false;
	}

	//if (GLHelper::mousestateLeft)
	//{
	//	GLHelper::mousestateLeft = false;
	//	//vector2D::Point2D *dest{};
	//	double* glfwDestX = new(double), * glfwDestY = new(double);
	//	glfwGetCursorPos(GLHelper::ptr_window, glfwDestX, glfwDestY);
	//	
	//	//std::cout << "this is my mouse pos: " << *glfwDestX << " " << *glfwDestY << std::endl;

	//	/*******************************
	//		Get coords of window corners
	//		in world coords
	//	*******************************/
	//	int windowWidth{ GLHelper::width }, windowHeight{ GLHelper::height };
	//	//std::cout << "this is win width and height: " << windowWidth << " " << windowHeight << std::endl;
	//	float halfWindowWidth{ static_cast<float>(windowWidth) / 2 }, halfWindowHeight{ static_cast<float>(windowHeight) / 2 };

	//	/*******************************
	//		Map mouse pos from glfw 
	//		coord sys to world coord sys
	//	*******************************/

	//	vector2D::Point2D convertedWorldDest;

	//	matrix3x3::mat3x3 ndcVpXform { halfWindowWidth, 0.f, halfWindowWidth,
	//									0.f, halfWindowHeight, halfWindowHeight,
	//									0.f, 0.f, 1.f};
	//	matrix3x3::mat3x3 VpToNdcXform;
	//	matrix3x3::Mtx33Inverse(&VpToNdcXform, ndcVpXform);

	//	matrix3x3::mat3x3 NdcToWorldXform;
	//	matrix3x3::mat3x3 worldToNdcXform{ obj.world_to_ndc_xform[0][0], obj.world_to_ndc_xform[1][0], obj.world_to_ndc_xform[2][0],
	//										obj.world_to_ndc_xform[0][1], obj.world_to_ndc_xform[1][1], obj.world_to_ndc_xform[2][1], 
	//										obj.world_to_ndc_xform[0][2], obj.world_to_ndc_xform[1][2], obj.world_to_ndc_xform[2][2] };
	//	//matrix3x3::mat3x3 NdcToCamwinXform;
	//	//matrix3x3::mat3x3 camwinToNdcXform{ cam., obj.world_to_ndc_xform[1][0], obj.world_to_ndc_xform[2][0],
	//	//									cam.camwin_to_ndc_xform[0][1], obj.world_to_ndc_xform[1][1], obj.world_to_ndc_xform[2][1], 
	//	//									cam.camwin_to_ndc_xform[0][2], obj.world_to_ndc_xform[1][2], obj.world_to_ndc_xform[2][2] };
	//	matrix3x3::Mtx33Inverse(&NdcToWorldXform, worldToNdcXform);

	//	convertedWorldDest = NdcToWorldXform * (VpToNdcXform * vector2D::Point2D(*glfwDestX, *glfwDestY));

	//	vector2D::Point2D worldDest;
	//	//std::cout << "bef translation: " << obj.modelCenterPos.x << " " << obj.modelCenterPos.y << std::endl;
	//	std::cout << "mouse input in world: " << convertedWorldDest.x << " " << convertedWorldDest.y << std::endl;
	//	worldDest.x = cam.worldCenterPos.x - halfWindowWidth + convertedWorldDest.x;
	//	worldDest.y = cam.worldCenterPos.y - halfWindowHeight + convertedWorldDest.y;

	//	obj.modelCenterPos.x = convertedWorldDest.x;
	//	obj.modelCenterPos.y = convertedWorldDest.y;
	//	//std::cout << "aft translation: " << obj.modelCenterPos.x << " " << obj.modelCenterPos.y << std::endl;


	//	//obj.worldCenterPos += obj.vel;

	//	//for (size_t i{ 0 }; i < obj.worldVertices.size(); ++i)
	//	//{
	//	//	obj.worldVertices;
	//	//}
	//	delete glfwDestX;
	//	delete glfwDestY;
	//}


}
