/*
 * The parameters structure is a container for all the parameters
 * which are constant throughout the simulation. It further contains
 * all parameters which define the individual entities in the simulation.
 */
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <iostream>
#include <vector>
#include <cstring> // strcpy
#include <filesystem> // create directories

#include "AgentClassParameters.h"
#include "Color.h"
#include "helpers.h"

// Include TOML parser for configuration file
// https://github.com/marzer/tomlplusplus.git
#include "toml.hpp"

template <typename T>
class SimulationParametersImGuiHandle;

template <typename T>
class SimulationParameters
{
 public:

  /* Public member functions */

  // Default constructor
  SimulationParameters();

  // Copy constructor
  SimulationParameters(const SimulationParameters &p);

  // Operator = to copy the parameters
  SimulationParameters &operator=(const SimulationParameters &p);

  // Destructor
  ~SimulationParameters();

  // Scale parameters to unit space and timestep per iteration
  SimulationParameters<T> scale();

  // Parse configuration file
  void parseSimulationConfig();
  void parseSimulationConfig(std::string filename);

  // Init distributions
  void initDistributions();

  // Count number of agents and set start and stop indices
  void countAgents();

  // Set/get world size
  void setWorldsize(const T dims[3]);
  void getWorldsize(T* dims) const;
  const T* getWorldsize() const;

  // Set/get initialized flag for simulation parameters
  void setSimParamsInitialized(bool flag);
  bool getSimParamsInitialized();

  // Get the number of agents
  int getNumAgents() const;

  // Set/get number of agent classes
  void setNumAgentClasses(int n);
  int getNumAgentClasses();

  // Get the class id of a class name
  int getClassId(std::string name) const;

  // Set/get all agent classes
  void setAgentClasses(std::vector<AgentClassParameters<T>> &c);
  std::vector<AgentClassParameters<T>> &getAgentClasses();

  // Set/get single agent class
  void setAgentClass(int i, AgentClassParameters<T> &c);
  AgentClassParameters<T> &getAgentClass(int i);

  // Add agent class
  void addAgentClass(AgentClassParameters<T> c);
  void addAgentClass();

  // Remove agent class
  void removeAgentClass(int i);
  void removeAgentClasses();

  // Set/get time step
  void setDt(T dt);
  T getDt() const;

  // Set/get simulated time
  void setSimulatedTime(int t);
  int getSimulatedTime();

  // Set/get random number generator seed
  void setSeed(int s);
  int getSeed();

  // Set/get simulation config file
  void setSimulationConfigFile(std::string path);
  void getSimulationConfigFile(std::string &path);
  void setDefaultSimulationConfigFile(std::string path);
  void getDefaultSimulationConfigFile(std::string &path);

  // Set number of runs
  void setNumRuns(int n);
  int getNumRuns();

  // Set memory size
  void setMemorySize(int n);
  int getMemorySize();

  // Get/set food sources
  void setFoodSources(std::vector<std::string> &food_sources, int class_id);
  std::vector<std::string> getFoodSources(int class_id) const;
  std::vector<int> getFoodSourceIds(int class_id) const;

  // Get/set friendly classes
  void setFriends(std::vector<std::string> &friends, int class_id);
  std::vector<std::string> getFriends(int class_id) const;
  std::vector<int> getFriendIds(int class_id) const;

  // Get/set enemy classes
  void setEnemies(std::vector<std::string> &enemies, int class_id);
  std::vector<std::string> getEnemies(int class_id) const;
  std::vector<int> getEnemyIds(int class_id) const;

  // Get space scaling factor
  T getSpaceScalingFactor() const;
  T getInverseSpaceScalingFactor() const;

  // Initialize the simulation parameters
  void init();

  // Print classes
  void printAgentClassParameters(std::ostream &stream = std::cout) const;

  // Print parameters
  void printSimulationParameters(std::ostream &stream = std::cout) const;

  // Save parameters to file
  void save(std::string filename) const;

  // Test if a point is inside the world
  bool isInsideWorld(const Point3D<T> &p) const;


  /* Public member variables */

  // The total number of agents
  int num_agents_total;

  // Number of agent classes
  int num_agent_classes;

  // AgentClassParameters definitions
  std::vector<AgentClassParameters<T>> agent_class_params;

  // Flag to check if the simulation parameters have been initialized
  bool simulation_parameters_initialized;

  // Path to configuration file
  std::string simulation_config_file;
  std::string default_simulation_config_file;

  // World size 
  T worldsize[3];

  // Time step in time/iteration
  T dt;

  // Simulated time in time units
  // This is the timelimit for the simulation
  int simulated_time;

  // Random engine seed
  // If set to 0, the seed is random
  int seed;

  // Scaling factors
  T space_scaling_factor;
  T inv_space_scaling_factor;

  // Number of runs (not iterations, total repetitions)
  int num_runs;

  // Memory size, number of states to store
  int memory_size;



 private:
  /* Private member functions */
  void parseAgentClassSection(toml::table &config);

  friend class SimulationParametersImGuiHandle<T>;

};

#endif //PARAMETERS_H


/* vim: set ts=2 sw=2 tw=0 et :*/
