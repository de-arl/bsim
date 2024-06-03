#ifndef IM_GUI_HANDLER_H
#define IM_GUI_HANDLER_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <dirent.h>
#include <sstream>

#include "glm/vec3.hpp"

#ifndef OPENGL_INITIALIZED
#define OPENGL_INITIALIZED
#define GLAD_GL_IMPLEMENTATION
#include "gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif //OPENGL_INITIALIZED

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuiFileDialog.h"

#include "imgui_freetype.h"
#include "ft2build.h"
#include FT_FREETYPE_H

#include "SimulationParameters.h"
#include "SimulationParametersImGuiHandle.h"
#include "AnimationParameters.h"
#include "AnimationParametersImGuiHandle.h"
#include "AgentClassParameters.h"
#include "AgentClassParametersImGuiHandle.h"
#include "GlobalParameters.h"
#include "GlobalParametersImGuiHandle.h"
#include "Color.h"
#include "Grid.h"
#include "GridImGuiHandle.h"
#include "Axes.h"
#include "AxesImGuiHandle.h"
#include "BoundingBox.h"
#include "BoundingBoxImGuiHandle.h"
#include "helpers.h"
#include "ImGuiStream.h"
#include "StateMachine.h"
#include "PointCloud.h"
#include "PointCloudImGuiHandle.h"
#include "Shader.h"
#include "Simulation.h"
#include "SimulationImGuiHandle.h"


/**
 * NOTE: If you get a runtime error alike:
 * ImGui_ImplOpenGL3_CreateDeviceObjects: Failed to compile/link shaders...
 * Check your OpenMP setup. The implementation of ImGui is not thread-safe.
 */

/* Enum class for the different menus */
enum class Menu
{
  AGENT_CLASS_EDITOR,
  AGENT_CLASS_PARAM_EDITOR,
  ANIM_PARAMS,
  AXES,
  BACKGROUND_COLOR,
  BOUNDING_BOX,
  CONSOLE,
  GRID,
  GLOBAL_PARAMS,
  INFO_POPUP,
  LOAD_AGENT_CLASS_PARAMS,
  ORIENTATION,
  POINT_CLOUDS,
  SAVE_AGENT_CLASS_PARAMS,
  SIMULATION_CONTROL,
  SIM_PARAMS,
  NUM_MENUS
};

template <typename T>
class ImGuiHandler
{
public:
  // Default constructor
  ImGuiHandler(Simulation<T> &simulation, 
      Shader<T> &shader, 
      GlobalParameters<T> &global_params,
      SimulationParameters<T> &sim_params,
      AnimationParameters<T> &anim_params, 
      Grid<T> &grid, BoundingBox<T> &bounding_box, Axes<T> &axes,
      std::vector<PointCloud<T>> &point_clouds);

  // Default destructor
  ~ImGuiHandler();

  // Initialize ImGui
  void init(GLFWwindow *window, const std::string glsl_version);

  // Create a new frame
  void newFrame();

  // Orientation window
  void orientationWindow(
      glm::vec3 &rotate_view, glm::vec3 &center,
      GLfloat &zoom, const GLfloat zoom_limits[2]);


  // Render the ImGui frame
  void render(bool &record_frame, bool &record_movie, bool &record_webp);

  // Set window
  void setWindow(GLFWwindow *window);

  // Finalize ImGui
  void finalize();

private:

  /* Private member functions */

  // Update the ImGui menus
  void updateMenus(bool &record_frame, bool &record_movie, bool &record_webp);

  // Main menu
  void mainMenu(bool &record_frame, bool &record_movie, bool &record_webp);

  // Reload the objects (grid, axes, bounding box), 
  // after changing animation parameters
  void reloadObjects();

  // Background color window
  void backgroundColorWindow();

  // Draw a console window
  void drawConsoleWindow();

  // Draw an info window
  void drawInfoPopup();

  

  /* Private member variables */

  // Simulation
  Simulation<T> &simulation;
  SimulationImGuiHandle<T> simulation_imgui_handle;

  // Shader program
  Shader<T> &shader;

  // Global parameters
  GlobalParameters<T> &global_params;
  GlobalParametersImGuiHandle<T> global_params_imgui_handle;

  // Animation parameters
  AnimationParameters<T> &anim_params;
  AnimationParametersImGuiHandle<T> anim_params_imgui_handle;

  // Simulation parameters
  SimulationParameters<T> &sim_params;
  SimulationParameters<T> sim_params_new;
  SimulationParametersImGuiHandle<T> sim_params_imgui_handle;

  // Vector of agent class parameters
  std::vector<AgentClassParameters<T>> agent_class_params_new;
  AgentClassParameters<T> agent_class_param_new;
  AgentClassParametersImGuiHandle<T> agent_class_params_imgui_handle;
  int edit_agent_class_index;
  int save_agent_class_index;

  // Point clouds
  std::vector<PointCloud<T>> &point_clouds;
  PointCloudImGuiHandle<T> point_cloud_imgui_handle;

  // Pointer to the GLFW window
  GLFWwindow *window;

  // Grid
  Grid<T> &grid;
  GridImGuiHandle<T> grid_imgui_handle;

  // Axes
  Axes<T> &axes;
  AxesImGuiHandle<T> axes_imgui_handle;

  // Bounding box
  BoundingBox<T> &bounding_box;
  BoundingBoxImGuiHandle<T> bounding_box_imgui_handle;

  // GLSL version
  std::string glsl_version;

  // Ostream for the ImGui stream
  ImGuiStream *imgui_stream;

  // Info popup text
  std::string info_popup_text;

  // State flags
  bool show_window[(int)Menu::NUM_MENUS] = {false};

  // Dark mode
  bool dark_mode = true;

  // Scale GUI
  bool large_font = false;

  // Data or animation mode
  bool data_mode = false;

};
#endif // IM_GUI_HANDLER_H
/* vim: set ts=2 sw=2 tw=0 et :*/
