#include <glapp.h>
#include <collisiondebug.h>
#include <transform.h>
#include <mat3x3.h>
#include <vec2D.h>
#include <iostream>

void collisionDebug(GLApp::GLObject& object)
{
	std::cout << "Vertice size " << object.boundingBoxWorldVertices.size() << std::endl;
	//mdl_ref->second.posvtx_cnt, ndc_coords.data()
	matrix3x3::mat3x3 worldtondc = transform.getWorldtoNDC();
	//object.mdl_ref->second.posvtx_cnt += object.boundingBoxWorldVertices.size();
	//std::cout << "Pos vtx num " << object.mdl_ref->second.posvtx_cnt << std::endl;
	for (int i = 0; i < object.boundingBoxWorldVertices.size(); i++)
	{
		
		//std::cout << "Vertices " << object.boundingBoxWorldVertices[i].x << ", " << object.boundingBoxWorldVertices[i].y << std::endl;
		vector2D::Vec2 newndc = worldtondc * object.boundingBoxWorldVertices[i];
		//object.worldVertices.emplace_back(object.boundingBoxWorldVertices[i]);
		//object.ndc_coords.emplace_back(newndc);
	}
	//std::cout << "New additions " << object.boundingBoxWorldVertices.size() << std::endl;
	//std::cout << "Matrix " << std::endl
	//	<< tmp.m[0] << ", " << tmp.m[1] << ", " << tmp.m[2] << std::endl
	//	<< tmp.m[3] << ", " << tmp.m[4] << ", " << tmp.m[5] << std::endl
	//	<< tmp.m[6] << ", " << tmp.m[7] << ", " << tmp.m[8] << std::endl << std::endl;
}