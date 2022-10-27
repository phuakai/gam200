/* !
@file    app.h
@author  a.weiren@digipen.edu, ruoyan.go@digipen.edu, p.jiankai@digipen.edu
@date    10/08/2022

This file contains the declaration of namespace GLApp that encapsulates the
functionality required to implement an OpenGL application including
compiling, linking, and validating shader programs
setting up geometry and index buffers,
configuring VAO to present the buffered geometry and index data to
vertex shaders,
configuring textures (in later labs),
configuring cameras (in later labs),
and transformations (in later labs).
*//*__________________________________________________________________________*/

#ifndef APP_H
#define APP_H

/*                                                             
----------------------------------------------------------------------------- */
#include <input.h>
#include <glslshader.h>
#include <list> 
#include <random>
#include <map>
#include <unordered_map>
#include <vec2D.h>
#include <mat3x3.h>
#include <graphics.h>
#include <model.h>
#include <physicsRigidBody.h>


struct GLApp {

	static void init();
	static void update();
	static void draw();
	static void cleanup();


  //added for physics testing
  enum class collisionType
  {
	NIL = 0,					//0
	CircleDetection,			//1
	CirclePushResolution,		//2
	CircleBlockResolution,		//3
	PolygonDetection,			//4
	PolygonPushResolution,		//5
	PolygonBlockResolution,		//6
	PolygonCircleDetection,		//7
	PolygonCircleResolution		//8
  };

  static collisionType currentCollision;
  static bool movableShape;
  static std::unordered_map<collisionType, std::string> collisionInfo;

  struct GLObject {
		vector2D::vec2D orientation{};													// orientation x is angle disp, orientation y is angle speed specified in degrees
		vector2D::vec2D scaling{};														// scaling parameters
		vector2D::vec2D modelCenterPos{};													// center of shape coordinates
		float speed{};																	// speed
		physicsRigidBody body;															// param for collision
		int objId;																		// For collision debugger
		int texId;
		int totalsprites;

		matrix3x3::mat3x3 mdl_to_ndc_xform{}; // model to ndc transformation
		matrix3x3::mat3x3 mdl_to_world_xform{}; // model to world transformation
		matrix3x3::mat3x3 world_to_ndc_xform{}; // world to ndc transformation

		vector2D::vec2D ndcposition{}; // translation vector coordinates
		
		std::vector <vector2D::vec2D> controlworldpos;
		std::vector <vector2D::vec2D> controlndcpos;

		std::vector <vector2D::vec2D> ndc_coords;

		//added for physics testing
		bool overlap{ false };														// flag for overlap
		//added for physics testing (collision response)
		//glm::mat3 worldToMdlXform{};

		std::map<std::string, ModelNS::Model>::iterator mdl_ref{};
		std::map<std::string, GLSLShader>::iterator shd_ref{};

		//added for physics testing
		vector3D::vec3D color{};
		matrix3x3::mat3x3 mdlXform{};															// model to world transformation
		vector2D::vec2D worldCenterPos{};
		std::vector<vector2D::vec2D> boundingBoxWorldVertices;
		std::vector<vector2D::vec2D> worldVertices;												// vertices coordinates
		std::vector<vector2D::vec2D> modelVertices;												// vertices coordinates

		void init();
		// function to render object's model (specified by index mdl_ref)
		// uses model transformation matrix mdl_to_ndc_xform matrix
		// and shader program specified by index shd_ref ...
		void draw() const;
		// function to update the object's model transformation matrix
		void update(GLdouble delta_time);

		static void gimmeObject(std::string modelname, std::string objname, vector2D::vec2D scale, vector2D::vec2D pos, vector3D::vec3D colour, int id = 0, int texid = 0, int totalsprite = 1); // Temp
	};


	static void entitydraw();
	//static void init_models_cont();
	using VPSS = std::vector<std::pair<std::string, std::string>>;

	// function to insert shader program into container GLApp::shdrpgms ...
	static void insert_shdrpgm(std::string, std::string, std::string);
	// function to parse scene file ...
	static void init_scene(std::string);

	static std::map<std::string, GLObject> objects; // singleton

	static std::map<std::string, GLSLShader> shdrpgms; // singleton

	static bool modulate;
	static bool alphablend;
	static bool textures;
	static bool coldebug;
	static bool velocitydirectiondebug;
	static bool graphicsmode;

	static int objectcounter;
};

#endif /* APP_H */
