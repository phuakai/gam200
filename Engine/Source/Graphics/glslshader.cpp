/* !
@file    glslshader.cpp
@author  a.weiren@digipen.edu
@date    20/8/2022

Note: The contents of this file must not be updated by students. Otherwise,
something that works for you will not work for me. If you want something to be
modified, updated, or altered and it is useful for the entire class, please
speak to me.

This file contains definitions of member functions of class GLShader.
Please see the class declaration for information about these functions.

*//*__________________________________________________________________________*/
#include <glslshader.h>
#include <iostream>
#include <fstream>
#include <sstream>

GLint GLSLShader::GetUniformLocation(GLchar const* name) 
{
  return glGetUniformLocation(pgm_handle, name); // Get shader location
}

GLboolean GLSLShader::FileExists(std::string const& file_name) 
{
  std::ifstream infile(file_name); // Check if file exists
  return infile.good();
}

void GLSLShader::DeleteShaderProgram() 
{
  if (pgm_handle > 0) 
  {
    glDeleteProgram(pgm_handle); // Delete shader prog
  }
}

GLboolean GLSLShader::CompileLinkValidate(std::vector<std::pair<GLenum, std::string>> vec) 
{
  for (auto& elem : vec) 
  {
    if (GL_FALSE == CompileShaderFromFile(elem.first, elem.second.c_str())) // Compile link and validate shader
    {
      return GL_FALSE;
    }
  }
  if (GL_FALSE == Link()) 
  {
    return GL_FALSE;
  }
  if (GL_FALSE == Validate()) 
  {
    return GL_FALSE;
  }
  return GL_TRUE;
}

GLboolean GLSLShader::CompileShaderFromFile(GLenum shader_type, const std::string& file_name) 
{
  if (GL_FALSE == FileExists(file_name)) 
  {
    log_string = "File not found";
    return GL_FALSE;
  }
  if (pgm_handle <= 0) 
  {
    pgm_handle = glCreateProgram();
    if (0 == pgm_handle) {
      log_string = "Cannot create program handle";
      return GL_FALSE;
    }
  }

  std::ifstream shader_file(file_name, std::ifstream::in);
  if (!shader_file) {
    log_string = "Error opening file " + file_name;
    return GL_FALSE;
  }
  std::stringstream buffer;
  buffer << shader_file.rdbuf();
  shader_file.close();
  return CompileShaderFromString(shader_type, buffer.str());
}

GLboolean
GLSLShader::CompileShaderFromString(GLenum shader_type,
  const std::string& shader_src) {
  if (pgm_handle <= 0) {
    pgm_handle = glCreateProgram();
    if (0 == pgm_handle) {
      log_string = "Cannot create program handle";
      return GL_FALSE;
    }
  }

  GLuint shader_handle = 0;
  switch (shader_type) {
  case VERTEX_SHADER: shader_handle = glCreateShader(GL_VERTEX_SHADER); break;
  case FRAGMENT_SHADER: shader_handle = glCreateShader(GL_FRAGMENT_SHADER); break;
  case GEOMETRY_SHADER: shader_handle = glCreateShader(GL_GEOMETRY_SHADER); break;
  case TESS_CONTROL_SHADER: shader_handle = glCreateShader(GL_TESS_CONTROL_SHADER); break;
  case TESS_EVALUATION_SHADER: shader_handle = glCreateShader(GL_TESS_EVALUATION_SHADER); break;
    //case COMPUTE_SHADER: shader_handle = glCreateShader(GL_COMPUTE_SHADER); break;
  default:
    log_string = "Incorrect shader type";
    return GL_FALSE;
  }

  // load shader source code into shader object
  GLchar const* shader_code[] = { shader_src.c_str() };
  glShaderSource(shader_handle, 1, shader_code, NULL);

  // compile the shader
  glCompileShader(shader_handle);

  // check compilation status
  GLint comp_result;
  glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &comp_result);
  if (GL_FALSE == comp_result) {
    log_string = "Vertex shader compilation failed\n";
    GLint log_len;
    glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0) {
      GLchar* log = new GLchar[log_len];
      GLsizei written_log_len;
      glGetShaderInfoLog(shader_handle, log_len, &written_log_len, log);
      log_string += log;
      delete[] log;
    }
    return GL_FALSE;
  } else { // attach the shader to the program object
    glAttachShader(pgm_handle, shader_handle);
    return GL_TRUE;
  }
}

GLboolean GLSLShader::Link() 
{
  if (GL_TRUE == is_linked) 
  {
    return GL_TRUE;
  }
  if (pgm_handle <= 0) 
  {
    return GL_FALSE;
  }

  glLinkProgram(pgm_handle); // link the various compiled shaders

  // verify the link status
  GLint lnk_status;
  glGetProgramiv(pgm_handle, GL_LINK_STATUS, &lnk_status);
  if (GL_FALSE == lnk_status) 
  {
    log_string = "Failed to link shader program\n";
    GLint log_len;
    glGetProgramiv(pgm_handle, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0) 
    {
      GLchar* log_str = new GLchar[log_len];
      GLsizei written_log_len;
      glGetProgramInfoLog(pgm_handle, log_len, &written_log_len, log_str);
      log_string += log_str;
      delete[] log_str;
    }
    return GL_FALSE;
  }
  return is_linked = GL_TRUE;
}

