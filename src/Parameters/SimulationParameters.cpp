// Code style according to google style guide: 
// https://google.github.io/styleguide/cppguide.html, 
// using snake_case for variables and camelCase for functions.
#include "SimulationParameters.h"

template <typename T>
SimulationParameters<T>::SimulationParameters()
  : num_agents_total(0),
    num_agent_classes(0),
    simulation_parameters_initialized(false),
    simulation_config_file(""),
    worldsize{1.0, 1.0, 1.0},
    dt(1.0),
    simulated_time(1),
    seed(0),
    space_scaling_factor(1.0),
    inv_space_scaling_factor(1.0),
    num_runs(1),
    memory_size(1)
{
#ifdef DEBUG
  std::cout << "SimulationParameters: Default constructor" << std::endl;
#endif
  // Initializes default useless values to avoid uninitialized values
}

template <typename T>
SimulationParameters<T>::SimulationParameters(const SimulationParameters &p)
{
#ifdef DEBUG
  std::cout << "SimulationParameters: Copy constructor" << std::endl;
#endif
  // Copy using the assignment operator
  *this = p;
}

template <typename T>
SimulationParameters<T> 
&SimulationParameters<T>::operator=(const SimulationParameters &p)
{
#ifdef DEBUG
  std::cout << "SimulationParameters: Operator=" << std::endl;
#endif
  // Return if the other object is the same as this
  if (this == &p)
  {
    return *this;
  }

  // Set total number of agents
  num_agents_total = p.num_agents_total;

  // Set  number of agent classes
  num_agent_classes = p.num_agent_classes;

  // Copy agent classes
  agent_class_params = p.agent_class_params;

  // Set initialized flag for simulation parameters
  simulation_parameters_initialized = p.simulation_parameters_initialized;

  // Set simulation config file
  simulation_config_file = p.simulation_config_file;
  default_simulation_config_file = p.default_simulation_config_file;

  // Set world size
  for (int i = 0; i < 3; i++){
    worldsize[i] = p.worldsize[i];
  }
  
  // Set time step
  dt = p.dt;

  // Set simulated time
  simulated_time = p.simulated_time;
  
  // Set seed
  seed = p.seed;

  // Set number of runs
  num_runs = p.num_runs;

  // Set memory size
  memory_size = p.memory_size;

  // Set scaling factors
  space_scaling_factor = p.space_scaling_factor;
  inv_space_scaling_factor = p.inv_space_scaling_factor;

  // Return object
  return *this;
}

template <typename T>
SimulationParameters<T>::~SimulationParameters()
{
#ifdef DEBUG
  std::cout << "SimulationParameters: Destructor" << std::endl;
#endif
}

template <typename T>
SimulationParameters<T> SimulationParameters<T>::scale()
{
#ifdef DEBUG
  std::cout << "SimulationParameters::scale \
    Scaling parameters to unit space and timestep per iteration" << std::endl;
#endif

  // Create a copy of the parameters
  SimulationParameters<T> scaled_params = *this;

  // Calculate spatial scaling factor
  scaled_params.inv_space_scaling_factor =
    std::max(worldsize[0], std::max(worldsize[1], worldsize[2]));
  scaled_params.space_scaling_factor = 
    1.0 / scaled_params.inv_space_scaling_factor;

  // Scale world size
for (int i = 0; i < 3; i++){
    scaled_params.worldsize[i] *= scaled_params.space_scaling_factor;
  }

  // Scale agent classes
  for (int i = 0; i < num_agent_classes; i++){
    scaled_params.agent_class_params[i] = 
      AgentClassParameters<T>(agent_class_params[i], dt, scaled_params.space_scaling_factor);
}

  // Set scaled start position
  for (int i = 0; i < num_agent_classes; i++){
    scaled_params.agent_class_params[i].start_position = 
      agent_class_params[i].start_position * scaled_params.space_scaling_factor;
  }

  // Set the initialized flag
  scaled_params.setSimParamsInitialized(true);

  // Move the right hand side object using move semantics
  return std::move(scaled_params);
}

template <typename T>
void SimulationParameters<T>::setSimulationConfigFile(std::string file)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setSimulationConfigFile \
    Setting simulation configuration file: " << file << std::endl;
