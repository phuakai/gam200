#include "physics.h"
#define _USE_MATH_DEFINES //for M_PI
#include <iostream>
#include <math.h>
#include <input.h>
#include <iostream>


vector2D::vec2D mouseMovement(vector2D::vec2D& src, vector2D::vec2D const& dest, float& speed)
{
	float dt = GLHelper::delta_time;

	static vector2D::vec2D destination{ 0.f, 0.f };
	static vector2D::vec2D mouseVel{ 0.f, 0.f };

	if (GLHelper::mousestateLeft)
	{
		vector2D::Vector2DNormalize(mouseVel, dest - src);
		destination = dest;
	}

	else if (mouseVel.x != 0.f && mouseVel.y != 0.f)
	{
		vector2D::Vector2DNormalize(mouseVel, destination - src); //normalized directional vec
		mouseVel *= dt * speed;
		vector2D::vec2D pos{ src };
		pos += mouseVel;
		if (vector2D::Vector2DDistance(destination, pos) <= vector2D::Vector2DLength(mouseVel)) //position has passed dest
		{

			src = destination;
			mouseVel = vector2D::vec2D(0.f, 0.f);
			return mouseVel;
		}
	}

	//std::cout << mouseVel.x << mouseVel.y << std::endl;
	return mouseVel;
}

vector2D::vec2D keyboardMovement (vector2D::vec2D &pos, float &speed)
{
	float dt = GLHelper::delta_time;

	vector2D::vec2D keyboardVel{0.f, 0.f};
	if (GLHelper::keystateI == true) //up
		keyboardVel.y += speed;
	
	if (GLHelper::keystateK == true) //down
		keyboardVel.y -= speed;

	if (GLHelper::keystateJ == true) //left
		keyboardVel.x -= speed;

	if (GLHelper::keystateL == true) //right
		keyboardVel.x += speed;

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


