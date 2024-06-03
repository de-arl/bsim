#ifndef GLOBAL_PARAMETERS_H
#define GLOBAL_PARAMETERS_H

#include <ctime>
#include <iostream>
#include <iomanip>
#include <filesystem>


// Command line option parser
#include "cxxopts.hpp"

// Helpers
#include "helpers.h"

// Simulation parameters
#include "SimulationParameters.h"
#define SIMULATION_CONFIG_FILE "config/simulation_parameters.toml"
#define DEFAULT_SIMULATION_CONFIG_FILE "config/simulation_parameters_default.toml"

// Animation parameters
#ifdef ANIMATION
#include "AnimationParameters.h"
#define ANIMATION_CONFIG_FILE "config/animation_parameters.toml"
#define DEFAULT_ANIMATION_CONFIG_FILE "config/animation_parameters_default.toml"
#endif

// Forward declaration of imgui handle
template <typename T>
class GlobalParametersImGuiHandle;

template <typename T>
class GlobalParameters
{
 public:

  /* Public member functions */

  // Initialize constructor
  GlobalParameters(int argc, char **argv, 
#ifdef ANIMATION
      AnimationParameters<T> &ap,
#endif
      SimulationParameters<T> &sp
      );

  // Destructor
  ~GlobalParameters();

  // Increment run counter
  void incrementRunCounter();

  // Get/set the run counter
  int getRunCounter();
  void setRunCounter(int r);

  void initRunDirectory();

  // Set output directory
  void setOutputDirectory(std::string output_dir);  

  // Get the runtime directory
  std::string getRuntimeDirectory() const;

  // Get the run directory
  std::string getRunDirectory() const;

  // Get the time string
  std::string getTimeString() const;

  // Set/Get the verbosity level
  int getVerbosity() const;
  void setVerbosity(int v);

  // Set the output precision
  void setOutputPrecision(int p);
  int getOutputPrecision() const;

  // Set/Get the log level
  void setLogLevel(int l);
  int getLogLevel() const;

  // Get the GUI flag
  bool getGuiFlag() const;

  // Get simulation parameters
  SimulationParameters<T> &getSimulationParameters();

#ifdef ANIMATION
  // Get animation parameters
  AnimationParameters<T> &getAnimationParameters();
#endif


 private:

  /* Private member functions */

  // Set the execution time
  void setExecutionTime();

  // Parse command line arguments
  void parseCommandLineArguments(int argc, char **argv);

  // Init output directory
  void initRunDirectoryName();
  void initRuntimeDirectoryName();
  void initBaseDirectory();
  void initRuntimeDirectory();


  /* Private member variables */

  // Simulation parameters
  SimulationParameters<T> &sp;

#ifdef ANIMATION
  // Animation parameters
  AnimationParameters<T> &ap;
#endif

  // Graphical user interface flag
  bool gui_flag;

  // Verbosity level
  // 0: no output
  // 1: minimal output
  // 2: verbose output
  int verbosity;

  // Log level
  int log_level;

  // Counter for the simulation runs
  int run_counter;

  // Execution time
  char time_string[std::size("yyyy-mm-ddThh-mm-ssZ")];

  // Base path for output
  std::string basename_output;

  // Runtime directory, e.g output/yyyy-mm-ddThh:mm:ssZ
  std::string runtime_directory;

  // Run directory, e.g output/yyyy-mm-ddThh:mm:ssZ/run_0000
  std::string run_directory;

  // Output precision
  int output_precision;

  // Friends
  friend class GlobalParametersImGuiHandle<T>;

};

#endif //GLOBAL_PARAMETERS_H
/* vim: set ts=2 sw=2 tw=0 et :*/