#endif
  
  // Set simulation configuration file
  simulation_config_file = file;
}

template <typename T>
void SimulationParameters<T>::getSimulationConfigFile(std::string &file)
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getSimulationConfigFile \
    Getting simulation configuration file" << std::endl;
    */
#endif

  // Get simulation configuration file
  file = simulation_config_file;
}

template <typename T>
void SimulationParameters<T>::setDefaultSimulationConfigFile(std::string file)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setDefaultSimulationConfigFile \
    Setting default simulation configuration file: " << file << std::endl;
#endif

  // Set default simulation configuration file
  default_simulation_config_file = file;
}

template <typename T>
void SimulationParameters<T>::getDefaultSimulationConfigFile(std::string &file)
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getDefaultSimulationConfigFile \
    Getting default simulation configuration file" << std::endl;
    */
#endif

  // Get default simulation configuration file
  file = default_simulation_config_file;
}



template <typename T>
void SimulationParameters<T>::parseSimulationConfig(std::string filename)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::parseSimulationConfig \
    Parsing simulation configuration file: " << filename << std::endl;
#endif
  // Parse configuration file
  toml::table config;
  try {
    config = toml::parse_file(filename);
  } catch (const toml::parse_error &e) {
    std::runtime_error("Error parsing configuration file, file: " + filename + 
        " error: " + e.what());
    throw (e);
  }

  // Parse world section
  if (config.contains("world")){
    // Set world size
    if (config["world"].as_table()->contains("worldsize")){
        for (int i = 0; i < 3; i++) {
          this->worldsize[i] = config["world"]["worldsize"][i].value_or(1.0f); 
        }
    } else {
      // Worldsize not found
      for (int i = 0; i < 3; i++) {
        this->worldsize[i] = 1.0;
      }
      handleError("Key 'worldsize' not found in section 'world'\n\
          using default values: 1.0, 1.0, 1.0", WARNING);
    }
  } else {
    // World section not found
    for (int i = 0; i < 3; i++) {
      this->worldsize[i] = 1.0;
    }
    handleError("Section 'world' not found in configuration file\n\
        using default values: 1.0, 1.0, 1.0", WARNING);
  }


  // Parse simulation section
  if (config.contains("simulation")){

    // Set time step
    if (config["simulation"].as_table()->contains("dt")){
      dt = config["simulation"]["dt"].value_or(1.0f);
    } else {
      // Time step not found
      dt = 1.0;
      handleError("Key 'dt' not found in section 'simulation', using 1.0", 
          WARNING);
    }

    // Set simulated time
    if (config["simulation"].as_table()->contains("simulated_time")){
      simulated_time = config["simulation"]["simulated_time"].value_or(1);
    } else {
      // Simulated time not found
      simulated_time = 86400;
      handleError("Key 'simulated_time' not found in section 'simulation',\
          \n using default value: 86400",
          WARNING);
    }

    // Set seed 
    if (config["simulation"].as_table()->contains("seed")){
      seed = config["simulation"]["seed"].value_or(1);
    } else {
      // Seed not found
      seed = 0;
      handleError("Key 'seed' not found in section 'simulation', using random\
          seed", 
          WARNING);
    }

    // Set number of runs
    if (config["simulation"].as_table()->contains("num_runs")){
      num_runs = config["simulation"]["num_runs"].value_or(1);
    } else {
      // Number of runs not found
      num_runs = 1;
      handleError("Key 'num_runs' not found in section 'simulation', using 1", 
          WARNING);
    }

    // Set memory size
    if (config["simulation"].as_table()->contains("memory_size")){
      memory_size = config["simulation"]["memory_size"].value_or(1);
    } else {
      // Memory size not found
      memory_size = 1;
      handleError("Key 'memory_size' not found in section 'simulation', using 1", 
          WARNING);
    }

  } else {
    // Simulation section not found
      
    // Set default values
    dt = 1.0;
    simulated_time = 86400;
    seed = 0;
    num_runs = 1;
    memory_size = 1;

    handleError("Section 'simulation' not found in configuration file.\n\
       Using default values: dt=1.0, simulated_time=86400, seed=0, num_runs=1",
       WARNING);
  }

  // Parse agent class section
  parseAgentClassSection(config);

  // Set the initialized flag
  if (agent_class_params.size() > 0){
    init();
  }
}

