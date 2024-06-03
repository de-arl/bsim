#ifndef GRID_IM_GUI_HANDLE_H
#define GRID_IM_GUI_HANDLE_H

#include "imgui.h"
#include "Grid.h"
#include "Axes.h"
#include "AnimationParameters.h"

template <typename T>
class GridImGuiHandle
{
public:
  GridImGuiHandle(
      Grid<T> &grid, 
      Axes<T> &axes,
      AnimationParameters<T> &anim_params)
    : grid(grid), axes(axes), anim_params(anim_params)
  {
  }

  void render(bool &show_window)
  {
    // Return if the window should not be shown 
    if (!show_window) return;

    int window_flags = ImGuiWindowFlags_AlwaysAutoResize;
    if (!ImGui::Begin("Grid settings", &show_window, window_flags)){
      ImGui::End();
      return;
    } 
      // Separator
      ImGui::SeparatorText("Number of gridlines");
      // Get number of gridlines
      int num_gridlines = grid.getNumGridlines();
      // Slider for the number of gridlines
      ImGui::SliderInt("lines", &num_gridlines, 1, 100);

      // Apply 
      if (num_gridlines != grid.getNumGridlines()){
        grid.setNumGridlines(num_gridlines);
        grid.generateGrids();
        anim_params.num_gridlines = num_gridlines;
      }

      // Set number of gridlines to number of ticks
      if (ImGui::Button("Adjust to ticks")){
        grid.setNumGridlines(axes.getNumTicks());
        grid.generateGrids();
      }

      // Separator
      ImGui::SeparatorText("Grid selection");

      // Checkboxes for the six grids
      if (!anim_params.autoselect_grids){
        ImGui::Checkbox("xy-plane, lower z", &anim_params.draw_grid[0]);
        ImGui::Checkbox("xy-plane, upper z", &anim_params.draw_grid[1]);
        ImGui::Checkbox("xz-plane, lower y", &anim_params.draw_grid[2]);
        ImGui::Checkbox("xz-plane, upper y", &anim_params.draw_grid[3]);
        ImGui::Checkbox("yz-plane, lower x", &anim_params.draw_grid[4]);
        ImGui::Checkbox("yz-plane, upper x", &anim_params.draw_grid[5]);
        // Deselect all button
        if (ImGui::Button("Select all")){
          for (int i = 0; i < 6; i++){
            anim_params.draw_grid[i] = true;
          }
        }
        ImGui::SameLine();
        // Select all button
        if (ImGui::Button("Select None")){
          for (int i = 0; i < 6; i++){
            anim_params.draw_grid[i] = false;
          }
        }
        // Apply button
        if (ImGui::Button("Apply selection")){
          grid.setToDraw(anim_params.draw_grid);
        }

        // Separator
        ImGui::Separator();
      } 


      // Checkbox for autoselection of grids
      ImGui::Checkbox("Auto-select grids", &anim_params.autoselect_grids);

      // Separator
      ImGui::SeparatorText("Appearance");
      
      // Color picker
      Color<T> grid_color = grid.getColor();
      ImGui::ColorEdit3("Grid color", &anim_params.grid_color.r);
      if (grid_color != anim_params.grid_color){
        grid.setColor(anim_params.grid_color);
      }
    ImGui::End();
  }

private:
  Grid<T> &grid;
  Axes<T> &axes;
  AnimationParameters<T> &anim_params;

};

// Explicit instantiation
template class GridImGuiHandle<float>;
template class GridImGuiHandle<double>;

#endif // GRID_IM_GUI_HANDLE_H
/* vim: set ts=2 sw=2 tw=0 et :*/
