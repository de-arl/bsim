#include "PointCloud.h"

template <typename T>
PointCloud<T>::PointCloud(Shader<T> &shader)
  :shader(shader),
  color(1.0f, 1.0f, 1.0f),
  shader_id(0),
  VAO(0),
  VBO(0),
  window(nullptr),
  num_points(0),
  points(nullptr)
{
#ifdef DEBUG
  std::cout << "PointCloud::PointCloud: \
    Default constructor of PointCloud" << std::endl;
#endif

  // Init buffers
  initBuffers();
}

template <typename T>
PointCloud<T>::PointCloud(Shader<T> &shader, GLFWwindow* window, T* positions, 
    int num_points, T pointsize, std::string name, Color<T> color)
  :shader(shader),
  color(color),
  shader_id(0),
  window(window),
  num_points(num_points),
  points(positions),
  pointsize((float)pointsize),
  show(true),
  name(name)
{
#ifdef DEBUG
  std::cout << "PointCloud::PointCloud: \
    Memory allocating constructor of PointCloud" << std::endl;
#endif

  // Init buffers
  initBuffers();

  // Initialize the point cloud
  initPointCloud();
}

template <typename T>
PointCloud<T>::~PointCloud()
{
#ifdef DEBUG
  std::cout << "PointCloud::~PointCloud: \
    Destructor of PointCloud" << std::endl;
#endif
  
  // Delete buffers
  deleteBuffers();
}

template <typename T>
PointCloud<T>::PointCloud(const PointCloud &c)
  :shader(c.shader),
  color(c.color),
  shader_id(c.shader_id),
  window(c.window),
  num_points(c.num_points),
  points(c.points),
  pointsize(c.pointsize),
  show(c.show),
  name(c.name)
{
#ifdef DEBUG
  std::cout << "PointCloud::PointCloud: \
    Copy constructor of PointCloud" << std::endl;
#endif
  
  // Init buffers
  initBuffers();

  // Initialize the point cloud
  initPointCloud();
}

template <typename T>
PointCloud<T> &PointCloud<T>::operator=(const PointCloud &c)
{
#ifdef DEBUG
  std::cout << "PointCloud::operator=: \
    Copy assignment operator of PointCloud" << std::endl;
#endif

  // Check for self-assignment
  if (this == &c) return *this;

  // Copy the shader
  shader = c.shader;

  // Copy the color
  color = c.color;

  // Copy the shader id
  shader_id = c.shader_id;

  // Copy the window
  window = c.window;

  // Copy the number of points
  num_points = c.num_points;

  // Copy the points
  points = c.points;

  // Copy the pointsize
  pointsize = c.pointsize;

  // Copy the show
  show = c.show;

  // Copy the name
  name = c.name;

  // Init buffers
  initBuffers();

  // Initialize the point cloud
  initPointCloud();

  return *this;
}

template <typename T>
void PointCloud<T>::init(
    GLFWwindow* window, T* positions, int num_points, T pointsize,
    std::string name)
{
#ifdef DEBUG
  std::cout << "PointCloud::init: \
    Initializing the point cloud" << std::endl;
#endif
  // Little hack to avoid errors when data type is double
  // avoids all drawing of points
  if (std::is_same<T, double>::value)
  {
    num_points = 0; 
  }

  // Set window
  setWindow(window);

  // Set positions
  setPoints(positions);

  // Set number of points
  setNumPoints(num_points);

  // Set pointsize
  setPointsize(pointsize);

  // Set name
  setName(name);

  // Initialize the point cloud
  initPointCloud();
}

template <typename T>
void PointCloud<T>::initPointCloud()
{
#ifdef DEBUG
  std::cout << "PointCloud::initPointCloud: \
    Initializing the point cloud" << std::endl;
  std::cout << "PointCloud::initPointCloud: \
    Number of points: " << num_points << std::endl;
#endif

  // Bind the VAO
  glBindVertexArray(VAO);

  // Bind the VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Copy the data
  glBufferData(GL_ARRAY_BUFFER, 3 * num_points * sizeof(T), 
      points, GL_DYNAMIC_DRAW);

  // Set the vertex attribute pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(T), (void*)0);

  // Enable the vertex attribute array
  glEnableVertexAttribArray(0);

  // Unbind the VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Unbind the VAO
  glBindVertexArray(0);

}

