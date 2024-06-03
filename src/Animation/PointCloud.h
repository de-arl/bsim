#ifndef POINTCLOUD_H
#define POINTCLOUD_H

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


// Point cloud imgui handle forward declaration
template <typename T>
class PointCloudImGuiHandle;

template <typename T>
class PointCloud
{
 public:

  // Constructor
  PointCloud(Shader<T> &shader);

  // Initialize constructor
  PointCloud(Shader<T> &shader, GLFWwindow* window, T* positions, 
      int num_points, T pointsize, std::string name, Color<T> color);

  // Destructor
  ~PointCloud();

  // Copy constructor
  PointCloud(const PointCloud &c);

  // Copy assignment operator
  PointCloud &operator=(const PointCloud &c);

  // Initialize all
  void init(GLFWwindow* window, T* positions, int num_points, T pointsize, 
      std::string name);

  // Initialize the point cloud
  void initPointCloud();

  // Initialize the parameters
  void initParams();

  // Update the point cloud
  void update();

  // Draw the point cloud
  void draw();

  // Set/get color of the point cloud
  void setColor(Color<T> &color);
  Color<T> getColor();

  // Set/get the shader program
  void setShaderId(int shader_id);
  int getShaderId();

  // Set/get the window
  void setWindow(GLFWwindow* window);
  GLFWwindow* getWindow();

  // Set/get the points
  void setPoints(T* points);
  T* getPoints();

  // Set/get the number of points
  void setNumPoints(int num_points);
  int getNumPoints();

  // Set/get the pointsize
  void setPointsize(T pointsize);
  T getPointsize();

  // Set/get the name
  void setName(const std::string &name);
  std::string getName();

 private:

  // Delete/init buffers
  void deleteBuffers();
  void initBuffers();

  // Window
  GLFWwindow* window;

  // Name
  std::string name;

  // Shader program id
  int shader_id;

  // Reference to the shader program
  Shader<T> &shader;

  // Color
  Color<T> color;

  // Pointsize
  float pointsize;

  // Buffers
  GLuint VAO, VBO;

  // Number of points
  int num_points;

  // Positions (XYZ) of the points
  T* points;

  // Show/hide the point cloud
  bool show;

  friend class PointCloudImGuiHandle<T>;
};
  

#endif //POINTCLOUD_H
