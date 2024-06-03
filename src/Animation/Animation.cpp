#include "Animation.h"
#define TOP true
#define BOTTOM false

/* Called if some glfw errors happen in the window context */
static void error_callback(int error, const char *description)
{
  fprintf(stderr, "Error: %s\n", description);
  throw;
}

// Global accross file
bool run_gui = true;

/* Catches keypresses and triggers set actions */
static void key_callback(
    GLFWwindow* window, int key, int scancode, int action, int mods)
{
  // Set window to close if escape key is pressed
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    //glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
  // Set window to close if ctrl+q key is pressed
  if (key == GLFW_KEY_Q && action == GLFW_PRESS){
    if (mods == GLFW_MOD_CONTROL){
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  }
}
bool mirror = false;
unsigned char *image_;
unsigned char *frame_image_;
int width, height;
bool window_resized = false;
int memory_size = 0;
bool memory_allocated = false;
bool memory_lock = true;

static void allocateMemory()
{
  memory_lock = true;
  if (memory_allocated){
    free(image_);
    free(frame_image_);
    memory_allocated = false;
  }
  memory_size = 3*width*height;
  image_ = (unsigned char*) calloc(memory_size, sizeof(unsigned char));
  frame_image_ = (unsigned char*) calloc(memory_size, sizeof(unsigned char));
  memory_allocated = true;
  memory_lock = false;
}

/* Called when window size changes to make rendering fit window size */
static void framebuffer_size_callback(GLFWwindow* window, int wdth, int hght)
{
  window_resized = true;
  glViewport(0,0,wdth,hght);
  width = wdth;
  height = hght;
}

bool finalize = false;
static void window_close_callback(GLFWwindow* window)
{
  finalize = true; }

glm::vec4 mouse = {0.0, 0.0, 0.0, 0.0} ;
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
  mouse.x = (GLfloat)xpos;
  mouse.y = (GLfloat)ypos;
}

bool left_mouse_button_pressed = false;
bool middle_mouse_button_pressed = false;
bool right_mouse_button_pressed = false;
bool ctrl_button_pressed = false;
bool shift_button_pressed = false;
glm::vec4 last_mouse = {0.0, 0.0, 0.0, 0.0} ;
glm::vec4 last_shift = {0.0, 0.0, 0.0, 0.0} ;
static void mouse_button_callback(
    GLFWwindow* window, int button, int action, int mods)
{
  // left button
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    left_mouse_button_pressed = true;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
      ctrl_button_pressed = true;
      last_mouse.x = mouse.x;
      last_mouse.y = mouse.y;
    } else {
      shift_button_pressed = false;
      ctrl_button_pressed = false;
      last_shift.x = mouse.x;
      last_shift.y = mouse.y;
    }
  } 
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
    left_mouse_button_pressed = false;
  }
  // middle button
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    middle_mouse_button_pressed = true;
    last_mouse.z = mouse.x;
  } 
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE){
    middle_mouse_button_pressed = false;
  }
  // right button
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
    right_mouse_button_pressed = true;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
    right_mouse_button_pressed = false;
  }
}

static GLfloat zoom = -1.618f;
GLfloat scroll_scale = 0.08f;
static GLfloat zoom_limits[2] = {-3.0f, 3.0f};
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  ImGuiIO& io = ImGui::GetIO();
  if (io.WantCaptureMouse) return;
  // yoffset is either -1 or 1
  zoom  += scroll_scale * (GLfloat)yoffset;
  if (zoom < zoom_limits[0]) 
    {zoom = zoom_limits[0];}
  if (zoom > zoom_limits[1]) 
    {zoom = zoom_limits[1];}
}

