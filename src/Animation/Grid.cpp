#include "Grid.h"

template <typename T>
Grid<T>::Grid(Shader<T> &shader)
  : shader(shader),
  num_gridlines_1D(0),
  num_gridlines{0, 0, 0, 0, 0, 0},
  color(0.5, 0.5, 0.5),
  shader_id(0)
{
#ifdef DEBUG
  std::cout << "Grid::Grid Default constructor" << std::endl;
#endif
}

template <typename T>
Grid<T>::~Grid()
{
#ifdef DEBUG
  std::cout << "Grid::~Grid Default destructor" << std::endl;
#endif
  glDeleteVertexArrays(6, VAO);
  glDeleteBuffers(6, VBO);
  glDeleteBuffers(6, EBO);
}

template <typename T>
void Grid<T>::init(GLFWwindow* window, T dims[3],
    AnimationParameters<T> &anim_params)
{
#ifdef DEBUG
  std::cout << "Grid::init Initialization of grids" << std::endl;
#endif

  // Set the window
  setWindow(window);

  // Generate buffers
  glGenVertexArrays(6, VAO);
  glGenBuffers(6, VBO);
  glGenBuffers(6, EBO);

  // Set the dimensions of the grid
  setDims(dims);

  // Init parameters
  initParams(anim_params);

  // Generate the grids
  generateGrids();
}

template <typename T>
void Grid<T>::init(T dims[3], AnimationParameters<T> &anim_params)
{
#ifdef DEBUG
  std::cout << "Grid::init Initialization of grids" << std::endl;
#endif

  // Set the dimensions of the grid
  setDims(dims);

  // Init parameters
  initParams(anim_params);

  // Generate the grids
  generateGrids();
}

template <typename T>
void Grid<T>::initParams(AnimationParameters<T> &anim_params)
{
#ifdef DEBUG
  std::cout << "Grid::initParams Initialization of grids" << std::endl;
#endif
  
    // Set the number of gridlines
    setNumGridlines(anim_params.num_gridlines);
  
    // Set the color
    setColor(anim_params.grid_color);
  
    // Set the grids to draw
    setToDraw(anim_params.draw_grid);
  
    // Set the shader program
    setShaderId(anim_params.shader_id);
}

