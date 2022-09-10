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

}