template <typename T>
Animation<T>::Animation(
    Simulation<T> &simulation,
    StateMachine<T> &state_machine,
    GlobalParameters<T> &global_parameters)
  : 
  simulation(simulation),
  state_machine(&state_machine),
  sim_params(this->state_machine->getParameters()),
  scaled_sim_params(this->state_machine->getScaledParameters()),
  anim_params(global_parameters.getAnimationParameters()),
  global_params(global_parameters),
  shader_directory(anim_params.shader_dir),
  window_title("bsim"),
  shader(),
  bounding_box(shader),
  grid(shader),
  axes(shader),
  imgui_handler(
      simulation, shader, global_params, sim_params, anim_params, 
      grid, bounding_box, axes, 
      point_clouds),
  finalized(false),
  running(false)
{
  // Set up window and OpenGL context, init variables
  init();
}

template <typename T>
Animation<T>::~Animation()
{
  // Terminate GLFW
 // finalize();
}

template <typename T>
void Animation<T>::init()
{
  // Init window
  initGlfwWindow();

  // Init OpenGL
  initGlad();

  // Init mouse input
  initMouse();

  // Init ImGui
  imgui_handler.init(window, "#version 460");

  // Init Webp
  initWebp();

  // Reserve memory to capture frames and write movie
  allocateMemory();

  // Init shader
  std::string vertex_shader = shader_directory + "/shader.vs";
  std::string fragment_shader = shader_directory + "/shader.fs";
  shader.init(
      vertex_shader.c_str(), 
      fragment_shader.c_str());

  // Set up simulation
  initSimulation();
}


template <typename T>
bool &Animation<T>::isRunning()
{
  return running;
}

template <typename T>
void Animation<T>::checkFFmpegPipe()
{
  // Check validity of pipe, get file descriptor
  int fd = fileno(ffmpeg_process);
  if (fd < 0){
    handleError(
        "Pipe to ffmpeg broken, recording not possible", WARNING);
    write_movie = false;
  } else {
    // Check status flags of file descriptor
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1){
      handleError(
          "Pipe to ffmpeg broken, recording not possible", WARNING);
      write_movie = false;
    }
    if (!(flags & O_WRONLY)) {
      handleError(
          "Pipe to ffmpeg not writable, recording not possible", WARNING);
      write_movie = false;
    }
  }
}

template <typename T>
void Animation<T>::initFFmpeg()
{
  if (ffmpeg_running) {
    checkFFmpegPipe();
  } else {
    // Create command for encoding with ffmpeg
    std::string timestring = global_params.getTimeString();
    ffmpeg_command = "ffmpeg -y -s " 
      + std::to_string(width) + "x" 
      + std::to_string(height) 
      + " -f rawvideo -pix_fmt rgb24 -i - "
      +  " -c:v libx265 -loglevel error -v fatal -preset medium -crf 0 -vf vflip -an -r 60 -b:v 2M -threads 8 " 
      + " " 
      + global_params.getRunDirectory()
      + movie_folder
      + "/bsim_animation_"  + timestring + "_" + 
      std::to_string(movie_counter++) + ".mp4";
      ffmpeg_process = popen(ffmpeg_command.c_str(), "w");
      if (!ffmpeg_process){
        std::cerr << "Failed to open ffmpeg process with command:\n"
          << ffmpeg_command << std::endl;
        return;
      } 
      ffmpeg_running = true;
#ifdef DEBUG
      std::cout<<"FFmpeg opened with command: "<< ffmpeg_command <<std::endl;
#endif
      /* ffmpeg_encoding_options, working example:
     // +  " -c:v libx265 -loglevel error -v fatal -preset medium -crf 0 -vf vflip -an -r 60 -b:v 2M -threads 8 " 
        */
  }
}

template <typename T>
void Animation<T>::initSimulation()
{
  // Create folders if necessary
  initFrameDirectory();
  initMovieDirectory();

  // Set up bounding box
  bounding_box.init(window, scaled_sim_params.worldsize, anim_params);

  // Set up grid
  grid.init(window, scaled_sim_params.worldsize, anim_params);

  // Set up axes
  axes.init(window, scaled_sim_params.worldsize, anim_params);

}

