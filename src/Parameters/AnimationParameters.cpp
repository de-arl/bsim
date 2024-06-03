#include "AnimationParameters.h"

template <typename T>
AnimationParameters<T>::AnimationParameters()
 :autoselect_grids(true),
  draw_bounding_box(true),
  draw_axes(true),
  draw_ticks(true),
  draw_grid{true, true, true, true, true, true},
  animate(false),
  shader_id(0)
{
}

template <typename T>
AnimationParameters<T>::~AnimationParameters()
{
}

template <typename T>
void AnimationParameters<T>::setAnimationConfigFile(std::string file)
{
  animation_config_file = file;
}

template <typename T>
void AnimationParameters<T>::setDefaultAnimationConfigFile(std::string file)
{
  default_animation_config_file = file;
}

template <typename T>
void AnimationParameters<T>::getAnimationConfigFile(std::string &file)
{
  file = animation_config_file;
}

template <typename T>
void AnimationParameters<T>::getDefaultAnimationConfigFile(std::string &file)
{
  file = default_animation_config_file;
}

template <typename T>
void AnimationParameters<T>::parseAnimationConfig()
{
#ifdef DEBUG
  std::cout << "AnimationParameters::parseAnimationConfig \
    Parsing animation configuration file" << std::endl;
#endif

  // Parse configuration file
  toml::table config;
  try {
    config = toml::parse_file(animation_config_file);
  } catch (const toml::parse_error &e) {
    std::cerr << "Error parsing animation configuration file: " << e.what() 
      << std::endl;
    std::cerr << "animation_config_file: " 
      << animation_config_file << std::endl;
    std::cerr << "Using default animation configuration file: "
      << default_animation_config_file << std::endl;
    try {
      config = toml::parse_file(default_animation_config_file);
    } catch (const toml::parse_error &e) {
      std::cerr << "Error parsing default animation configuration file: " 
        << e.what() << std::endl;
      std::cerr << "Using default values" << std::endl;
    }
  }

  // Parse the GUI section
  parseGUISection(config);

  // Parse the OpenGL section
  parseOpenGLSection(config);
   
  // Parse the general section  
  parseGeneralSection(config);

  // Parse the bounding box section
  parseBoundingBoxSection(config);

  // Parse the grid section
  parseGridSection(config);

  // Parse the axes section
  parseAxesSection(config);
}

template <typename T>
void AnimationParameters<T>::parseGUISection(toml::table config)
{
  if (config.contains("GUI")) {

    // Parse the window width and height
    if (config["GUI"].as_table()->contains("window_width")) {
      window_width = config["GUI"]["window_width"].value_or(800);
    } else {
      window_width = 800;
      handleError("Window width not found in animation configuration file, \n\
          using default value of 800", WARNING);
    }
    if (config["GUI"].as_table()->contains("window_height")) {
      window_height = config["GUI"]["window_height"].value_or(600);
    } else {
      window_height = 600;
      handleError("Window height not found in animation configuration file, \n\
          using default value of 600", WARNING);
    }

    // Parse the font directory
    if (config["GUI"].as_table()->contains("font_dir")) {
      font_dir = config["GUI"]["font_dir"].value_or("config/fonts/");
    } else {
      font_dir = "config/fonts/";
      handleError(
          "Font directory not found in animation configuration file, \n\
          using default directory 'config/fonts'", WARNING);
    }

    // Parse the font 
    if (config["GUI"].as_table()->contains("font_file")) {
       font_file = config["GUI"]["font_file"].value_or("Roboto-Medium.ttf");
    } else {
       font_file = "Roboto-Medium.ttf";
       handleError("Font not found in animation configuration file, \n\
          using default font", WARNING);
    }
    
    // Parse the font size
    if (config["GUI"].as_table()->contains("font_size")) {
       font_size = config["GUI"]["font_size"].value_or(16);
    } else {
       font_size = 16;
       handleError("Font size not found in animation configuration file, \n\
          using default font size", WARNING);
    }


  } else {
    window_width = 800;
    window_height = 600;
    font_dir = "config/fonts/";
    font_file = "Roboto-Medium.ttf";
    font_size = 16;
    handleError("GUI section not found in animation configuration file, \n\
        using default values", WARNING);
  }
}

