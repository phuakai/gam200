/* !
@file    glapp.h
@author  pghali@digipen.edu, a.weiren@digipen.edu
@date    03/06/2022

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

struct GLApp {
	static int objcounter;
	static int boxcounter;
	static int mysterycounter;
	
  static void init();
  static void update();
  static void draw();
  static void cleanup();

  static GLboolean reduc;

  static void init_models_cont(); // new in tutorial 3
  static std::vector<GLSLShader> shdrpgms; // new in tutorial 3
  using VPSS = std::vector<std::pair<std::string, std::string>>;
  static void init_shdrpgms_cont(GLApp::VPSS const&); // new in tutorial 3

  struct GLModel {
	  GLenum primitive_type = GL_TRIANGLES; // which OpenGL primitive to be rendered?
	  GLuint primitive_cnt = 0; // added for tutorial 2
	  //GLSLShader shdr_pgm; // removed in tutorial 3
	  GLuint vaoid = 0; // handle to VAO
	  GLuint draw_cnt = 0; // added for tutorial 2
	  // primitive_type are to be rendered
	  // member functions defined in glapp.cpp
	  //void setup_shdrpgm(std::string vtx_shdr, // removed in tutorial 3
		//  std::string frg_shdr); 
	  //void draw(); // removed in tutorial 3
  };

  // data member to represent geometric model to be rendered
  // C++ requires this object to have a definition in glapp.cpp!!!

  static std::vector<GLModel> models;
  // TUTORIAL 3
  // Viewport 1 / box model
  static GLApp::GLModel box_model();

  // Viewport 2 / mystery model
  static GLApp::GLModel mystery_model();

  // tutorial 3: encapsulates state required to update
  // and render an instance of a model
  struct GLObject {

	  std::default_random_engine dre;

	 
	// these two variables keep track of the current orientation
	// of the object.
	// angle_speed: rate of change of rotation angle per second
	// angle_disp: current absolute orientation angle
	// all angles refer to rotation about Z-axis
	// it is up to you whether angles are tracked in degrees or radians
	GLfloat angle_speed = 0, angle_disp = 0; // orientation
	glm::vec2 scaling; // scaling parameters
	glm::vec2 position; // translation vector coordinates
	// compute objet's model transform matrix using scaling,
	// rotation, and translation attributes ...
	// this must be computed by the CPU and not by vertex shader (why?)
	glm::mat3 mdl_to_ndc_xform;
	// which model is this object an instance of?
	// since models are contained in a vector, we keep track of the
	// specific model that was instanced by index into vector container
	GLuint mdl_ref = 0;
	// how to draw this instanced model?
	// since shader programs are contained in a vector, we keep track of
	// specific shader program using an index into vector container
	GLuint shd_ref = 0;
	// member functions that must be defined in glapp.cpp
	// function to initialize object's state
	void init();
	// function to render object's model (specified by index mdl_ref)
	// uses model transformation matrix mdl_to_ndc_xform matrix
	// and shader program specified by index shd_ref ...
	void draw() const;
	// function to update the object's model transformation matrix
	void update(GLdouble delta_time);
  };
  static std::list<GLApp::GLObject> objects; // singleton




  //// viewport 1 / points
  //static GLApp::GLModel points_model(int slices, int stacks,
	 // std::string vtx_shdr, std::string frg_shdr);
  //// viewport 2 / lines
  //static GLApp::GLModel lines_model(int slices, int stacks,
	 // std::string vtx_shdr, std::string frg_shdr);
  //// viewport 3 / triangle fan
  //static GLApp::GLModel trifans_model(int slices, std::string vtx_shdr,
	 // std::string frg_shdr);
  //// viewport 4 / triangle strip
  //static GLModel tristrip_model(int slices, int stacks,
	 // std::string vtx_shdr, std::string frg_shdr);




  struct GLViewport {
	  GLint x, y;
	  GLsizei width, height;
  };
  static std::vector<GLViewport> vps; // container for viewports
};

#endif /* GLAPP_H */
