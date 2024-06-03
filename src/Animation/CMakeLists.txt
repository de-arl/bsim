# Define Animation library
add_library(
  Animation 
  Animation.cpp 
  Shader.cpp
  ImGuiHandler.cpp
  BoundingBox.cpp
  Grid.cpp
  Axes.cpp
  PointCloud.cpp
)

# Set the include directories for Animation library
target_include_directories(
  Animation 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Parameters

)

# Compile options for Animation library
target_compile_options(
  Animation 
  PRIVATE -w
)

# Find required packages
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(WebP REQUIRED)

# Link required libraries to animation
target_link_libraries(
  Animation 
  PUBLIC OpenGL::GL
  PUBLIC Glad
  PUBLIC glfw
  PUBLIC glm::glm
  PUBLIC webp
  PUBLIC webpmux
  PUBLIC sharpyuv
  PUBLIC Simulation
  PUBLIC Parameters
  PUBLIC Utils
  PUBLIC ImGui
)