template <typename T>
void AnimationParameters<T>::parseOpenGLSection(toml::table config)
{
  if (config.contains("OpenGL")) {
    // Parse the shader directory
    if (config["OpenGL"].as_table()->contains("shader_dir")) {
      shader_dir = config["OpenGL"]["shader_dir"].value_or("shaders");
    } else {
      shader_dir = "shaders";
      handleError(
          "Shader directory not found in animation configuration file, \n\
          using default directory 'shaders'", WARNING);
    }
    // Parse the shader id
    if (config["OpenGL"].as_table()->contains("shader_id")) {
      shader_id = config["OpenGL"]["shader_id"].value_or(0);
    } else {
      shader_id = 0;
      handleError(
          "Shader id not found in animation configuration file, \n\
          using default value of 0", WARNING);
    }
  } else {
    shader_dir = "shaders";
    shader_id = 0;
    handleError(
        "OpenGL section not found in animation configuration file, \n\
        using default values", WARNING);
  }
}

template <typename T>
void AnimationParameters<T>::parseGeneralSection(toml::table config)
{
  if (config.contains("General")) {
    // Parse the background color
    if (config["General"].as_table()->contains("background_color")) {
      background_color = Color<T>(
          config["General"]["background_color"][0].value_or(0.0),
          config["General"]["background_color"][1].value_or(0.0),
          config["General"]["background_color"][2].value_or(0.0));
    } else {
      background_color = Color<T>(0.0, 0.0, 0.0);
      handleError(
          "Background color not found in animation configuration file, \n\
          using default color (0.0, 0.0, 0.0)", WARNING);
    }
  } else {
    background_color = Color<T>(0.0, 0.0, 0.0);
    handleError(
        "General section not found in animation configuration file, \n\
        using default values", WARNING);
  }
}

template <typename T>
void AnimationParameters<T>::parseBoundingBoxSection(toml::table config)
{
  if (config.contains("BoundingBox")) {
    // Parse the bounding box color
    if (config["BoundingBox"].as_table()->contains("bounding_box_color")) {
      bounding_box_color = Color<T>(
          config["BoundingBox"]["bounding_box_color"][0].value_or(1.0),
          config["BoundingBox"]["bounding_box_color"][1].value_or(1.0),
          config["BoundingBox"]["bounding_box_color"][2].value_or(1.0));
    } else {
      bounding_box_color = Color<T>(1.0, 1.0, 1.0);
      handleError(
          "Bounding box color not found in animation configuration file, \n\
          using default color (1.0, 1.0, 1.0)", WARNING);
    }

    // Parse the draw bounding box flag
    if (config["BoundingBox"].as_table()->contains("show_bounding_box")) {
      draw_bounding_box = 
        config["BoundingBox"]["show_bounding_box"].value_or(true);
    } else {
      draw_bounding_box = true;
      handleError(
          "Show bounding box flag not found in animation configuration file, \n\
          using default value of true", WARNING);
    }

  } else {
    bounding_box_color = Color<T>(1.0, 1.0, 1.0);
    draw_bounding_box = true;
    handleError(
        "Bounding box section not found in animation configuration file, \n\
        using default values", WARNING);
  }
}

template <typename T>
void AnimationParameters<T>::parseGridSection(toml::table config)
{
  if (config.contains("Grid")) {
    // Parse the grid color
    if (config["Grid"].as_table()->contains("grid_color")) {
      grid_color = Color<T>(
          config["Grid"]["grid_color"][0].value_or(0.5),
          config["Grid"]["grid_color"][1].value_or(0.5),
          config["Grid"]["grid_color"][2].value_or(0.5));
    } else {
      grid_color = Color<T>(0.5, 0.5, 0.5);
      handleError(
          "Grid color not found in animation configuration file, \n\
          using default color (0.5, 0.5, 0.5)", WARNING);
    }

    // Parse the number of gridlines
    if (config["Grid"].as_table()->contains("num_gridlines")) {
      num_gridlines = config["Grid"]["num_gridlines"].value_or(10);
    } else {
      num_gridlines = 10;
      handleError(
          "Number of gridlines not found in animation configuration file, \n\
          using default value of 10", WARNING);
    }

    // Parse the autoselect grids flag
    if (config["Grid"].as_table()->contains("autoselect_grids")) {
      autoselect_grids = config["Grid"]["autoselect_grids"].value_or(true);
    } else {
      autoselect_grids = true;
      handleError(
          "Autoselect grids flag not found in animation configuration file, \n\
          using default value of true", WARNING);
    }

    // Parse the draw grid flags
    if (config["Grid"].as_table()->contains("show_grid")) {
      bool show_grid = config["Grid"]["show_grid"].value_or(true);
      if (show_grid){
        for (int i = 0; i < 6; i++) {
          draw_grid[i] = true;
        }
        autoselect_grids = true;
      } else {
        for (int i = 0; i < 6; i++) {
          draw_grid[i] = false;
        }
        autoselect_grids = false;
      }
    } else {
      for (int i = 0; i < 6; i++) {
        draw_grid[i] = true;
      }
      autoselect_grids = true;
      handleError(
          "Show grid flag not found in animation configuration file, \n\
          using default value of true", WARNING);
    }
  } else {
    grid_color = Color<T>(0.5, 0.5, 0.5);
    num_gridlines = 10;
    autoselect_grids = true;
    for (int i = 0; i < 6; i++) {
      draw_grid[i] = true;
    }
    handleError(
        "Grid section not found in animation configuration file, \n\
        using default values", WARNING);
  }
}