template <typename T>
void Animation<T>::initFrameDirectory()
{
  // Create frame folder if necessary
  std::string dir = global_params.getRunDirectory() + frame_folder;
  if (std::filesystem::exists(global_params.getRunDirectory())){
    if (!std::filesystem::exists(dir)){
      if (!std::filesystem::create_directory(dir)){
        std::cerr << "Failed to create frame folder" << std::endl;
      }
    } else return; // Folder already exists
  }else{
    if (!std::filesystem::create_directory(global_params.getRunDirectory())){
      std::cerr << "Failed to create output directory" << std::endl;
    
    }
  }
}

template <typename T>
void Animation<T>::initMovieDirectory()
{
  // Create folders if necessary
  std::string dir = global_params.getRunDirectory() + movie_folder;
  if (std::filesystem::exists(global_params.getRunDirectory())){
    if (!std::filesystem::exists(dir)){
      if (!std::filesystem::create_directory(dir)){
        std::cerr << "Failed to create movie folder" << std::endl;
      }
    }
  }else{
    if (std::filesystem::create_directory(global_params.getRunDirectory())){
      if (!std::filesystem::create_directory(dir)){
        std::cerr << "Failed to create movie folder" << std::endl;
        throw;
      }
    } else {
      std::cerr << "Failed to create output directory" << std::endl;
      throw;
    }
  }
}


template <typename T>
void Animation<T>::initGlfwWindow()
{
  // Init GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Register callback-function to handle errors
  glfwSetErrorCallback(error_callback);

  // Create window
  window = initWindow(window_title);

  // Set window as current context
  glfwMakeContextCurrent(window);

  // Register callback-function to handle key input
  glfwSetKeyCallback(window, key_callback);

  // Register callback-function to handle changes of framebuffer (=window) size
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Set buffer swap interval, number of frames between buffer swaps
  glfwSwapInterval(1);
}

template <typename T>
void Animation<T>::initGlad()
{
  int gl_version;
  gl_version = gladLoadGL(glfwGetProcAddress);
  if (!gl_version) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    throw;
  } 

  // Set window as current context
  glfwMakeContextCurrent(window);

  // Enable setting point size from vertex shader
  glEnable(GL_PROGRAM_POINT_SIZE);
  
  // Enable line width setting from vertex shader
  glEnable(GL_LINE_SMOOTH);
#ifdef DEBUG
  
  if (gl_version) {
    std::cout << "Loaded OpenGL "
      << GLAD_VERSION_MAJOR(gl_version) << "." << GLAD_VERSION_MINOR(gl_version)
      << std::endl;
  }
#endif
}

template <typename T>
GLFWwindow* Animation<T>::initWindow(const std::string title)
{
  // Settings for window
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
  // requests wayland if available
  //glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifdef DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

  // Set width and height, later handled by framebuffer_size_callback
  width = anim_params.window_width;
  height = anim_params.window_height;

  // Create window
  GLFWwindow* window = 
    glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  return window;
}

template <typename T>
void Animation<T>::initMouse()
{
  glfwSetCursorPosCallback(window, cursor_position_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);
}

template <typename T>
void Animation<T>::handleMouseInput()
{
  ImGuiIO& io = ImGui::GetIO();
  if (io.WantCaptureMouse) return;

  // Change view parameters based on mouse
  if (left_mouse_button_pressed){
    if (ctrl_button_pressed){
     // std::cout << "ctrl button pressed" << std::endl;
      rotate_view.x -= rotate_scale * (last_mouse.x - mouse.x);
      rotate_view.y += rotate_scale * (last_mouse.y - mouse.y);
      if (rotate_view.x < -180.0f) {rotate_view.x += 360.0f;}
      if (rotate_view.x > 180.0f) {rotate_view.x -= 360.0f;}
      if (rotate_view.y < -180.0f) {rotate_view.y += 360.0f;}
      if (rotate_view.y > 180.0f) {rotate_view.y -= 360.0f;}
    } else {
      center.x += shift_scale * (last_shift.x - mouse.x);
      center.y -= shift_scale * (last_shift.y - mouse.y);
      if (center.x < -1.0f) {center.x = -1.0f;} 
      if (center.x > 1.0f) {center.x = 1.0f;} 
      if (center.y < -1.0f) {center.y = -1.0f;} 
      if (center.y > 1.0f) {center.y = 1.0f;} 
    }
  }
  if (middle_mouse_button_pressed){
    rotate_view.z += rotate_scale * (last_mouse.z - mouse.x);
    if (rotate_view.z < -180.0f) {rotate_view.z += 360.0f;}
    if (rotate_view.z > 180.0f) {rotate_view.z -=  360.0f;}
  }
}

