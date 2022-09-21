#pragma once

#include <vec2D.h>
#include <string>
#include <vector>

//#if false
enum class ShapeType
{
	circle = 0, 
	box
};

class physicsRigidBody
{
private:
	vector2D::vec2D pos;
	vector2D::vec2D linearVel;
	float rot;
	float rotVel;

	float density;
	float mass;
	float restitution;				//how bouncy
	float area;
	
	bool isStatic;					//will not be affected by physics

	float radius;					//for circle
	float width;					//for box
	float height;					//for box
	float halfWidth;				//for box
	float halfHeight;				//for box

	ShapeType shapeType;
	
	std::vector <vector2D::vec2D> vertices;
	std::vector <int> tri;
	std::vector <vector2D::vec2D> transformedVertices;
	bool plsUpdateTfm;				//change to true whenever there is rot/scale/trans

public:
	/******************************
	* Constructors
	******************************/
	physicsRigidBody();
	physicsRigidBody(vector2D::vec2D Pos, vector2D::vec2D LinearVel, float Rot, float RotVel, float Density, float Mass,
	float Restituition, float Area, bool IsStatic, float Radius, float Width, float Height);
	//physicsRigidBody() :pos{ 0.f, 0.f }, linearVel{ 0.f, 0.f }, rot{ 0.f }, rotVel{ 0.f }, density{ 0.f },
	//	mass{ 0.f }, restitution{ 0.f }, area{ 0.f }, isStatic{ false }, radius{ 0.f }, width{ 0.f }, height{ 0.f }, shapeType{ shapeType::box }{}
	
	/******************************
	* Deconstructors
	******************************/
	~physicsRigidBody();

	/******************************
	* Creator
	******************************/
	static bool createCircleBody(float rad, vector2D::vec2D pos, float density, bool isStatic, float restituition, physicsRigidBody *body, std::string errMsg);
	static bool createBoxBody(float width, float height, vector2D::vec2D pos, float density, bool isStatic, float restituition, physicsRigidBody *body, std::string errMsg);

	/******************************
	* Getters
	******************************/
	vector2D::vec2D getPos();
	float getRad();
	float getWidth();
	float getHeight();
	float getHalfWidth();
	float getHalfHeight();
	ShapeType getShape();
	std::vector<vector2D::vec2D> getTfmVtx();
	bool getPlsUpdateTfm();

	/******************************
	* Setters
	******************************/
	void setPos(vector2D::Point2D position);
	void setRad(float rad);

	/******************************
	* Transformations
	******************************/
	void transformVertices();									// Transform
	void move(vector2D::vec2D& vel);							// Translate
	void rotate(float angle);									// Rotate
	//void setTransformRequired(bool transform);				


	//{
	//	pos = Pos;
	//	linearVel = LinearVel;
	//	rot = Rot;
	//	rotVel = RotVel;
	//	density = Density;
	//	mass = Mass;
	//	restitution = Restituition;
	//	area = Area;
	//	isStatic = IsStatic;
	//	radius = Radius;
	//	width = Width;
	//	height = Height;
	//}

};
//#endif

