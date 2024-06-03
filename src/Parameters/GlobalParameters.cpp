#include "GlobalParameters.h"

template <typename T>
GlobalParameters<T>::GlobalParameters(int argc, char **argv, 
#ifdef ANIMATION
    AnimationParameters<T> &a,
#endif
    SimulationParameters<T> &p
    )
  :sp(p), 
#ifdef ANIMATION
  ap(a),
#endif
  run_counter(0),
  gui_flag(false),
  verbosity(0),
  log_level(0),
  output_precision(6),
  time_string("yyyy-mm-ddThh-mm-ssZ"),
  basename_output("output"),
  runtime_directory("output"),
  run_directory("output")
{
  // Set output precision
  std::cout.precision(output_precision);

  // Initialize simulation parameters
  p = SimulationParameters<T>();

#ifdef ANIMATION
  // Initialize animation parameters
  a = AnimationParameters<T>();
#endif

  // Set execution time
  setExecutionTime();

  // Parse command line arguments
  parseCommandLineArguments(argc, argv);

  // Initialize the directory names
  initRuntimeDirectoryName();
  initRunDirectoryName();

}

template <typename T>
GlobalParameters<T>::~GlobalParameters()
{
#ifdef DEBUG
  std::cout << "GlobalParameters::~GlobalParameters: \
    Destructor of GlobalParameters" << std::endl;
#endif
}

template <typename T>
void GlobalParameters<T>::parseCommandLineArguments(int argc, char **argv)
{
#ifdef DEBUG
  std::cout << "GlobalParameters::parseCommandLineArguments \
    Parsing command line arguments" << std::endl;
#endif

  // Define command line options
  cxxopts::Options options("bsim", 
      "--- bsim ---\n\
      Simulate swarms");

  options.add_options()

    // Help
    ("h,help", "Print usage")

    // Animation config file
    ("a,animation-config", "Animation configuration file", 
        cxxopts::value<std::string>())

    // Simulation config file
    ("c,simulation-config", "Simulation configuration file", 
        cxxopts::value<std::string>())

#ifdef ANIMATION
    // Graphical user interface
    ("g,gui", "Graphical user interface")
#endif

    // Output path
    ("o,output-path", "Output path", cxxopts::value<std::string>())

    // Output precision
    ("p,output-precision", "Output precision, number of decimal places", 
        cxxopts::value<int>())

    // Random seed
    ("s,seed", "Random seed (default: 0=non-deterministic)", cxxopts::value<int>())

    // Simulated time t
    ("t,simulated-time", "Simulated time", cxxopts::value<int>())

    // Time step
    ("d,dt", "Time step", cxxopts::value<float>())

    // Number of runs
    ("n,num-runs", "Number of runs", cxxopts::value<int>())

    // Verbosity level
    ("v,verbose", "Verbosity level", cxxopts::value<int>()->default_value("0"))

    // Log level
    ("l,log-level", "Log level", cxxopts::value<int>()->default_value("0"))

    // Version
    ("version", "Print version");

  ;// end options

  // Parse the command line options
  auto result = options.parse(argc, argv);

  // Check if the help flag is set
  if (result.count("help")){
    std::cout << options.help() << std::endl;
    exit(0);
  }

  // Print version and exit
  if (result.count("version")){
    std::cout << "bsim version 1.0" << std::endl;
    exit(0);
  }

  // Check if the gui flag is set
  if (result.count("gui")){
    gui_flag = true;
  }

  // Get log level
  log_level = result["log-level"].as<int>();

  // Get verbosity level
  verbosity = result["verbose"].as<int>();

  // Get output path
  if (result.count("output-path")){
    basename_output = result["output-path"].as<std::string>();
  }
  if (verbosity > 0){
    std::cout << "Output path: " << basename_output << std::endl;
  }

  // Get output precision
  if (result.count("output-precision")){
    output_precision = result["output-precision"].as<int>();
  }

  // Get simulation configuration file
  if (result.count("simulation-config")){
    std::string simulation_config_file = 
      result["simulation-config"].as<std::string>();
    if (std::filesystem::exists(simulation_config_file) == false){
      handleError("Error: Simulation configuration file does not exist: " 
          + simulation_config_file, CRITICAL);
    }else{
      sp.setSimulationConfigFile(simulation_config_file);
    }
  }else{
    sp.setSimulationConfigFile(SIMULATION_CONFIG_FILE);
  }
  // Set default simulation config file
  sp.setDefaultSimulationConfigFile(DEFAULT_SIMULATION_CONFIG_FILE);

  // Trigger parsing of the simulation configuration file
  sp.parseSimulationConfig();

#ifdef ANIMATION
  // Get animation configuration file
  if (gui_flag){
    if (result.count("animation-config")){
      ap.setAnimationConfigFile(result["animation-config"].as<std::string>());
    } else {
      ap.setAnimationConfigFile(ANIMATION_CONFIG_FILE);
    }
    // Set default animation config file
    ap.setDefaultAnimationConfigFile(DEFAULT_ANIMATION_CONFIG_FILE);
    // Trigger parsing of the animation configuration file
    ap.parseAnimationConfig();
  }
#endif

  // Get time step
  if (result.count("dt")){
    sp.setDt(result["dt"].as<float>());
  }

  // Get random seed
  if (result.count("seed")){
    sp.setSeed(result["seed"].as<int>());
  } 

  // Get simulated time
  if (result.count("simulated-time")){
    sp.setSimulatedTime(result["simulated-time"].as<int>());
  }

  // Get number of runs
  if (result.count("num-runs")){
    sp.setNumRuns(result["num-runs"].as<int>());
  }
}

