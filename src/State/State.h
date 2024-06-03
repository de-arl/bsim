/*
 * The state structure is a container for all the state variables
 * which define the state of the simulation. It further contains
 * derived variables which are calculated from the state variables.
 */
#ifndef STATE_H
#define STATE_H

#include "Point3D.h"
#include "toml.hpp"
#include "helpers.h"

#include <vector>
#include <cstdlib>
#include <omp.h>


// Forward declaration of StateMachine
template <typename T>
class StateMachine;

template <typename T>
class Agent;

template <typename T>
class AgentClass;

template <typename T>
class Search;

template <typename T>
class LevySearch;

template <typename T>
class BrownianSearch;

template <typename T>
struct State
{

 public:

  // Default constructor
  State();

  // Constructor, allocate memory for the state
  State(int num_agents_total, int num_classes, int verbosity = 1);

  // Copy constructor
  State(const State& other);

  // Move constructor
  State(State&& other);

  // Destructor
  ~State();

  // Move assignment operator
  State& operator=(State&& other);

  // Copy operator
  State& operator=(const State& other);
  
  // Allocate/free memory for the state, fill with zeros
  void allocateMemory(int num_agents_total, int num_classes);
  void freeMemory();
  void zeroMemory();

  // Get/set the number of agents
  int getNumAgentsTotal() const;
  void setNumAgentsTotal(int num_agents_total);

  // Get/set the positions
  T* getPositions() const;  
  void setPositions(Point3D<T>* positions);

  // Get/set the energy
  T* getEnergy() const;
  void setEnergy(T* energy);

  // Get/set the iteration
  T getIteration() const;
  void setIteration(T iteration);

  // Get/set the run
  int getRunCounter() const;
  void setRunCounter(int run_counter);

  // Get/set the time
  double getTimestamp() const;
  void setTimestamp(double time);

  // Get/set the verbosity
  int getVerbosity() const;
  void setVerbosity(int verbosity);

  // Get/set the initialized flag
  bool getInitialized() const;
  void setInitialized(bool initialized);

  // Calculate distances between agents
  void calculateDistances();

  // Calculate audibility of agents
  void calculateAudibility();

  // Calculate reachability of agents
  void calculateReachability();

  // Calculate contact of agents
  void calculateContact();

  // Calculate sum of energy in the system
  T sumEnergy() const; 

  // Calculate average
  void calculateAverage(T* data, int start, int end, T& avg) const;

  // Calculate sum
  void calculateSum(const int* data, const int& start, const int& end, int& sum) const;
  void calculateSum(const T* data, const int& start, const int& end, T& sum) const;

  // Count true values
  void countTrue(int* data, int start, int end, int& count) const;

  // Print positions
  void printPositions(std::ostream &os = std::cout, 
      int dim = 3,
      std::string sep = ",") const;
  void printPositionsToml(std::ostream &os = std::cout) const;

  // Print energy
  void printEnergy(std::ostream &os = std::cout, 
      bool append = true, std::string sep = ",") const;
  void printEnergyToml(std::ostream &os = std::cout) const;

  // Print distances
  void printDistances(std::ostream &os = std::cout, 
      bool append = true, std::string sep = ",") const;

  // Print agent to class distances
  void printDistancesAgentToClass(
      std::ostream &os = std::cout, 
      bool append = true, std::string sep = ",") const;

  // Print class to class distances
  void printDistancesClassToClass(
      std::ostream &os = std::cout, 
      bool append = true, std::string sep = ",") const;

  // Print audibility
  void printAudibility(std::ostream &os = std::cout, bool append = true, 
      std::string sep = ",") const;
  void printAudibilityAgentToClass(std::ostream &os = std::cout,
      bool append = true, std::string sep = ",") const;
  void printAudibilityClassToClass(std::ostream &os = std::cout,
      bool append = true, std::string sep = ",") const;

  // Print reachability
  void printReachability(std::ostream &os = std::cout, bool append = true, 
      std::string sep = ",") const;
  void printReachabilityAgentToClass(std::ostream &os = std::cout,
      bool append = true, std::string sep = ",") const;
  void printReachabilityClassToClass(std::ostream &os = std::cout,
    bool append = true, std::string sep = ",") const;

  // Print contact
  void printContact(std::ostream &os = std::cout, bool append = true, 
      std::string sep = ",") const;
  void printContactAgentToClass(std::ostream &os = std::cout,
      bool append = true, std::string sep = ",") const;
  void printContactClassToClass(std::ostream &os = std::cout,
    bool append = true, std::string sep = ",") const;

  // Print class id
  void printClassIdToml(std::ostream &os = std::cout) const;
  void printClassId(std::ostream &os = std::cout, bool append = true, 
      std::string sep = ",") const;

  // Print strategy
  void printStrategy(std::ostream &os = std::cout, bool append = true, 
      std::string sep = ",") const;
  void printStrategyToml(std::ostream &os = std::cout) const;

