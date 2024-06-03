add_library(
  Parameters
  SimulationParameters.cpp
  AnimationParameters.cpp
  GlobalParameters.cpp
  AgentClassParameters.cpp
)

target_include_directories(
  Parameters 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_SOURCE_DIR}/src/ConfigParser
  PUBLIC ${CMAKE_SOURCE_DIR}/src/CmdLineParser
  PUBLIC ${CMAKE_SOURCE_DIR}/src/State
  PUBLIC ${CMAKE_SOURCE_DIR}/src/Utils
)

target_link_libraries(Parameters
  INTERFACE ConfigParser
  INTERFACE CmdLineParser
  INTERFACE Utils
  PUBLIC State
)
