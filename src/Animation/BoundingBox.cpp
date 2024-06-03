#include "BoundingBox.h"


template <typename T>
BoundingBox<T>::BoundingBox(Shader<T> &shader)
  :dims{1.0f, 1.0f, 1.0f},
  VAO(0), VBO(0), EBO(0),
  shader(shader),
  color(1.0f, 1.0f, 1.0f),
  shader_id(0)
{
#ifdef DEBUG
  std::cout << "BoundingBox::BoundingBox: \
    Default constructor of BoundingBox" << std::endl;
#endif
}

template <typename T>
BoundingBox<T>::~BoundingBox()
{
#ifdef DEBUG
  std::cout << "BoundingBox::~BoundingBox: \
    Destructor of BoundingBox" << std::endl;
#endif

  // Delete vertex array object
  glDeleteVertexArrays(1, &VAO);

  // Delete vertex buffer object
  glDeleteBuffers(1, &VBO);

  // Delete element buffer object
  glDeleteBuffers(1, &EBO);
}

template <typename T>
void BoundingBox<T>::init(
    GLFWwindow* window, T dims[3], AnimationParameters<T> &anim_params)
{
#ifdef DEBUG
  std::cout << "BoundingBox::init: \
    Initializing the bounding box" << std::endl;
#endif
  // Set window
  setWindow(window);

  // Generate vertex array object
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // Set dimensions of bounding box
  setDims(dims);

  // Initialize the parameters
  initParams(anim_params);

  // Initialize the bounding box
  initBoundingBox();
}

template <typename T>
void BoundingBox<T>::init(T dims[3], AnimationParameters<T> &anim_params)
{
#ifdef DEBUG
  std::cout << "BoundingBox::init: \
    Initializing the bounding box" << std::endl;
#endif
  // Set dimensions of bounding box
  setDims(dims);

  // Initialize the parameters
  initParams(anim_params);

  // Initialize the bounding box
  initBoundingBox();
}

template <typename T>
void BoundingBox<T>::initParams(AnimationParameters<T> &anim_params)
{
#ifdef DEBUG
  std::cout << "BoundingBox::initParams: \
    Initializing the parameters of the bounding box" << std::endl;
#endif
  // Set color
  setColor(anim_params.bounding_box_color);

  // Set shader program
  setShaderId(anim_params.shader_id);
}

template <typename T>
void BoundingBox<T>::initBoundingBox()
{
#ifdef DEBUG
  std::cout << "BoundingBox::init: \
    Initializing the bounding box" << std::endl;
  std::cout << "BoundingBox::init: \
    Dimensions of the bounding box: " << dims[0] << ", " << dims[1] << ", " 
    << dims[2] << std::endl;
#endif

  // Set dimensions of bounding box
  GLfloat x = (GLfloat) dims[0];
  GLfloat y = (GLfloat) dims[1];
  GLfloat z = (GLfloat) dims[2];

  // Set vertices of bounding box
  GLfloat vertices[] =
  {
    -x, -y, -z,
     x, -y, -z,
     x,  y, -z,
    -x,  y, -z,
    -x, -y,  z,
     x, -y,  z,
     x,  y,  z,
    -x,  y,  z
  };

  // Set indices of bounding box
  GLuint indices[] =
  {
    0, 1, 3, 2, 4, 5, 7, 6,
    0, 3, 1, 2, 5, 6, 4, 7,
    0, 4, 1, 5, 2, 6, 3, 7
  };

  // Make the window's context current
  glfwMakeContextCurrent(window);

  // Bind vertex array object
  glBindVertexArray(VAO);

  // Bind vertex buffer object (allocated on GPU)
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // Copy vertices to vertex buffer on GPU
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Bind element buffer object (allocated on GPU)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  // Copy indices to element buffer on GPU
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Set vertex attribute pointers
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  // Enable vertex attribute array
  glEnableVertexAttribArray(0);

  // Unbind buffer
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Unbind vertex array object
  glBindVertexArray(0);
}

template <typename T>
void BoundingBox<T>::draw()
{
#ifdef DEBUG
  //std::cout << "BoundingBox::draw: \
   // Drawing the bounding box" << std::endl;
#endif
  // Make the window's context current
  glfwMakeContextCurrent(window);

  // Set antialiasing
  glEnable(GL_LINE_SMOOTH);

  // Bind vertex array object
  glBindVertexArray(VAO);

  // Activate shader
  shader.use(shader_id);

  // Set color
  shader.setVec3(shader_id, "color", color);

  // Draw bounding box
  glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

  // Unbind vertex array object
  glBindVertexArray(0);

  // Unset antialiasing
  glDisable(GL_LINE_SMOOTH);
}

template <typename T>
void BoundingBox<T>::setColor(Color<T> &color)
{
#ifdef DEBUG
  std::cout << "BoundingBox::setColor: \
    Setting the color of the bounding box" << std::endl;
#endif
  this->color = color;
}

template <typename T>
Color<T> BoundingBox<T>::getColor()
{
#ifdef DEBUG
  /*
  std::cout << "BoundingBox::getColor: \
    Getting the color of the bounding box" << std::endl;
    */
#endif
  return color;
}

template <typename T>
void BoundingBox<T>::setShaderId(int shader_id)
{
#ifdef DEBUG
  std::cout << "BoundingBox::setShaderId: \
    Setting the shader program of the bounding box" << std::endl;
#endif
  this->shader_id = shader_id;
}

template <typename T>
void BoundingBox<T>::setDims(T dims[3])
{
#ifdef DEBUG
  std::cout << "BoundingBox::setDims: \
    Setting the dimensions of the bounding box" << std::endl;
#endif
  this->dims[0] = dims[0];
  this->dims[1] = dims[1];
  this->dims[2] = dims[2];
}

template <typename T>
void BoundingBox<T>::getDims(float* dims)
{
#ifdef DEBUG
  std::cout << "BoundingBox::getDims: \
    Getting the dimensions of the bounding box" << std::endl;
#endif
  dims[0] = this->dims[0];
  dims[1] = this->dims[1];
  dims[2] = this->dims[2];
}

template <typename T>
void BoundingBox<T>::setWindow(GLFWwindow* window)
{
#ifdef DEBUG
  std::cout << "BoundingBox::setWindow: \
    Setting the window of the bounding box" << std::endl;
#endif
  this->window = window;
}

// Explicit instantiation
template class BoundingBox<float>;
template class BoundingBox<double>;