template <typename T>
void AnimationParameters<T>::parseAxesSection(toml::table config)
{
  if (config.contains("Axes")) {
    // Parse the axes color
    if (config["Axes"].as_table()->contains("axes_color")) {
      axes_color = Color<T>(
          config["Axes"]["axes_color"][0].value_or(1.0),
          config["Axes"]["axes_color"][1].value_or(1.0),
          config["Axes"]["axes_color"][2].value_or(1.0));
    } else {
      axes_color = Color<T>(1.0, 1.0, 1.0);
      handleError(
          "Axes color not found in animation configuration file, \n\
          using default color (1.0, 1.0, 1.0)", WARNING);
    }

    // Parse the tick color
    if (config["Axes"].as_table()->contains("tick_color")) {
      tick_color = Color<T>(
          config["Axes"]["tick_color"][0].value_or(1.0),
          config["Axes"]["tick_color"][1].value_or(1.0),
          config["Axes"]["tick_color"][2].value_or(1.0));
    } else {
      tick_color = Color<T>(1.0, 1.0, 1.0);
      handleError(
          "Tick color not found in animation configuration file, \n\
          using default color (1.0, 1.0, 1.0)", WARNING);
    }

    // Parse the label color
    if (config["Axes"].as_table()->contains("label_color")) {
      label_color = Color<T>(
          config["Axes"]["label_color"][0].value_or(1.0),
          config["Axes"]["label_color"][1].value_or(1.0),
          config["Axes"]["label_color"][2].value_or(1.0));
    } else {
      label_color = Color<T>(1.0, 1.0, 1.0);
      handleError(
          "Label color not found in animation configuration file, \n\
          using default color (1.0, 1.0, 1.0)", WARNING);
    }

    // Parse the number of ticks
    if (config["Axes"].as_table()->contains("num_ticks")) {
      num_ticks = config["Axes"]["num_ticks"].value_or(10);
    } else {
      num_ticks = 10;
      handleError(
          "Number of ticks not found in animation configuration file, \n\
          using default value of 10", WARNING);
    }

    // Parse the tick length
    if (config["Axes"].as_table()->contains("tick_length")) {
      tick_length[0] = config["Axes"]["tick_length"][0].value_or(0.05);
      tick_length[1] = config["Axes"]["tick_length"][1].value_or(0.05);
      tick_length[2] = config["Axes"]["tick_length"][2].value_or(0.05);
      tick_length[3] = config["Axes"]["tick_length"][3].value_or(0.00);
      tick_length[4] = config["Axes"]["tick_length"][4].value_or(0.00);
      tick_length[5] = config["Axes"]["tick_length"][5].value_or(0.00);
    } else {
      tick_length[0] = 0.05;
      tick_length[1] = 0.05;
      tick_length[2] = 0.05;
      tick_length[3] = 0.00;
      tick_length[4] = 0.00;
      tick_length[5] = 0.00;
      handleError(
          "Tick length not found in animation configuration file, \n\
          using default values [0.05, 0.05, 0.05, 0.0, 0.0, 0.0]", WARNING);
    }

    // Parse the draw axes flag
    if (config["Axes"].as_table()->contains("show_axes")) {
      draw_axes = config["Axes"]["show_axes"].value_or(true);
    } else {
      draw_axes = true;
      handleError(
          "Show axes flag not found in animation configuration file, \n\
          using default value of true", WARNING);
    }

    // Parse the draw ticks flag
    if (config["Axes"].as_table()->contains("show_ticks")) {
      draw_ticks = config["Axes"]["show_ticks"].value_or(true);
    } else {
      draw_ticks = true;
      handleError(
          "Show ticks flag not found in animation configuration file, \n\
          using default value of true", WARNING);
    }

    // Parse the draw labels flag
    if (config["Axes"].as_table()->contains("show_labels")) {
      draw_labels = config["Axes"]["show_labels"].value_or(true);
    } else {
      draw_labels = true;
      handleError(
          "Show labels flag not found in animation configuration file, \n\
          using default value of true", WARNING);
    }

  } else {
    axes_color = Color<T>(1.0, 1.0, 1.0);
    tick_color = Color<T>(1.0, 1.0, 1.0);
    label_color = Color<T>(1.0, 1.0, 1.0);
    num_ticks = 10;
    tick_length[0] = 0.05;
    tick_length[1] = 0.05;
    tick_length[2] = 0.05;
    tick_length[3] = 0.00;
    tick_length[4] = 0.00;
    tick_length[5] = 0.00;
    draw_axes = true;
    draw_ticks = true;
    draw_labels = true;
    handleError(
        "Axes section not found in animation configuration file, \n\
        using default values", WARNING);
  }
}



