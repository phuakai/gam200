#include "physics.h"
#define _USE_MATH_DEFINES //for M_PI
#include <iostream>
#include <math.h>
#include <input.h>
#include <iostream>

//void movement(GLApp::GLObject& obj, GLApp::GLObject const& cam, bool const& stepByStep)
vector2D::vec2D movement (vector2D::vec2D &pos, float &speed)
{
	float dt = GLHelper::delta_time;

	//if (GLHelper::mousestateLeft)
	//{
	//	GLHelper::mousestateLeft = false;
	//	double destX, destY;

	//	// Get mouse pos in world
	//	Graphics::Input::getCursorPos(&destX, &destY);
	//	obj.untravelledDistance.first = vector2D::vec2D{ static_cast<float>(destX) , static_cast<float>(destY) }; // travel destination

	//	obj.directionVec = obj.untravelledDistance.first - obj.modelCenterPos;					// travel direction
	//	obj.untravelledDistance.second = vector2D::Vector2DLength(obj.directionVec);			// total travel distance
	//	vector2D::Vector2DNormalize(obj.directionVec, obj.directionVec);						// normalize travel direction
	//}

	//if (obj.untravelledDistance.second > 0.f)
	//{
	//	vector2D::vec2D velocity{ obj.directionVec * obj.speed * dt};
	//	//std::cout << "direction vec in phy: " << vecTravelled.x << " " << vecTravelled.y << std::endl;

	//	float distanceTravelled{ vector2D::Vector2DLength(velocity) };						// distance travelled this frame
	//	
	//	obj.modelCenterPos = distanceTravelled < obj.untravelledDistance.second ? obj.modelCenterPos + velocity : lerpVec(obj.modelCenterPos, obj.untravelledDistance.first, obj.untravelledDistance.second/ distanceTravelled);
	//	obj.untravelledDistance.second = distanceTravelled < obj.untravelledDistance.second ? obj.untravelledDistance.second - distanceTravelled : 0.f;
	//}

	vector2D::vec2D keyboardVel{0.f, 0.f};
	if (GLHelper::keystateI == true) //up
	{
		keyboardVel.y += speed;
		//obj.untravelledDistance.second = 0.f;
		//if (stepByStep)
			//GLHelper::keystateI = false;
	}
	
	if (GLHelper::keystateK == true) //down
	{
		keyboardVel.y -= speed;
		//obj.untravelledDistance.second = 0.f;
		//if (stepByStep)
			//GLHelper::keystateK = false;
	}

	if (GLHelper::keystateJ == true) //left
	{
		keyboardVel.x -= speed;
		//obj.untravelledDistance.second = 0.f;
		//if (stepByStep)
			//GLHelper::keystateJ = false;
	}

	if (GLHelper::keystateL == true) //right
	{
		keyboardVel.x += speed;
		//obj.untravelledDistance.second = 0.f;
		//if (stepByStep)
			//GLHelper::keystateL = false;
	}

	if (keyboardVel.x != 0.f || keyboardVel.y != 0.f)
	{
		vector2D::Vector2DNormalize(keyboardVel, keyboardVel);
		keyboardVel *= dt * speed;
	}
	return keyboardVel;
}

//void move(vector2D::vec2D &vel, vector2D::Point2D &pos)
//{
//	//std::cout << "this is vel: " << vel.x << " " << vel.y << std::endl;
//	pos += vel;
//}


