#ifndef SIMULATION_PARAMETERS_IM_GUI_HANDLE_H
#define SIMULATION_PARAMETERS_IM_GUI_HANDLE_H

#include "SimulationParameters.h"
#include "AgentClassParametersImGuiHandle.h"

#include "imgui.h"
#include "ImGuiFileDialog.h"

enum class SPMenu
{
  MAIN,
  LOAD,
  SAVE,
  SAVE_AS,
  EDIT,
  NUM_MENU_ITEMS
};

template <typename T>
class SimulationParametersImGuiHandle
{
 public:
  // Constructor
  SimulationParametersImGuiHandle(
      AgentClassParametersImGuiHandle<T> &agent_class_params_imgui_handle,
      SimulationParameters<T> &p, 
      SimulationParameters<T> &p_new,
      std::string &popup_info_text,
      bool &show_info_popup)
    : 
    agent_class_params_imgui_handle(agent_class_params_imgui_handle),
    sim_params(p),
    sim_params_new(p_new),
    popup_info_text(popup_info_text),
    show_info_popup(show_info_popup)
  {
  }

  // Render the simulation parameters menu
  void render(bool &show){
    // Return if the window is not shown
    if (!show){
      return;
    }

    // Main menu
    mainMenu(show);

    // Windows
    loadDialog();
    saveDialog();
    editDialog();
  }

 private:

  // Main menu
  void mainMenu(bool &show){
    // If the window is not shown, return
    if (!show) return;

    // Render the window
    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (!ImGui::Begin("Simulation Parameter Editor", &show, window_flags))
    {
      ImGui::End();
      return;
    }

    ImGui::Text("To use these parameters, click on the init button in the simulation control window");

    // Button to edit simulation parameters
    ImGui::SeparatorText("Simulation settings");

    // Button to load simulation parameters
    std::string id = "SPMenu::LOAD";
    ImGui::PushID(id.c_str());
    if (ImGui::Button("Load")){
      show_window[(int)SPMenu::LOAD] = true;
    }
    ImGui::PopID();

    // Save as button
    ImGui::SameLine();
    id = "SPMenu::SAVE_AS";
    ImGui::PushID(id.c_str());
    if (ImGui::Button("Save")){
      show_window[(int)SPMenu::SAVE_AS] = true;
    }
    ImGui::PopID();

    // Show button 
    ImGui::SameLine();
    id = "SPMenu::SHOW";
    ImGui::PushID(id.c_str());
    if (ImGui::Button("Show")){
      std::stringstream ss;
      if (sim_params.getSimParamsInitialized()){
        sim_params.printSimulationParameters(ss);
      }else{
        ss << "Simulation parameters not initialized";
      }
      popup_info_text = ss.str();
      show_info_popup = true;
    }
    ImGui::PopID();

    id = "SPMenu::EDIT";
    ImGui::PushID(id.c_str());
    if (ImGui::Button("Simulation Settings Editor")){
      show_window[(int)SPMenu::EDIT] = true;
    }
    ImGui::PopID();

    // Agent class parameters
    agent_class_params_imgui_handle.render(show);

    
    // End the window
    ImGui::End();

  }// Main menu

  // Load dialog
  void loadDialog(){
    // If the window is not shown, return
    if (!show_window[(int)SPMenu::LOAD]){
      return;
    }

    // Dialog configuration
    IGFD::FileDialogConfig config;
    config.path = "./config/";
    config.flags = ImGuiFileDialogFlags_Modal;
    // Window size
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
    // Default file
    std::string default_file;
    sim_params.getSimulationConfigFile(default_file);
    config.fileName = default_file;
    // Open the file dialog
    const char* window_title = "Load simulation parameters";
    ImGuiFileDialog::Instance()->OpenDialog(window_title, "Load", ".toml", 
        config);

    // If the file dialog is open
    if (ImGuiFileDialog::Instance()->Display(window_title)){
      // Get the selected file
      if (ImGuiFileDialog::Instance()->IsOk()){
        std::string file = ImGuiFileDialog::Instance()->GetFilePathName();
        sim_params.setSimulationConfigFile(file);
        sim_params.parseSimulationConfig();
      }
      // End the file dialog
      ImGuiFileDialog::Instance()->Close();
      // Close the window
      show_window[(int)SPMenu::LOAD] = false;
    }
  }// Load dialog