template <typename T>
void Grid<T>::generateGrids()
{
#ifdef DEBUG
  std::cout << "Grid::generateGrids Generating six grids" << std::endl;
#endif
  // Set dimensions of the grid
  this->dims[0] = dims[0];
  this->dims[1] = dims[1];
  this->dims[2] = dims[2];

  // Convenience variables
  GLfloat x = dims[0];
  GLfloat y = dims[1];
  GLfloat z = dims[2];

  // Set up counter for the number of lines
  int lines = 0;
  int counter = 0;

  // Set number of gridlines in each dimension to zero
  memset(num_gridlines, 0, 6*sizeof(int));

  // Flush the vertices
  for (int i=0; i<6; i++) vertices[i].clear();

  // Calculate the space in between the gridlines
  T delta = 2.0f / (T)num_gridlines_1D;

  // Calculate the endpoints of the gridlines
  // xy-plane, lower x
  GLfloat grid = 0.0f; // Current axis position
  while (grid < 1.0f){ // Loop over the gridlines, using symmetry
    grid = (T)counter * delta;
    if (grid <= y){
      vertices[0].insert(vertices[0].end(), { -x, -grid, -z,  -x, -grid, z});
      vertices[0].insert(vertices[0].end(), { -x,  grid, -z,  -x,  grid, z});
      lines+=2; num_gridlines[0]+=2;
    }
    if (grid <= z){  
      vertices[0].insert(vertices[0].end(), { -x, -y, -grid, -x, y, -grid});
      vertices[0].insert(vertices[0].end(), { -x, -y,  grid, -x, y,  grid});
      lines+=2; num_gridlines[0]+=2;
    }
    counter++;
  }

  // xy-plane, upper x
  // Reset counter and grid
  counter = 0;
  grid = 0.0f;
  while (grid < 1.0f){
    grid = (T)counter * delta;
    if (grid <= y){
      vertices[1].insert(vertices[1].end(), { x, -grid, -z,  x, -grid, z});
      vertices[1].insert(vertices[1].end(), { x,  grid, -z,  x,  grid, z});
      lines+=2; num_gridlines[1]+=2;
    }
    if (grid <= z){  
      vertices[1].insert(vertices[1].end(), { x, -y, -grid, x, y, -grid});
      vertices[1].insert(vertices[1].end(), { x, -y,  grid, x, y,  grid});
      lines+=2; num_gridlines[1]+=2;
    }
    counter++;
  }

  // xz-plane, lower y
  counter = 0;
  grid = 0.0f;
  while (grid < 1.0f){
    grid = (T)counter * delta;
    if (grid <= x){
      vertices[2].insert(vertices[2].end(), { -grid, -y, -z, -grid, -y, z});
      vertices[2].insert(vertices[2].end(), {  grid, -y, -z,  grid, -y, z});
      lines+=2; num_gridlines[2]+=2;
    }
    if (grid <= z){  
      vertices[2].insert(vertices[2].end(), { -x, -y, -grid, x, -y, -grid});
      vertices[2].insert(vertices[2].end(), { -x, -y,  grid, x, -y,  grid});
      lines+=2; num_gridlines[2]+=2;
    }
    counter++;
  }

  // xz-plane, upper y
  counter = 0;
  grid = 0.0f;
  while (grid < 1.0f){
    grid = (T)counter * delta;
    if (grid <= x){
      vertices[3].insert(vertices[3].end(), { -grid, y, -z, -grid, y, z});
      vertices[3].insert(vertices[3].end(), {  grid, y, -z,  grid, y, z});
      lines+=2; num_gridlines[3]+=2;
    }
    if (grid <= z){  
      vertices[3].insert(vertices[3].end(), { -x, y, -grid, x, y, -grid});
      vertices[3].insert(vertices[3].end(), { -x, y,  grid, x, y,  grid});
      lines+=2; num_gridlines[3]+=2;
    }
    counter++;
  }

  // yz-plane, lower z
  counter = 0;
  grid = 0.0f;
  while (grid < 1.0f){
    grid = (T)counter * delta;
    if (grid <= x){  
      vertices[4].insert(vertices[4].end(), {-grid, -y, -z, -grid, y, -z});
      vertices[4].insert(vertices[4].end(), { grid, -y, -z,  grid, y, -z});
      lines+=2; num_gridlines[4]+=2;
    }
    if (grid <= y){
      vertices[4].insert(vertices[4].end(), { -x, -grid, -z,  x, -grid, -z});
      vertices[4].insert(vertices[4].end(), { -x,  grid, -z,  x,  grid, -z});
      lines+=2; num_gridlines[4]+=2;
    }
    counter++;
  }

  // yz-plane, upper z
  counter = 0;
  grid = 0.0f;
  while (grid < 1.0f){
    grid = (T)counter * delta;
    if (grid <= x){  
      vertices[5].insert(vertices[5].end(), {-grid, -y, z, -grid, y, z});
      vertices[5].insert(vertices[5].end(), { grid, -y, z,  grid, y, z});
      lines+=2; num_gridlines[5]+=2;
    }
    if (grid <= y){
      vertices[5].insert(vertices[5].end(), { -x, -grid, z,  x, -grid, z});
      vertices[5].insert(vertices[5].end(), { -x,  grid, z,  x,  grid, z});
      lines+=2; num_gridlines[5]+=2;
    }
    counter++;
  }

  // Upload the vertices to the GPU
  for (int i=0; i<6; i++){

    // Generate indices
    std::vector<GLuint> indices(2*num_gridlines[i]);

    // Fill indices with sequence 0,1,2...2*lines-1
    std::iota(indices.begin(), indices.end(), 0);

    // Bind vertex array object
    glBindVertexArray(VAO[i]);

    // Bind vertex buffer object (allocated on GPU)
    glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);

    // Copy vertices to vertex buffer on GPU
    glBufferData(
        GL_ARRAY_BUFFER, 
        vertices[i].size()*sizeof(GLfloat), 
        vertices[i].data(), GL_STATIC_DRAW);

    // Bind element bufer object (allocated on GPU)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);

    // Copy indices to element buffer on GPU
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Enable vertex attribute array
    glEnableVertexAttribArray(0);

    // Unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  
}

