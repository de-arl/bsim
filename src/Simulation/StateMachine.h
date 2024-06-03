#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <random>
#include <ctime>
#include <omp.h>

#include "helpers.h"
#include "SimulationParameters.h"
#include "GlobalParameters.h"
#include "State.h"
#include "Agent.h"
#include "AgentClass.h"
#include "AgentMetrics.h"
#include "SystemMetrics.h"
#include "toml.hpp"
#include "Search.h"

enum class LogFlags
{
  NONE,
  ENERGY,
  POSITIONS,
  ENERGY_CONSUMPTION_RATE_TIME,
  ENERGY_CONSUMPTION_RATE_DISTANCE,
  ENERGY_UPTAKE_RATE,
  MOVESPEED,
  SIZES,
  STATUS,
  CLASS_ID,
  STRATEGY,
  AUDIBILITY_THRESHOLD,
  DISTANCES_AGENT_TO_AGENT,
  DISTANCES_AGENT_TO_CLASS,
  DISTANCES_CLASS_TO_CLASS,
  AUDIBILITY_AGENT_TO_AGENT,
  AUDIBILITY_AGENT_TO_CLASS,
  AUDIBILITY_CLASS_TO_CLASS,
  REACHABILITY_AGENT_TO_AGENT,
  REACHABILITY_AGENT_TO_CLASS,
  REACHABILITY_CLASS_TO_CLASS,
  CONTACT_AGENT_TO_AGENT,
  CONTACT_AGENT_TO_CLASS,
  CONTACT_CLASS_TO_CLASS,
  NUM_LOG_FLAGS
};

template <typename T>
class StateMachine
{
 public:

  // Constructor
  StateMachine(std::random_device &rd, GlobalParameters<T> &p_global);

  // Destructor
  ~StateMachine();

  // Initialize the state of the simulation with a given set of parameters
  void init(SimulationParameters<T> &sp);

  // Reset the state of the simulation
  void reset();

  // Finalize metrics of the simulation
  void finalize(std::string dirname);

  // Update the state of the simulation
  void updateState();

  // Transition to the next state
  void transition(const State<T> &old_state, const State<T> &state, 
      State<T> &new_state);

  // Print the state of the simulation
  void printState(const State<T> &state, 
      std::ostream &stream = std::cout, bool append = false, 
      std::string sep = ",", int verbosity = 1) const;
  void printState() const;

  // Save the state of the simulation to a file
  void saveState(
      const State<T> &state,
      std::string dirname,
      bool append = false,
      std::string sep = ",") const;
  void saveState(
      std::string dirname,
      bool append = false,
      std::string sep = ",") const;
  void saveStateToml(
      const State<T> &state,
      std::string dirname) const;
  void saveStateToml(std::string dirname) const;

  // Load the state of the simulation from a file
  void loadStateToml(State<T> &state, std::string filename);
  void loadStateToml(std::string filename);

  // Get/set restore file
  std::string getRestoreFile();
  void setRestoreFile(std::string filename);


  // Save the parameters of the simulation to a file
  void saveParameters(std::string dirname);

  // Save energy of the agents to a file
  void saveEnergy(const State<T> &state, 
      std::string filename, bool append = false,
      std::string sep = ",") const; 
  void saveEnergyConsumptionRateTime(const State<T> &state, 
      std::string filename, bool append = false,
      std::string sep = ",") const;
  void saveEnergyConsumptionRateDistance(const State<T> &state,
      std::string filename, bool append = false,
      std::string sep = ",") const;
  void saveEnergyUptakeRate(const State<T> &state, std::string filename,
      bool append = false,
      std::string sep = ",") const;

  // Save status of the agents to a file
  void saveStatus(const State<T> &state, 
      std::string filename, bool append = false,
      std::string sep = ",") const;

  // Save strategy of the agents to a file
  void saveStrategy(const State<T> &state, 
      std::string filename, bool append = false,
      std::string sep = ",") const;

  // Save sizes of the agents to a file
  void saveSizes(const State<T> &state, 
      std::string filename, bool append = false,
      std::string sep = ",") const;