template <typename T>
void GlobalParameters<T>::setExecutionTime()
{
#ifdef DEBUG
  std::cout << "GlobalParameters::setExecutionTime \
    Setting execution time" << std::endl;
#endif
  
  // Set execution time
  time_t init_time = std::time(nullptr);
  // in the format yyyy-mm-ddThh-mm-ssZ
  std::strftime(std::data(time_string), std::size(time_string),
      "%FT%H-%M-%SZ", std::localtime(&init_time));
}

template <typename T>
void GlobalParameters<T>::initBaseDirectory()
{
#ifdef DEBUG
  std::cout << "GlobalParameters::initOutputDirectory \
    Initializing output directory" << std::endl;
#endif
  
  // First create base directory
  if (std::filesystem::exists(basename_output) == false) {
    if (std::filesystem::create_directory(basename_output) == false) {
      handleError("Error creating base directory: " 
          + basename_output, CRITICAL);
    } else {
      std::cout << "Created base directory: " << basename_output << std::endl;
    }
  }
}

template <typename T>
void GlobalParameters<T>::initRuntimeDirectory()
{
#ifdef DEBUG
  std::cout << "GlobalParameters::initRuntimeDirectory \
    Initializing runtime directory" << std::endl;
#endif
  // Initialize base directory
  initBaseDirectory();

  // Set runtime directory
  initRuntimeDirectoryName();

  // If base directory exists, create runtime directory
  if (std::filesystem::exists(runtime_directory) == false) {
    if (std::filesystem::create_directory(runtime_directory) == false) {
      handleError("Error creating runtime directory: " 
          + runtime_directory, CRITICAL);
    } else {
      std::cout << "Created runtime directory: " 
        << runtime_directory << std::endl;
    }
  }
}

template <typename T>
void GlobalParameters<T>::initRunDirectory()
{
#ifdef DEBUG
  std::cout << "GlobalParameters::initRunDirectory \
    Initializing run directory" << std::endl;
#endif
  // Initialize runtime directory
  initRuntimeDirectory();

  // Initialize run directory name
  initRunDirectoryName();
  
  // If runtime directory exists, create run directory
  if (std::filesystem::exists(run_directory) == false) {

    if (std::filesystem::create_directory(run_directory) == false) {
      handleError("Error creating run directory: " 
          + run_directory, CRITICAL);
    } else {
      std::cout << "GlobalParameters::initRunDirectory \
        Created run directory: " << run_directory << std::endl;
    }
  }
}

template <typename T>
void GlobalParameters<T>::setOutputDirectory(std::string output_dir)
{
  basename_output = output_dir;
  initRunDirectory();
}

template <typename T>
void GlobalParameters<T>::incrementRunCounter()
{
#ifdef DEBUG
  std::cout << "GlobalParameters::incrementRunCounter \
    Incrementing run counter" << std::endl;
#endif

  // Increment run counter
  run_counter++;

  // Initialize run directory name
  initRunDirectoryName();
}

template <typename T>
void GlobalParameters<T>::initRuntimeDirectoryName()
{
  // Set runtime directory
  this->runtime_directory = basename_output + "/" + time_string;
}

template <typename T>
void GlobalParameters<T>::initRunDirectoryName()
{
  // Format run counter to 4 digits
  std::string formatted_run_counter = 
  std::string(4 - std::to_string(run_counter).length(), '0') 
  + std::to_string(run_counter);

  // Set run directory
  this->run_directory = runtime_directory + "/run_" + formatted_run_counter;
}

template <typename T>
int GlobalParameters<T>::getRunCounter()
{
  return run_counter;
}

template <typename T>
void GlobalParameters<T>::setRunCounter(int r)
{
  run_counter = r;
  initRunDirectoryName();
}

template <typename T>
std::string GlobalParameters<T>::getRuntimeDirectory() const
{
  return runtime_directory;
}

template <typename T>
std::string GlobalParameters<T>::getRunDirectory() const
{
  return run_directory;
}

template <typename T>
std::string GlobalParameters<T>::getTimeString() const
{
  return time_string;
}

template <typename T>
void GlobalParameters<T>::setVerbosity(int v)
{
  verbosity = v;
}

template <typename T>
int GlobalParameters<T>::getVerbosity() const
{
  return verbosity;
}

template <typename T>
void GlobalParameters<T>::setOutputPrecision(int p)
{
  output_precision = p;
  std::cout.precision(output_precision);
}

template <typename T>
int GlobalParameters<T>::getOutputPrecision() const
{
  return output_precision;
}

template <typename T>
void GlobalParameters<T>::setLogLevel(int l)
{
  log_level = l;
}

template <typename T>
int GlobalParameters<T>::getLogLevel() const
{
  return log_level;
}

template <typename T>
bool GlobalParameters<T>::getGuiFlag() const
{
  return gui_flag;
}

template <typename T>
SimulationParameters<T> &GlobalParameters<T>::getSimulationParameters() 
{
  return sp;
}

#ifdef ANIMATION
template <typename T>
AnimationParameters<T> &GlobalParameters<T>::getAnimationParameters() 
{
  return ap;
}
#endif

// Explicit instantiations
template class GlobalParameters<float>;
template class GlobalParameters<double>;
/* vim: set ts=2 sw=2 tw=0 et :*/