template <typename T>
void SimulationParameters<T>::parseSimulationConfig()
{
#ifdef DEBUG
  std::cout << "SimulationParameters::parseSimulationConfig \
    Parsing simulation configuration file" << std::endl;
#endif

  // Check if the configuration file exists
  if (std::filesystem::exists(simulation_config_file)){
    parseSimulationConfig(simulation_config_file);
#ifdef DEBUG
    std::cout << "SimulationParameters::parseSimulationConfig \
      Configuration file found: " << simulation_config_file << std::endl;
#endif
  } else {
    // Configuration file not found
    handleError("Configuration file not found: " + simulation_config_file +
        " using default configuration file: " 
        + default_simulation_config_file, 
        WARNING);
    if (std::filesystem::exists(default_simulation_config_file)){
      // Parse default configuration file
      parseSimulationConfig(default_simulation_config_file);
#ifdef DEBUG
      std::cout << "SimulationParameters::parseSimulationConfig \
        Configuration file not found: " << simulation_config_file << std::endl;
      std::cout << "SimulationParameters::parseSimulationConfig \
        Using default configuration file: " 
        << default_simulation_config_file << std::endl;
#endif
    } else {
      // Configuration file not found
      handleError("Configuration file not found: " + simulation_config_file, 
          WARNING);
      handleError("Configuration file not found: " 
          + default_simulation_config_file, 
          WARNING);
    }
  } 

}

template <typename T>
void SimulationParameters<T>::parseAgentClassSection(toml::table &config)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::parseAgentClassSection \
    Parsing agent class section" << std::endl;