template <typename T>
void Grid<T>::drawGrid(int i)
{
#ifdef DEBUG
  //std::cout << "Grid::drawGrid Drawing grid number: " << i << std::endl;
#endif
  // Set current window
  glfwMakeContextCurrent(window);

  // Set the shader program
  shader.use(shader_id);

  // Set the color
  shader.setVec3(0, "color", color);

  // Bind vertex array object
  glBindVertexArray(VAO[i]);

  // Draw the grid
  glDrawElements(GL_LINES, 2*num_gridlines[i], GL_UNSIGNED_INT, 0);

  // Unbind vertex array object
  glBindVertexArray(0);
}

template <typename T>
void Grid<T>::draw()
{
#ifdef DEBUG
 // std::cout << "Grid::draw Drawing selected grids" << std::endl;
#endif
  // Draw the grids
  for (int i=0; i<6; i++){
    if (draw_grid[i]) drawGrid(i);
  }
}

template <typename T>
void Grid<T>::setNumGridlines(int num_gridlines_1D)
{
#ifdef DEBUG
  std::cout << "Grid::setNumGridlines to: " << num_gridlines_1D << std::endl;
#endif
  // Set the number of gridlines
  this->num_gridlines_1D = num_gridlines_1D;
}

template <typename T>
int Grid<T>::getNumGridlines()
{
#ifdef DEBUG
  /*
  std::cout 
    << "Grid::getNumGridlines Num gridlines: " 
    << num_gridlines_1D << std::endl;
    */
#endif
  // Get the number of gridlines
  return this->num_gridlines_1D;
}

template <typename T>
void Grid<T>::setColor(Color<T> &color)
{
#ifdef DEBUG
  std::cout << "Grid::setColor to: " << color << std::endl;
#endif
  // Set the color
  this->color = color;
}

template <typename T>
Color<T> Grid<T>::getColor()
{
#ifdef DEBUG
 // std::cout << "Grid::getColor Color: " << color << std::endl;
#endif

  // Get the color
  return this->color;
}

template <typename T>
void Grid<T>::setShaderId(int shader_id)
{
#ifdef DEBUG
  std::cout << "Grid::setShaderId to: " << shader_id << std::endl;
#endif
  // Set the shader program
  this->shader_id = shader_id;
}

template <typename T>
void Grid<T>::setWindow(GLFWwindow* window)
{
#ifdef DEBUG
  std::cout << "Grid::setWindow to: " << window << std::endl;
#endif
  // Set the window
  this->window = window;
}

template <typename T>
void Grid<T>::setDims(T dims[3])
{
#ifdef DEBUG
  std::cout << "Grid::setDims to: " 
    << dims[0] << " " << dims[1] << " " << dims[2] << std::endl;
#endif
  // Set the dimensions of the grid
  this->dims[0] = (GLfloat) dims[0];
  this->dims[1] = (GLfloat) dims[1];
  this->dims[2] = (GLfloat) dims[2];
}

template <typename T>
void Grid<T>::setToDraw(bool draw_grid[6])
{
#ifdef DEBUG
  /*
  std::cout << "Grid::setToDraw to: " 
    << draw_grid[0]
    << draw_grid[1]
    << draw_grid[2]
    << draw_grid[3]
    << draw_grid[4]
    << draw_grid[5]
    << std::endl;
    */
#endif
  // Set the grids to draw
  for (int i=0; i<6; i++) this->draw_grid[i] = draw_grid[i];
}

template <typename T>
void Grid<T>::getToDraw(bool draw_grid[6])
{
#ifdef DEBUG
  std::cout << "Grid::getToDraw draw_grid: " 
    << draw_grid[0]
    << draw_grid[1]
    << draw_grid[2]
    << draw_grid[3]
    << draw_grid[4]
    << draw_grid[5]
    << std::endl;
#endif
  // Get the grids to draw
  for (int i=0; i<6; i++) draw_grid[i] = this->draw_grid[i];
}

// Explicit instantiation
template class Grid<float>;
template class Grid<double>;
