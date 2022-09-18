

#ifndef PHYSICS_H
#define PHYSICS_H

#pragma once
#include "glapp.h"
#include "vec2D.h"
#include "mat3x3.h"


vector2D::vec2D movement(vector2D::vec2D& pos, float& speed, bool stepByStep);
//vector2D::vec2D movement(vector2D::vec2D& pos, float& speed);

//void move(vector2D::vec2D &vel, vector2D::Point2D &pos);

#endif