#endif

  // Clear agent classes
  agent_class_params.clear();

  // Parse agent class section
  if (config.contains("agent_class_params")){
    auto configured_classes = config["agent_class_params"];

    // Parse classnames
    if (config["agent_class_params"].as_table()->contains("classes")){
      auto classes = configured_classes["classes"].as_array();
      num_agent_classes = classes->size();

      // Parse class definitions
      for (int i=0; i<num_agent_classes; i++){
        // Get the class definition
        std::string classname = classes->at(i).value_or("unknown");

        // Check if class definition exists
        if (config.contains(classname)) {
          auto classdef = config[classname];
          auto c_tab = classdef.as_table();

          // Parse class definition
          if (c_tab->contains("num_agents") &&
              c_tab->contains("friends") &&
              c_tab->contains("enemies") &&
              c_tab->contains("food_sources") &&
              c_tab->contains("strategy") &&
              c_tab->contains("mu_levy") &&
              c_tab->contains("c_levy") &&
              c_tab->contains("brownian_search_duration") &&
              c_tab->contains("movespeed_search") &&
              c_tab->contains("movespeed_approach_friends") &&
              c_tab->contains("movespeed_avoid_enemies") &&
              c_tab->contains("movespeed_approach_food") &&
              c_tab->contains("status") &&
              c_tab->contains("audibility") &&
              c_tab->contains("mu_energy") &&
              c_tab->contains("sigma_energy") &&
              c_tab->contains("energy_uptake_rate") &&
              c_tab->contains("movespeed") &&
              c_tab->contains("energy_consumption_per_distance") &&
              c_tab->contains("energy_consumption_per_time") &&
              c_tab->contains("size") &&
              c_tab->contains("start_position") &&
              c_tab->contains("random_start_positions") &&
              c_tab->contains("color") &&
              c_tab->contains("pointsize")) {

            // Get start position
            Point3D<T> start_position = {
              classdef["start_position"][0].value_or(0.0f),
              classdef["start_position"][1].value_or(0.0f),
              classdef["start_position"][2].value_or(0.0f)
            };

            // Get color
            Color<T> color = Color<T> {
              classdef["color"][0].value_or(0.0f),
              classdef["color"][1].value_or(0.0f),
              classdef["color"][2].value_or(0.0f)
            };

            // Get friend classes
            std::vector<std::string> friends;
            for (int i=0; i<classdef["friends"].as_array()->size(); i++){
              std::string def = "unknown";
              friends.push_back(classdef["friends"][i].value_or(def));
            }
            // Get enemy classes
            std::vector<std::string> enemies;
            for (int i=0; i<classdef["enemies"].as_array()->size(); i++){
              std::string def = "unknown";
              enemies.push_back(classdef["enemies"][i].value_or(def));
            }

            // Get food sources
            std::vector<std::string> food_sources;
            for (int i=0; i<classdef["food_sources"].as_array()->size(); i++){
              std::string def = "unknown";
              food_sources.push_back(classdef["food_sources"][i].value_or(def));
            }

            // Instantiate AgentClassParameters object
            agent_class_params.emplace_back(classname, i, 
                friends,
                enemies,
                food_sources,
                classdef["strategy"].value_or(0),
                classdef["mu_levy"].value_or(1.0f),
                classdef["c_levy"].value_or(1.0f),
                classdef["brownian_search_duration"].value_or(1.0f),
                classdef["movespeed_search"].value_or(1.0f),
                classdef["movespeed_approach_friends"].value_or(1.0f),
                classdef["movespeed_avoid_enemies"].value_or(1.0f),
                classdef["movespeed_approach_food"].value_or(1.0f),
                classdef["status"].value_or(1),
                classdef["num_agents"].value_or(1),
                classdef["audibility"].value_or(1.0f),
                classdef["mu_energy"].value_or(1.0f),
                classdef["sigma_energy"].value_or(0.1f),
                classdef["energy_uptake_rate"].value_or(1.0f),
                classdef["movespeed"].value_or(1.0f),
                classdef["energy_consumption_per_time"].value_or(1.0f),
                classdef["energy_consumption_per_distance"].value_or(1.0f),
                classdef["size"].value_or(1.0f),
                start_position,
                classdef["random_start_positions"].value_or(true),
                color,
                classdef["pointsize"].value_or(1.0f));

          } else {
            // AgentClassParameters definition incomplete
            // Set default values
            agent_class_params.emplace_back();
            handleError("AgentClassParameters definition incomplete for class "
                + classname + "USING DEFAULT VALUES", WARNING);
          }


        } else {
          // AgentClassParameters definition not found
            agent_class_params.emplace_back();
            handleError("AgentClassParameters definition incomplete for class "
                + classname + "USING DEFAULT VALUES", WARNING);
        }

      } //end for agent classes
    } else { //classnames not found
      handleError("Key 'classes' not found in section 'agent_class_params'", 
          WARNING);
    }


  } else {
    // AgentClassParameters section not found
    handleError("Section 'agent_class_params' not found in configuration file.", 
        WARNING);
  }

}

template <typename T>
void SimulationParameters<T>::setWorldsize(const T dims[3])
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setWorldsize \
    Setting world size" << std::endl;
#endif

  // Set world size
  for (int i = 0; i < 3; i++){
    worldsize[i] = dims[i];
  }
}

template <typename T>
void SimulationParameters<T>::getWorldsize(T* dims) const
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getWorldsize \
    Getting world size" << std::endl;
    */
#endif
  
  // Get world size
  for (int i = 0; i < 3; i++){
    dims[i] = worldsize[i];
  }
}

template <typename T>
const T* SimulationParameters<T>::getWorldsize() const
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getWorldsize \
    Getting world size" << std::endl;
    */
#endif

  // Get world size
  return worldsize;
}

template <typename T>
void SimulationParameters<T>::initDistributions()
{
#ifdef DEBUG
  std::cout << "SimulationParameters::initDistributions \
    Initializing distributions" << std::endl;
#endif

  // Set distributions for the energy
  for (int i=0; i<num_agent_classes; i++){
    agent_class_params[i].setEnergyDistribution(
        agent_class_params[i].mu_energy, 
        agent_class_params[i].sigma_energy);
  }
}

template <typename T>
void SimulationParameters<T>::countAgents()
{
  // Count number of agents
  num_agents_total = 0;
  for (int i=0; i<num_agent_classes; i++){
    agent_class_params[i].start_index = num_agents_total;
    num_agents_total += agent_class_params[i].num_agents;
    agent_class_params[i].stop_index = num_agents_total; 
  }

#ifdef DEBUG
  std::cout << "SimulationParameters::countAgents \
    Counting number of agents" << std::endl;
  std::cout << "SimulationParameters::countAgents \
    Total number of agents: " << num_agents_total << std::endl;
  std::cout << "SimulationParameters::countAgents \
    Number of agent classes: " << num_agent_classes << std::endl;
#endif
}

