#include <glapp.h>
#include <collisiondebug.h>
#include <transform.h>
#include <mat3x3.h>
#include <vec2D.h>
#include <iostream>

void collisionDebug(GLApp::GLObject& object)
{
	//std::cout << "Vertice size " << object.body.getTfmVtx().size() << std::endl;
	std::vector<vector2D::vec2D> tmp = object.body.getTfmVtx();

	matrix3x3::mat3x3 worldtondc = transform.getWorldtoNDC();
	float width{0}, height{0};

	for (int i = 0; i < object.body.getTfmVtx().size(); i++)
	{
		
		//std::cout << "Vertices " << tmp[i].x << ", " << tmp[i].y << std::endl;
		vector2D::Vec2 newndc = worldtondc * tmp[i];
		width = object.body.getWidth();
		height = object.body.getHeight();

	}
	//std::cout << "Width & Height " << width << ", " << height << std::endl;
	GLApp::GLObject::gimmeObject("square", "atest", vector2D::vec2D(width + 10.f, height + 10.f), vector2D::vec2D(object.modelCenterPos.x, object.modelCenterPos.y), vector3D::vec3D(0.0f, 1.0f, 0.0f), false);
	
}