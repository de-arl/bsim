#ifndef SHADER_H
#define SHADER_H
/* Class based on the code provided online by Joey de Vries, downloaded 27.01.2024:
 * https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_s.h
 */
#ifndef OPENGL_INITIALIZED
#define OPENGL_INITIALIZED
#define GLAD_GL_IMPLEMENTATION
#include "gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif
#include <glm/vec3.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

template <typename T>
class Shader
{
 public:
  GLuint ID[1];
  /* Constructor */
  Shader(){};

  /* Initialize the shader */
  void init(const char* vertexPath, const char* fragmentPath);

  /* Activate the shader */
  void use(GLuint num_shader);

  /* Utility uniform functions */
  void setBool(GLuint num_shader, const std::string &name, bool value) const;
  void setInt(GLuint num_shader, const std::string &name, int value) const;
  void setFloat(GLuint num_shader, const std::string &name, T value) const;
  void setVec3(
      GLuint num_shader, const std::string &name, const T* value) const;
  void setMat4(
      GLuint num_shader, const std::string &name, const GLfloat* value) const;

 private:
  /* Utility function for checking shader compilation/linking errors. */
  void checkCompileErrors(unsigned int shader, std::string type);
  void initGlad();
};
#endif // SHADER_H
/* vim: set ts=2 sw=2 tw=0 et :*/
