#ifndef AGENT_H
#define AGENT_H

#include <random>

#include "Point3D.h"
#include "SimulationParameters.h"
#include "AgentClassParameters.h"
#include "State.h"
#include "AgentClass.h"
#include "Search.h"
#include "helpers.h"
#include "AgentMetrics.h"

#define STATUS_DEAD 0
#define STATUS_ALIVE 1

#define DEATH_THRESHOLD 0.1 // Do not put this to zero, 
                            // otherwise problems will occur

enum class Status{
  DEAD,
  ALIVE,
  ATTACKED
};

template <typename T>
class Agent
{
 public:

  // Constructor
  Agent(int id, 
      int verbosity,
      SimulationParameters<T> &sim_params, 
      std::vector<Agent<T>> &agents,
      std::vector<AgentClass<T>> &agent_classes,
      AgentMetrics<T> &agent_metrics,
      std::mt19937 &gen,
      std::uniform_real_distribution<T> &dist_uni,
      std::uniform_real_distribution<T> &dist_x,
      std::uniform_real_distribution<T> &dist_y,
      std::uniform_real_distribution<T> &dist_z
      );

  // Destructor
  ~Agent();

  // Initialize the agent
  void init(const State<T> &state);

  // Print the agent id
  void printId(std::ostream &os = std::cout);

  // Print the agent state
  void printState(const State<T> &state, std::ostream &os = std::cout);

  /* 
   * The following functions can be executed for all agents in 
   * each iteration of the simulation
   */

  // Update position
  void updatePosition(const State<T> &state, State<T> &new_state);

  // Update movement speed
  void updateMovespeed(const State<T> &state, State<T> &new_state);

  // Update size  
  void updateSize(const State<T> &state, State<T> &new_state);

  // Update class id  
  void updateClassId(const State<T> &state, State<T> &new_state);

  // Update strategy
  void updateStrategy(const State<T> &state, State<T> &new_state);

  // Update status
  void updateStatus(const State<T> &state, State<T> &new_state);

  // Update audibility to others
  void updateAudibilityThreshold(const State<T> &state, State<T> &new_state);

  // Update energy consumption rate per time step
  void updateEnergyConsumptionRateTime(
      const State<T> &state, State<T> &new_state);

  // Update energy consumption rate per distance
  void updateEnergyConsumptionRateDistance(
      const State<T> &state, State<T> &new_state);

  // Update energy uptake rate
  void updateEnergyUptakeRate(
      const State<T> &state, State<T> &new_state);

  // Update energy, this excludes energy exchange between agents
  void updateEnergy(const State<T> &state, State<T> &new_state);


  // Energy exchange, i.e. food consumption
  // This has to be done in two steps:
  // Part 1: all agents register themselves at the agents they want to eat, 
  // Part 2: all agents calculate how much energy to give to the agents that 
  // want to eat them and distribute the energy
  void energyExchangePartOneRegister(const State<T> &state, 
      State<T> &new_state);
  void energyExchangePartTwoDistribute(const State<T> &state, 
      State<T> &new_state);

  
 private:

  // Random number generator
  std::mt19937 &gen;

  // Distribution for random numbers
  std::uniform_real_distribution<T> &dist_uni;
  std::uniform_real_distribution<T> &dist_x;
  std::uniform_real_distribution<T> &dist_y;
  std::uniform_real_distribution<T> &dist_z;

  // Id of the agent
  int id;

  // Transition state, not necessaryly equal to the status, only
  // relevant within one transition, currently 
  // for example used for respawn,
  // thus death and rebirth are possible within one iteration; 
  // 
  // The status in the state object should only be changed by the
  // updateStatus function, other functions should use the transition_state,
  // updateStatus should handle the synchronization with the state object
  Status transition_state;

  // Reference to vector of other agents
  std::vector<Agent<T>> &agents;

  // Reference to the agent metrics
  AgentMetrics<T> &agent_metrics;

  // Energy exchange register
  std::vector<std::pair<int, T>> energy_exchange_register;

  // Verbosity level
  int verbosity;

  // Search class object
  Search<T> search_memory;

  // Simulation parameters
  SimulationParameters<T> &sim_params;

  // Agent classes  
  std::vector<AgentClass<T>> &agent_classes;

  // CreateMove
  Point3D<T> createMove(const State<T> &state);

  
  // Move strategies
  Point3D<T> moveStrategy0(const State<T> &state);
  Point3D<T> moveStrategy1(const State<T> &state);
  Point3D<T> moveStrategy2(const State<T> &state);
  Point3D<T> moveStrategy3(const State<T> &state);
  Point3D<T> moveStrategy4(const State<T> &state);
  Point3D<T> moveStrategy5(const State<T> &state);
  Point3D<T> moveStrategy6(const State<T> &state);
  Point3D<T> moveStrategy7(const State<T> &state);
  Point3D<T> moveStrategy8(const State<T> &state);