  // Print status
  void printStatus(std::ostream &os = std::cout, bool append = true, 
      std::string sep = ",") const;
  void printStatusToml(std::ostream &os = std::cout) const;

  // Print movespeed
  void printMovespeed(std::ostream &os = std::cout, bool append = true, 
      std::string sep = ",") const;
  void printMovespeedToml(std::ostream &os = std::cout) const;

  // Print sizes
  void printSizes(std::ostream &os = std::cout, bool append = true, 
      std::string sep = ",") const;
  void printSizesToml(std::ostream &os = std::cout) const;

  // Print alive
  void printAlive(std::ostream &os = std::cout, bool append = true, 
      std::string sep = ",") const;
  void printAliveToml(std::ostream &os = std::cout) const;

  // Print audibility threshold
  void printAudibilityThreshold(std::ostream &os = std::cout, 
      bool append = true, std::string sep = ",") const;
  void printAudibilityThresholdToml(std::ostream &os = std::cout) const;

  // Print energy consumption per time
  void printEnergyConsumptionPerTime(std::ostream &os = std::cout, 
      bool append = true, std::string sep = ",") const;
  void printEnergyConsumptionPerTimeToml(std::ostream &os = std::cout) const;

  // Print energy consumption per distance
  void printEnergyConsumptionPerDistance(std::ostream &os = std::cout, 
     bool append = true, std::string sep = ",") const;
  void printEnergyConsumptionPerDistanceToml(
      std::ostream &os = std::cout) const;

  // Print energy uptake rate
  void printEnergyUptakeRate(std::ostream &os = std::cout, 
      bool append = true, std::string sep = ",") const;
  void printEnergyUptakeRateToml(std::ostream &os = std::cout) const;

  // Print state
  void printState(std::ostream &os = std::cout, bool append = true, 
      std::string sep = ",", int verbosity = 1) const;

  // Print state toml
  void printStateToml(std::ostream &os = std::cout) const;

  // Load functions
  void loadStateToml(std::string filename);
  void loadPositions(const toml::table &table);
  void loadEnergy(const toml::table &table);
  void loadClassId(const toml::table &table);
  void loadStrategy(const toml::table &table);
  void loadStatus(const toml::table &table);
  void loadMovespeed(const toml::table &table);
  void loadSizes(const toml::table &table);
  void loadAudibilityThreshold(const toml::table &table);
  void loadEnergyConsumptionPerTime(const toml::table &table);
  void loadEnergyConsumptionPerDistance(const toml::table &table);
  void loadEnergyUptakeRate(const toml::table &table);

 private:

  // Verbosity level
  int verbosity;

  // Number of agents
  int num_agents_total;

  // Number of classes
  int num_classes;

  // Iteration counter
  int iteration;

  // Run counter
  int run_counter;

  // Timestamp
  double time;

  // Initialized flag
  bool initialized;

  // Element positions
  Point3D<T> *positions;

  // Element energy
  T *energy;

  // Movement speed
  T *movespeed;

  // Size
  T *sizes;


  /* Intrinsic properties */

  // Agent status, can be used to define different states of the agent
  // e.g. alive, dead, sleeping, pregnant, unborn, etc.
  int *status;

  // Class id
  int *class_id;

  // Move strategy, depending on this variable the createMove function
  // selects the corresponding movement strategy
  int *strategy;

  // Audibility threshold
  T *audibility_threshold;

  // Energy consumption per time
  T *energy_consumption_per_time;

  // Energy consumption per distance
  T *energy_consumption_per_distance;

  // Energy uptake rate
  T *energy_uptake_rate;


  /* Derived variables */

  // Distance matrix NxN 
  // agent to agent
  T *distances_agent_to_agent;

  // Distance matrix #num_classesxN
  // agent to class average
  T *distances_agent_to_class;

  // Distance matrix #num_classesx#num_classes 
  // class to class average
  T *distances_class_to_class;

  // Audibility matrix NxN, agent to agent
  int *audibility;
  
  // Sum of audibile agents of other classes
  int *audibility_agent_to_class;

  // Class audibility
  int *audibility_class_to_class;

  // Reachability matrix NxN
  int *reachability;

  // Sum of reachable agents of other classes
  int *reachability_agent_to_class;

  // Class reachability
  int *reachability_class_to_class;

  // Contact matrix NxN
  int *contact;

  // Sum of contact agents of other classes
  int *contact_agent_to_class;

  // Class contact
  int *contact_class_to_class;


  // Lock
#ifdef ANIMATION
  omp_lock_t lock;
#endif


  // Friend classes
  friend class StateMachine<T>;
  friend class Agent<T>;
  friend class AgentClass<T>;
  friend class Search<T>;
  friend class LevySearch<T>;

};

#endif //STATE_H
/* vim: set ts=2 sw=2 tw=0 et :*/
