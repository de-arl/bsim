#include "Axes.h"

template <typename T>
Axes<T>::Axes(Shader<T> &shader)
  :shader(shader),
  shader_id(0),
  axes_color(1.0f, 1.0f, 1.0f),
  tick_color(1.0f, 1.0f, 1.0f),
  label_color(1.0f, 1.0f, 1.0f),
  n_ticks(10),
  num_ticks(0)
{
  // Set the tick length
  for (int i = 0; i < 3; i++){
    tick_length[i] = 0.03f;
    tick_length[i+3] = 0.0f;
  }
}

template <typename T>
Axes<T>::~Axes()
{
  // Delete the vertex array object, vertex buffer object, 
  // and element buffer object
  glDeleteVertexArrays(2, VAO);
  glDeleteBuffers(2, VBO);
  glDeleteBuffers(2, EBO);
}

template <typename T>
void Axes<T>::init(GLFWwindow* window, T dims[3],
    AnimationParameters<T> &anim_params)
{
  // Set the window
  setWindow(window);

  // Set the dimensions of the axes
  setDims(dims);

  // Initialize the parameters
  initParams(anim_params);

  // Initialize the axes
  initAxes();

  // Initialize the ticks
  generateTicks();
}

template <typename T>
void Axes<T>::init(T dims[3], AnimationParameters<T> &anim_params)
{
  // Set the dimensions of the axes
  setDims(dims);

  // Initialize the parameters
  initParams(anim_params);

  // Initialize the axes
  initAxes();

  // Initialize the ticks
  generateTicks();
}

template <typename T>
void Axes<T>::initParams(AnimationParameters<T> &anim_params)
{
  // Set the number of ticks
  setNumTicks(anim_params.num_ticks);

  // Set the tick length
  setTickLength(anim_params.tick_length);

  // Set the axes color
  setAxesColor(anim_params.axes_color);

  // Set the tick color
  setTickColor(anim_params.tick_color);

  // Set the label color
  setLabelColor(anim_params.label_color);

  // Set the shader program
  setShaderId(anim_params.shader_id);
}

