cmake_minimum_required(
  VERSION 3.27
)
cmake_policy(VERSION 3.27)
project(
  bsim 
  VERSION 0.1 
  DESCRIPTION "bird simulation" 
  LANGUAGES CXX C)

# Set C++ standard
set(
  CMAKE_CXX_STANDARD 17
  CMAKE_CXX_STANDARD_REQUIRED True
  CMAKE_WARN_DEPRECATED False
)

# Set build defaults
set ( CMAKE_C_FLAGS_DEBUG "-DDEBUG -Og -g" )
set ( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" )

set ( CMAKE_C_FLAGS_RELEASE "-DRELEASE -O3 -march=native" )
set ( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" )

set ( CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELEASE} -DDEBUG -g -O3" )
set ( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Define options
option(
  USE_ADDRESS_SANITIZER 
  "Check for memory leaks" 
  OFF)

option(
  PRINT_OPT_INFO 
  "Print optimization information during compilation" 
  OFF)

option(
  COMPILATION_WARNINGS 
  "Print warnings during compilation" 
  OFF)

option(
  PARALLEL_VERSION 
  "Parallize the simulation with OpenMP" 
  OFF)

option(
  ENABLE_ANIMATION 
  "Enable animation of the simulation" 
  OFF)

option(
  USE_PAPI_HL 
  "Use High-Level Performance Application Programming Interface(PAPI)" 
  OFF)

option(
  USE_PAPI_LL 
  "Use Low-Level Performance Application Programming Interface(PAPI)" 
  OFF)

option(
  PRINT_GLM_MESSAGES
  "Print glm compilation info"
  OFF)

option(
  CUSTOM_STACKSIZE
  "Set custom stacksize for the simulation"
  OFF)




# Handle options

if(CUSTOM_STACKSIZE)
  set(STACKSIZE "16777216" CACHE STRING "Set custom stacksize for the simulation")
  message(STATUS "Custom stacksize set to ${STACKSIZE}")
  add_compile_options("-Wl,--stack,${STACKSIZE}")
endif()

if(USE_ADDRESS_SANITIZER)
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

if(PRINT_OPT_INFO)
  add_compile_options(-fopt-info)
endif()

if(PRINT_GLM_MESSAGES)
  add_compile_definitions(GLM_FORCE_MESSAGES)
endif()

if(COMPILATION_WARNINGS)
  add_compile_options(-Wall -Wextra -Wconversion -pedantic -Wshadow)
endif()

if(ENABLE_ANIMATION)
  add_compile_definitions(ANIMATION)
  if(NOT PARALLEL_VERSION)
    message("Warning: Animation is not supported in conjunction with 
    non-parallel version. Parallel version will be enabled.")
    set(PARALLEL_VERSION ON)
  endif()
  if(USE_PAPI_LL)
    set(USE_PAPI_LL OFF)
    message("Warning: PAPI is not supported in conjunction with animation.
    PAPI_LL will be disabled.")
  endif()
  if(USE_PAPI_HL)
    set(USE_PAPI_HL OFF)
    message("Warning: PAPI is not supported in conjunction with animation.
    PAPI_HL will be disabled.")
    endif()
endif()

if(USE_PAPI_HL)
  add_compile_definitions(PAPI_HL)
  if(USE_PAPI_LL)
    set(USE_PAPI_LL OFF)
    message("Warning: PAPI_HL and PAPI_LL are mutually exclusive. 
    PAPI_LL will be disabled.")
  endif()
endif()

if(USE_PAPI_LL)
  add_compile_definitions(PAPI_LL)
  if(USE_PAPI_HL)
    set(USE_PAPI_HL OFF)
    message("Warning: PAPI_HL and PAPI_LL are mutually exclusive. 
    PAPI_HL will be disabled.")
  endif()
endif()

if(PARALLEL_VERSION)
  add_compile_definitions(PARALLEL)
endif()

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Add source directory
add_subdirectory(src/Simulation)
add_subdirectory(src/Parameters)
add_subdirectory(src/ConfigParser)
add_subdirectory(src/CmdLineParser) 
add_subdirectory(src/Utils)
add_subdirectory(src/State)

# Link dependencies
target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC Parameters
  PUBLIC Simulation
  PUBLIC Utils
  PUBLIC State
  PUBLIC ConfigParser
  PUBLIC CmdLineParser
)

if(ENABLE_ANIMATION)
  add_subdirectory(src/Animation)
  add_subdirectory(src/glad)
  add_subdirectory(src/imgui)
  # Link animation library to main project
  target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC Animation
    PUBLIC Glad
    PUBLIC ImGui
  )
endif()

if(USE_PAPI_LL)
  if(USE_ADDRESS_SANITIZER)
    message("Warning: PAPI used in conjunction with address sanitizer. 
    Address sanitizer will throw a false positive memory leak 
    in the PAPI library for it does not understand the 
    PAPI deallocation mechanism.")
  endif()
  add_subdirectory(src/PAPI)
  # Link papi library to main project
  target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC PAPI)
elseif(USE_PAPI_HL)
  target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC papi)
endif()

if(PARALLEL_VERSION)
  find_package(OpenMP REQUIRED)
  if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
  endif()
endif()


# Install targets

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR} CACHE PATH "..." FORCE)
endif()

set(INSTALL_DIR "bsim")

install(
  DIRECTORY 
  DESTINATION ${INSTALL_DIR})

install(
  DIRECTORY
  DESTINATION ${INSTALL_DIR}/config)

install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/src/Animation/shaders 
  DESTINATION ${INSTALL_DIR}
  FILES_MATCHING PATTERN "*"
  PATTERN ".*" EXCLUDE)

install(
  FILES ${PROJECT_SOURCE_DIR}/src/examples/simulation.toml
  DESTINATION ${INSTALL_DIR}/config
  RENAME simulation_parameters_default.toml)

if(ENABLE_ANIMATION)
install(
  FILES ${PROJECT_SOURCE_DIR}/src/examples/setenv.sh
  DESTINATION ${INSTALL_DIR})

install(
  FILES ${PROJECT_SOURCE_DIR}/src/examples/animation.toml
  DESTINATION ${INSTALL_DIR}/config
  RENAME animation_parameters_default.toml)

install(
  DIRECTORY ${PROJECT_SOURCE_DIR}/src/imgui/fonts
  DESTINATION ${INSTALL_DIR}/config
  FILES_MATCHING PATTERN "*"
  PATTERN ".*" EXCLUDE)
endif()

install(
  TARGETS ${PROJECT_NAME} 
  DESTINATION ${INSTALL_DIR})
