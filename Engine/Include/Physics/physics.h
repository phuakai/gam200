
/* Start Header ************************************************************************/
/*!
\file		physics.h
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Sep 23, 2022
\brief		This file contains the function declarations and structs
			used by physics.cpp

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef PHYSICS_H
#define PHYSICS_H

#pragma once
#include "app.h"
#include "vec2D.h"
#include "mat3x3.h"

/******************************************************************************/
/*!
	This helper function implements movement for collision testing purposes
*/
/******************************************************************************/
vector2D::vec2D mouseMovement(vector2D::vec2D& src, vector2D::vec2D const& dest, float& speed);

//vector2D::vec2D keyboardMovement(vector2D::vec2D& pos, float& speed);

//vector2D::vec2D movement(vector2D::vec2D& pos, float& speed);

//void move(vector2D::vec2D &vel, vector2D::Point2D &pos);

#endif