template <typename T>
void Axes<T>::initAxes()
{
  // Tipsize
  GLfloat tipsize = 0.05f;
  GLfloat tipwidth = (1.0f - 0.618f) * tipsize;

  // Letter position
  GLfloat lpos[3];
  for (int i = 0; i < 3; i++){
    lpos[i] = dims[i] + 1.618f * tipsize * 1.618f;
  }
  // Letter size
  GLfloat lsize = tipsize * 1.618f;
  GLfloat hsize = lsize / 2.0f;

  // Set the vertices of the axes
  GLfloat vertices[114] = {
    // x-axis
    -dims[0], 0.0f, 0.0f,             //0
    dims[0]-tipsize, 0.0f, 0.0f,
    // y-axis
    0.0f, -dims[1], 0.0f,
    0.0f, dims[1]-tipsize, 0.0f,
    // z-axis
    0.0f, 0.0f, -dims[2],
    0.0f, 0.0f, dims[2]-tipsize,
    // x-axis tip
    dims[0], 0.0f, 0.0f,              //6
    dims[0]-tipsize, tipwidth, 0.0f,
    dims[0]-tipsize, -tipwidth, 0.0f,
    dims[0], 0.0f, 0.0f,
    dims[0]-tipsize, 0.0f, tipwidth,
    dims[0]-tipsize, 0.0f, -tipwidth,
    // y-axis tip                     //12
    0.0f, dims[1], 0.0f,
    tipwidth, dims[1]-tipsize, 0.0f,
    -tipwidth, dims[1]-tipsize, 0.0f,
    0.0f, dims[1], 0.0f,
    0.0f, dims[1]-tipsize, tipwidth,
    0.0f, dims[1]-tipsize, -tipwidth,
    // z-axis tip
    0.0f, 0.0f, dims[2],             //18 
    0.0f, tipwidth, dims[2]-tipsize, 
    0.0f, -tipwidth, dims[2]-tipsize,
    0.0f, 0.0f, dims[2],
    tipwidth, 0.0f, dims[2]-tipsize,
    -tipwidth, 0.0f, dims[2]-tipsize,
    // X letter
    lpos[0]-hsize, hsize, 0.0f,              //24
    lpos[0]+hsize, -hsize, 0.0f,
    lpos[0]-hsize, -hsize, 0.0f,
    lpos[0]+hsize, hsize, 0.0f,
    // Y letter
    -hsize, lpos[1]-hsize, 0.0f,              //28
    hsize, lpos[1]+hsize, 0.0f,
    -hsize, lpos[1]+hsize, 0.0f,
    0.0f, lpos[1], 0.0f,
    // Z letter
    0.0f, hsize, lpos[2]-hsize,              //32
    0.0f, hsize, lpos[2]+hsize,
    0.0f, -hsize, lpos[2]-hsize,
    0.0f, -hsize, lpos[2]+hsize,
    0.0f, -hsize, lpos[2]-hsize,
    0.0f, hsize, lpos[2]+hsize               
    //38*3=114
  };

  // Set the indices of the axes
  GLint indices[40] = {
    // x-axis
    0, 1, 
    // y-axis
    2, 3, 
    // z-axis
    4, 5,
    // x-axis tip
    6, 7, 8, 9, 10, 11,
    // y-axis tip
    12, 13, 14, 15, 16, 17,
    // z-axis tip
    18, 19, 20, 21, 22, 23,
    // X letter
    24, 25, 26, 27,
    // Y letter
    28, 29, 30, 31,
    // Z letter
    32, 33, 34, 35, 36, 37
  };

  // Make window the current context
  glfwMakeContextCurrent(window);

  // Generate the vertex array object, vertex buffer object, 
  // and element buffer object
  glGenVertexArrays(1, &VAO[0]); glGenBuffers(1, &VBO[0]);
  glGenBuffers(1, &EBO[0]);

  // Bind the vertex array object
  glBindVertexArray(VAO[0]);

  // Bind the vertex buffer object
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(
      GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Bind the element buffer object
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
      sizeof(indices), indices, GL_STATIC_DRAW);

  // Set the vertex
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  // Unbind the vertex array object
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template <typename T>
void Axes<T>::generateTicks()
{
  // Vector of vertices and indices
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices;

  // Tick distance
  GLfloat delta = 2.0f / n_ticks;

  // Loop over the length of the axes
  // x-axis
  unsigned counter = 0;
  unsigned index = 0;
  GLfloat x = -dims[0]+delta;

  while (x < dims[0]-0.05f){
    if (nearlyEqual(x, 0.0f)){
      x = -dims[0] + counter * delta + delta;
      counter++;
      continue;
    }
    vertices.insert(vertices.end(), 
        {x, tick_length[2], 0.0f, x, -tick_length[2], 0.0f});
    vertices.insert(vertices.end(),
        {x, 0.0f, tick_length[4], x, 0.0f, -tick_length[4]});
    indices.insert(indices.end(), {4*index, 4*index+1});
    indices.insert(indices.end(), {4*index+2, 4*index+3});
    counter++;
    index++;
    x = -dims[0] + counter * delta + delta;
  }

  // y-axis
  counter = 0;
  GLfloat y = -dims[1]+delta;

  while (y < dims[1]-0.05f){
    if (nearlyEqual(y, 0.0f)){
      y = -dims[1] + counter * delta + delta;
      counter++;
      continue;
    }
    vertices.insert(vertices.end(), 
        {tick_length[0], y, 0.0f, -tick_length[0], y, 0.0f});
    vertices.insert(vertices.end(),
        {0.0f, y, tick_length[5], 0.0f, y, -tick_length[5]});
    indices.insert(indices.end(), {4*index, 4*index+1});
    indices.insert(indices.end(), {4*index+2, 4*index+3});
    counter++;
    index++;
    y = -dims[1] + counter * delta + delta;
  }

  // z-axis
  counter = 0;
  GLfloat z = -dims[2]+delta;

  while (z < dims[2]-0.05f){
    if (nearlyEqual(z, 0.0f)){
      z = -dims[2] + counter * delta + delta;
      counter++;
      continue;
    }
    vertices.insert(vertices.end(), 
        {tick_length[1], 0.0f, z, -tick_length[1], 0.0f, z});
    vertices.insert(vertices.end(),
        {0.0f, tick_length[3], z, 0.0f, -tick_length[3], z});
    indices.insert(indices.end(), {4*index, 4*index+1});
    indices.insert(indices.end(), {4*index+2, 4*index+3});
    counter++;
    index++;
    z = -dims[2] + counter * delta + delta;
  }

  // Set the total number of ticks
  num_ticks = vertices.size() / 3;

  // Make the window the current context
  glfwMakeContextCurrent(window);

  // Generate the vertex array object, vertex buffer object,
  // and element buffer object
  glGenVertexArrays(1, &VAO[1]); glGenBuffers(1, &VBO[1]);
  glGenBuffers(1, &EBO[1]);

  // Bind the vertex array object
  glBindVertexArray(VAO[1]);

  // Bind the vertex buffer object
  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

  // Copy vertices to vertex buffer on GPU
  glBufferData(
      GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), 
      vertices.data(), GL_STATIC_DRAW);

  // Bind the element buffer object
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);

  // Copy indices to element buffer on GPU
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), 
      indices.data(), GL_STATIC_DRAW);

  // Set the vertex attribute pointers
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);

  // Unbind the buffers
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template <typename T>
void Axes<T>::drawAxes()
{
  // Make the window the current context
  glfwMakeContextCurrent(window);

  // Set antialiasing
  glEnable(GL_LINE_SMOOTH);

  // Use the shader program
  shader.use(shader_id);

  // Bind the vertex array object
  glBindVertexArray(VAO[0]);

  // Set color
  shader.setVec3(shader_id, "color", axes_color);

  // Draw the axes
  glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);

  // Draw the axes tips
  glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, (void*)(6*sizeof(GLuint)));

  // Unbind the vertex array object
  glBindVertexArray(0);

  // Unset antialiasing
  glDisable(GL_LINE_SMOOTH);
}

