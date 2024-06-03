#ifndef GRID_H
#define GRID_H

#include <vector>
#include <iostream>
#include <numeric> // std::iota
#include <cstring>
                   
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

// ImGui handle forward declaration
template <typename T>
class GridImGuiHandle;

template <typename T>
class Grid
{
 public:
  // Constructor
  Grid(Shader<T> &shader);

  // Destructor
  ~Grid();

  // Initialize all
  void init(GLFWwindow* window, T dims[3],
      AnimationParameters<T> &anim_params);

  void init(T dims[3], AnimationParameters<T> &anim_params);

  // Initialize the grids 
  void initGrids();

  // Initialize the parameters
  void initParams(AnimationParameters<T> &anim_params);

  // Generate the grids
  void generateGrids();

  // Draw grids
  void draw();

  // Set/get the number of gridlines
  void setNumGridlines(int num_gridlines_1D);
  int getNumGridlines();

  // Set/get the color
  void setColor(Color<T> &color);
  Color<T> getColor();

  // Set the shader program
  void setShaderId(int shader_id);

  // Set the window
  void setWindow(GLFWwindow* window);

  // Set the dimensions of the grid
  void setDims(T dims[3]);

  // Set/get the grids to draw
  void setToDraw(bool draw_grid[6]);
  void getToDraw(bool draw_grid[6]);


 private:


  // Draw a grid
  void drawGrid(int i);

  // Window
  GLFWwindow* window;
  
  // Dimensions of the grid
  GLfloat dims[3];

  // Vertex array objects, vertex buffer objects, element buffer objects
  GLuint VAO[6], VBO[6], EBO[6];

  // Vertices of the grid
  // 0: xy-plane, lower z
  // 1: xy-plane, upper z
  // 2: xz-plane, lower y
  // 3: xz-plane, upper y
  // 4: yz-plane, lower x
  // 5: yz-plane, upper x
  std::vector<GLfloat> vertices[6];

  // Reference to the shader program
  Shader<T> &shader;

  // Number of the shader program
  int shader_id;

  // Color
  Color<T> color;

  // Number of gridlines
  int num_gridlines_1D;
  int num_gridlines[6]; // Number of gridlines in each dimension
  
  // Select the grids to draw
  bool draw_grid[6] = {true, true, true, true, true, true};

  friend class GridImGuiHandle<T>;

};

#endif // GRID_H
