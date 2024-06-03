add_library(
  Simulation 
  StateMachine.cpp 
  Simulation.cpp
  Results.cpp 
)

target_include_directories(
  Simulation 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${PROJECT_SOURCE_DIR}/src/PAPI
  PUBLIC ${PROJECT_SOURCE_DIR}/src/Parameters
  PUBLIC ${PROJECT_SOURCE_DIR}/src/State
  PUBLIC ${PROJECT_SOURCE_DIR}/src/Utils
  PUBLIC ${PROJECT_SOURCE_DIR}/src/ConfigParser
) 

target_link_libraries(
  Simulation 
  PUBLIC Parameters
  PUBLIC State
  PUBLIC ConfigParser
  INTERFACE Utils
)

if(PARALLEL_VERSION)
  # if compiled with g++, use -fopenmp
  find_package(OpenMP REQUIRED)
  if(OpenMP_CXX_FOUND)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      target_link_libraries(Simulation PUBLIC OpenMP::OpenMP_CXX)
    endif()
  endif()
  # if compiled with icpx, use -qopenmp
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    target_link_libraries(Simulation PUBLIC -qopenmp)
  endif()
endif()