template <typename T>
glm::mat4 Animation<T>::createProjectionViewMatrix(
    glm::vec3 const& in_rotate, glm::vec3 const& center, GLfloat zoom, 
    glm::vec3 const& up)
{
  glm::vec3 rotate = glm::radians(in_rotate);
  glm::vec3 view_direction = glm::normalize(center - eye);
  glm::mat4 view = glm::lookAt(eye, center, up);
  glm::mat4 view_translate = 
    glm::translate(view, -view_direction * zoom);
  glm::mat4 view_rotate_x = glm::rotate(
      view_translate, rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
  glm::mat4 view_rotate_y = glm::rotate(
      view_rotate_x, rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 View = glm::rotate(view_rotate_y,
      rotate.z, glm::vec3(0.0f, 0.0f, -1.0f));
  glm::mat4 Projection =
        glm::infinitePerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f);
  glm::mat4 PV = Projection * View;
  return PV;
}

template <typename T>
void Animation<T>::setCameraView(
    glm::vec3 const& in_rotate, glm::vec3 const& center, GLfloat zoom, 
    glm::vec3 const& up)
{
  glm::mat4 PV = createProjectionViewMatrix(in_rotate, center, zoom, up);
  shader.use(0);
  shader.setMat4(0, "view", glm::value_ptr(PV));
}


float dot_product[6];
template <typename T>
void Animation<T>::selectGrids(glm::vec3 view)
{
  glm::vec3 planes[3] = {
    {1.0f, 0.0f, 0.0f},  //  X
    {0.0f, 1.0f, 0.0f},  //  Y
    {0.0f, 0.0f, 1.0f}  //  Z
  };

  // Set up view matrix
  glm::mat4 PV = createProjectionViewMatrix(camera_view, center, zoom, up);
  glm::vec3 view_direction = glm::normalize(center - eye);

  for (int i=0; i<3; i++){
    // use PV on planes
    glm::vec4 plane = glm::vec4(planes[i], 0.0f);
    plane = glm::transpose(glm::inverse(PV)) * plane;
    // dot product with view
    dot_product[i] = glm::dot(glm::vec3(plane), view_direction);
  }
  
  // X planes
  if (dot_product[0] > 0.0f) {
    auto_gridside[0] = true;
    auto_gridside[1] = false;
  } else if (dot_product[0] < 0.0f){
    auto_gridside[0] = false;
    auto_gridside[1] = true;
  } else {
    auto_gridside[0] = true;
    auto_gridside[1] = true;
  }

  // Y planes
  if (dot_product[1] > 0.0f) {
    auto_gridside[2] = true;
    auto_gridside[3] = false;
  } else if (dot_product[1] < 0.0f){
    auto_gridside[2] = false;
    auto_gridside[3] = true;
  } else {
    auto_gridside[2] = true;
    auto_gridside[3] = true;
  }

  // Z planes
  if (dot_product[2] > 0.0f) {
    auto_gridside[4] = true;
    auto_gridside[5] = false;
  } else if (dot_product[2] < 0.0f){
    auto_gridside[4] = false;
    auto_gridside[5] = true;
  } else {
    auto_gridside[4] = true;
    auto_gridside[5] = true;
  }

}


template <typename T>
void Animation<T>::runGui()
{
#ifdef DEBUG
  // Print thread number
  std::cout << "Thread number (animation thread):\t"
    <<omp_get_thread_num()<<std::endl;
#endif

  // GUI loop
  GUI();

  // Clean up
  finalize();
}
template <typename T>
void Animation<T>::GUI()
{
  /*
   * GUI loop
   * All rendered objects get stacked on top of each other
   * in the order they are rendered, so be careful with the order
   */

  // Set running flag to true
  running = true;
  simulation.setGuiFlag(true);

  int run = global_params.getRunCounter();

  bool record_frames = false;
  bool record_movie = false;
  bool record_webp = false;

  while (!glfwWindowShouldClose(window)){

    // Check wether a simulation run is finished
    if (global_params.getRunCounter() != run){
      reset();
      run = global_params.getRunCounter();
    }

    // Poll to render
    glfwPollEvents(); //instant rendering
                      
    // Clear the screen
    glClearColor(anim_params.background_color.r, 
        anim_params.background_color.g, 
        anim_params.background_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Handle resizing of window
    handleWindowResize(window_resized);

    // Start new imgui Frame
    imgui_handler.newFrame();

    // Handle mouse input for OpenGL
    handleMouseInput();

    // Set camera view
    camera_view = rotate_view;
    setCameraView(camera_view, center, zoom, up);

    // Capture frames
    if (anim_params.animate){
      drawGraphicalObjects(rotate_view);
    }// end if animate
    
    // Read framebuffer if necessary
    if (record_frames || record_movie || record_webp){
      if (!readFramebuffer(frame_image_)){
        std::cerr<<"Error: Could not read framebuffer"<<std::endl;
      }
    }

    // Export single frame
    if (record_frames){
      saveFrame();
    }

    // Append frame to animated webp
    if (record_webp){
      appendFrameToAnimatedWebp();
    }

    // Write movies
    if (record_movie){
      appendFrameToMovie();
    }
    
    // Render ImGui
    imgui_handler.orientationWindow(rotate_view, center, zoom, zoom_limits);
    imgui_handler.render(record_frames, record_movie, record_webp);

    // Swap buffers
    glfwSwapBuffers(window);

  } // end while

  // Set running flag to false
  running = false;
  simulation.setGuiFlag(false);
}

template <typename T>
void Animation<T>::handleWindowResize(bool &resized)
{
  if (!resized) return;
  allocateMemory();
  finalizeFFmpeg();
  initFFmpeg();
  resized = false;
}

template <typename T>
bool Animation<T>::readFramebuffer(uint8_t *target_)
{
  if (memory_size != 3*width*height){ 
    handleError("No capturing possible, memory size incorrect", WARNING);
    return false;
  }
  if (memory_lock){
    handleError("No capturing possible, memory locked", WARNING);
    return false;
  }
  // Set buffer to read
  glReadBuffer(GL_BACK);

  // Set packing for pixel storage
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  // Read framebuffer
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, target_);

  return true;
}

template <typename T>
void Animation<T>::initWebp()
{
  WebPDataInit(&webp_data);
  if (!WebPAnimEncoderOptionsInit(&anim_config) ||
      !WebPConfigInit(&webp_config) ||
      !WebPPictureInit(&webp_pic)){
    handleError("Error initializing webp", WARNING);
    return;
  }
  // Set animation configuration (global)
  // infinite loop
  anim_config.anim_params.loop_count = 0;
  // black background
  anim_config.allow_mixed = 0;
  // disable key frame insertion
  anim_config.kmax = 0;
  // Set webp configuration (could be set per frame)
  webp_config.method = 0; // 0 = fastest, 6 = slowest
  webp_config.lossless = 1;
  webp_config.quality = 0; // if lossless = 1, quality 0 is fastest
  webp_config.alpha_compression = 0;
  webp_config.alpha_quality = 0;
  webp_config.alpha_filtering = 0;
  webp_config.sns_strength = 0;
  webp_config.filter_strength = 0;
  webp_config.filter_sharpness = 0;
  webp_config.thread_level = 1;
  webp_config.pass = 1;
  webp_config.image_hint = WEBP_HINT_GRAPH;
  if (!WebPValidateConfig(&webp_config)){
    handleError("Error validating webp configuration", WARNING);
    return;
  }
  webp_encoder = WebPAnimEncoderNew(width, height, &anim_config);
  webp_initialized = true;
  current_ffmpeg_dir = global_params.getRunDirectory();
}

template <typename T>
void Animation<T>::appendFrameToAnimatedWebp()
{ 
  if (!webp_initialized) return;

  // Encode image
  uint8_t *webp_data = nullptr;
  // Import image data from data read from framebuffer
  webp_pic.use_argb = 0;
  webp_pic.width = width;
  webp_pic.height = height;

  if (!WebPPictureImportRGB(&webp_pic, frame_image_, 3 * width)){
    handleError("Error importing frame to webp animation", WARNING);
    return;
  }

  // Encode picture
  // see https://developers.google.com/speed/webp/docs/api
  if (!WebPAnimEncoderAdd(
        webp_encoder, &webp_pic, webp_frame_timestamp, &webp_config)){
    handleError("Error adding frame to webp animation", WARNING);
    return;
  }
  webp_frame_timestamp += webp_frame_duration;

}

template <typename T>
void Animation<T>::finalizeWebp()
{
  if (!webp_initialized) return;
  // Add final frame
  if (webp_frame_timestamp > 0){
    if (!WebPAnimEncoderAdd(webp_encoder, NULL, webp_frame_timestamp, &webp_config)){
      handleError("Error finalizing webp animation", WARNING);
    }
    // Assemble webp animation
    if (!WebPAnimEncoderAssemble(webp_encoder, &webp_data)){
      handleError("Error assembling webp animation", WARNING);
    }
  }
  WebPAnimEncoderDelete(webp_encoder);

  // Check if folder exists
  try {
    initMovieDirectory();
  } catch (std::exception &e){
    handleError("Error creating movie directory", WARNING);
    return;
  }

  // Write webp animation to file
  webp_animation_count++;
  std::stringstream run;
  run << std::setw(3) << std::setfill('0') << global_params.getRunCounter();
  std::string timestring = global_params.getTimeString();
  std::string filename = current_ffmpeg_dir
    + movie_folder 
    + "/animation" 
    + "_run_" + run.str() 
    + "_"
    + std::to_string(webp_animation_count) + "_" 
    + timestring + ".webp";
  FILE *webp_file = fopen(filename.c_str(), "wb");
  if (webp_file == nullptr) {
    handleError("No animation written, could not open file", WARNING);
    return;
  }
  int ret = fwrite(webp_data.bytes, sizeof(uint8_t), webp_data.size, webp_file);
  if (ret != webp_data.size){
    handleError("Error writing animation to file", WARNING);
  }
  fclose(webp_file);

  current_ffmpeg_dir = global_params.getRunDirectory();
  
  // Free memory
  WebPPictureFree(&webp_pic);
  WebPDataClear(&webp_data);
  webp_initialized = false;
}

template <typename T>
void Animation<T>::renderFrame()
{
  // Rotate scene 
  glm::vec3 view_angle = rotate_frame;
  glm::vec3 view_center = glm::vec3(0.0f);
  GLfloat view_zoom = frame_zoom;
  if (ig_frame_use_current_view) {
    view_center = center;
    view_zoom = zoom;
    if (rotate_scene){
      view_angle = glm::vec3(rotate_view+rotation_offset);
    } else {
      view_angle = rotate_view;
    }
  } 
  setCameraView(view_angle, view_center, view_zoom, -up);
  drawGraphicalObjects(view_angle);
}

template <typename T>
void Animation<T>::drawGraphicalObjects(glm::vec3 view)
{
  if (glfwGetCurrentContext() == NULL) {
    std::cout<<"No current GLFW context"<<std::endl;
    return;
  }

  // Draw grid
  if (anim_params.autoselect_grids){
    selectGrids(view);
    grid.setToDraw(auto_gridside);
  }
  grid.draw();

  // Draw axes
  axes.draw(anim_params.draw_axes, anim_params.draw_ticks, 
      anim_params.draw_labels);

  // Draw bounding box
  if (anim_params.draw_bounding_box){
    bounding_box.draw();
  }

  // Draw point clouds
  for (int i=0; i<point_clouds.size(); i++){
    point_clouds[i].setPoints(simulation.getAgentClassPointer(i));
    point_clouds[i].update();
    point_clouds[i].draw();
  }
}

template <typename T>
void Animation<T>::reset()
{
  current_frame_index = 0;
  finalizeFFmpeg();
  initFFmpeg();
  finalizeWebp();
  initWebp();
}

template <typename T>
void Animation<T>::saveFrame()
{
  std::cout<<"Saving frame"<<std::endl;
  initFrameDirectory();
  // Create filename
  std::stringstream framename;
  framename 
    << global_params.getRunDirectory()
    << frame_folder 
    << "/run_" << std::setw(3) << std::setfill('0')
    << global_params.getRunCounter() << "_"
    << "iteration" 
    << std::setw(6) << std::setfill('0')
    << state_machine->getIteration()  << "_" 
    << std::setw(6) << std::setfill('0')
    << current_frame_index++ << "_"
    << global_params.getTimeString() << ".webp"; 

  std::string filename = framename.str();

  // Open file
  frame_file = fopen(filename.c_str(), "wb");
  if (frame_file == nullptr) {
    std::string message = "No frame written, could not open file: " + filename;
    handleError(message.c_str(), WARNING);
    return;
  }

  // Encode frame as webp 
  uint8_t *webp_data = nullptr;
  size_t webp_size = 
    WebPEncodeLosslessRGB(frame_image_, width, height, 3 * width, &webp_data);
  if (webp_size == 0 || webp_data == nullptr){
    handleError("Error creating webp graphic", WARNING);
    fclose(frame_file);
    return;
  }

  // Write frame to file
  int ret = fwrite(webp_data, sizeof(uint8_t), webp_size, frame_file);
  if (ret != webp_size){
    handleError("Error writing frame to file", WARNING);
    return;
  }

  // Free memory
  WebPFree(webp_data);
  fclose(frame_file);
}

template <typename T>
void Animation<T>::appendFrameToMovie()
{
  // Ensure ffmpeg is running
  initFFmpeg();

  // Capture scene
  if (!readFramebuffer(image_)) {
      handleError("No movie frame written.", WARNING);
      return;
  }

  // Write frame to ffmpeg process
  int num_pixels = width * height;
  int ret = fwrite(image_, sizeof(unsigned char), 
        3 * sizeof(unsigned char) * num_pixels,
        ffmpeg_process);
  if ( ret != 3 * sizeof(unsigned char) * num_pixels){
      std::cerr << "Error writing frame data to ffmpeg" << std::endl;
  }
}

template <typename T>
void Animation<T>::finalizeFFmpeg()
{
  if (!ffmpeg_running) return;
  if (ffmpeg_process) {
    std::cout << "--- Terminating FFmpeg ---" << std::endl; 
    if (pclose(ffmpeg_process) == -1){
      std::cerr << "Error closing ffmpeg process" <<std::endl;
    } else {
      ffmpeg_running = false;
      std::cout << "--- FFmpeg terminated---" << std::endl; 
    }
  }
}

template <typename T>
void Animation<T>::finalize()
{
  if (finalized) return;

  // Finalize webp
  finalizeWebp();

  // Close the ffmpeg process
  finalizeFFmpeg();

  // Close window
  if (window){
    glfwDestroyWindow(window);
    window = nullptr;
  }

  // Clean up ImGui 
  imgui_handler.finalize();

  // Free memory
  free(image_);

  // Mark gui as finalized
  finalized = true;
}

//Explicit template instantiation
template class Animation<float>;
template class Animation<double>;
/* vim: set ts=2 sw=2 tw=0 et :*/
