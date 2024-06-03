#ifndef SIMULATION_IM_GUI_HANDLE_H
#define SIMULATION_IM_GUI_HANDLE_H

#include <string>
#include <iostream>

#include "imgui.h"
#include "ImGuiFileDialog.h"
#include <vector>

#include "SimulationParameters.h"
#include "AnimationParameters.h"
#include "PointCloud.h"
#include "Simulation.h"
#include "Shader.h"

#ifndef OPENGL_INITIALIZED
#define OPENGL_INITIALIZED
#define GLAD_GL_IMPLEMENTATION
#include "gl.h"
#include "khrplatform.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif //OPENGL_INITIALIZED

enum class SimMenu
{
  CHOOSE_OUTPUT_DIR,
  CHOOSE_TOML_OUTPUT_DIR,
  CHOOSE_INPUT_FILE,
  NUM_MENU_ITEMS
};

template <typename T>
class SimulationImGuiHandle
{
 public:
  
  // Constructor
  SimulationImGuiHandle(Simulation<T> &simulation,
      AnimationParameters<T> &animation_parameters,
      SimulationParameters<T> &simulation_parameters,
      Axes<T> &axes,
      BoundingBox<T> &bounding_box,
      Grid<T> &grid,
      std::vector<PointCloud<T>> &point_clouds,
      std::string &info_popup_text,
      bool &show_info_popup)
    : simulation(simulation),
    point_clouds(point_clouds),
    anim_params(animation_parameters),
    sim_params(simulation_parameters),
    axes(axes),
    bounding_box(bounding_box),
    grid(grid),
    info_popup_text(info_popup_text),
    show_info_popup(show_info_popup),
    initialized(false)
  {
  }

  void render(bool &show, Shader<T> &shader, GLFWwindow* window)
  {
    // Return if the window should not be shown
    if (!show) return;

    // Show the simulation control window in a popup window
    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    // Open the window
    if (!ImGui::Begin("Simulation", &show, window_flags)){
      ImGui::End();
      return;
    }

    // Simulation state
    simulationState(window, shader);

    // Show the simulation parameters
    simulationParameters(window, shader);

    // Output
    output();

    // Simulation control
    simulationControl();

    // End the window
    ImGui::End();

    // Choose output directory
    chooseOutputDirectory();
  }

  void simulationState(GLFWwindow* window, Shader<T> &shader)
  {
    if (ImGui::CollapsingHeader("Simulation state")){
      // Show the simulation state
      ImGui::Text("Simulation state: %s", simulation.getStateString().c_str());

      // Print state state button
      printStateButton();

      ImGui::Separator();

      // Save state button
      saveStateButton();
      saveStateDialog();

      ImGui::SameLine();

      // Load state button
      loadStateButton();
      loadStateDialog(window, shader);

    }
  }

  void saveStateButton()
  {
    // Save state button
    if (ImGui::Button("Save state to toml file")){
      show_window[(int)SimMenu::CHOOSE_TOML_OUTPUT_DIR] = true;
    }
  }

  void saveStateDialog()
  {
    // Return if the window is not shown
    if (!show_window[(int)SimMenu::CHOOSE_TOML_OUTPUT_DIR]){
      return;
    }

    // Get the current directory
    std::string current_dir = simulation.getOutputDir();

    // Set window size
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    // Show filename dialog
    IGFD::FileDialogConfig conf;
    std::string filename = "state";
    filename += 
      "_" + simulation.getTimeString()
      + "_" + std::to_string(simulation.getIteration());
      + "_" + std::to_string(simulation.getRunCounter());
    filename += ".toml";
    conf.fileName = filename;
    conf.path = simulation.getOutputDir();
    ImGuiFileDialog::Instance()->OpenDialog("ChooseOutputFileToml",
        "Save state to toml file", ".toml", conf);

    // If the file dialog is shown
    if (ImGuiFileDialog::Instance()->Display("ChooseOutputFileToml")){
      // If the user selects a directory
      if (ImGuiFileDialog::Instance()->IsOk()){
        // Get the selected directory
        std::string path = ImGuiFileDialog::Instance()->GetCurrentPath();
        filename = ImGuiFileDialog::Instance()->GetCurrentFileName();
#ifdef DEBUG
        std::cout << "Selected filename: " << path << "/" << filename << std::endl;
#endif
        // Save the state
        path += "/" + filename;
        simulation.state_machine.saveStateToml(path);
      }
      // Close the file dialog
      ImGuiFileDialog::Instance()->Close();
      // Close the window
      show_window[(int)SimMenu::CHOOSE_TOML_OUTPUT_DIR] = false;
    }
  }


  void loadStateButton()
  {
    // Load state button
    if (ImGui::Button("Load state from toml file")){
      show_window[(int)SimMenu::CHOOSE_INPUT_FILE] = true;
    }
  }

