#ifndef GLOBAL_PARAMETERS_IM_GUI_HANDLE_H
#define GLOBAL_PARAMETERS_IM_GUI_HANDLE_H

#include "GlobalParameters.h"
#include "imgui.h"


template <typename T>
class GlobalParametersImGuiHandle
{
 public:
  // Constructor
  GlobalParametersImGuiHandle(GlobalParameters<T> &p, std::string &popup_info_text,
      bool &show_info_popup)
    : 
    global_params(p),
    popup_info_text(popup_info_text),
    show_info_popup(show_info_popup)
  {
  }

  // Render the global parameters menu
  void render(bool &show){
    // Return if the window is not shown
    if (!show){
      return;
    }

    // Main menu
    mainMenu(show);
  }

 private:

  // Main menu
  void mainMenu(bool &show){
    // If the window is not shown, return
    if (!show){
      return;
    }

    // Main menu
    ImGui::Begin("Global Parameters", &show);

    // Verbosity
    ImGui::Text("Verbosity");
    ImGui::SliderInt("##verbosity", &global_params.verbosity, 0, 10);

    // Output precision
    ImGui::Text("Output precision");
    ImGui::SliderInt("##output_precision", &global_params.output_precision, 0, 10);

    // End main menu
    ImGui::End();

  }


  // Global parameters
  GlobalParameters<T> &global_params;

  // Popup info text
  std::string &popup_info_text;

  // Show info popup
  bool &show_info_popup;
};

// Explicit instantiation
template class GlobalParametersImGuiHandle<float>;
template class GlobalParametersImGuiHandle<double>;

#endif //GLOBAL_PARAMETERS_IM_GUI_HANDLE_H
/* vim: set ts=2 sw=2 tw=0 et :*/
