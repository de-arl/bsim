add_library(
  ImGui 
  imgui.cpp
  imgui_draw.cpp
  imgui_impl_glfw.cpp
  imgui_impl_opengl3.cpp
  imgui_widgets.cpp
  imgui_tables.cpp
  imgui_demo.cpp
  imgui_freetype.cpp
  ImGuiFileDialog.cpp
)

target_compile_options(
  ImGui 
  PRIVATE -w 
)

target_link_libraries(
  ImGui 
  PUBLIC freetype
)

target_include_directories(
  ImGui 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

set(ImGui_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_PREFIX_PATH ${ImGui_DIR})
