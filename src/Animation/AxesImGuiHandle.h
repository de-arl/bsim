#ifndef AXES_IM_GUI_HANDLE_H
#define AXES_IM_GUI_HANDLE_H

#include "imgui.h"
#include "Axes.h"
#include "Grid.h"
#include "Color.h"
#include "AnimationParameters.h"

template <typename T>
class AxesImGuiHandle
{
public:
  AxesImGuiHandle(Axes<T> &axes, Grid<T> &grid, 
      AnimationParameters<T> &anim_params)
    : axes(axes), grid(grid), anim_params(anim_params)
  {
  }

  void render(bool &show_window)
  {
    // Return if the window should not be shown 
    if (!show_window) return;

    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;

    if (!ImGui::Begin("Axes settings", &show_window, window_flags)){
      ImGui::End();
      return;
    } 

    // Checkbox for showing the axes
    ImGui::Checkbox("Show axes", &anim_params.draw_axes);

    // Checkbox for showing the ticks
    ImGui::Checkbox("Show ticks", &anim_params.draw_ticks);

    // Checkbox for showing the labels
    ImGui::Checkbox("Show labels", &anim_params.draw_labels);

    // Separator
    ImGui::SeparatorText("Axes settings");

    // Number of ticks
    ImGui::SliderInt("Number of ticks", &anim_params.num_ticks, 1, 100);
    if (anim_params.num_ticks != axes.getNumTicks()){
      axes.setNumTicks(anim_params.num_ticks);
      axes.generateTicks();
      
    }

    // Set number of ticks to number of gridlines
    if (ImGui::Button("Adjust to grid")){
      axes.setNumTicks(grid.getNumGridlines());
      axes.generateTicks();
    }

    // Separator
    ImGui::SeparatorText("Tick length (inner grid)");

    // Tick length
    float* tick_length_old = new float[6];
    float* tick_length = new float[6];
    axes.getTickLength(tick_length);
    axes.getTickLength(tick_length_old);
    // Get the axes length
    float* axes_length = new float[3];
    axes.getDims(axes_length);
    // Slider for the tick length
    ImGui::SliderFloat("XY", &tick_length[2], 0.0f, axes_length[1]);
    ImGui::SliderFloat("YX", &tick_length[0], 0.0f, axes_length[0]);
    ImGui::SliderFloat("XZ", &tick_length[4], 0.0f, axes_length[2]);
    ImGui::SliderFloat("ZX", &tick_length[1], 0.0f, axes_length[0]);
    ImGui::SliderFloat("YZ", &tick_length[5], 0.0f, axes_length[2]);
    ImGui::SliderFloat("ZY", &tick_length[3], 0.0f, axes_length[1]);
    // Apply
    if (tick_length[0] != tick_length_old[0] ||
        tick_length[1] != tick_length_old[1] ||
        tick_length[2] != tick_length_old[2] ||
        tick_length[3] != tick_length_old[3] ||
        tick_length[4] != tick_length_old[4] ||
        tick_length[5] != tick_length_old[5]){
      axes.setTickLength(tick_length);
      axes.generateTicks();
    }
    delete[] tick_length_old;
    delete[] tick_length;
    delete[] axes_length;


    // Color picker for the axes
    Color<T> axes_color = axes.getAxesColor();
    ImGui::ColorEdit3("Axes color", &anim_params.axes_color.r);
    if (axes_color != anim_params.axes_color){
      axes.setColor(anim_params.axes_color);
    }

    // Color picker for the ticks
    Color<T> tick_color = axes.getTickColor();
    ImGui::ColorEdit3("Tick color", &anim_params.tick_color.r);
    if (tick_color != anim_params.tick_color){
      axes.setTickColor(anim_params.tick_color);
    }
    
    // Color picker for the labels
    Color<T> label_color = axes.getLabelColor();
    ImGui::ColorEdit3("Label color", &anim_params.label_color.r);
    if (label_color != anim_params.label_color){
      axes.setLabelColor(anim_params.label_color);
    }

    ImGui::End();
  }

private:
  Axes<T> &axes;
  Grid<T> &grid;
  AnimationParameters<T> &anim_params;
};

#endif // AXES_IM_GUI_HANDLE_H
/* vim: set ts=2 sw=2 tw=0 et :*/
