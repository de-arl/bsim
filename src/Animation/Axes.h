#ifndef AXES_H
#define AXES_H

#include <vector>

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
#include "helpers.h"


template <typename T>
class Axes
{
 public:

  // Constructor
  Axes(Shader<T> &shader);

  // Destructor
  ~Axes();

  // Initialize all
  void init(
      GLFWwindow* window,
      T dims[3],
      AnimationParameters<T> &anim_params);
  void init(
      T dims[3],
      AnimationParameters<T> &anim_params);

  // Initialize the parameters
  void initParams(AnimationParameters<T> &anim_params);

  // Draw the axes
  void drawAxes();

  // Draw the ticks
  void drawTicks();

  // Draw the labels
  void drawLabels();

  // Draw the axes, ticks, and labels
  void draw(
      bool draw_axes=true, bool draw_ticks=true, bool draw_labels=true);

  // Set/get color of the axes
  void setColor(Color<T> &color);
  void setAxesColor(Color<T> &color);
  void setTickColor(Color<T> &color);
  void setLabelColor(Color<T> &color);
  Color<T> getAxesColor();
  Color<T> getTickColor();
  Color<T> getLabelColor();

  // Set the shader program
  void setShaderId(int shader_id);

  // Set the window
  void setWindow(GLFWwindow* window);

  // Set/get number of ticks
  void setNumTicks(int n_ticks);
  int getNumTicks();

  // Set/get tick length
  void setTickLength(float tick_length[6]);
  void getTickLength(float* tick_length);

  // Set/get the dimensions of the axes
  void getDims(float* dims);
  void setDims(T dims[3]);

  // Generate the ticks
  void generateTicks();


 private:

  // Initialize the axes
  void initAxes();

  // Window
  GLFWwindow* window;
  
  // Dimensions of the axes (x, y, z)
  GLfloat dims[3];

  // Number of ticks in the interval [-1, 1]
  int n_ticks;
  
  // Total number of ticks
  int num_ticks;

  // Length of the ticks
  GLfloat tick_length[6];

  // Vertex array object, vertex buffer object, element buffer object
  GLuint VAO[2], VBO[2], EBO[2];

  // Reference to the shader program
  Shader<T> &shader;

  // Color of the axes
  Color<T> axes_color;

  // Color of the ticks
  Color<T> tick_color;

  // Color of the labels
  Color<T> label_color;

  // ID of the shader program
  int shader_id;
};

#endif //AXES_H