void GLSLShader::Use() 
{
  if (pgm_handle > 0 && is_linked == GL_TRUE) 
  {
    glUseProgram(pgm_handle);
  }
}

void GLSLShader::UnUse() 
{
  glUseProgram(0);
}

GLboolean GLSLShader::Validate() 
{
  if (pgm_handle <= 0 || is_linked == GL_FALSE) 
  {
    return GL_FALSE;
  }

  glValidateProgram(pgm_handle);
  GLint status;
  glGetProgramiv(pgm_handle, GL_VALIDATE_STATUS, &status);
  if (GL_FALSE == status) 
  {
    log_string = "Failed to validate shader program for current OpenGL context\n";
    GLint log_len;
    glGetProgramiv(pgm_handle, GL_INFO_LOG_LENGTH, &log_len);
    if (log_len > 0) 
    {
      GLchar* log_str = new GLchar[log_len];
      GLsizei written_log_len;
      glGetProgramInfoLog(pgm_handle, log_len, &written_log_len, log_str);
      log_string += log_str;
      delete[] log_str;
    }
    return GL_FALSE;
  } 
  else 
  {
    return GL_TRUE;
  }
}

GLuint GLSLShader::GetHandle() const 
{
  return pgm_handle;
}

GLboolean GLSLShader::IsLinked() const 
{
  return is_linked;
}

std::string GLSLShader::GetLog() const 
{
  return log_string;
}

void GLSLShader::BindAttribLocation(GLuint index, GLchar const* name) 
{
  glBindAttribLocation(pgm_handle, index, name); // Bind attribute location
}

void GLSLShader::BindFragDataLocation(GLuint color_number, GLchar const* name) 
{
  glBindFragDataLocation(pgm_handle, color_number, name); // Bind fragment shader
}

void GLSLShader::SetUniform(GLchar const* name, GLboolean val) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniform1i(loc, val);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

void GLSLShader::SetUniform(GLchar const* name, GLint val) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniform1i(loc, val);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

void GLSLShader::SetUniform(GLchar const* name, GLfloat val) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniform1f(loc, val);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

void GLSLShader::SetUniform(GLchar const* name, GLfloat x, GLfloat y) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniform2f(loc, x, y);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

void GLSLShader::SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniform3f(loc, x, y, z);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

void
GLSLShader::SetUniform(GLchar const* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniform4f(loc, x, y, z, w);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

void GLSLShader::SetUniform(GLchar const* name, glm::vec2 const& val) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniform2f(loc, val.x, val.y);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

void GLSLShader::SetUniform(GLchar const* name, glm::vec3 const& val) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniform3f(loc, val.x, val.y, val.z);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

void GLSLShader::SetUniform(GLchar const* name, glm::vec4 const& val) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniform4f(loc, val.x, val.y, val.z, val.w);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

void GLSLShader::SetUniform(GLchar const* name, glm::mat3 const& val) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniformMatrix3fv(loc, 1, GL_FALSE, &val[0][0]);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

void GLSLShader::SetUniform(GLchar const* name, glm::mat4 const& val) 
{
  GLint loc = glGetUniformLocation(pgm_handle, name);
  if (loc >= 0) 
  {
    glUniformMatrix4fv(loc, 1, GL_FALSE, &val[0][0]);
  } 
  else 
  {
    std::cout << "Uniform variable " << name << " doesn't exist" << std::endl;
  }
}

/*  _________________________________________________________________________*/
/*!  insert_shdrpgm

@param std::string shdr_pgm_name
Name of shader program to be used

@param std::string vtx_shdr
Name of vertex shader to be used

@param std::string frg_shdr
Name of fragment shader to be used

@return none

This function is called at the initialization of the scene to insert the different shader programs into a map container
*/
void insert_shdrpgm(std::vector<std::string>& shdrnamelist, std::vector<GLSLShader>& shdrpgmlist, std::string shdr_pgm_name, std::string vtx_shdr, std::string frg_shdr)
{
    std::vector<std::pair<GLenum, std::string>> shdr_files
    {
        std::make_pair(GL_VERTEX_SHADER, vtx_shdr),
        std::make_pair(GL_FRAGMENT_SHADER, frg_shdr)
    };
    GLSLShader shdr_pgm;
    shdr_pgm.CompileLinkValidate(shdr_files);
    if (GL_FALSE == shdr_pgm.IsLinked()) {
        std::cout << "Unable to compile/link/validate shader programs\n";
        std::cout << shdr_pgm.GetLog() << "\n";
        std::exit(EXIT_FAILURE);
    }
    shdrnamelist.emplace_back(shdr_pgm_name);
    shdrpgmlist.emplace_back(shdr_pgm);
}

void insertallshdrs(std::map<std::string, GLSLShader>& shdrlist)
{
    std::vector<std::string> shdrnames;
    std::vector<GLSLShader> shdrpgms;
    insert_shdrpgm(shdrnames, shdrpgms, "framebuffershader", "../shaders/framebuffer.vert", "../shaders/framebuffer.frag");
    insert_shdrpgm(shdrnames, shdrpgms, "instanceshader", "../shaders/instancing.vert", "../shaders/instancing.frag");
    for (size_t i = 0; i < shdrnames.size(); i++)
    {
        shdrlist[shdrnames[i]] = shdrpgms[i];
    }
}