#ifndef ANIMATION_PARAMETERS_IM_GUI_HANDLE_H
#define ANIMATION_PARAMETERS_IM_GUI_HANDLE_H

#include <vector>
#include <string>

#include "AnimationParameters.h"
#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "BoundingBox.h"
#include "Grid.h"
#include "Axes.h"
#include "PointCloud.h"

enum class APMenu
{
  ANIM_PARAMS_SAVE,
  ANIM_PARAMS_LOAD,
  ANIM_PARAMS_EDIT,
  NUM_MENUS
};

template <typename T>
class AnimationParametersImGuiHandle
{
 public:
  // Constructor
  AnimationParametersImGuiHandle(
      AnimationParameters<T> &anim_params,
      BoundingBox<T> &bounding_box,
      Grid<T> &grid,
      Axes<T> &axes,
      std::vector<PointCloud<T>> &point_clouds,
      std::string &info_popup_text,
      bool &show_info_popup
      )
    : anim_params(anim_params),
      bounding_box(bounding_box),
      grid(grid),
      axes(axes),
      point_clouds(point_clouds),
      info_popup_text(info_popup_text),
      show_info_popup(show_info_popup)
  {
  }

  // Render the ImGui window
  void render(bool &show)
  {
    // Render the main menu
    mainMenu(show);

    // Render the windows
    saveDialog();
    loadDialog();

    edit();
  }

  void mainMenu(bool &show)
  {

    // If the window is not shown, return
    if (!show) return;

    // Render the window
    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (!ImGui::Begin("Animation Parameters", &show, window_flags))
    {
      ImGui::End();
      return;
    }

    // Button to save the animation parameters
    std::string id = "SaveAP##" + std::to_string((int)APMenu::ANIM_PARAMS_SAVE);
    ImGui::PushID(id.c_str());
    if (ImGui::Button("Save")){
      std::string file;
      anim_params.getAnimationConfigFile(file);
      anim_params.save(file);
    }
    ImGui::PopID();

    // Save as button
    ImGui::SameLine();
    id = "SaveAsAP##" + std::to_string((int)APMenu::ANIM_PARAMS_SAVE);
    ImGui::PushID(id.c_str());
    if (ImGui::Button("Save as...")){
      this->show_window[(int)APMenu::ANIM_PARAMS_SAVE] = true;
    }
    ImGui::PopID();


    // Button to load the animation parameters
    ImGui::SameLine();
    id = "LoadAP##" + std::to_string((int)APMenu::ANIM_PARAMS_LOAD);
    ImGui::PushID(id.c_str());
    if (ImGui::Button("Load")){
      this->show_window[(int)APMenu::ANIM_PARAMS_LOAD] = true;
    }
    ImGui::PopID();

    // Edit button
    ImGui::SameLine();
    id = "EditAP##" + std::to_string((int)APMenu::ANIM_PARAMS_EDIT);
    ImGui::PushID(id.c_str());
    if (ImGui::Button("Edit")){
      this->show_window[(int)APMenu::ANIM_PARAMS_EDIT] = true;
    }
    ImGui::PopID();


    // Show button
    ImGui::SameLine();
    id = "ShowAP##" + std::to_string((int)APMenu::ANIM_PARAMS_EDIT);
    ImGui::PushID(id.c_str());
    if (ImGui::Button("Show")){
      std::stringstream ss;
      anim_params.print(ss);
      info_popup_text = ss.str();
      show_info_popup = true;
    }
    ImGui::PopID();

    // End the window
    ImGui::End();
  }

  void saveDialog()
  {
    // If the window should not be shown, return
    if (!show_window[(int)APMenu::ANIM_PARAMS_SAVE]) return;

    // Dialog configuration
    IGFD::FileDialogConfig config;
    config.path = "./config/";
    config.flags = ImGuiFileDialogFlags_Modal;
    // Window size
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
    // Default file
    std::string default_file;
    anim_params.getAnimationConfigFile(default_file);
    config.fileName = default_file;
    // Open the file dialog
    const char* window_title = "Save animation parameters as...";
    ImGuiFileDialog::Instance()->OpenDialog(window_title, "Save", ".toml", 
        config);

    // If the file dialog is open
    if (ImGuiFileDialog::Instance()->Display(window_title)){
      // Get the selected file
      if (ImGuiFileDialog::Instance()->IsOk()){
        std::string file = ImGuiFileDialog::Instance()->GetFilePathName();
        anim_params.save(file);
      }
      // End the file dialog
      ImGuiFileDialog::Instance()->Close();
      // Close the window
      show_window[(int)APMenu::ANIM_PARAMS_SAVE] = false;
    }

  }

