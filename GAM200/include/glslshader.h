/* !
@file    glslshader.h
@author  a.weiren@digipen.edu
@date    20/8/2022

This file contains the declaration of class GLSLShader that encapsulates the
functionality required to load shader source; compile shader source; link
shader objects into a program object, validate program object; log msesages
from compiling linking, and validation steps; install program object in
appropriate shaders; and pass uniform variables from the client to the
program object.

*//*__________________________________________________________________________*/

/*                                                                      guard
----------------------------------------------------------------------------- */
#ifndef GLSLSHADER_H
#define GLSLSHADER_H

/*                                                                   includes
----------------------------------------------------------------------------- */
#include <GL/glew.h> // for access to OpenGL API declarations 
#include <glm/glm.hpp>
#include <string>
#include <vector>


class GLSLShader 
{
public:

    /******************************************************************************/
    /*!
        This function constructs the GLSL shader
    */
    /******************************************************************************/
  GLSLShader() : pgm_handle(0), is_linked(GL_FALSE) { /* empty by design */ }

    /******************************************************************************/
    /*!
        This function compiles, links and validates the shaders
    */
    /******************************************************************************/
  GLboolean CompileLinkValidate(std::vector<std::pair<GLenum, std::string>>);

  /******************************************************************************/
  /*!
      This function compiles the shaders from the file
  */
  /******************************************************************************/
  GLboolean CompileShaderFromFile(GLenum shader_type, std::string const& file_name);

  /******************************************************************************/
  /*!
      This function compiles the shader from a string
  */
  /******************************************************************************/
  GLboolean CompileShaderFromString(GLenum shader_type, std::string const& shader_src);

  /******************************************************************************/
  /*!
      This function links the shaders
  */
  /******************************************************************************/
  GLboolean Link();

  /******************************************************************************/
  /*!
      This function uses the shaders
  */
  /******************************************************************************/
  void Use();

  /******************************************************************************/
  /*!
      This function unuses the shaders
  */
  /******************************************************************************/  
  void UnUse();

  /******************************************************************************/
  /*!
      This function validates the shaders
  */
  /******************************************************************************/
  GLboolean Validate();

  /******************************************************************************/
  /*!
      This function gets the handle to the shaders
  */
  /******************************************************************************/
  GLuint GetHandle() const;

  /******************************************************************************/
  /*!
      This function checks if the shaders are linked
  */
  /******************************************************************************/
  GLboolean IsLinked() const;

  std::string GetLog() const;

  /******************************************************************************/
  /*!
      This function finds the bind attrib location of the shader
  */
  /******************************************************************************/
  void BindAttribLocation(GLuint index, GLchar const* name);


  void BindFragDataLocation(GLuint color_number, GLchar const* name);

  /******************************************************************************/
  /*!
      This function deletes the shaders
  */
  /******************************************************************************/
  void DeleteShaderProgram();

  /******************************************************************************/
  /*!
      This function sets the uniforms for the shader
  */
  /******************************************************************************/
  void SetUniform(GLchar const* name, GLboolean val);
  void SetUniform(GLchar const* name, GLint val);
  void SetUniform(GLchar const* name, GLfloat val);
  void SetUniform(GLchar const* name, GLfloat x, GLfloat y);
  void SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z);
  void SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  void SetUniform(GLchar const* name, glm::vec2 const& val);
  void SetUniform(GLchar const* name, glm::vec3 const& val);
  void SetUniform(GLchar const* name, glm::vec4 const& val);
  void SetUniform(GLchar const* name, glm::mat3 const& val);
  void SetUniform(GLchar const* name, glm::mat4 const& val);

  /******************************************************************************/
  /*!
      This function prints the active vertice attribs in the shaders
  */
  /******************************************************************************/
  void PrintActiveAttribs() const;

  /******************************************************************************/
  /*!
      This function prints the active uniforms of the shaders
  */
  /******************************************************************************/
  void PrintActiveUniforms() const;

private:
  enum ShaderType {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
    TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER,
    TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,

  };

  GLuint pgm_handle = 0;  // handle to linked shader program object
  GLboolean is_linked = GL_FALSE; // has the program successfully linked?
  std::string log_string; // log for OpenGL compiler and linker messages

private:
  GLint GetUniformLocation(GLchar const* name);

  GLboolean FileExists(std::string const& file_name);
};

#endif /* GLSLSHADER_H */