template <typename T>
void SimulationParameters<T>::setAgentClasses(std::vector<AgentClassParameters<T>> &c)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setAgentClassParameters \
    Setting agent classes" << std::endl;
#endif

  // Clear agent classes
  agent_class_params.clear();

  // Set agent classes
  agent_class_params = c;

  // Set number of classes
  num_agent_classes = agent_class_params.size();
}

template <typename T>
std::vector<AgentClassParameters<T>> 
&SimulationParameters<T>::getAgentClasses()
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getAgentClassParameters \
    Getting agent classes" << std::endl;
    */
#endif

  // Get agent classes
  return agent_class_params;
}

template <typename T>
void SimulationParameters<T>::setAgentClass(int i, 
    AgentClassParameters<T> &c)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setAgentClassParameters \
    Setting agent class" << std::endl;
#endif

  // Check if i is within bounds
  if (i < 0 || i >= agent_class_params.size()){
    std::cerr << "Agent class index out of bounds" << std::endl;
    return;
  }

  // Set agent class
  agent_class_params[i] = c;
}

template <typename T>
AgentClassParameters<T> &SimulationParameters<T>::getAgentClass(
    int i)
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getAgentClassParameters \
    Getting agent class" << std::endl;
    */
#endif

  // Get agent class
  return agent_class_params[i];
}

template <typename T>
void SimulationParameters<T>::addAgentClass(
    AgentClassParameters<T> c)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::addAgentClassParameter \
    Adding agent class: " << c.name << std::endl;
#endif

  // Add agent class
  agent_class_params.push_back(c);

  // Set id 
  agent_class_params.back().id = num_agent_classes;

  // Sync number of classes
  num_agent_classes = agent_class_params.size();
}

template <typename T>
void SimulationParameters<T>::addAgentClass()
{
#ifdef DEBUG
  std::cout << "SimulationParameters::addAgentClassParameter \
    Adding agent class" << std::endl;
#endif

  // Add agent class
  agent_class_params.emplace_back();

  // Sync number of classes
  num_agent_classes = agent_class_params.size();

  // Set id
  agent_class_params.back().id = num_agent_classes;

  // Set name
  agent_class_params.back().name = "class_" + std::to_string(num_agent_classes);

}

template <typename T>
void SimulationParameters<T>::removeAgentClass(int i)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::removeAgentClassParameter \
    Removing agent class: " << agent_class_params[i].name << std::endl;
#endif

  // Check if i is within bounds
  if (i < 0 || i >= agent_class_params.size()){
    std::cerr << "Agent class index out of bounds" << std::endl;
    return;
  }

  // Remove agent class
  agent_class_params.erase(agent_class_params.begin() + i);

  // Give new ids
  for (int i=0; i<agent_class_params.size(); i++){
    agent_class_params[i].id = i;
  }

  // Sync number of classes
  num_agent_classes = agent_class_params.size();
}

template <typename T>
void SimulationParameters<T>::removeAgentClasses()
{
#ifdef DEBUG
  std::cout << "SimulationParameters::removeAgentClasses \
    Removing all agent classes" << std::endl;
#endif

  // Remove all agent classes
  agent_class_params.clear();

  // Sync number of classes
  num_agent_classes = agent_class_params.size();
}


template <typename T>
void SimulationParameters<T>::setDt(T dt)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setDt \
    Setting time step" << std::endl;
#endif

  // Set time step
  this->dt = dt;
}

template <typename T>
T SimulationParameters<T>::getDt() const
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getDt \
    Getting time step" << std::endl;
    */
#endif
  
  // Get time step
  return dt;
}

template <typename T>
void SimulationParameters<T>::setSimulatedTime(int t)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setSimulatedTime \
    Setting simulated time" << std::endl;
#endif

  // Set simulated time
  simulated_time = t;
}

template <typename T>
void SimulationParameters<T>::setNumAgentClasses(int n)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setNumAgentClasses \
    Setting number of agent classes" << std::endl;
