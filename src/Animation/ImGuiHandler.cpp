#include "ImGuiHandler.h"

template <typename T>
ImGuiHandler<T>::ImGuiHandler(
    Simulation<T> &simulation,
    Shader<T> &shader,
    GlobalParameters<T> &global_params,
    SimulationParameters<T> &sim_params,
    AnimationParameters<T> &anim_params,
    Grid<T> &grid, BoundingBox<T> &bounding_box, Axes<T> &axes, 
    std::vector<PointCloud<T>> &point_clouds)
  :
   simulation(simulation),
   shader(shader),
   global_params(global_params),
   sim_params(sim_params),
   anim_params(anim_params), 
   grid(grid), 
   bounding_box(bounding_box),
   axes(axes),
   point_clouds(point_clouds),
   grid_imgui_handle(grid, axes, anim_params),
   bounding_box_imgui_handle(bounding_box, anim_params),
   axes_imgui_handle(axes, grid, anim_params),
   point_cloud_imgui_handle(point_clouds, sim_params, anim_params),
   global_params_imgui_handle(global_params, info_popup_text, 
       show_window[(int)Menu::INFO_POPUP]),
   anim_params_imgui_handle(
       anim_params, bounding_box, grid, axes, point_clouds, 
       info_popup_text, show_window[(int)Menu::INFO_POPUP]),
   agent_class_params_imgui_handle(
       sim_params,
       sim_params_new,
       agent_class_param_new, info_popup_text, 
       show_window[(int)Menu::INFO_POPUP]),
   sim_params_imgui_handle(
       agent_class_params_imgui_handle,
       sim_params, sim_params_new, info_popup_text, 
       show_window[(int)Menu::INFO_POPUP]),
   simulation_imgui_handle(simulation, anim_params, sim_params,
       axes, bounding_box, grid, point_clouds, info_popup_text,
       show_window[(int)Menu::INFO_POPUP]),
   imgui_stream(nullptr),
   edit_agent_class_index(-1)
{
}

template <typename T>
ImGuiHandler<T>::~ImGuiHandler()
{
  // Finalize the Stream buffer 
  if (imgui_stream != nullptr){
    delete imgui_stream;
  }
}

template <typename T>
void ImGuiHandler<T>::init(GLFWwindow *window, const std::string glsl_version)
{
  // Set the window and GLSL version
  this->window = window;
  this->glsl_version = glsl_version;
  glfwMakeContextCurrent(window);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); 
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  //ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());

  // Setup freetype font
  std::string font = anim_params.font_dir + anim_params.font_file;
  io.Fonts->AddFontFromFileTTF(font.c_str(), anim_params.font_size);
  
  // Setup stream buffer
  size_t buffer_size = 1024*1024*10; // 10 MB
  imgui_stream = new ImGuiStream(buffer_size);
//#ifdef DEBUG//TODO: remove
  std::cout << "ImGuiHandler::init: Initialized ImGuiHandler" << std::endl;
  // Redirect cout to the cout buffer (disable console)
  imgui_stream->restore();
//#endif

}

template <typename T>
void ImGuiHandler<T>::newFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

template <typename T>
void ImGuiHandler<T>::orientationWindow(
    glm::vec3 &rotate_view, 
    glm::vec3 &center,
    GLfloat &zoom, const GLfloat zoom_limits[2])
{
  // Return if the window should not be shown 
  if (!show_window[(int)Menu::ORIENTATION]) return;

  const char* format = "%.0f";
  if (!ImGui::Begin("Orientation panel", 
      &show_window[(int)Menu::ORIENTATION])){
    ImGui::End();
    return;
  } 

  /*
  if (ImGui::MenuItem(
        "Enable autorotation", NULL, &Animation<T>::rotate_scene)){
    glfwSetTime(0.0);
    if (!rotate_scene) Animation<T>::rotate_view 
    += Animation<T>::rotation_offset;
  } 
  */

  ImGui::SeparatorText("Live view");
  ImGui::Text("View angle (degrees):");
  ImGui::SliderFloat("X", &rotate_view.x, -180.0f, 180.0f, format);
  ImGui::SliderFloat("Y", &rotate_view.y, -180.0f, 180.0f, format);
  ImGui::SliderFloat("Z", &rotate_view.z, -180.0f, 180.0f, format);
  ImGui::SliderFloat("View zoom", &zoom, zoom_limits[0], zoom_limits[1]);
  if (ImGui::MenuItem("Reset view")){
    rotate_view = center = glm::vec3(0.0f,0.0f,0.0f);
    zoom = 0.0f;
  }

  /*
  ImGui::SeparatorText("Frame recording");
  ImGui::Text("Frame view angle (degrees):");
  ImGui::SliderFloat("X ", &rotate_frame.x, -180.0f, 180.0f, format);
  ImGui::SliderFloat("Y ", &rotate_frame.y, -180.0f, 180.0f, format);
  ImGui::SliderFloat("Z ", &rotate_frame.z, -180.0f, 180.0f, format);
  ImGui::SliderFloat("Frame zoom ", &frame_zoom, zoom_limits[0], zoom_limits[1]);
  ImGui::MenuItem("Use current view", NULL, &ig_frame_use_current_view);
  if (ImGui::MenuItem("Set to current view angle")){
    rotate_frame = rotate_view;
    frame_zoom = zoom;
  }
  if (ImGui::MenuItem("Preview frame angle")){
    rotate_view = rotate_frame;
    zoom = frame_zoom;
  }
  if (ImGui::MenuItem("Reset frame view")){
    rotate_frame = glm::vec3(0.0f,0.0f,0.0f);
    zoom = 0.0f;
  }

  ImGui::SeparatorText("Movie recording");
  ImGui::Text("Movie view angle (degrees):");
  ImGui::SliderFloat("X  ", &rotate_movie.x, -180.0f, 180.0f, format);
  ImGui::SliderFloat("Y  ", &rotate_movie.y, -180.0f, 180.0f, format);
  ImGui::SliderFloat("Z  ", &rotate_movie.z, -180.0f, 180.0f, format);
  ImGui::SliderFloat("Movie zoom  ", &movie_zoom, zoom_limits[0], zoom_limits[1]);
  ImGui::MenuItem("Use current view", NULL, &ig_movie_use_current_view);
  if (ImGui::MenuItem("Set to current view angle")){
    rotate_movie = rotate_view;
    movie_zoom = zoom;
  }
  if (ImGui::MenuItem("Preview movie angle")){
    rotate_view = rotate_movie;
    zoom = movie_zoom;
  }
  if (ImGui::MenuItem("Reset movie view")){
    rotate_movie = glm::vec3(0.0f,0.0f,0.0f);
    zoom = 0.0f;
  }
  */

  ImGui::End();

}