  // Save dialog
  void saveDialog(){
    // If the window is not shown, return
    if (!show_window[(int)SPMenu::SAVE_AS]){
      return;
    }

    // Dialog configuration
    IGFD::FileDialogConfig config;
    config.flags = ImGuiFileDialogFlags_Modal;
    config.flags |= ImGuiFileDialogFlags_ConfirmOverwrite;
    // Window size
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
    // Default file
    std::string default_file;
    sim_params.getSimulationConfigFile(default_file);
    config.fileName = default_file;
    // Open the file dialog
    const char* window_title = "Save simulation parameters as...";
    ImGuiFileDialog::Instance()->OpenDialog(window_title, "Save", ".toml", 
        config);

    // If the file dialog is open
    if (ImGuiFileDialog::Instance()->Display(window_title)){
      // Get the selected file
      if (ImGuiFileDialog::Instance()->IsOk()){
        std::string file = ImGuiFileDialog::Instance()->GetFilePathName();
        sim_params.save(file);
#ifdef DEBUG
        std::cout << "Saved simulation parameters to " << file << std::endl;
#endif
      }
      // End the file dialog
      ImGuiFileDialog::Instance()->Close();
      // Close the window
      show_window[(int)SPMenu::SAVE_AS] = false;
    }
  }// Save dialog
  
  // Edit dialog
  void editDialog(){

    // If the window is not shown, return
    if (!show_window[(int)SPMenu::EDIT]){
      return;
    }
    // Show the simulation parameter editor in a popup window
    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    // Open the window
    if (!ImGui::Begin("Simulation parameter editor", 
        &show_window[(int)SPMenu::EDIT] , window_flags)){
      ImGui::End();
      return;
    }

    // Set world size
    T dims[3];
    sim_params_new.getWorldsize(dims);
    if (ImGui::InputScalarN("World size", ImGuiDataType_Float, dims, 3)){
      sim_params_new.setWorldsize(dims);
    }

    // Set time step
    T dt = sim_params_new.getDt();
    if (ImGui::InputScalar("Time step", ImGuiDataType_Float, &dt)){
      sim_params_new.setDt(dt);
    }

    // Set simulated time
    ImGui::SeparatorText("Simulation time limit, 0 for unlimited time");
    int t = sim_params_new.getSimulatedTime();
    if (ImGui::InputInt("Simulated time", &t)){
      sim_params_new.setSimulatedTime(t);
    }

    // Set number of runs
    int num_runs = sim_params_new.getNumRuns();
    if (ImGui::InputInt("Number of runs", &num_runs)){
      sim_params_new.setNumRuns(num_runs);
    }

    ImGui::SeparatorText("Seed = 0 for non-deterministic behavior");

    // Set random number generator seed
    int seed = sim_params_new.getSeed();
    if (ImGui::InputInt("Random Seed", &seed)){
      sim_params_new.setSeed(seed);
    }

    // Ask the user wether to edit the current simulation parameters
    if (ImGui::Button("Set to current")){
      sim_params_new = sim_params;
    }

    // Reset the simulation parameters
    ImGui::SameLine();
    if (ImGui::Button("Reset")){
      sim_params_new = SimulationParameters<T>();
    }

    // Apply the new simulation parameters
    ImGui::SameLine();
    if (ImGui::Button("Apply")){
      sim_params = sim_params_new;
      sim_params.init();
    }

    // End the window
    ImGui::End();

  }// Edit dialog
   
  /* Private member variables */
  SimulationParameters<T> &sim_params;
  SimulationParameters<T> &sim_params_new;
  AgentClassParametersImGuiHandle<T> &agent_class_params_imgui_handle;

  std::string &popup_info_text;
  bool &show_info_popup;

  // Show window flags
  bool show_window[(int)SPMenu::NUM_MENU_ITEMS] = {false};

};

// Explicit instantiation
template class SimulationParametersImGuiHandle<float>;
template class SimulationParametersImGuiHandle<double>;

#endif //SIMULATION_PARAMETERS_IM_GUI_HANDLE_H
/* vim: set ts=2 sw=2 tw=0 et :*/
