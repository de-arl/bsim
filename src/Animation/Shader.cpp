#include "Shader.h"

template <typename T>
void Shader<T>::init(
    const char* vertexPath,  
    const char* fragmentPath)
{
  // Init Glad
  initGlad();

  // init glfw
  if (glfwGetCurrentContext() == NULL) {
    std::cerr 
      << "Error in your code: initialize shader AFTER window setup" 
      << std::endl;
    exit(EXIT_FAILURE);
  }
  // 1. retrieve the vertex/fragment source code from filePath
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;
  // ensure ifstream objects can throw exceptions:
  vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  try 
  {
    // open files
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // convert stream into string
    vertexCode   = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  }
  catch (std::ifstream::failure& e)
  {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " 
      << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  const char* vShaderCode = vertexCode.c_str();
  const char * fShaderCode = fragmentCode.c_str();
  // 2. compile shaders
  unsigned int vertex, fragment;
  // vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  checkCompileErrors(vertex, "VERTEX");
  // fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  checkCompileErrors(fragment, "FRAGMENT");
  // shader Program
  ID[0] = glCreateProgram();
  glAttachShader(ID[0], vertex);
  glAttachShader(ID[0], fragment);
  glLinkProgram(ID[0]);
  checkCompileErrors(ID[0], "PROGRAM");
#ifdef DEBUG
  std::cout<<"Shader ID: "<<ID[0]<<std::endl;
#endif
  // delete the shaders as they're linked into 
  // our program now and no longer necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

template <typename T>
void Shader<T>::initGlad()
{
  int gl_version;
  gl_version = gladLoadGL(glfwGetProcAddress);
  if (!gl_version) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    throw;
  } 
}

template <typename T>
void Shader<T>::use(GLuint num_shader) 
{ 
  glUseProgram(ID[num_shader]); 
}
 
template <typename T>
void Shader<T>::setBool(
    GLuint num_shader, const std::string &name, bool value) const
{         
  glUniform1i(glGetUniformLocation(ID[num_shader], name.c_str()), (int)value); 
}

template <typename T>
void Shader<T>::setInt(
    GLuint num_shader, const std::string &name, int value) const
{ 
  glUniform1i(glGetUniformLocation(ID[num_shader], name.c_str()), value); 
}

template <typename T>
void Shader<T>::setFloat(
    GLuint num_shader, const std::string &name, T value) const
{ 
  glUniform1f(
      glGetUniformLocation(ID[num_shader], name.c_str()), (GLfloat) value); 
}

template <typename T>
void Shader<T>::setVec3(
    GLuint num_shader, const std::string &name, const T* value) const
{ 
  glUniform3fv(
      glGetUniformLocation(ID[num_shader], name.c_str()), 1, (GLfloat*)value); 
}

template <typename T>
void Shader<T>::setMat4(
    GLuint num_shader, const std::string &name, const GLfloat* value) const
{
  glUniformMatrix4fv(
      glGetUniformLocation(
        ID[num_shader], name.c_str()), 1, GL_FALSE, value);
}

template <typename T>
void Shader<T>::checkCompileErrors(unsigned int shader, std::string type)
{
  int success;
  char infoLog[1024];
  if (type != "PROGRAM")
  {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" 
        << infoLog << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
        << infoLog << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

// Explicit instantiation
template class Shader<float>;
template class Shader<double>;

/* vim: set ts=2 sw=2 tw=0 et :*/