template <typename T>
void ImGuiHandler<T>::render(bool &record_frame, bool &record_movie, 
    bool &record_webp)
{
  // Update the ImGui Menus
  updateMenus(record_frame, record_movie, record_webp);

  // Rendering
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

template <typename T>
void ImGuiHandler<T>::finalize()
{
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

template <typename T>
void ImGuiHandler<T>::updateMenus(bool &record_frame, bool &record_movie, 
    bool &record_webp)
{
  // Demo window
  //ImGui::ShowDemoWindow();
  
  // Main menu
  mainMenu(record_frame, record_movie, record_webp);

  // Console window
  drawConsoleWindow();

  // Info popup
  drawInfoPopup();
  
  // Grid window
  grid_imgui_handle.render(show_window[(int)Menu::GRID]);

  // Axes window
  axes_imgui_handle.render(show_window[(int)Menu::AXES]);

  // Bounding box window
  bounding_box_imgui_handle.render(show_window[(int)Menu::BOUNDING_BOX]);

  // Point cloud window
  point_cloud_imgui_handle.render(show_window[(int)Menu::POINT_CLOUDS]);

  // Background color window
  backgroundColorWindow();

  // Animation parameters window
  anim_params_imgui_handle.render(show_window[(int)Menu::ANIM_PARAMS]);

  // Simulation parameters window
  sim_params_imgui_handle.render(show_window[(int)Menu::SIM_PARAMS]);

  // Simulation control window
  simulation_imgui_handle.render(show_window[(int)Menu::SIMULATION_CONTROL],
      shader, window);

  // Global parameters window
  global_params_imgui_handle.render(show_window[(int)Menu::GLOBAL_PARAMS]);

}

template <typename T>
void ImGuiHandler<T>::mainMenu(bool &record_frame, bool &record_movie, 
    bool &record_webp)
{
  if (ImGui::BeginMainMenuBar()){

    if (ImGui::BeginMenu("File")){

      // Exit button
      if (ImGui::MenuItem("Exit")){
        glfwSetWindowShouldClose(window, GL_TRUE);
      }

      // End file menu
      ImGui::EndMenu();

    }

    // Parameters menu
    if (ImGui::BeginMenu("Parameters")){

      // Global parameters menu
      if (ImGui::MenuItem("Global parameters")){
        show_window[(int)Menu::GLOBAL_PARAMS] = true;
      }

      // Animation parameter menu 
      if (ImGui::MenuItem("Animation parameters")){
        show_window[(int)Menu::ANIM_PARAMS] = true;
      }

      // Simulation parameter menu
      if (ImGui::MenuItem("Simulation parameters")){
        show_window[(int)Menu::SIM_PARAMS] = true;
      }

      // End parameters menu
      ImGui::EndMenu();
    }

    // Simulation menu
    if (ImGui::BeginMenu("Simulation")){


      // Separator
      ImGui::Separator();
      
      // Simulation control window
      if (ImGui::MenuItem("Simulation control")){
        show_window[(int)Menu::SIMULATION_CONTROL] = true;
      }

      // Separator
      ImGui::Separator();

      // Animate checkbox, if type of T is float
      if (std::is_same<T, float>::value){
        if (ImGui::MenuItem("Animate", NULL, &anim_params.animate)){
          if (anim_params.animate){
            simulation_imgui_handle.initAnimation(window, shader);
            data_mode = false;
          } else {
            data_mode = true;
          }
        }
      } 

      // End simulation menu
      ImGui::EndMenu();
    }

    // Record menu
    if (ImGui::BeginMenu("Record")){
      // Checkboxes for recording
      ImGui::MenuItem("Record frames", NULL, &record_frame);
      ImGui::MenuItem("Record movie", NULL, &record_movie);
      ImGui::MenuItem("Record webp", NULL, &record_webp);
      ImGui::EndMenu();
    }


    // View menu
    if (ImGui::BeginMenu("View")){
      ImGui::MenuItem(
          "Orientation", NULL, &show_window[(int)Menu::ORIENTATION]);

      // Separator
      ImGui::Separator();

      ImGui::MenuItem(
                "Outer Grid", NULL, &show_window[(int)Menu::GRID]);
      ImGui::MenuItem(
          "Axes (inner Grid)", NULL, &show_window[(int)Menu::AXES]);
      ImGui::MenuItem(
          "Bounding box", NULL, &show_window[(int)Menu::BOUNDING_BOX]);
      ImGui::MenuItem(
          "Background color", NULL, &show_window[(int)Menu::BACKGROUND_COLOR]);

      // Separator
      ImGui::Separator();

      // Point clouds
      ImGui::MenuItem(
          "Point clouds", NULL, &show_window[(int)Menu::POINT_CLOUDS]);

      // Separator
      ImGui::Separator();

      // Show animation parameters
      if (ImGui::MenuItem("Show animation parameters")){
        std::stringstream ss;
        anim_params.print(ss);
        info_popup_text = ss.str();
        show_window[(int)Menu::INFO_POPUP] = true;
      }

      // Separator
      ImGui::Separator();

      // Dark/light mode
      if (ImGui::MenuItem("Dark mode", NULL, &dark_mode)){
        if (dark_mode){
          ImGui::StyleColorsDark();
        } else {
          ImGui::StyleColorsLight();
        }
      }

      // Large/small font
      if (ImGui::MenuItem("Large font", NULL, &large_font)){
        if (large_font){
          ImGui::GetIO().FontGlobalScale = 1.5f;
        } else {
          ImGui::GetIO().FontGlobalScale = 1.0f;
        }
      }

      // Separator
      ImGui::Separator();

      // Console
      ImGui::MenuItem(
          "Console", NULL, &show_window[(int)Menu::CONSOLE]);
      
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }// end main menu
}

template <typename T>
void ImGuiHandler<T>::reloadObjects()
{
  // Reload the objects 
  bounding_box.initParams(anim_params);
  bounding_box.initBoundingBox();
  grid.initParams(anim_params);
  grid.generateGrids();
  axes.initParams(anim_params);
  axes.generateTicks();
}

template <typename T>
void ImGuiHandler<T>::backgroundColorWindow()
{
  // Return if the window should not be shown 
  if (!show_window[(int)Menu::BACKGROUND_COLOR]) return;

  int window_flags = ImGuiWindowFlags_AlwaysAutoResize;

  if (!ImGui::Begin("Background color settings", 
      &show_window[(int)Menu::BACKGROUND_COLOR], window_flags)){
    ImGui::End();
    return;
  } 

  // Separator
  ImGui::SeparatorText("Background color");

  // Color picker
  Color<T> background_color = anim_params.background_color;
  ImGui::ColorEdit3("Background color", &anim_params.background_color.r);
  if (background_color != anim_params.background_color){
    glClearColor(
        anim_params.background_color.r,
        anim_params.background_color.g,
        anim_params.background_color.b,
        1.0f);
  }

  ImGui::End();
}

template <typename T>
void ImGuiHandler<T>::drawConsoleWindow()
{
  // Return if the window should not be shown 
  if (!show_window[(int)Menu::CONSOLE]) return;

  // Window flags, always scroll to bottom
  int window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;

  // Set the size of the window
  ImGui::SetNextWindowSize(ImVec2(640,160), ImGuiCond_FirstUseEver);


  // Open the window
  if (!ImGui::Begin("Console", 
      &show_window[(int)Menu::CONSOLE], window_flags)){
    ImGui::End();
    return;
  }

  // Print the content of the cout buffer
  ImGui::Text("%s", imgui_stream->getBuffer());


  // End the window
  ImGui::End();
}

template <typename T>
void ImGuiHandler<T>::drawInfoPopup()
{
  // Return if the window should not be shown
  if (!show_window[(int)Menu::INFO_POPUP]) return;

  // Set window flags
  int window_flags = ImGuiWindowFlags_AlwaysAutoResize;

  // Show the info popup in a popup window
  ImGui::OpenPopup("Info");
  if (ImGui::BeginPopupModal("Info", NULL, window_flags)){
    ImGui::Text("%s", info_popup_text.c_str());
    if (ImGui::Button("Close")){
      ImGui::CloseCurrentPopup();
      show_window[(int)Menu::INFO_POPUP] = false;
    }
    ImGui::EndPopup();
  }

}

template <typename T>
void ImGuiHandler<T>::setWindow(GLFWwindow* window)
{
  this->window = window;
}


// Explicit template instantiation
template class ImGuiHandler<float>;
template class ImGuiHandler<double>;

/* vim: set ts=2 sw=2 tw=0 et :*/
