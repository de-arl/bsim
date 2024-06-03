#ifndef POINT_CLOUD_IM_GUI_HANDLE_H
#define POINT_CLOUD_IM_GUI_HANDLE_H

#include <vector>
#include "imgui.h"

#include "PointCloud.h"
#include "SimulationParameters.h"
#include "AnimationParameters.h"

template <typename T>
class PointCloudImGuiHandle
{
 public:
  // Constructor
  PointCloudImGuiHandle(
    std::vector<PointCloud<T>> &point_clouds,
    SimulationParameters<T> &sim_params,
    AnimationParameters<T> &anim_params) 
    : point_clouds(point_clouds),
      sim_params(sim_params),
      anim_params(anim_params)
  {
  }

  // Render the ImGui window
  void render(bool &show_window)
  {

    // If the window is not shown, return
    if (!show_window) return;

    // Render the window
    ImGui::Begin("Point Clouds", &show_window);

    // If point clouds is empty, return
    if (point_clouds.empty()) {
      ImGui::Text("No point clouds initialized. Initialize simulation.");
      return;
    }

    // Loop over all point clouds
    for (int i=0; i<point_clouds.size(); i++)
    {
      // Get the point cloud
      PointCloud<T> &point_cloud = point_clouds[i];

      // Get the name of the point cloud
      std::string name = point_cloud.getName();

      // Separator
      ImGui::SeparatorText(name.c_str());

      // Render the checkbox
      std::string id = "Show##" + std::to_string(i);
      ImGui::PushID(id.c_str());
      ImGui::Checkbox("show", &point_cloud.show);
      ImGui::PopID();

      // Point size
      id = "Point size##" + std::to_string(i);
      ImGui::PushID(id.c_str());
      ImGui::SliderFloat("Point size", &point_cloud.pointsize, 1.0, 10.0);
      ImGui::PopID();

      // Color picker
      id = "Color##" + std::to_string(i);
      ImGui::PushID(id.c_str());
      ImGui::ColorEdit3("Color", &point_cloud.color.r);
      ImGui::PopID();

    }

    // End the window
    ImGui::End();
    
  }

 private:
  std::vector<PointCloud<T>> &point_clouds;
  SimulationParameters<T> &sim_params;
  AnimationParameters<T> &anim_params;
  
};

#endif //POINT_CLOUD_IM_GUI_HANDLE_H
/* vim: set ts=2 sw=2 tw=0 et :*/
