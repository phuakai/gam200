/* !
@file    glapp.h
@author  pghali@digipen.edu, lee.g@digipen.edu
@date    17/06/2022

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
#include <map>
#include <unordered_map>


struct GLApp {
  static double constexpr WorldRange = 5000.0;
  static float constexpr PI = 3.14159f;
  //static const void* ptr_texels;

  static void init();
  static void update();
  static void draw();
  static void cleanup();
  static GLuint setup_texobj(std::string);


  struct GLModel {
	  GLenum primitive_type = GL_TRIANGLES; // which OpenGL primitive to be rendered?
	  GLuint vaoid = 0; // handle to VAO
	  GLuint draw_cnt = 0;
  };

  static GLApp::GLModel modelInit();
  static std::vector<GLModel> models;


  static std::unordered_map<std::string, GLSLShader> shdrpgms; // singleton
  using PSS = std::pair<std::string, std::string>;
  static void insert_shdrpgm(std::string key, PSS address);

  static std::unordered_map<std::string, GLSLShader>::iterator shd_ref;


  struct vertexData {
	  glm::vec2 posVtx;
	  glm::vec3 clrVtx;
	  glm::vec2 txtVtx;
  };

};

#endif /* GLAPP_H */
