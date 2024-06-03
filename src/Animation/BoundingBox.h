#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#ifndef OPENGL_INITIALIZED
#define OPENGL_INITIALIZED
#define GLAD_GL_IMPLEMENTATION
#include "gl.h"
#include "khrplatform.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif //OPENGL_INITIALIZED
       
#include "Shader.h"
#include "Color.h"
#include "AnimationParameters.h"


template <typename T>
class BoundingBox
{
 public:

  // Constructor
  BoundingBox(Shader<T> &shader);

  // Destructor
  ~BoundingBox();

  // Initialize all
  void init(GLFWwindow* window, T dims[3],
      AnimationParameters<T> &anim_params);

  void init(T dims[3], AnimationParameters<T> &anim_params);
  
  // Initialize the bounding box
  void initBoundingBox();

  // Initialize the parameters
  void initParams(AnimationParameters<T> &anim_params);

  // Draw the bounding box
  void draw();

  // Set/get dimensions of the bounding box
  void setDims(T dims[3]);
  void getDims(float* dims);

  // Set/get color of the bounding box
  void setColor(Color<T> &color);
  Color<T> getColor();

  // Set the shader program
  void setShaderId(int shader_id);

  // Set the window
  void setWindow(GLFWwindow* window);


 private:

  // Window
  GLFWwindow* window;
  
  // Dimensions of the bounding box
  GLfloat dims[3];

  // Vertex array object, vertex buffer object, element buffer object
  GLuint VAO, VBO, EBO;

  // Reference to the shader program
  Shader<T> &shader;

  // Color of the bounding box
  Color<T> color;

  // ID of the shader program
  int shader_id;
};

#endif //BOUNDING_BOX_H
