#ifndef BOUNDING_BOX_IM_GUI_HANDLE_H
#define BOUNDING_BOX_IM_GUI_HANDLE_H

#include "imgui.h"
#include "BoundingBox.h"
#include "Color.h"

template <typename T>
class BoundingBoxImGuiHandle
{
public:
  BoundingBoxImGuiHandle(
      BoundingBox<T> &bounding_box,
      AnimationParameters<T> &anim_params)
    : bounding_box(bounding_box), anim_params(anim_params)
  {
  }

  void render(bool &show_window)
  {
    // Return if the window should not be shown 
    if (!show_window) return;

    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    if (!ImGui::Begin("Bounding box settings", &show_window, window_flags)){
      ImGui::End();
      return;
    } 

    // Hide/show bounding box
    ImGui::Checkbox("Show bounding box", &anim_params.draw_bounding_box);

    // Separator
    ImGui::SeparatorText("Bounding box color");

    // Color picker
    Color<T> bounding_box_color = bounding_box.getColor();
    ImGui::ColorEdit3("Bounding box color", &anim_params.bounding_box_color.r);
    if (bounding_box_color != anim_params.bounding_box_color){
      bounding_box.setColor(anim_params.bounding_box_color);
    }

    ImGui::End();
  }

private:
  BoundingBox<T> &bounding_box;
  AnimationParameters<T> &anim_params;
};

#endif // BOUNDING_BOX_IM_GUI_HANDLE_H
/* vim: set ts=2 sw=2 tw=0 et :*/
