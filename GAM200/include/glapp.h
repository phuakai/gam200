/* !
@file    glapp.h
@author  pghali@digipen.edu, a.weiren@digipen.edu
@date    10/06/2022

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

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLAPP_H
#define GLAPP_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <glhelper.h>
#include <glslshader.h>
#include <list> // added in tutorial 3
#include <random>
#include <map>
#include <unordered_map>
#include <vec2D.h>
#include <mat3x3.h>
#include <model.h>
struct GLApp {

  static void init();
  static void update();
  static void draw();
  static void cleanup();


  //added for physics testing
  static constexpr glm::vec3 red{1.f, 0.f, 0.f};
  static constexpr glm::vec3 green{0.f, 1.f, 0.f};
  static constexpr glm::vec3 blue{0.f, 0.f, 1.f};
  enum class collisionType
  {
	NIL,			//0
	SAT,			//1
	DIAG,			//2
	SNAPDIAGSTATIC, //3
	AABBSTATIC,		//5
	SATSTATIC,		//4
	AABBDYNAMIC		//6
  };
  static short currentCollision;
  static bool stepByStepCollision;
  static std::unordered_map<collisionType, std::string> collisionInfo;

 

  struct GLObject {
	  vector2D::vec2D orientation{};													// orientation x is angle disp, orientation y is angle speed specified in degrees
	  vector2D::vec2D scaling{};														// scaling parameters
	  vector2D::vec2D modelCenterPos{};													// center of shape coordinates
	  vector2D::vec2D vel{};															// velocity

	  matrix3x3::mat3x3 mdl_to_ndc_xform{}; // model to ndc transformation
	  matrix3x3::mat3x3 mdl_to_world_xform{}; // model to world transformation
	  matrix3x3::mat3x3 world_to_ndc_xform{}; // world to ndc transformation

	  vector2D::vec2D ndcposition{}; // translation vector coordinates

	  std::vector <vector2D::vec2D> ndc_coords;

	  //added for physics testing
	  bool overlap{ false };														// flag for overlap
	  //added for physics testing (collision response)
	  //glm::mat3 worldToMdlXform{};

	  std::map<std::string, Graphics::Model>::iterator mdl_ref{};
	  std::map<std::string, GLSLShader>::iterator shd_ref{};

	  //added for physics testing
	  glm::vec3 color{};
	  matrix3x3::mat3x3 mdlXform{};															// model to world transformation
	  vector2D::vec2D worldCenterPos{};
	  std::vector<vector2D::vec2D> boundingBoxWorldVertices;									
	  std::vector<vector2D::vec2D> worldVertices;												// vertices coordinates
	  std::vector<vector2D::vec2D> modelVertices;												// vertices coordinates

	  // member functions that must be defined in glapp.cpp
	  // function to initialize object's state
	  void init();
	  // function to render object's model (specified by index mdl_ref)
	  // uses model transformation matrix mdl_to_ndc_xform matrix
	  // and shader program specified by index shd_ref ...
	  void draw() const;
	  // function to update the object's model transformation matrix
	  void update(GLdouble delta_time);

	  static void gimmeObject(std::string modelname, std::string objname, vector2D::vec2D scale, vector2D::vec2D pos, glm::vec3 colour); // Temp
  };



  //static void init_models_cont(); // new in tutorial 3
  using VPSS = std::vector<std::pair<std::string, std::string>>;

  // function to insert shader program into container GLApp::shdrpgms ...
  static void insert_shdrpgm(std::string, std::string, std::string);
  // function to parse scene file ...
  static void init_scene(std::string);

  //static std::vector<GLApp::GLModel> models; // removed
  // added for tutorial 4: repository of models
  //static std::map<std::string, Graphics::Model> models; // singleton

  
  //static std::list<GLApp::GLObject> objects; // from tutorial 3
  // added for tutorial 4: repository of objects
  static std::map<std::string, GLObject> objects; // singleton

  //static void init_shdrpgms_cont(GLApp::VPSS const&); // removed
  // added for tutorial 4: repository of shader programs
  static std::map<std::string, GLSLShader> shdrpgms; // singleton


  
};

#endif /* GLAPP_H */