template <typename T>
void PointCloud<T>::initParams()
{
#ifdef DEBUG
  std::cout << "PointCloud::initParams: \
    Initializing the parameters of the point cloud" << std::endl;
#endif
  // Set the current window
  glfwMakeContextCurrent(window);

  // Set the shader program
  shader.use(shader_id);

  // Set the color
  shader.setVec3(shader_id, "color", color);

  // Set the pointsize
  shader.setFloat(shader_id, "pointsize", pointsize);

}

template <typename T>
void PointCloud<T>::update()
{
#ifdef DEBUG
  /*
  std::cout << "PointCloud::update: \
    Updating the point cloud" << std::endl;
    */
#endif

  // Check if the point cloud is visible
  if (!show) return;

  // Return if points are not set
  if (points == nullptr) return;

  // Set the current window
  glfwMakeContextCurrent(window);

  // Bind the VAO
  glBindVertexArray(VAO);

  // Bind the VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Copy the data
  glBufferData(GL_ARRAY_BUFFER, 3 * num_points * sizeof(T), 
      points, GL_DYNAMIC_DRAW);

  // Unbind the VBO
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Unbind the VAO
  glBindVertexArray(0);

}

template <typename T>
void PointCloud<T>::draw()
{
#ifdef DEBUG
  /*
  std::cout << "PointCloud::draw: \
    Drawing the point cloud" << std::endl;
  std::cout << "PointCloud::draw: \
    Number of points: " << num_points << std::endl;
  std::cout << "PointCloud::draw: \
    Pointsize: " << pointsize << std::endl;
  std::cout << "PointCloud::draw: \
    Name: " << name << std::endl;
  std::cout << "PointCloud::draw: \
    shader_id: " << shader_id << std::endl;
    */
#endif

  // Check if the point cloud is visible
  if (!show) return;

  // Set the current window
  glfwMakeContextCurrent(window);

  // Set the shader program
  shader.use(shader_id);

  // Set the color
  shader.setVec3(shader_id, "color", color);

  // Set the pointsize
  shader.setFloat(shader_id, "pointsize", pointsize);

  // Bind the VAO
  glBindVertexArray(VAO);

  // Draw the point cloud
  glDrawArrays(GL_POINTS, 0, num_points);

}

template <typename T>
void PointCloud<T>::setColor(Color<T> &color)
{
  this->color = color;
}

template <typename T>
Color<T> PointCloud<T>::getColor()
{
  return color;
}

template <typename T>
void PointCloud<T>::setShaderId(int shader_id)
{
  this->shader_id = shader_id;
}

template <typename T>
int PointCloud<T>::getShaderId()
{
  return shader_id;
}

template <typename T>
void PointCloud<T>::setWindow(GLFWwindow* window)
{
  this->window = window;
}

template <typename T>
GLFWwindow* PointCloud<T>::getWindow()
{
  return window;
}

template <typename T>
void PointCloud<T>::setPoints(T* points)
{
  this->points = points;
}

template <typename T>
T* PointCloud<T>::getPoints()
{
  return points;
}

template <typename T>
void PointCloud<T>::setNumPoints(int num_points)
{
  this->num_points = num_points;
}

template <typename T>
int PointCloud<T>::getNumPoints()
{
  return num_points;
}

template <typename T>
void PointCloud<T>::setPointsize(T pointsize)
{
  this->pointsize = (float) pointsize;
}
  
template <typename T>
T PointCloud<T>::getPointsize()
{
  return (T)pointsize;
}

template <typename T>
void PointCloud<T>::setName(const std::string &name)
{
  this->name = name;
}

template <typename T>
std::string PointCloud<T>::getName()
{
  return name;
}

template <typename T>
void PointCloud<T>::initBuffers()
{
  // Make sure the window is set
  if (window == nullptr)
  {
    std::cerr << "PointCloud::initBuffers: \
      Window is not set" << std::endl;
    return;
  }

  // Set the current window
  glfwMakeContextCurrent(window);

  // Generate buffers
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
}

template <typename T>
void PointCloud<T>::deleteBuffers()
{
  // Delete buffers
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}

// Explicit instantiation
template class PointCloud<float>;
template class PointCloud<double>;