template <typename T>
void Axes<T>::drawLabels()
{
  // Make the window the current context
  glfwMakeContextCurrent(window);

  // Set antialiasing
  glEnable(GL_LINE_SMOOTH);

  // Use the shader program
  shader.use(shader_id);

  // Bind the vertex array object
  glBindVertexArray(VAO[0]);

  // Set color
  shader.setVec3(shader_id, "color", label_color);

  // Draw the letters
  glDrawElements(GL_LINES, 16, GL_UNSIGNED_INT, (void*)(24*sizeof(GLuint)));

  // Unbind the vertex array object
  glBindVertexArray(0);

  // Unset antialiasing
  glDisable(GL_LINE_SMOOTH);
}

template <typename T>
void Axes<T>::drawTicks()
{
  // Make the window the current context
  glfwMakeContextCurrent(window);

  // Use the shader program
  shader.use(shader_id);

  // Set color
  shader.setVec3(shader_id, "color", tick_color);

  // Bind the vertex array object
  glBindVertexArray(VAO[1]);

  // Draw the ticks
  glDrawElements(GL_LINES, 2*num_ticks, GL_UNSIGNED_INT, 0);

  // Unbind the vertex array object
  glBindVertexArray(0);
}

template <typename T>
void Axes<T>::draw(
    bool draw_axes, bool draw_ticks, bool draw_labels)
{
  // Draw the axes
  if (draw_axes) {drawAxes();}

  // Draw the ticks
  if (draw_ticks) {drawTicks();}

  // Draw the labels
  if (draw_labels) {drawLabels();}
}

template <typename T>
void Axes<T>::setColor(Color<T> &color)
{
  this->axes_color = color;
  this->tick_color = color;
  this->label_color = color;
}

template <typename T>
Color<T> Axes<T>::getAxesColor()
{
  return axes_color;
}

template <typename T>
Color<T> Axes<T>::getTickColor()
{
  return tick_color;
}

template <typename T>
Color<T> Axes<T>::getLabelColor()
{
  return label_color;
}

template <typename T>
void Axes<T>::setTickColor(Color<T> &color)
{
  this->tick_color = color;
}

template <typename T>
void Axes<T>::setLabelColor(Color<T> &color)
{
  this->label_color = color;
}

template <typename T>
void Axes<T>::setAxesColor(Color<T> &color)
{
  this->axes_color = color;
}

template <typename T>
void Axes<T>::setShaderId(int shader_id)
{
  this->shader_id = shader_id;
}

template <typename T>
void Axes<T>::setWindow(GLFWwindow* window)
{
  this->window = window;
}

template <typename T>
void Axes<T>::setNumTicks(int n_ticks)
{
  this->n_ticks = n_ticks;
}

template <typename T>
int Axes<T>::getNumTicks()
{
  return n_ticks;
}

template <typename T>
void Axes<T>::setTickLength(float tick_length[6])
{
  for (int i = 0; i < 6; i++)
    this->tick_length[i] = tick_length[i];
}

template <typename T>
void Axes<T>::getTickLength(float* tick_length)
{
  for (int i = 0; i < 6; i++)
    tick_length[i] = this->tick_length[i];
}

template <typename T>
void Axes<T>::getDims(float* dims)
{
  for (int i = 0; i < 3; i++)
    dims[i] = this->dims[i];
}

template <typename T>
void Axes<T>::setDims(T dims[3])
{
  for (int i = 0; i < 3; i++)
    this->dims[i] = (float)dims[i];
}


// Explicit instantiation
template class Axes<float>;
template class Axes<double>;



