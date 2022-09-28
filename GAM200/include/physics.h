

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
