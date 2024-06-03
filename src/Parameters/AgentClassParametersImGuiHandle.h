#ifndef AGENT_CLASS_PARAMETERS_IM_GUI_HANDLE_H
#define AGENT_CLASS_PARAMETERS_IM_GUI_HANDLE_H

#include "AgentClassParameters.h"

#include "imgui.h"
#include "ImGuiFileDialog.h"

enum class ACMenu
{
  MAIN,
  LOAD,
  SAVE,
  SAVE_AS,
  EDIT,
  NUM_MENU_ITEMS
};

template <typename T>
class AgentClassParametersImGuiHandle
{ 
 public:
  // Constructor
  AgentClassParametersImGuiHandle(
      SimulationParameters<T> &sim_params,
      SimulationParameters<T> &sim_params_new,
      AgentClassParameters<T> &agent_class_params_new,
      std::string &popup_info_text,
      bool &show_info_popup)
    : 
    sim_params(sim_params),
    sim_params_new(sim_params_new),
    agent_class_params_new(agent_class_params_new),
    popup_info_text(popup_info_text),
    show_info_popup(show_info_popup)
  {
  }

  // Render the agent class parameters menu
  void render(bool show){
    // Return if the window is not shown
    if (!show){
      return;
    }

    // Main menu
    mainMenu();

    // Windows
    loadDialog();
    saveDialog();
    editDialog();
    editClass();
  }

  // Main menu
  void mainMenu(){

    // Separator
    ImGui::SeparatorText("Agent Class Parameters Settings");
    // Show button
    if (ImGui::Button("Show Agent Class Editor")){
      show_window[(int)ACMenu::MAIN] = true;
    }
  }

  // Load dialog
  void loadDialog(){
    // If the window is not shown, return
    if (!show_window[(int)ACMenu::LOAD]) return;

    // Show the agent class parameter editor in a popup window
    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    // Dialog configuration
    IGFD::FileDialogConfig config;
    config.path = "./config/";
    config.flags = ImGuiFileDialogFlags_Modal;

    // Window size
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);

    // Open the file dialog
    const char* window_title = "Load agent class parameters";
    ImGuiFileDialog::Instance()->OpenDialog(window_title, "Load", ".toml", 
        config);

