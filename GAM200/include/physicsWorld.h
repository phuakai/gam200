/* Start Header ************************************************************************/
/*!
\file		physicsWorld.h
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Sep 23, 2022
\brief		This file contains the struct for the standard rigid body

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

class flatWorld
{
public:
	float minBodySize;// { 0.01f * 0.01f };
	float maxBodySize;// { 64.f * 64.f };

	float minDensity;// = 0.5f;
	float maxDensity;// = 21.4f;

	flatWorld();
	flatWorld(float minSize, float maxSize, float minDen, float maxDen);
};

static flatWorld world;