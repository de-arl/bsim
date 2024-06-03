add_library(
  PAPI 
  Papi.cpp)

target_include_directories(
  PAPI 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
  PAPI 
  PUBLIC -lpapi)
