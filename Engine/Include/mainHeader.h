
#pragma once

#include <vector>
#include <string>
#include "vec2D.h"
#include "vec3D.h"

#define MAX_GRID_X 25
#define MAX_GRID_Y 25
#define WALL 255

struct Render // Sprite
{
	std::string name;
	std::string type;					// still need this? Thought it is all squares?
	vector2D::vec2D position;

	vector3D::vec3D color;

	vector2D::vec2D dimension;

	unsigned int vaoID;
	unsigned int vboID;
	unsigned int eboID;
	std::string shaderName;
	bool render;
};

struct Texture
{
	int textureID;
	int spriteStep;
	int numberOfSprites;
	std::string textureName;
};

struct Movement
{
	vector2D::vec2D velocity;
	vector2D::vec2D target;
	float force;
	float speed;
	bool collisionFlag;
	vector2D::vec2D collisionResponse;
};

struct Stats {
	int health;
};

void engineInit();
void engineUpdate();
void engineDraw();
void engineFree();
void swapBuffer();
