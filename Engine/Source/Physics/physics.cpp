/* Start Header ************************************************************************/
/*!
\file		physics.cpp
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Sep 23, 2022
\brief		This file contains the function definitions for mouse movement 

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "physics.h"
#include "eventManager.h"
#include "pathfinding.h"
#include "ECS.h"
#define _USE_MATH_DEFINES //for M_PI
#include <iostream>
#include <math.h>
#include <input.h>
#include <iostream>
#include <chrono>

extern EventManager eventManager;
extern std::vector<FormationManager> formationManagers;

//auto loopStart = std::chrono::system_clock::now();
//auto loopEnd = std::chrono::system_clock::now();
//std::chrono::duration<float> diff = loopEnd - loopStart;
//std::chrono::seconds sec(1);
//float x = 0;
//float performanceNumber = 0;
//int totalFps = 0;

void physicsUpdate()
{
	//loopStart;
	std::vector<Event>& eventQueue = eventManager.findQueue(Systems::Physics);

	for (int i = 0; i < eventQueue.size(); ++i)
	{
		pathfindingCalculation(eventQueue[i].id);
	}

	while (eventQueue.size())
	{
		Event currentEvent = eventManager.dequeue(Systems::Physics);

		vector2D::vec2D oldPosition = ecs.GetComponent<BaseInfo>(currentEvent.id)->position;
		
		ecs.GetComponent<BaseInfo>(currentEvent.id)->position += ecs.GetComponent<Physics>(currentEvent.id)->velocity * (static_cast<float>(Graphics::Input::delta_time) > 1 / 60.f ? 1 / 60.f : static_cast<float>(Graphics::Input::delta_time)) * 100;
		
		mainTree.updatePoint(currentEvent.id, oldPosition, ecs.GetComponent<BaseInfo>(currentEvent.id)->position, mainTree);
	}
	//loopEnd;
	//std::cout << std::fixed << diff << std::endl;
	//
	//if (x != 1)
	//{
	//	while (x != 1)
	//	{
	//	totalFps++;
	//		x += sec.count(); //(float)Graphics::Input::delta_time;
	//	}
	//	//std::cout << totalFps << std::endl;
	//}
	//else if (x = 1)
	//{
	//	performanceNumber = (diff.count() * 100) / totalFps;

	//	//std::cout << std::fixed << performanceNumber << std::endl;
	//}
}

vector2D::vec2D mouseMovement(vector2D::vec2D& src, vector2D::vec2D const& dest, float& speed)
{
	float dt = static_cast<float>(Graphics::Input::delta_time);
	dt = dt > (1 / 60.f) ? (1 / 60.f) : dt;

	static vector2D::vec2D destination{ 0.f, 0.f };
	static vector2D::vec2D mouseVel{ 0.f, 0.f };

	if (Graphics::Input::mousestateLeft)
	{
		vector2D::Vector2DNormalize(mouseVel, dest - src);
		destination = dest;
		mouseVel *= dt * speed;
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

#if false
vector2D::vec2D keyboardMovement (vector2D::vec2D &pos, float &speed)
{
	float dt = Graphics::Input::delta_time;

	vector2D::vec2D keyboardVel{0.f, 0.f};
	//if (Graphics::Input::keystateI == true) //up
	//	keyboardVel.y += speed;
	//
	//if (Graphics::Input::keystateK == true) //down
	//	keyboardVel.y -= speed;

	//if (Graphics::Input::keystateJ == true) //left
	//	keyboardVel.x -= speed;

	//if (Graphics::Input::keystateL == true) //right
	//	keyboardVel.x += speed;

	if (keyboardVel.x != 0.f || keyboardVel.y != 0.f)
	{
		vector2D::Vector2DNormalize(keyboardVel, keyboardVel);
		keyboardVel *= dt * speed;
	}
	return keyboardVel;
}
#endif
//void move(vector2D::vec2D &vel, vector2D::Point2D &pos)
//{
//	//std::cout << "this is vel: " << vel.x << " " << vel.y << std::endl;
//	pos += vel;
//}


