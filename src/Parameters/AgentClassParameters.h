#ifndef AGENT_CLASS_PARAMETERS_H
#define AGENT_CLASS_PARAMETERS_H

#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include "Point3D.h"
#include "Color.h"

#include "toml.hpp"

// Forward declaration of AgentClass
template <typename T>
class AgentClass;

template <typename T>
class Agent;

template <typename T>
struct AgentClassParameters
{
  // Default constructor
  AgentClassParameters();

  // Member initialize constructor
  AgentClassParameters(
      std::string name, 
      int id,
      std::vector<std::string> friends,
      std::vector<std::string> enemies,
      std::vector<std::string> food_sources,
      int strategy,
      T mu_levy,
      T c_levy,
      T brownian_search_duration,
      T movespeed_search,
      T movespeed_approach_friends,
      T movespeed_avoid_enemies,
      T movespeed_approach_food,
      int status,
      int num_agents, 
      T audibility, 
      T mu_energy, 
      T sigma_energy, 
      T energy_uptake_rate, 
      T movespeed, 
      T energy_consumption_per_distance, 
      T energy_consumption_per_time, 
      T size, 
      Point3D<T> start_position,
      bool random_start_positions,
      Color<T> color, 
      T pointsize);

  // Copy constructor
  AgentClassParameters(const AgentClassParameters<T> &c);

  // Copy scale constructor, scales in space and time 
  AgentClassParameters(const AgentClassParameters<T> &c, T dt, T space_scaling_factor);

  // Operator = to copy the parameters
  AgentClassParameters &operator=(const AgentClassParameters<T> &c);

  // Destructor
  ~AgentClassParameters();

  // Set energy distribution
  void setEnergyDistribution(T mu, T sigma);

  // Print the class
  void print(std::ostream &os = std::cout) const;

  // Save the class to a file
  void save(std::string filename, bool append = false);

  // Load the class from a file
  void load(std::string filename);

  // Setters/getters
  // Class name
  void setName(std::string name);
  std::string getName() const;

  // Class id
  int getId() const;

  // Friends
  void setFriends(std::vector<std::string> friends);
  std::vector<std::string> getFriends() const;

  // Enemies
  void setEnemies(std::vector<std::string> enemies);
  std::vector<std::string> getEnemies() const;

  // Food sources
  void setFoodSources(std::vector<std::string> food_sources);
  std::vector<std::string> getFoodSources() const;

  // Set strategy
  void setStrategy(int strategy);
  int getStrategy() const;

  // Levy search parameters
  void setMuLevy(T mu_levy);
  void setCLevy(T c_levy);
  T getMuLevy() const;
  T getCLevy() const;

  // Brownian search parameters
  void setBrownianSearchDuration(T brownian_search_duration);
  T getBrownianSearchDuration() const;

  // Movespeed, fraction of the maximal movespeed for each movement type
  void setMovespeedSearch(T movespeed_search);
  void setMovespeedApproachFriends(T movespeed_approach_friends);
  void setMovespeedAvoidEnemies(T movespeed_avoid_enemies);
  void setMovespeedApproachFood(T movespeed_approach_food);
  T getMovespeedSearch() const;
  T getMovespeedApproachFriends() const;
  T getMovespeedAvoidEnemies() const;
  T getMovespeedApproachFood() const;

  // Status
  void setStatus(int status);
  int getStatus() const;

  // Number of agents
  void setNumAgents(int num_agents);
  int getNumAgents() const;

  // Audibility
  void setAudibility(T audibility);
  T getAudibility() const;

  // Energy
  void setSigmaEnergy(T sigma_energy);
  void setMuEnergy(T mu_energy);
  T getSigmaEnergy() const;
  T getMuEnergy() const;

  // Energy uptake rate
  void setEnergyUptakeRate(T energy_uptake_rate);
  T getEnergyUptakeRate() const;

  // Movespeed (this is the maximal movespeed)
  void setMovespeed(T movespeed);
  T getMovespeed() const;

  // Energy consumption per time 
  void setEnergyConsumptionPerTime(T energy_consumption_per_time);
  T getEnergyConsumptionPerTime() const;

  // Energy consumption per distance 
  void setEnergyConsumptionPerDistance(T energy_consumption_per_distance);
  T getEnergyConsumptionPerDistance() const;

  // Size
  void setSize(T size);
  T getSize() const;

  // Start position
  void setStartPosition(Point3D<T> start_position);
  Point3D<T> getStartPosition() const;

  // Random start positions
  void setRandomStartPositions(bool random_start_positions);
  bool getRandomStartPositions() const;

  // Color
  void setColor(Color<T> color);
  Color<T> getColor() const;

  // Pointsize
  void setPointsize(T pointsize);
  T getPointsize() const;

  // Initialize the parameters
  void init();

  /* Member variables */

  // AgentClassParameters name
  std::string name;

  // AgentClassParameters id
  int id;

  // Friend classes 
  std::vector<std::string> friends;

  // Enemy classes
  std::vector<std::string> enemies;

  // Prey classes
  std::vector<std::string> food_sources;

  // strategy of the agents
  int strategy;

  // Levy search parameters
  T mu_levy;
  T c_levy;

  // Brownian search parameters
  T brownian_search_duration;

  // Movespeed
  T movespeed_search;
  T movespeed_approach_friends;
  T movespeed_avoid_enemies;
  T movespeed_approach_food;

  // Status of the agents
  int status;

  // number of agents
  int num_agents;

  // global start and stop index of the agents
  int start_index;
  int stop_index;

  // audibility range in distance 
  T audibility;

  // initial energy, normally distributed
  T mu_energy;
  T sigma_energy;

  // energy uptake rate in energy per time 
  T energy_uptake_rate;

  // maximal movespeed in distance-units/time
  T movespeed;

  // energy consumption per time 
  T energy_consumption_per_time;

  // energy consumption per distance 
  T energy_consumption_per_distance;

  // size of the agents in distance units
  T size;

  // Start position of the agents
  Point3D<T> start_position;

  // Random start instead of start_position,
  // each agent starts at different position
  bool random_start_positions;

  // color of the agents 
  Color<T> color;

  // pointsize of the agents 
  T pointsize;

  // energy distribution
  std::normal_distribution<T> dist_energy;

  // Flag to check if the parameters have been initialized
  bool initialized;

  friend class AgentClass<T>;
  friend class Agent<T>;

};

#endif //AGENT_CLASS_PARAMETERS_H

/* vim: set ts=2 sw=2 tw=0 et :*/
