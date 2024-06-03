#ifndef ANIMATION_H
#define ANIMATION_H

#include <omp.h>
#include <filesystem> // create directories
//#include <stdio.h>
//#include <fstream>
//#include <string>
//#include <cstring>
//#include <cstdint>
//#include <unistd.h>
#include <fcntl.h>
//#include <vector>
//#include <numeric>
#include <webp/encode.h>
#include <webp/mux.h>
//#include <thread>


#ifndef OPENGL_INITIALIZED
#define OPENGL_INITIALIZED
#define GLAD_GL_IMPLEMENTATION
#include "gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif //OPENGL_INITIALIZED

#define GLM_FORCE_INTRINSICS
#include <glm/glm.hpp> // vec3, vec4, ivec4, mat4
#include <glm/gtc/matrix_transform.hpp> // translate, rotate, scale, perspective
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <glm/gtc/constants.hpp> 

#include "helpers.h"
#include "StateMachine.h"
#include "Shader.h"
#include "AnimationParameters.h"
#include "SimulationParameters.h"
#include "GlobalParameters.h"
#include "BoundingBox.h"
#include "Grid.h"
#include "Axes.h"
#include "Color.h"
#include "PointCloud.h"
#include "ImGuiHandler.h"
#include "Simulation.h"


template <typename T>
class Animation
{

 public:

  /* Public functions */
  Animation(Simulation<T> &simulation, 
      StateMachine<T> &state_machine, 
      GlobalParameters<T> &global_parameters);
  ~Animation();
  void runGui();
 // void runSimulation();
  bool &isRunning();
 // void restart();

 private: 

  /* Private functions */

  // Initialize the animation
  void init();

  // Create a GLFW window
  GLFWwindow* initWindow(const std::string title);

  // Initialize the GLFW window
  void initGlfwWindow();

  // Load OpenGL function pointers
  void initGlad();

  // Initialize mouse input handling
  void initMouse();

  // Initialize the webp encoder
  void initWebp();

  // Initialize a simulation run
  void initSimulation();

  // Create folders for frames and movies
  void initFrameDirectory();
  void initMovieDirectory();

  // Handle mouse input
  void handleMouseInput();

  // Create a projection view matrix
  glm::mat4 createProjectionViewMatrix(
      glm::vec3 const& in_rotate, glm::vec3 const& center, GLfloat zoom, 
      glm::vec3 const& up);

  // Set the camera view
  void setCameraView(
      glm::vec3 const& in_rotate, glm::vec3 const& center, GLfloat zoom, 
      glm::vec3 const& up);


  /* Private variables */

  // Pointer to the GLFW window
  GLFWwindow *window;

  // Pointers to the state machine and the parameters
  StateMachine<T> *state_machine;

  // Reference to the simulation
  Simulation<T> &simulation;

  // Reference to the global parameters
  GlobalParameters<T> &global_params;

  // Reference to the animation parameters
  AnimationParameters<T> &anim_params;

  // Copy of the simulation parameters
  SimulationParameters<T> sim_params;

  // Copy of the scaled simulation parameters
  SimulationParameters<T> scaled_sim_params;
  
  // The vertex shader
  Shader<T> shader;

  // The imgui menu interface
  ImGuiHandler<T> imgui_handler;

  // Bounding box
  BoundingBox<T> bounding_box;

  // Grid
  Grid<T> grid;

  // Axes
  Axes<T> axes;

  // Point clouds 
  std::vector<PointCloud<T>> point_clouds;

  // Output directory for the frames and movies
  std::string frame_folder = "/frames";
  std::string movie_folder = "/movies";
  
  // Input directory for the shaders
  const std::string shader_directory;

  // The window title
  const std::string window_title;

  // Rotation parameters
  GLfloat mouse_zoom_offset = 0.0f;
  GLfloat rotate_scale = 0.008f;
  GLfloat shift_scale = 0.00008f;

  // Camera parameters
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 eye = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 center = {0.0f, 0.0f, 0.0f};
  glm::vec3 rotate_view = {0.0f, -1.0f, 0.0f}; 
  glm::vec3 camera_view = {0.0f, 0.0f, 0.0f};

  // Flag to indicate wether the animation is finished
  bool finalized;

  bool auto_gridside[6] = {true, true, true, true, true, true};

  // Flag to indicate wether the animation is running
  bool running;
  FILE *ffmpeg_process = nullptr;
  FILE *frame_file = nullptr;
  std::string current_ffmpeg_dir;
  WebPConfig webp_config;
  WebPPicture webp_pic;
  WebPData webp_data;
  WebPAnimEncoderOptions anim_config;
  WebPAnimEncoder *webp_encoder;
  int webp_animation_count = 0;
  int webp_frame_timestamp = 0; // milliseconds
  int webp_frame_duration = 17; // milliseconds, 60 fps
  int current_frame_index = 0;
  int movie_counter = 0;
  glm::vec3 rotate_frame = {0.0f, 0.0f, 0.0f};
  glm::vec3 rotation_offset = {0.0f, 0.0f, 0.0f};
  GLfloat frame_zoom = 0.0f;
  bool rotate_scene = false;
  bool write_movie = false;
  bool ffmpeg_running = false;  

  // Flag to indicate wether the animation is finished
  bool webp_initialized = false;

  /* imgui status variables */
  bool ig_frame_use_current_view = false;
  std::string ffmpeg_command;
  /* Private functions */
  void selectGrids(glm::vec3 view);
  void drawBuffers(glm::vec3 view);
  void render(glm::vec3 view);
  void drawGraphicalObjects(glm::vec3 view);
  void handleWindowResize(bool &resized);

  /* Read pixels from framebuffer */
  bool readFramebuffer(uint8_t *target_);

  /* Write single frame as webp*/
  void saveFrame(); // call before appendFrameToMovie

  /* Write frames to disk */
  void appendFrameToMovie();

  /* Render frame horizontally flipped for webp */
  void renderFrame();

  /* Add rendered frame to animated webp */
  void appendFrameToAnimatedWebp();

  /* Write webp picture to disk */
  void exportWebpPictureToDisk(WebPPicture *pic);

  /* Finalize the animated webp */
  void finalizeWebp();

  /* Start ffmpeg process */
  void initFFmpeg();
  void checkFFmpegPipe();

  /* Handle the gui */
  void GUI();

  void finalizeFFmpeg();
  void finalize();

  /* Reset the animation */
  void reset();

  // declare imgui handler as friend
  template <typename U> friend class ImGuiHandler;

};

#endif //ANIMATION_H
/* vim: set ts=2 sw=2 tw=0 et :*/
