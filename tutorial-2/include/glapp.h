/* !
@file    glapp.h
@author  pghali@digipen.edu, a.weiren@digipen.edu
@date    28/05/2022

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

struct GLApp {
  static void init();
  static void update();
  static void draw();
  static void cleanup();

  struct GLModel {
	  GLenum primitive_type = GL_TRIANGLES; // which OpenGL primitive to be rendered?
	  GLuint primitive_cnt = 0; // added for tutorial 2

	  GLSLShader shdr_pgm; // which shader program?
	  GLuint vaoid = 0; // handle to VAO
	  GLuint draw_cnt = 0; // added for tutorial 2
	  // primitive_type are to be rendered
	  // member functions defined in glapp.cpp
	  void setup_shdrpgm(std::string vtx_shdr, // added to tutorial 2
		  std::string frg_shdr);

	  void draw();
  };
  // data member to represent geometric model to be rendered
  // C++ requires this object to have a definition in glapp.cpp!!!
  static std::vector<GLModel> models;

  // viewport 1 / points
  static GLApp::GLModel points_model(int slices, int stacks,
	  std::string vtx_shdr, std::string frg_shdr);
  // viewport 2 / lines
  static GLApp::GLModel lines_model(int slices, int stacks,
	  std::string vtx_shdr, std::string frg_shdr);
  // viewport 3 / triangle fan
  static GLApp::GLModel trifans_model(int slices, std::string vtx_shdr,
	  std::string frg_shdr);
  // viewport 4 / triangle strip
  static GLModel tristrip_model(int slices, int stacks,
	  std::string vtx_shdr, std::string frg_shdr);


  struct GLViewport {
	  GLint x, y;
	  GLsizei width, height;
  };
  static std::vector<GLViewport> vps; // container for viewports
};

#endif /* GLAPP_H */