  void loadDialog()
  {
    // If the window should not be shown, return
    if (!show_window[(int)APMenu::ANIM_PARAMS_LOAD]) return;

    // Dialog configuration
    IGFD::FileDialogConfig config;
    config.path = "./config/";
    config.flags = ImGuiFileDialogFlags_Modal;
    // Window size
    ImGui::SetNextWindowSize(ImVec2(640, 480), ImGuiCond_FirstUseEver);
    // Default file
    std::string default_file;
    anim_params.getAnimationConfigFile(default_file);
    config.fileName = default_file;
    // Open the file dialog
    const char* window_title = "Load animation parameters";
    ImGuiFileDialog::Instance()->OpenDialog(window_title, "Load", ".toml", 
        config);

    // If the file dialog is open
    if (ImGuiFileDialog::Instance()->Display(window_title)){
      // Get the selectedfile
      if (ImGuiFileDialog::Instance()->IsOk()){
        std::string file = ImGuiFileDialog::Instance()->GetFilePathName();
        anim_params.setAnimationConfigFile(file);
        anim_params.parseAnimationConfig();
        reloadObjects();
      }
      // End the file dialog
      ImGuiFileDialog::Instance()->Close();
      // Close the window
      show_window[(int)APMenu::ANIM_PARAMS_LOAD] = false;
    }
  }

  // Edit the animation parameters
  void edit()
  {
    // Close the window if it should not be shown
    if (!show_window[(int)APMenu::ANIM_PARAMS_EDIT]) return;

    // Open the window

    // Render the window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (!ImGui::Begin("Edit Animation Parameters", 
        &show_window[(int)APMenu::ANIM_PARAMS_EDIT], window_flags)){
      ImGui::End();
      return;
    }

    // Separator
    ImGui::SeparatorText("Settings applied at restart:");
    // Window size input fields
    ImGui::InputInt("Window width", &anim_params.window_width);
    ImGui::InputInt("Window height", &anim_params.window_height);

    // Font
    // Separator
    ImGui::SeparatorText("Font settings");
    std::string font_dir = anim_params.font_dir;
    char font_dir_cstr[256];
    strcpy(font_dir_cstr, font_dir.c_str());
    if (ImGui::InputText("Font directory", font_dir_cstr, 256)){
      anim_params.font_dir = std::string(font_dir_cstr);
    }

    std::string font_file = anim_params.font_file;
    char font_file_cstr[256];
    strcpy(font_file_cstr, font_file.c_str());
    if (ImGui::InputText("Font file", font_file_cstr, 256)){
      anim_params.font_file = std::string(font_file_cstr);
    }

    ImGui::InputInt("Font size", &anim_params.font_size);

    // End the window
    ImGui::End();

  }

 private:

  // Reload the objects (grid, axes, bounding box),
  void reloadObjects()
  {
    // Grid
    grid.initParams(anim_params);
    grid.generateGrids();

    // Bounding box
    bounding_box.initBoundingBox();
    bounding_box.initParams(anim_params);

    // Axes
    axes.initParams(anim_params);
    axes.generateTicks();
  }

  // Animation parameters
  AnimationParameters<T> &anim_params;

  // Bounding box
  BoundingBox<T> &bounding_box;

  // Grid
  Grid<T> &grid;

  // Axes
  Axes<T> &axes;

  // Point clouds
  std::vector<PointCloud<T>> &point_clouds;

  // Show window flags
  bool show_window[(int)APMenu::NUM_MENUS] = {false};

  // Info popup text
  std::string &info_popup_text;
  bool &show_info_popup;
};

// Explicit instantiation
template class AnimationParametersImGuiHandle<float>;
template class AnimationParametersImGuiHandle<double>;
#endif
/* vim: set ts=2 sw=2 tw=0 et :*/
