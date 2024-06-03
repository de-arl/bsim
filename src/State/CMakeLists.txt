add_library(
  State
  State.cpp
  Agent.cpp
  AgentClass.cpp
  AgentMetrics.cpp
  SystemMetrics.cpp
  Search.cpp
  LevySearch.cpp
  BrownianSearch.cpp
  asa241.cpp
)

target_include_directories(
  State
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC ${CMAKE_SOURCE_DIR}/src/ConfigParser
  PUBLIC ${CMAKE_SOURCE_DIR}/src/Utils
)

target_link_libraries(
  State
  INTERFACE ConfigParser
  PUBLIC Parameters
)