  void loadStateDialog(GLFWwindow* window, Shader<T> &shader)
  {
    // Return if the window is not shown
    if (!show_window[(int)SimMenu::CHOOSE_INPUT_FILE]){
      return;
    }

    // Get the current directory
    std::string current_dir = simulation.getOutputDir();

    // Show the file dialog
    IGFD::FileDialogConfig conf;
    conf.path = current_dir;
    ImGuiFileDialog::Instance()->OpenDialog("ChooseInputFile",
        "Choose input file", ".toml", conf);

    // If the file dialog is shown
    if (ImGuiFileDialog::Instance()->Display("ChooseInputFile")){
      // If the user selects a file
      if (ImGuiFileDialog::Instance()->IsOk()){
        // Get the selected file
        std::string input_file = ImGuiFileDialog::Instance()->GetFilePathName();
        // Load the state
#ifdef DEBUG
        std::cout 
          << "SimulationImGuiHandle::loadStateDialog: Loading state from file" 
          << std::endl
          << "SimulationImGuiHandle::loadStateDialog: Selected file: " 
          << input_file << std::endl;
#endif

        sim_params.setSimulationConfigFile(input_file);
        sim_params.parseSimulationConfig();
        sim_params.init();
        simulation.load(input_file);
        initAnimation(window, shader);
      }
      // Close the file dialog
      ImGuiFileDialog::Instance()->Close();
      // Close the window
      show_window[(int)SimMenu::CHOOSE_INPUT_FILE] = false;
    }
  }

  void simulationParameters(GLFWwindow* window, Shader<T> &shader)
  {
    if (ImGui::CollapsingHeader("Simulation parameters")){
      // Init button
      if (simulation.getState() <= SimulationState::STOPPED){
        initButton(window, shader);
      }

      // Show the simulation parameters
      showButton();
    }
  }

  void simulationControl()
  {

    if (ImGui::CollapsingHeader("Simulation control")){

      ImGui::Separator();

      if (simulation.getState() == SimulationState::UNINITIALIZED){
        ImGui::Text("Simulation is not initialized");
        ImGui::Separator();
      }


      if (simulation.getState() < SimulationState::RUNNING){
        // Advance button
        advanceButton();
        ImGui::SameLine();
        // Play button
        playButton();
        ImGui::SameLine();
      }

      // Pause 
      pauseButton();

      // Stop button
      ImGui::SameLine();
      stopButton();

      // Reset and play buttons
      ImGui::SameLine();
      if (simulation.getState() <= SimulationState::STOPPED){
        resetButton();
      }

      ImGui::Separator();
    }

    int iteration = simulation.getIteration();
    int run_counter = simulation.getRunCounter();
    ImGui::Text("Iteration: %d", iteration);
    ImGui::Text("Run counter: %d", run_counter);
    // Simulated time
    double time = simulation.getTime();
    // Calculate hours, minutes, seconds
    int hours = time / 3600;
    int minutes = (time - hours * 3600) / 60;
    int seconds = time - hours * 3600 - minutes * 60;
    ImGui::Text("Time: %02d:%02d:%02d", hours, minutes, seconds);

    // Current iteration rate
    float actual_iteration_rate = (float)simulation.getActualIterationRate();
    ImGui::Text("Actual iteration rate: %.2f", actual_iteration_rate);

    float iteration_rate = (float)simulation.getIterationRate();
    ImGui::SliderFloat("Iteration rate", &iteration_rate, 1.0, 180.0);
    if (iteration_rate != simulation.getIterationRate()){
      simulation.setIterationRate((T)iteration_rate);
    }
    // Unlimted iteration rate checkbox
    bool unlimited_iteration_rate = simulation.getUnlimitedIterationRate();
    ImGui::Checkbox("Unlimited iteration rate", &unlimited_iteration_rate);
    if (unlimited_iteration_rate != simulation.getUnlimitedIterationRate()){
      simulation.setUnlimitedIterationRate(unlimited_iteration_rate);
    }

    
  }

  void output()
  {
    if (ImGui::CollapsingHeader("Output")){

      // Show the output directory
      ImGui::Text("Output directory: %s", simulation.getOutputDir().c_str());

      // Choose output directory menu item
      if (ImGui::Button("Choose output directory")){
        show_window[(int)SimMenu::CHOOSE_OUTPUT_DIR] = true;
      }

      ImGui::SameLine();
      saveButton();

      ImGui::SeparatorText("Logging");

      // Log level info button
      ImGui::SameLine();
      logLevelInfoButton();

      // Log level slider
      int log_level = simulation.state_machine.getLogLevel();
      ImGui::SliderInt("Log level", &log_level, 0, 5);
      if (log_level != simulation.state_machine.getLogLevel()){
        simulation.state_machine.setLogLevel(log_level);
      }
    }// End of output
  }

  void showButton()
  {
    // Show button
    if (ImGui::Button("Show current simulation parameters")){
      std::stringstream ss;
      simulation.state_machine.printParameters(ss);
      info_popup_text = ss.str();
      show_info_popup = true;
    }
  }

  void logLevelInfoButton()
  {
    // Log level info button
    if (ImGui::Button("Log level info")){
      show_info_popup = true;
      info_popup_text = "Log level info:\n\
        0: no logging\n\
        1: position, energy\n\
        2: scalar agent parameters\n\
        3: class to class metrics\n\
        4: agent to class metrics\n\
        5: agent to agent metrics";
    }
  }

