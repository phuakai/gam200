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


struct GLApp {
  static void init();
  static void update();
  static void draw();
  static void cleanup();

  static GLuint setup_texobj(std::string pathname);

  static GLboolean reduc;

  struct DrawVec {
	  glm::vec2 pos;
	  glm::vec3 color;
	  glm::vec2 textcoord;
  };

  struct GLModel {
	  GLenum primitive_type = GL_TRIANGLES; // which OpenGL primitive to be rendered?
	  GLuint primitive_cnt = 0; // added for tutorial 2

	  GLuint vaoid = 0; // handle to VAO
	  GLuint draw_cnt = 0; // added for tutorial 2

	  GLSLShader shdr_pgm;

	  void setup_vao();
	  void setup_shdrpgm(std::string, std::string);
	  void draw(); 
	  // you could add member functions for convenience if you so wish ...
	  //void init(); // read mesh data from file ...
	  //void release(); // return buffers back to GPU ...
  };

  struct GLObject {

	  // std::default_random_engine dre;


	 // these two variables keep track of the current orientation
	 // of the object.
	 // angle_speed: rate of change of rotation angle per second
	 // angle_disp: current absolute orientation angle
	 // all angles refer to rotation about Z-axis
	 // it is up to you whether angles are tracked in degrees or radians

	 //GLfloat angle_speed = 0, angle_disp = 0; // orientation (removed in tut4)
	  glm::vec2 orientation{}; // orientation x is angle disp, orientation y is angle speed specified in degrees
	  glm::vec2 scaling{}; // scaling parameters
	  glm::vec2 position{}; // translation vector coordinates
	  // compute object's model transform matrix using scaling,
	  // rotation, and translation attributes ...
	  // this must be computed by the CPU and not by vertex shader (why?)
	  glm::mat3 mdl_to_ndc_xform{}; // model to ncd transformation

	  // which model is this object an instance of?
	  // since models are contained in a vector, we keep track of the
	  // specific model that was instanced by index into vector container

	  //GLuint mdl_ref = 0; // removed in tut 4

	  // how to draw this instanced model?
	  // since shader programs are contained in a vector, we keep track of
	  // specific shader program using an index into vector container

	  //GLuint shd_ref = 0; // removed in tut 4

	  // added in tutorial 4
	  std::map<std::string, GLApp::GLModel>::iterator mdl_ref{};
	  std::map<std::string, GLSLShader>::iterator shd_ref{};

	  // added to tutorial 4
	  glm::vec3 color{};
	  glm::mat3 mdl_xform{}; // model to world transformation

	  // member functions that must be defined in glapp.cpp
	  // function to initialize object's state
	  void init();
	  // function to render object's model (specified by index mdl_ref)
	  // uses model transformation matrix mdl_to_ndc_xform matrix
	  // and shader program specified by index shd_ref ...
	  //void draw() const;
	  // function to update the object's model transformation matrix
	  //void update(GLdouble delta_time);
  };

  struct GLViewport {
	  GLint x, y;
	  GLsizei width, height;
  };

  // Draw vertex member
  static DrawVec drawer;

  // data member to represent geometric model to be rendered
  // C++ requires this object to have a definition in glapp.cpp!!!
  static GLModel mdl;

  static void init_models_cont(); // new in tutorial 3
  using VPSS = std::vector<std::pair<std::string, std::string>>;

  // function to parse scene file ...
  static void init_scene(std::string);

  //static std::vector<GLApp::GLModel> models; // removed
  // added for tutorial 4: repository of models
  static std::map<std::string, GLModel> models; // singleton

  
  //static std::list<GLApp::GLObject> objects; // from tutorial 3
  // added for tutorial 4: repository of objects
  static std::map<std::string, GLObject> objects; // singleton


  
  static std::vector<GLViewport> vps; // container for viewports
};

#endif /* GLAPP_H */
