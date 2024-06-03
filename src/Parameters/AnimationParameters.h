#ifndef ANIMATION_PARAMETERS_H
#define ANIMATION_PARAMETERS_H

#include <string>
#include <fstream>
#include <iostream>

#include "helpers.h"
#include "toml.hpp"
#include "Color.h"

template <typename T>
class AnimationParametersImGuiHandle;

template <typename T>
class AnimationParameters
{
 public:
  // Default constructor
  AnimationParameters();

  // Destructor
  ~AnimationParameters();

  // Set animation configuration file
  void setAnimationConfigFile(std::string file);
  void setDefaultAnimationConfigFile(std::string file);
  void getAnimationConfigFile(std::string &file);
  void getDefaultAnimationConfigFile(std::string &file);

  // Parse configuration file
  void parseAnimationConfig();

  // Print parameters
  void print(std::ostream &os = std::cout);

  // Save parameters
  void save(std::string file);


  /* Public member variables */

  // GUI 
  int window_width;
  int window_height;
  std::string font_dir; // Directory of the fonts
  std::string font_file;
  int font_size;

  // Shader 
  std::string shader_dir; // Directory of the shaders
  int shader_id; // ID of the shader program 
                 // (case the shader class contains multiple shaders)

  // Background color
  Color<T> background_color;

  // Bounding box
  Color<T> bounding_box_color; // Color of the bounding box
  bool draw_bounding_box; // Draw the bounding box

  // Grid 
  Color<T> grid_color; // Color of the grid
  int num_gridlines; // Number of gridlines in the interval [-1, 1]
  bool draw_grid[6]; // Draw the grids
  bool autoselect_grids; // Automatically select the grids to draw

  // Axes color and line width
  Color<T> axes_color;
  Color<T> tick_color;
  Color<T> label_color;
  int num_ticks; // Number of ticks
  float tick_length[6]; // Length of the ticks
  bool draw_axes; // Draw the axes
  bool draw_ticks; // Draw the ticks
  bool draw_labels; // Draw the labels

  // Show/hide 
  bool animate; // Animate the simulation
                

 private:

  // Parse the configuration file
  void parseGUISection(toml::table config);
  void parseOpenGLSection(toml::table config);
  void parseGeneralSection(toml::table config);
  void parseBoundingBoxSection(toml::table config);
  void parseGridSection(toml::table config);
  void parseAxesSection(toml::table config);

  // Animation configuration file
  std::string animation_config_file;
  std::string default_animation_config_file;

  friend class AnimationParametersImGuiHandle<T>;

};

#endif //ANIMATION_PARAMETERS_H
/* vim: set ts=2 sw=2 tw=0 et :*/