template <typename T>
void AnimationParameters<T>::print(std::ostream &stream)
{
  // OpenGL
  stream << "[OpenGL]" << std::endl;
  stream << "  shader_dir = \"" << shader_dir << "\"" << std::endl;
  stream << "  shader_id = " << shader_id << std::endl;
  stream << std::endl;

  // GUI
  stream << "[GUI]" << std::endl;
  stream << "  window_width = " << window_width << std::endl;
  stream << "  window_height = " << window_height << std::endl;
  stream << "  font_dir = \"" << font_dir << "\"" << std::endl;
  stream << "  font_file = \"" << font_file << "\"" << std::endl;
  stream << "  font_size = " << font_size << std::endl;
  stream << std::endl;

  // General
  stream << "[General]" << std::endl;
  stream << "  background_color = [" << background_color.toString() << "]" 
    << std::endl;
  stream << std::endl;

  // Bounding box
  stream << "[BoundingBox]" << std::endl;
  stream << "  bounding_box_color = [" << bounding_box_color.toString() << "]" 
    << std::endl;
  stream << "  show_bounding_box = " << draw_bounding_box << std::endl;
  stream << std::endl;

  // Grid
  stream << "[Grid]" << std::endl;
  stream << "  grid_color = [" << grid_color.toString() << "]" << std::endl;
  stream << "  num_gridlines = " << num_gridlines << std::endl;
  stream << "  autoselect_grids = " << autoselect_grids << std::endl;
  stream << "  show_grid = [ " << draw_grid[0] << ", "
    << draw_grid[1] << ", "
    << draw_grid[2] << ", "
    << draw_grid[3] << ", "
    << draw_grid[4] << ", "
    << draw_grid[5] << " ]"
    << std::endl;
  stream << std::endl;

  // Axes
  stream << "[Axes]" << std::endl;
  stream << "  axes_color = [" << axes_color.toString() << "]" << std::endl;
  stream << "  tick_color = [" << tick_color.toString() << "]" << std::endl;
  stream << "  label_color = [" << label_color.toString() << "]" << std::endl;
  stream << "  num_ticks = " << num_ticks << std::endl;
  stream << "  tick_length = ["
    << tick_length[0] << ", "
    << tick_length[1] << ", "
    << tick_length[2] << ", "
    << tick_length[3] << ", "
    << tick_length[4] << ", "
    << tick_length[5] << "]" << std::endl;
  stream << "  show_axes = " << draw_axes << std::endl;
  stream << "  show_ticks = " << draw_ticks << std::endl;
  stream << "  show_labels = " << draw_labels << std::endl;
}

template <typename T>
void AnimationParameters<T>::save(std::string file)
{
  // Open the file
  std::ofstream ofs(file);

  // Write the config in toml format
  ofs << "# Animation configuration file" << std::endl;
  ofs << std::endl;

  // Print the animation parameters
  print(ofs);

  // Close the file
  ofs.close();

}

// Explicit instantiations
template class AnimationParameters<float>;
template class AnimationParameters<double>;

/* vim: set ts=2 sw=2 tw=0 et :*/