  // Save class id of the agents to a file
  void saveClassId(const State<T> &state, 
      std::string filename, bool append = false,
      std::string sep = ",") const;

  // Save audibility threshold of the agents to a file
  void saveAudibilityThreshold(const State<T> &state, std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save positions of the agents to a file
  void savePositions(int dim, const State<T> &state, 
      std::string filename, bool append = false,
      std::string sep = ",") const;

  // Save distances between agents to a file
  void saveDistances(const State<T> &state, std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save distances between agents and classes to a file
  void saveDistancesAgentToClass(const State<T> &state, std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save distances between classes to a file
  void saveDistancesClassToClass(const State<T> &state, std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save audibility of the agents to a file
  void saveAudibility(const State<T> &state, std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save audibility of the agents to classes to a file
  void saveAudibilityAgentToClass(const State<T> &state, std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save audibility of the classes to classes to a file
  void saveAudibilityClassToClass(const State<T> &state, std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save reachability of the agents to a file
  void saveReachability(const State<T> &state, std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save reachability of the agents to classes to a file
  void saveReachabilityAgentToClass(const State<T> &state, 
      std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save reachability of the classes to classes to a file
  void saveReachabilityClassToClass(const State<T> &state, 
      std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save contact of the agents to a file
    void saveContactAgentToAgent(const State<T> &state, std::string filename, 
        bool append = false,
        std::string sep = ",") const;

  // Save contact of the agents to classes to a file
  void saveContactAgentToClass(const State<T> &state, std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Save contact of the classes to classes to a file
  void saveContactClassToClass(const State<T> &state, std::string filename, 
      bool append = false,
      std::string sep = ",") const;

  // Print parameters of the simulation
  void printParameters(std::ostream &stream = std::cout);

  // Print scaled parameters of the simulation
  void printScaledParameters();

  // Print distances between agents
  void printDistances(const State<T> &state, 
      std::ostream &stream = std::cout, 
      bool append = false,
      std::string sep = ",") const;

  // Print distances between agents and classes
  void printDistancesAgentToClass(const State<T> &state, 
      std::ostream &stream = std::cout, 
      bool append = false,
      std::string sep = ",") const;

  // Print distances between classes
  void printDistancesClassToClass(const State<T> &state, 
      std::ostream &stream = std::cout, 
      bool append = false,
      std::string sep = ",") const;


  /* Getters */
  SimulationParameters<T> getParameters();
  SimulationParameters<T> getScaledParameters();

  // Get agent class pointer
  T* getAgentClassPointer(State<T> &state, int class_id);
  T* getAgentClassPointer(int class_id);

  // Get the state of the simulation
  State<T> &getState();

  // Get the dimensions of the simulation
  void getDims(T* dims); 

  // Get the iteration counter
  int getIteration();

  // Get the run counter
  int getRunCounter();

  // Get the time of the simulation
  double getTime();
  double getTimelimit();

  // Set log level
  void setLogLevel(int level);
  int getLogLevel();

  // Get initialized flag
  bool getInitialized();

  // Print agent class summaries  
  void printAgentClassSummaries() const;

 private:

  // Global parameters, not a local copy, but a reference
  GlobalParameters<T> &p_global;

  // Configured parameters
  SimulationParameters<T> sp_conf;

  // Simulation parameters, scaled to unit space and timestep per iteration
  SimulationParameters<T> sp_scaled;

  // State of the simulation
  State<T> new_state;
  std::vector<State<T>> states;

  // Agents
  std::vector<Agent<T>> agents;

  // Agent classes
  std::vector<AgentClass<T>> agent_classes;

  // System metrics
  SystemMetrics<T> system_metrics;

  // Random device 
  std::random_device &rd;

  // Random number generator
  std::mt19937 gen;

  // Uniform distributions for random numbers
  std::uniform_real_distribution<T> dist_uni;
  std::uniform_real_distribution<T> dist_x;
  std::uniform_real_distribution<T> dist_y;
  std::uniform_real_distribution<T> dist_z;

  // Log flags
  int log_level;
  bool log_flags[(int)LogFlags::NUM_LOG_FLAGS];
  bool log_state;

  // Initialized flag
  bool initialized;

  // Restore file 
  std::string restore_file;


  /*** Private functions ***/

  /* Initialization functions */

  // Set parameters of the simulation
  void setParameters(SimulationParameters<T> &p);

  // Init the states vector
  void initStates();

  // Init the agents vector
  void initAgents(const State<T> &state);

  // Init the agent classes vector
  void initAgentClasses();

  // Init the agent metrics
  void initAgentMetrics();

  // Init log flags
  void initLogFlags();

  // Initialize the parameters
  void initParameters();

  // Initialize the random number generator and distributions
  void initRandom();

  // Initialize the state of the simulation
  void initState(State<T> &state);

  // Initialize the audibility of the agents
  void initAudibility(State<T> &state);

  // Initialize the positions of the agents
  void initPositions(State<T> &state);

  // Initialize the energy of the agents
  void initEnergy(State<T> &state);
  void initEnergyConsumptionRateTime(State<T> &state);
  void initEnergyConsumptionRateDistance(State<T> &state);
  void initEnergyUptakeRate(State<T> &state);

  // Initialize the movement speed of the agents
  void initMovespeed(State<T> &state);

  // Initialize the size of the agents
  void initSizes(State<T> &state);

  // Initialize the status of the agents
  void initStatus(State<T> &state);

  // Initialize the class id of the agents
  void initClassId(State<T> &state);

  // Initialize the strategy of the agents
  void initStrategy(State<T> &state);


  /* Transition functions */

  // Update the positions of the agents
  void updatePositions(const State<T> &state, State<T> &new_state);

  // Update the energy of the agents
  void updateEnergy(const State<T> &state, State<T> &new_state);
  void updateEnergyConsumptionRateTime(const State<T> &state, 
      State<T> &new_state);
  void updateEnergyConsumptionRateDistance(const State<T> &state, 
      State<T> &new_state);
  void updateEnergyUptakeRate(const State<T> &state, State<T> &new_state);

  // Register at food sources for energy consumption
  void attackFood(const State<T> &state, State<T> &new_state);

  // Update the audibility of the agents
  void updateAudibility(const State<T> &state, State<T> &new_state);

  // Update the movement speed of the agents
  void updateMovespeed(const State<T> &state, State<T> &new_state);

  // Update the sizes of the agents
  void updateSizes(const State<T> &state, State<T> &new_state);

  // Update the class id of the agents
  void updateClassId(const State<T> &state, State<T> &new_state);

  // Update the strategy of the agents
  void updateStrategy(const State<T> &state, State<T> &new_state);

  // Update the status of the agents
  void updateStatus(const State<T> &state, State<T> &new_state);


  /* Update meta functions */

  // Calculate the audibility of agents
  void calculateAudibilityAgentToAgent(State<T> &state); 
  void calculateAudibilityAgentToClass(State<T> &state);
  void calculateAudibilityClassToClass(State<T> &state);

  // Calculate distances between agents
  void calculateDistancesAgentToAgent(State<T> &state);
  void calculateDistancesAgentToClass(State<T> &state);
  void calculateDistancesClassToClass(State<T> &state);

  // Calculate the reachable agents
  void calculateReachabilityAgentToAgent(State<T> &state);
  void calculateReachabilityAgentToClass(State<T> &state);
  void calculateReachabilityClassToClass(State<T> &state);

  // Calculate the contact of agents
  void calculateContactAgentToAgent(State<T> &state);
  void calculateContactAgentToClass(State<T> &state);
  void calculateContactClassToClass(State<T> &state);

  // Update the metadata of the state
  void updateMeta(State<T> &state);
  

  /* Other functions */

  // Swap the states
  void swapStates();

  // Open a file for writing
  void openFile(std::ofstream &file, std::string filename, bool append) const;

};

#endif //STATE_MACHINE_H
/* vim: set ts=2 sw=2 tw=0 et :*/
