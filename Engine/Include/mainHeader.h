
#pragma once

#include <vector>
#include <string>
#include "vec2D.h"
#include "vec3D.h"
#include "mat3x3.h"
#include <rttr/type>

#define MAX_GRID_X 25
#define MAX_GRID_Y 25
#define WALL 255

struct Render // Sprite
{
	std::string type;					// still need this? Thought it is all squares?

	vector3D::vec3D color{ 0 , 0 , 0 };

	unsigned int vaoID;
	unsigned int vboID;
	unsigned int eboID;
	std::string shaderName;
	bool render;
};

struct BaseInfo
{
	std::string name;
	vector2D::vec2D position{ 0 , 0 };
	vector2D::vec2D dimension{ 0 , 0 };
};

struct Texture
{
	int textureID;
	int spriteStep;
	int numberOfSprites;
	std::string textureName;
};

struct Physics
{
	vector2D::vec2D velocity;
	vector2D::vec2D target;
	vector2D::vec2D formationTarget;
	float force;
	float speed;
	bool collisionFlag;
	vector2D::vec2D collisionResponse;
	float radius;
	bool reached;
	int formationManagerID;
};

struct Stats {
	//int health;
	int getHealth() const;
	void setHealth(const int h);
	RTTR_ENABLE()
private:
	int health;
};

void engineInit();
void engineUpdate();
void engineDraw();
void engineFree();
void swapBuffer();