  // Approach closest food source
  Point3D<T> approachClosestFoodSource(const State<T> &state);

  // Approach food source with maximum energy
  Point3D<T> approachFoodMaxEnergy(const State<T> &state);

  // Approach food sources by center of gravity weighted by energy
  Point3D<T> approachFoodCogEnergy(const State<T> &state); 

  // Approach friends
  Point3D<T> approachClosestFriends(const State<T> &state);
  Point3D<T> approachClosestFriendsHearingFood(const State<T> &state);


  // Avoid enemies
  Point3D<T> avoidClosestEnemies(const State<T> &state);

  // Search
  Point3D<T> search(const State<T> &state);

  // Stay
  Point3D<T> stay();
  void stay(const State<T> &state, State<T> &new_state);

  // Move randomly
  Point3D<T> randomMove(const State<T> &state);

  // Get the scale factors for the movespeed
  T getSpeedFactorApproachFriends(const State<T> &state);
  T getSpeedFactorApproachFood(const State<T> &state);
  T getSpeedFactorAvoidEnemies(const State<T> &state);
  T getSpeedFactorSearch(const State<T> &state);

  // Find out how many agents of the given classes are within the distance
  int getNumContact(const State<T> &state, 
      const std::vector<int> &class_ids) const;
  int getNumReachable(const State<T> &state, 
      const std::vector<int> &class_ids) const;
  int getNumAudible(const State<T> &state, 
      const std::vector<int> &class_ids) const;

  // Find out wether there are agents of the categories within the distance
  bool foodContact(const State<T> &state) const;
  bool friendContact(const State<T> &state) const;
  bool enemyContact(const State<T> &state) const;

  bool foodReachable(const State<T> &state) const;
  bool friendReachable(const State<T> &state) const;
  bool enemyReachable(const State<T> &state) const;

  bool foodAudible(const State<T> &state) const;
  bool friendAudible(const State<T> &state) const;
  bool enemyAudible(const State<T> &state) const;

  // Find out wether the agent is attacked
  bool attacked(const State<T> &state) const;

  // Find out how many agents of the given categories are within the distance
  int numFoodContact(const State<T> &state) const;
  int numFriendContact(const State<T> &state) const;
  int numEnemyContact(const State<T> &state) const;

  int numFoodReachable(const State<T> &state) const;
  int numFriendReachable(const State<T> &state) const;
  int numEnemyReachable(const State<T> &state) const;

  int numFoodAudible(const State<T> &state) const;
  int numFriendAudible(const State<T> &state) const;
  int numEnemyAudible(const State<T> &state) const;

  // Find closest agent of a given agent class, return the index
  int findClosestContact(const State<T> &state, int &class_id);
  int findClosestReachable(const State<T> &state, int &class_id);
  int findClosestAudible(const State<T> &state, int &class_id);
  int findClosestAware(const State<T> &state, int class_id);
  int findClosest(const State<T> &state, 
      const std::vector<int> &target_classes);

  // Find closest agent of a given agent class, which at least hears one agent
  // of the given agent classes, return the index
  int findClosestContact(const State<T> &state, int &class_id, 
      const std::vector<int> &audible_classes);
  int findClosestReachable(const State<T> &state, int &class_id,
      const std::vector<int> &audible_classes);
  int findClosestAudible(const State<T> &state, int &class_id,
      const std::vector<int> &audible_classes);
  int findClosestAware(const State<T> &state, int class_id,
      const std::vector<int> &audible_classes);
  int findClosest(const State<T> &state, 
      const std::vector<int> &target_classes, 
      const std::vector<int> &audible_classes,
      bool two_masks = false);

  // Approach closest of a given class in awareness range
  Point3D<T> approachClosest(const State<T> &state, 
      const std::vector<int> &target_classes, T scale);
  Point3D<T> approachClosest(const State<T> &state, 
      const std::vector<int> &target_classes, 
      const std::vector<int> &audible_classes,
      T scale);

  // Avoid closest of a given class in awareness range
  Point3D<T> avoidClosest(const State<T> &state, 
      const std::vector<int> &target_classes, T scale);

  // Boundary control
  void boundaryControl(Point3D<T> &pos);

  // Init search
  void initSearch(
      int class_id,
      SimulationParameters<T> &sim_params, 
      std::mt19937 &gen);

  // Register for energy exchange
  void registerForEnergyExchange(int &other_id, T &amount);

  // Give energy
  void giveEnergy(const State<T> &state, 
      State<T> &new_state, int &other_id, double amount);

  // Respawn, completely overrides all other updates
  void respawn(const State<T> &state, State<T> &new_state, const int &class_id);

  // Prevent overshooting
  void preventOvershooting(const State<T> &state, const Point3D<T> &move);

  // Set transition state
  void setTransitionState(Status status);

};

#endif //AGENT_H
/* vim: set ts=2 sw=2 tw=0 et :*/