#endif
  
  // Set number of agent classes
  num_agent_classes = n;
}

template <typename T>
int SimulationParameters<T>::getNumAgentClasses()
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getNumAgentClasses \
    Getting number of agent classes" << std::endl;
  */
#endif

  // Get number of agent classes
  return num_agent_classes;
}

template <typename T>
int SimulationParameters<T>::getClassId(std::string name) const
{
#ifdef DEBUG
  /*
    std::cout << "SimulationParameters::getClassId \
      Getting class id" << std::endl;
    */
#endif

  // Get class id
  for (int i=0; i<num_agent_classes; i++){
    if (agent_class_params[i].name == name){
#ifdef DEBUG
      /*
      std::cout << "SimulationParameters::getClassId: ID of class " << name
        << ": " << i << std::endl;
        */
#endif
      return i;
    }
  }

  // Class not found
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getClassId: Class " << name
    << " not found" << std::endl;
    */
#endif
  return -1;
}

template <typename T>
int SimulationParameters<T>::getSimulatedTime()
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getSimulatedTime \
    Getting simulated time" << std::endl;
    */
#endif
  
  // Get simulated time
  return simulated_time;
}

template <typename T>
void SimulationParameters<T>::setSeed(int s)
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::setSeed \
    Setting random number generator seed" << std::endl;
    */
#endif
  
  // Set seed
  seed = s;
}

template <typename T>
int SimulationParameters<T>::getSeed()
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getSeed \
    Getting random number generator seed" << std::endl;
    */
#endif

  // Get seed
  return seed;
}
  

template <typename T>
void SimulationParameters<T>::setNumRuns(int n)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setNumRuns \
    Setting number of runs" << std::endl;
#endif
  
  // Set number of runs
  num_runs = n;
}

template <typename T>
int SimulationParameters<T>::getNumRuns()
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getNumRuns \
    Getting number of runs" << std::endl;
    */
#endif

  // Get number of runs
  return num_runs;
}

template <typename T>
void SimulationParameters<T>::setMemorySize(int n)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setMemorySize \
    Setting memory size" << std::endl;
#endif

  // Set memory size
  memory_size = n;
}

template <typename T>
int SimulationParameters<T>::getMemorySize()
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getMemorySize \
    Getting memory size" << std::endl;
    */
#endif

  // Get memory size
  return memory_size;
}

template <typename T>
void SimulationParameters<T>::setFoodSources(
    std::vector<std::string> &food_sources, int class_id)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setFoodSources \
    Setting food sources" << std::endl;

  // Check if class_id is within bounds
  if (class_id < 0 || class_id >= agent_class_params.size()){
    std::cerr << "Agent class index out of bounds" << std::endl;
    return;
  }
#endif

  // Set food sources
  agent_class_params[class_id].setFoodSources(food_sources);
}

template <typename T>
std::vector<std::string> SimulationParameters<T>::getFoodSources(int class_id) const
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getFoodSources \
    Getting food sources" << std::endl;
    */

  // Check if class_id is within bounds
  if (class_id < 0 || class_id >= agent_class_params.size()){
    std::cerr << "Agent class index out of bounds" << std::endl;
    std::vector<std::string> empty;
    return (empty);
  }
#endif

  // Get food sources
  return agent_class_params[class_id].getFoodSources();
}

template <typename T>
std::vector<int> SimulationParameters<T>::getFoodSourceIds(int class_id) const
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getFoodSourceIds \
    Getting food source ids" << std::endl;
    */

  // Check if the parameters have been initialized
  if (!simulation_parameters_initialized){
    std::cout << "Simulation parameters not initialized" << std::endl;
    std::vector<int> empty;
    return (empty);
  }
#endif

  // Get food source ids
  std::vector<std::string> food_sources 
    = agent_class_params[class_id].getFoodSources();
  std::vector<int> food_source_ids;
  for (int i=0; i<food_sources.size(); i++){
    int id = getClassId(food_sources[i]);
    if (id >= 0) {
      food_source_ids.push_back(id);
    }
  }
  return food_source_ids;
}