  void initButton(GLFWwindow* window, Shader<T> &shader)
  {
    // Init button
    if (ImGui::Button("Init parameters")){

      // Initialize the simulation parameters
      sim_params.init();

      // Initialize the simulation
      simulation.init(sim_params);

      // Initialize the animation
      initAnimation(window, shader);

#ifdef DEBUG
      // Print state
      //simulation.printState();
#endif
    }// End of init button
  }

  void initAnimation(GLFWwindow* window, Shader<T> &shader)
  {
#ifdef DEBUG
    std::cout << "SimulationImGuiHandle::initAnimation: \
      Initializing animation" << std::endl;
#endif
      // Set animation off  
      bool animate = anim_params.animate;
      anim_params.animate = false;

      // Create point clouds
      createPointClouds(shader, window);

      // Get the dimensions
      T dims[3];
      simulation.getDims(dims);

      // Initialize the bounding box
      bounding_box.init(dims, anim_params);
      bounding_box.initBoundingBox();

      // Initialize the grid
      grid.init(dims, anim_params);
      grid.generateGrids();

      // Initialize the axes
      axes.init(dims, anim_params);
      axes.generateTicks();

      // Reset the animation mode
      anim_params.animate = animate;

      // Set the initialized flag
      initialized = true;
  }

  void saveButton()
  {
    // Save button
    if (ImGui::Button("Save state to csv file")){
      simulation.saveState();
    }
  }

  void printStateButton()
  {
    // Print state button
    if (ImGui::Button("Print state to console")){
      simulation.printState();
    }
  }

  void resetButton()
  {
    // Reset button
    if (ImGui::Button("Reset")){
      simulation.reset();
      for (int i=0; i<point_clouds.size(); i++){
        // Update the buffer
        point_clouds[i].setPoints(simulation.getAgentClassPointer(i));
        point_clouds[i].initPointCloud();
      }
    }
  }

  void advanceButton()
  {
    // Advance button
    if (ImGui::Button("Advance")){
      simulation.advance();
    }
  }

  void playButton()
  {
    // Play button
    if (ImGui::Button("Start")){
      simulation.start();
    }
  }

  void stopButton()
  {
    if (ImGui::Button("Stop")){
      simulation.stop();
    }
  }

  void chooseOutputDirectory()
  {
    // Return if the window is not shown
    if (!show_window[(int)SimMenu::CHOOSE_OUTPUT_DIR]){
      return;
    }

    // Get the current directory
    std::string current_dir = simulation.getOutputDir();

    // Show the file dialog
    IGFD::FileDialogConfig conf;
    conf.path = current_dir;
    ImGuiFileDialog::Instance()->OpenDialog("ChooseOutputDir",
        "Choose output directory", NULL, conf);

    // If the file dialog is shown
    if (ImGuiFileDialog::Instance()->Display("ChooseOutputDir")){
      // If the user selects a file
      if (ImGuiFileDialog::Instance()->IsOk()){
        // Get the selected file
        std::string output_dir = ImGuiFileDialog::Instance()->GetCurrentPath();
        // Set the output directory
        simulation.setOutputDir(output_dir);
      }
      // Close the file dialog
      ImGuiFileDialog::Instance()->Close();
      // Close the window
      show_window[(int)SimMenu::CHOOSE_OUTPUT_DIR] = false;
    }
  }

 private:
  Simulation<T> &simulation;
  std::vector<PointCloud<T>> &point_clouds;
  AnimationParameters<T> &anim_params;
  SimulationParameters<T> &sim_params;
  Axes<T> &axes;
  BoundingBox<T> &bounding_box;
  Grid<T> &grid;
  std::string &info_popup_text;
  bool &show_info_popup = false;
  bool initialized = false;

  void createPointClouds(Shader<T> &shader, GLFWwindow* window)
  {
    // Clear the point clouds
    point_clouds.clear();

    // Get the actual simulation parameters
    SimulationParameters<T> sim_params 
      = simulation.state_machine.getScaledParameters();

    // For every agent class create a point cloud
    for (int i=0; i<sim_params.getNumAgentClasses(); i++){
      int pointsize = sim_params.getAgentClass(i).getPointsize();
      int num_points = sim_params.getAgentClass(i).getNumAgents();
      std::string name = sim_params.getAgentClass(i).getName();
      Color<T> color = sim_params.getAgentClass(i).getColor();
      // Pointer to the data
      T* pointer = simulation.getAgentClassPointer(i);
      point_clouds.emplace_back(
          shader, window, pointer, num_points, pointsize, name, color);
    }
  }

  void pauseButton()
  {
    if (simulation.getState() > SimulationState::PAUSED){
      // Pause button
      if (ImGui::Button("Pause")){
        simulation.pause();
      }
    }
  }

  // Show window flags
  bool show_window[(int)SimMenu::NUM_MENU_ITEMS] = {false};
};

#endif //SIMULATION_IM_GUI_HANDLE_H
/* vim: set ts=2 sw=2 tw=0 et :*/