    // If the file dialog is open
    if (ImGuiFileDialog::Instance()->Display(window_title)){
      // Get the selected file
      if (ImGuiFileDialog::Instance()->IsOk()){
        std::string file = ImGuiFileDialog::Instance()->GetFilePathName();
        // Append the agent class
        sim_params_new.addAgentClass();
        // get the index of the last agent class
        int load_agent_class_index = sim_params_new.getNumAgentClasses()-1;
        sim_params_new.getAgentClass(load_agent_class_index).load(file);
      }
      // End the file dialog
      ImGuiFileDialog::Instance()->Close();
      // Close the window
      show_window[(int)ACMenu::LOAD] = false;
    }
  }

  // Save dialog
  void saveDialog(){

    // If the window should not be shown, return 
    if (!show_window[(int)ACMenu::SAVE]){
      return;
    }

    // Show the agent class parameter editor in a popup window
    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    // Set the filename
    std::string filename = 
      sim_params_new.getAgentClass(save_agent_class_index).getName();
    filename += ".toml";

    // Dialog configuration
    IGFD::FileDialogConfig config;
    config.path = "./config/";
    config.flags = ImGuiFileDialogFlags_Modal;
    config.flags |= ImGuiFileDialogFlags_ConfirmOverwrite;
    config.fileName = filename;

    // Window size
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);

    // Open the file dialog
    const char* window_title = "Save agent class parameters as...";
    ImGuiFileDialog::Instance()->OpenDialog(window_title, "Save", ".toml", 
        config);

    // If the file dialog is open
    if (ImGuiFileDialog::Instance()->Display(window_title)){
      // Get the selected file
      if (ImGuiFileDialog::Instance()->IsOk()){
        std::string file = ImGuiFileDialog::Instance()->GetFilePathName();
        sim_params_new.getAgentClass(save_agent_class_index).save(file);
      }
      // End the file dialog
      ImGuiFileDialog::Instance()->Close();
      // Close the window
      show_window[(int)ACMenu::SAVE] = false;
    }
  }

  // Edit agent classes dialog
  void editDialog(){

    // If the window should not be shown, return 
    if (!show_window[(int)ACMenu::MAIN]){
      return;
    }

    // Show the agent class editor in a popup window
    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    // Open the window
    if (!ImGui::Begin("Agent class editor", 
        &show_window[(int)ACMenu::MAIN] , window_flags)){
      ImGui::End();
      return;
    }

    // Set to current button
    ImGui::SameLine();
    if (ImGui::Button("Set to current")){
      sim_params_new.setAgentClasses(sim_params.getAgentClasses());
    }

    // Add button
    ImGui::SameLine();
    if (ImGui::Button("Add")){
      sim_params_new.addAgentClass();
    }

    // Load button
    ImGui::SameLine();
    if (ImGui::Button("Load")){
      show_window[(int)ACMenu::LOAD] = true;
    }

    // Reset button
    ImGui::SameLine();
    if (ImGui::Button("Reset")){
      sim_params_new.removeAgentClasses();
    }

    // Apply button
    ImGui::SameLine();
    if (ImGui::Button("Apply")){
      sim_params.setAgentClasses(sim_params_new.getAgentClasses());
    }

    // Separator
    ImGui::SeparatorText("Agent classes");

    if (sim_params_new.getNumAgentClasses() == 0){
      ImGui::Text("No agent classes configured.");
    }

    // Loop over the agent classes
    for (int i=0; i<sim_params_new.getNumAgentClasses(); i++){

      // Separator
      ImGui::Separator();

      // Get the agent class name
      std::string name = sim_params_new.getAgentClass(i).getName();

      // Print the agent class name
      ImGui::Text("Agent Class %i: %s", i, name.c_str());

      // Edit button
      ImGui::PushID(i);
      if (ImGui::Button("Edit")){
        show_window[(int)ACMenu::EDIT] = true;
        agent_class_params_new = sim_params_new.getAgentClass(i);
        edit_agent_class_index = i;
      }
      ImGui::PopID();

      // Remove button

      ImGui::SameLine();
      ImGui::PushID(i+1);
      if (ImGui::Button("Remove")){
        sim_params_new.removeAgentClass(i);
        std::cout << "Removed agent class " << i << std::endl;
      }
      ImGui::PopID();

      // Duplicate button
      ImGui::SameLine();
      ImGui::PushID(i+2);
      if (ImGui::Button("Duplicate")){
        sim_params_new.addAgentClass(sim_params_new.getAgentClass(i));
        // Set the name of the new agent class
        std::string new_name = sim_params_new.agent_class_params.back().getName();
        new_name += "_copy";
        sim_params_new.agent_class_params.back().setName(new_name);
      }
      ImGui::PopID();

      // Print button
      ImGui::SameLine();
      ImGui::PushID(i+3);
      if (ImGui::Button("Show")){
        std::stringstream ss;
        sim_params_new.agent_class_params.at(i).print(ss);
        popup_info_text = ss.str();
        show_info_popup = true;
      }
      ImGui::PopID();

      // Save button
      ImGui::SameLine();
      ImGui::PushID(i+4);
      if (ImGui::Button("Save")){
        show_window[(int)ACMenu::SAVE] = true;
        save_agent_class_index = i;
      }
      ImGui::PopID();

    }// end for

    // end the window
    ImGui::End();

  }

  // Edit agent class dialog
  void editClass(){

    // If the window should not be shown, return 
    if (!show_window[(int)ACMenu::EDIT]){
      return;
    }

    // Show the agent class parameter editor in a popup window
    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    // Open the window
    if (!ImGui::Begin("Agent class parameter editor", 
          &show_window[(int)ACMenu::EDIT] , window_flags)){
      ImGui::End();
      return;
    }

    // Set the agent class name
    std::string name = agent_class_params_new.getName();
    char name_buffer[256];
    strcpy(name_buffer, name.c_str());
    if (ImGui::InputText("Name", name_buffer, 256)){
      agent_class_params_new.setName(name_buffer);
    }

    // Add friends  
    // Show friends in vector
    std::vector<std::string> friends = agent_class_params_new.getFriends();
    std::string friends_str = "";
    for (int i=0; i<friends.size(); i++){
      friends_str += friends[i];
      if (i < friends.size()-1){
        friends_str += ", ";
      }
    }

    // Set the friends
    char friends_buffer[1024];
    strcpy(friends_buffer, friends_str.c_str());
    if (ImGui::InputText("Friends", friends_buffer, 1024)){
      std::string friends_str = friends_buffer;
      std::vector<std::string> friends;
      std::string delimiter = ",";
      size_t pos = 0;
      std::string token;
      while ((pos = friends_str.find(delimiter)) != std::string::npos){
        token = friends_str.substr(0, pos);
        friends.push_back(token);
        friends_str.erase(0, pos + delimiter.length());
      }
      friends.push_back(friends_str);
      agent_class_params_new.setFriends(friends);
    }

    // Set the enemies
    std::vector<std::string> enemies = agent_class_params_new.getEnemies();
    std::string enemies_str = "";
    for (int i=0; i<enemies.size(); i++){
      enemies_str += enemies[i];
      if (i < enemies.size()-1){
        enemies_str += ", ";
      }
    }

    // Set the enemies
    char enemies_buffer[1024];
    strcpy(enemies_buffer, enemies_str.c_str());
    if (ImGui::InputText("Enemies", enemies_buffer, 1024)){
      std::string enemies_str = enemies_buffer;
      std::vector<std::string> enemies;
      std::string delimiter = ",";
      size_t pos = 0;
      std::string token;
      while ((pos = enemies_str.find(delimiter)) != std::string::npos){
        token = enemies_str.substr(0, pos);
        enemies.push_back(token);
        enemies_str.erase(0, pos + delimiter.length());
      }
      enemies.push_back(enemies_str);
      agent_class_params_new.setEnemies(enemies);
    }

    // Set the food sources
    std::vector<std::string> food_sources = agent_class_params_new.getFoodSources();
    std::string food_sources_str = "";
    for (int i=0; i<food_sources.size(); i++){
      food_sources_str += food_sources[i];
      if (i < food_sources.size()-1){
        food_sources_str += ", ";
      }
    }

    // Set the food sources
    char food_sources_buffer[1024];
    strcpy(food_sources_buffer, food_sources_str.c_str());
    if (ImGui::InputText("Food sources", food_sources_buffer, 1024)){
      std::string food_sources_str = food_sources_buffer;
      std::vector<std::string> food_sources;
      std::string delimiter = ",";
      size_t pos = 0;
      std::string token;
      while ((pos = food_sources_str.find(delimiter)) != std::string::npos){
        token = food_sources_str.substr(0, pos);
        food_sources.push_back(token);
        food_sources_str.erase(0, pos + delimiter.length());
      }
      food_sources.push_back(food_sources_str);
      agent_class_params_new.setFoodSources(food_sources);
    }


    // Set the number of agents
    int num_agents = agent_class_params_new.getNumAgents();
    if (ImGui::InputInt("Number of agents", &num_agents)){
      agent_class_params_new.setNumAgents(num_agents);
    }

    // Set the strategy
    int strategy = agent_class_params_new.getStrategy();
    if (ImGui::InputInt("Strategy", &strategy)){
      agent_class_params_new.setStrategy(strategy);
    }

    // Set the status 
    int status = agent_class_params_new.getStatus();
    if (ImGui::InputInt("Status", &status)){
      agent_class_params_new.setStatus(status);
    }

    // Set the audibility
    T audibility = agent_class_params_new.getAudibility();
    if (ImGui::InputScalar("Audibility", ImGuiDataType_Float, &audibility)){
      agent_class_params_new.setAudibility(audibility);
    }

    // Set the size
    T size = agent_class_params_new.getSize();
    if (ImGui::InputScalar("Size", ImGuiDataType_Float, &size)){
      agent_class_params_new.setSize(size);
    }

    // Set the start position
    Point3D<T> start_position = agent_class_params_new.getStartPosition();
    T start_position_buffer[3];
    start_position.get(start_position_buffer);
    if (ImGui::InputScalarN("Start position", ImGuiDataType_Float, 
          start_position_buffer, 3)){
      start_position.set(start_position_buffer);
      agent_class_params_new.setStartPosition(start_position);
    }

    // Set random start positions flag
    bool random_start_positions = agent_class_params_new.getRandomStartPositions();
    if (ImGui::Checkbox("Random start positions", &random_start_positions)){
      agent_class_params_new.setRandomStartPositions(random_start_positions);
    }

    ImGui::SeparatorText("Energy parameters");

    // Set the energy distribution
    T mu_energy = agent_class_params_new.getMuEnergy();
    if (ImGui::InputScalar("Energy mu", ImGuiDataType_Float, &mu_energy)){
      agent_class_params_new.setMuEnergy(mu_energy);
    }

    T sigma_energy = agent_class_params_new.getSigmaEnergy();
    if (ImGui::InputScalar("Energy sigma", ImGuiDataType_Float, &sigma_energy)){
      agent_class_params_new.setSigmaEnergy(sigma_energy);
    }

    // Set the energy consumption per time
    T energy_consumption_per_time = agent_class_params_new.getEnergyConsumptionPerTime();
    if (ImGui::InputScalar("Energy consumption per time", 
          ImGuiDataType_Float, &energy_consumption_per_time)){
      agent_class_params_new.setEnergyConsumptionPerTime(energy_consumption_per_time);
    }

    // Set the energy consumption per distance
    T energy_consumption_per_distance = 
      agent_class_params_new.getEnergyConsumptionPerDistance();
    if (ImGui::InputScalar("Energy consumption per distance", 
          ImGuiDataType_Float, &energy_consumption_per_distance)){
      agent_class_params_new.setEnergyConsumptionPerDistance(
          energy_consumption_per_distance);
    }

    // Set the energy uptake rate
    T energy_uptake_rate = agent_class_params_new.getEnergyUptakeRate();
    if (ImGui::InputScalar("Energy uptake rate", 
          ImGuiDataType_Float, &energy_uptake_rate)){
      agent_class_params_new.setEnergyUptakeRate(energy_uptake_rate);
    }

    // Separator
    ImGui::SeparatorText("Individual movement parameters\n are a fraction of the maximal movespeed");

    // Set the movespeed
    T movespeed = agent_class_params_new.getMovespeed();
    if (ImGui::InputScalar("Maximal movespeed", ImGuiDataType_Float, &movespeed)){
      agent_class_params_new.setMovespeed(movespeed);
    }


    // Set the movespeed search
    T movespeed_search = agent_class_params_new.getMovespeedSearch();
    if (ImGui::InputScalar("Movespeed search", 
          ImGuiDataType_Float, &movespeed_search)){
      agent_class_params_new.setMovespeedSearch(movespeed_search);
    }

    // Set the movespeed approach friends
    T movespeed_approach_friends = agent_class_params_new.getMovespeedApproachFriends();
    if (ImGui::InputScalar("Movespeed approach friends", 
          ImGuiDataType_Float, &movespeed_approach_friends)){
      agent_class_params_new.setMovespeedApproachFriends(movespeed_approach_friends);
    }

    // Set the movespeed avoid enemies
    T movespeed_avoid_enemies = agent_class_params_new.getMovespeedAvoidEnemies();
    if (ImGui::InputScalar("Movespeed avoid enemies", 
          ImGuiDataType_Float, &movespeed_avoid_enemies)){
      agent_class_params_new.setMovespeedAvoidEnemies(movespeed_avoid_enemies);
    }

    // Set the movespeed approach food
    T movespeed_approach_food = agent_class_params_new.getMovespeedApproachFood();
    if (ImGui::InputScalar("Movespeed approach food", 
          ImGuiDataType_Float, &movespeed_approach_food)){
      agent_class_params_new.setMovespeedApproachFood(movespeed_approach_food);
    }

    // Separator
    ImGui::SeparatorText("Search parameters");
    T mu_levy = agent_class_params_new.getMuLevy();
    if (ImGui::InputScalar("Mu levy", ImGuiDataType_Float, &mu_levy)){
      agent_class_params_new.setMuLevy(mu_levy);
    }

    T c_levy = agent_class_params_new.getCLevy();
    if (ImGui::InputScalar("C levy", ImGuiDataType_Float, &c_levy)){
      agent_class_params_new.setCLevy(c_levy);
    }

    T brownian_search_duration = agent_class_params_new.getBrownianSearchDuration();
    if (ImGui::InputScalar("Brownian search duration", 
          ImGuiDataType_Float, &brownian_search_duration)){
      agent_class_params_new.setBrownianSearchDuration(brownian_search_duration);
    }


    // Separator text
    ImGui::SeparatorText("Graphical parameters");

    // Set the color
    Color<T> color = agent_class_params_new.getColor();
    ImGui::ColorEdit3("Color", &color.r);
    if (color != agent_class_params_new.getColor()){
      agent_class_params_new.setColor(color);
    }

    // Set the pointsize
    T pointsize = agent_class_params_new.getPointsize();
    if (ImGui::InputScalar("Pointsize", ImGuiDataType_Float, &pointsize)){
      agent_class_params_new.setPointsize(pointsize);
    }

    // Separator  
    ImGui::Separator();

    // Reset button
    if (ImGui::Button("Reset")){
      agent_class_params_new 
        = sim_params_new.getAgentClass(edit_agent_class_index);
    }

    // Apply button
    ImGui::SameLine();
    if (ImGui::Button("Apply")){
      sim_params_new.setAgentClass(
          edit_agent_class_index, agent_class_params_new);
    }

    // Close button
    ImGui::SameLine();
    if (ImGui::Button("Close")){
      show_window[(int)ACMenu::EDIT] = false;
    }
    
    // Close the window
    ImGui::End();
  }
    

 private:
  SimulationParameters<T> &sim_params;
  SimulationParameters<T> &sim_params_new;
  AgentClassParameters<T> &agent_class_params_new;
  int edit_agent_class_index;
  int save_agent_class_index;
  std::string &popup_info_text;
  bool &show_info_popup;
  bool show_window[(int)ACMenu::NUM_MENU_ITEMS] = {false};

};

//Explicit template instantiation
template class AgentClassParametersImGuiHandle<float>;
template class AgentClassParametersImGuiHandle<double>;

#endif //AGENT_CLASS_PARAMETERS_IM_GUI_HANDLE_H
/* vim: set ts=2 sw=2 tw=0 et :*/