template <typename T>
void SimulationParameters<T>::setFriends(std::vector<std::string> &friends,
    int class_id)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setFriends \
    Setting friends" << std::endl;

  // Check if class_id is within bounds
  if (class_id < 0 || class_id >= agent_class_params.size()){
    std::cerr << "Agent class index out of bounds" << std::endl;
    return;
  }
#endif

  // Set friends
  agent_class_params[class_id].setFriends(friends);
}

template <typename T>
std::vector<std::string> SimulationParameters<T>::getFriends(int class_id) const
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getFriends \
    Getting friends" << std::endl;
    */

  // Check if class_id is within bounds
  if (class_id < 0 || class_id >= agent_class_params.size()){
    std::cerr << "Agent class index out of bounds" << std::endl;
    std::vector<std::string> empty;
    return (empty);
  }
#endif

  // Get friends
  return agent_class_params[class_id].getFriends();
}

template <typename T>
std::vector<int> SimulationParameters<T>::getFriendIds(int class_id) const
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getFriendIds \
    Getting friend ids" << std::endl;
    */

  // Check if the parameters have been initialized
  if (!simulation_parameters_initialized){
    std::cout << "Simulation parameters not initialized" << std::endl;
    std::vector<int> empty;
    return (empty);
  }
#endif

  // Get friend ids
  std::vector<std::string> friends 
    = agent_class_params[class_id].getFriends();
  std::vector<int> friend_ids;
  for (int i=0; i<friends.size(); i++){
    int id = getClassId(friends[i]);
    if (id >= 0) {
      friend_ids.push_back(id);
    }
  }
  return friend_ids;
}

template <typename T>
void SimulationParameters<T>::setEnemies(std::vector<std::string> &enemies,
    int class_id)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setEnemies \
    Setting enemies" << std::endl;

  // Check if class_id is within bounds
  if (class_id < 0 || class_id >= agent_class_params.size()){
    std::cerr << "Agent class index out of bounds" << std::endl;
    return;
  }
#endif

  // Set enemies
  agent_class_params[class_id].setEnemies(enemies);
}

template <typename T>
std::vector<std::string> SimulationParameters<T>::getEnemies(int class_id) const
{
#ifdef DEBUG
  std::cout << "SimulationParameters::getEnemies \
    Getting enemies" << std::endl;

  // Check if class_id is within bounds
  if (class_id < 0 || class_id >= agent_class_params.size()){
    std::cerr << "Agent class index out of bounds" << std::endl;
    std::vector<std::string> empty;
    return (empty);
  }
#endif

  // Get enemies
  return agent_class_params[class_id].getEnemies();
}

template <typename T>
std::vector<int> SimulationParameters<T>::getEnemyIds(int class_id) const
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getEnemyIds \
    Getting enemy ids" << std::endl;
    */

  // Check if the parameters have been initialized
  if (!simulation_parameters_initialized){
    std::cout << "Simulation parameters not initialized" << std::endl;
    std::vector<int> empty;
    return (empty);
  }
#endif

  // Get enemy ids
  std::vector<std::string> enemies 
    = agent_class_params[class_id].getEnemies();
  std::vector<int> enemy_ids;
  for (int i=0; i<enemies.size(); i++){
    int id = getClassId(enemies[i]);
    if (id >= 0) {
      enemy_ids.push_back(id);
    }
  }
  return enemy_ids;
}

template <typename T>
T SimulationParameters<T>::getSpaceScalingFactor() const
{
#ifdef DEBUG
  std::cout << "SimulationParameters::getSpaceScalingFactor \
    Getting space scaling factor" << std::endl;
#endif

  // Get space scaling factor
  return space_scaling_factor;
}

template <typename T>
T SimulationParameters<T>::getInverseSpaceScalingFactor() const
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getInverseSpaceScalingFactor \
    Getting inverse space scaling factor" << std::endl;
    */
#endif

  // Get inverse space scaling factor
  return inv_space_scaling_factor;
}

template <typename T>
void SimulationParameters<T>::setSimParamsInitialized(bool flag)
{
#ifdef DEBUG
  std::cout << "SimulationParameters::setSimParamsInitialized \
    Setting initialized flag for simulation parameters" << std::endl;
#endif

  // Set the initialized flag for simulation parameters
  simulation_parameters_initialized = flag;
}

