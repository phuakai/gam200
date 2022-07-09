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

  static GLboolean reduc;

  struct GLModel {
	  GLenum primitive_type = GL_TRIANGLES; // which OpenGL primitive to be rendered?
	  GLuint primitive_cnt = 0; // added for tutorial 2

	  GLuint vaoid = 0; // handle to VAO
	  GLuint draw_cnt = 0; // added for tutorial 2

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
	  void draw() const;
	  // function to update the object's model transformation matrix
	  void update(GLdouble delta_time);
  };

  struct GLViewport {
	  GLint x, y;
	  GLsizei width, height;
  };

  struct Camera2D {
	  GLObject* pgo{}; // pointer to game object that embeds camera
	  glm::vec2 right{};
	  glm::vec2 up{};
	  glm::mat3 view_xform{};
	  glm::mat3 camwin_to_ndc_xform{};
	  glm::mat3 world_to_ndc_xform{};

	  GLint height{ 1000 };
	  GLfloat ar{};

	  // window change parameters ...
	  GLint min_height{ 500 }, max_height{ 2000 };
	  // height is increasing if 1 and decreasing if -1
	  GLint height_chg_dir{ 1 };
	  // increments by which window height is changed per Z key press
	  GLint height_chg_val{ 5 };
	  // camera's speed when button U is pressed
	  GLfloat linear_speed{ 2.f };
	  // keyboard button press flags
	  GLboolean camtype_flag{ GL_FALSE }; // button V
	  GLboolean zoom_flag{ GL_FALSE }; // button Z
	  GLboolean left_turn_flag{ GL_FALSE }; // button H
	  GLboolean right_turn_flag{ GL_FALSE }; // button K
	  GLboolean move_flag{ GL_FALSE }; // button U

	  // you can implement these functions as you wish ...
	  void init(GLFWwindow*, GLObject* ptr);
	  void update(GLFWwindow*);
  };
  // define object of type Camera2D ...
  static Camera2D camera2d;


  static void init_models_cont(); // new in tutorial 3
  using VPSS = std::vector<std::pair<std::string, std::string>>;

  // function to insert shader program into container GLApp::shdrpgms ...
  static void insert_shdrpgm(std::string, std::string, std::string);
  // function to parse scene file ...
  static void init_scene(std::string);

  //static std::vector<GLApp::GLModel> models; // removed
  // added for tutorial 4: repository of models
  static std::map<std::string, GLModel> models; // singleton

  
  //static std::list<GLApp::GLObject> objects; // from tutorial 3
  // added for tutorial 4: repository of objects
  static std::map<std::string, GLObject> objects; // singleton

  //static void init_shdrpgms_cont(GLApp::VPSS const&); // removed
  // added for tutorial 4: repository of shader programs
  static std::map<std::string, GLSLShader> shdrpgms; // singleton


  
  static std::vector<GLViewport> vps; // container for viewports
};

#endif /* GLAPP_H */