template <typename T>
bool SimulationParameters<T>::getSimParamsInitialized()
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getSimParamsInitialized \
    Getting initialized flag for simulation parameters" << std::endl;
    */
#endif
  
  // Get the initialized flag for simulation parameters
  return simulation_parameters_initialized;
}

template <typename T>
int SimulationParameters<T>::getNumAgents() const
{
#ifdef DEBUG
  /*
  std::cout << "SimulationParameters::getNumAgentsTotal \
    Getting total number of agents" << std::endl;
    */
#endif

  // Get total number of agents
  return num_agents_total;
}

template <typename T>
void SimulationParameters<T>::init()
{
#ifdef DEBUG
  std::cout << "SimulationParameters::init \
    Initializing simulation parameters" << std::endl;
#endif
  
  // Initialize the distributions
  initDistributions();

  // Count the number of agents
  countAgents();

  // Set the initialized flag
  setSimParamsInitialized(true);
}

template <typename T>
void SimulationParameters<T>::printSimulationParameters(
    std::ostream& stream) const
{
#ifdef DEBUG
  std::cout << "SimulationParameters::printSimulationParameters \
    Printing simulation parameters" << std::endl;
#endif

  // Check if the parameters have been initialized
  if (!simulation_parameters_initialized){
    std::cout << "Simulation parameters not initialized" << std::endl;
    return;
  }

  // Simulation section
  stream << "[simulation]" << std::endl;
  stream << "  simulated_time = " << simulated_time << std::endl;
  stream << "  dt = " << dt << std::endl;
  stream << "  seed = " << seed << std::endl;
  stream << "  num_runs = " << num_runs << std::endl;
  stream << "  memory_size = " << memory_size << std::endl;
#ifdef DEBUG
  stream << "  num_agents_total = " << num_agents_total << std::endl;
#endif
  stream << std::endl;

  // World section
  stream << "[world]" << std::endl;
  stream << "  worldsize = [" << worldsize[0] << ", " 
    << worldsize[1] << ", " << worldsize[2] << "]" << std::endl;
  stream << std::endl;

  // Agent classes section
  stream << "[agent_class_params]" << std::endl;
  stream << "  classes = [";
  for (int i=0; i<num_agent_classes; i++){
    stream << "\"" << agent_class_params[i].name << "\"";
    if (i < num_agent_classes-1){
      stream << ", ";
    }
  }
  stream << "]" << std::endl;
  stream << std::endl;

  // Print agent classes
  printAgentClassParameters(stream);  

}

template <typename T>
void SimulationParameters<T>::printAgentClassParameters(
    std::ostream& stream) const
{
#ifdef DEBUG
  std::cout << "SimulationParameters::printAgentClassParameters \
    Printing agent classes" << std::endl;
#endif

  for (int i=0; i<agent_class_params.size(); i++){
    agent_class_params[i].print(stream);
  }
}

template <typename T>
void SimulationParameters<T>::save(std::string filename) const
{
#ifdef DEBUG
  std::cout << "SimulationParameters::save \
    Saving simulation parameters to file" << std::endl;
#endif
  
    // Check if the parameters have been initialized
    if (!simulation_parameters_initialized){
      std::cout << "Simulation parameters not initialized" << std::endl;
      return;
    }
  
    // Open file
    std::ofstream file;
    file.open(filename);
  
    // Write to file
    file << "# Simulation parameters" << std::endl;
    file << std::endl;

    // Print the parameters to the file
    printSimulationParameters(file);

    // Close file
    file.close();
  
}

template <typename T>
bool SimulationParameters<T>::isInsideWorld(const Point3D<T> &p) const
{
#ifdef DEBUG
  /*
    std::cout << "SimulationParameters::isInsideWorld \
      Checking if point " << p.toString() << " is inside world" << std::endl;
    */
#endif

  // Check if the point is inside the world
  if (p.x < -worldsize[0] || p.x > worldsize[0]) return false;
  if (p.y < -worldsize[1] || p.y > worldsize[1]) return false;
  if (p.z < -worldsize[2] || p.z > worldsize[2]) return false;

  return true;
}
  
// Explicit instantiation
template class SimulationParameters<float>;
template class SimulationParameters<double>;

/* vim: set ts=2 sw=2 tw=0 et :*/
