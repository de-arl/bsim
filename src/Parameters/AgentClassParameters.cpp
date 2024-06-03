#include "AgentClassParameters.h"

template <typename T>
AgentClassParameters<T>::AgentClassParameters()
  :
    name("default"),
    id(0),
    strategy(0),
    mu_levy(0.0),
    c_levy(0.0),
    brownian_search_duration(0.0),
    movespeed_search(0.0),
    movespeed_approach_friends(0.0),
    movespeed_avoid_enemies(0.0),
    movespeed_approach_food(0.0),
    status(0),
    num_agents(0),
    audibility(0.0),
    mu_energy(0.0),
    sigma_energy(0.0),
    energy_uptake_rate(0.0),
    movespeed(0.0),
    energy_consumption_per_time(0.0),
    energy_consumption_per_distance(0.0),
    size(0.0),
    start_position(Point3D<T>(0.0, 0.0, 0.0)),
    random_start_positions(false),
    color(Color<T>(0.0, 0.0, 0.0)),
    pointsize(0.0)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::AgentClassParameters: \
    Default constructor of AgentClassParameters" << std::endl;
#endif
}

template <typename T>
AgentClassParameters<T>::AgentClassParameters(
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
    T energy_consumption_per_time,
    T energy_consumption_per_distance,
    T size,
    Point3D<T> start_position,
    bool random_start_positions,
    Color<T> color,
    T pointsize)
    : 
      name(name),
      id(id),
      friends(friends),
      enemies(enemies),
      food_sources(food_sources),
      strategy(strategy),
      mu_levy(mu_levy),
      c_levy(c_levy),
      brownian_search_duration(brownian_search_duration),
      movespeed_search(movespeed_search),
      movespeed_approach_friends(movespeed_approach_friends),
      movespeed_avoid_enemies(movespeed_avoid_enemies),
      movespeed_approach_food(movespeed_approach_food),
      status(status),
      num_agents(num_agents),
      audibility(audibility),
      mu_energy(mu_energy),
      sigma_energy(sigma_energy),
      energy_uptake_rate(energy_uptake_rate),
      movespeed(movespeed),
      energy_consumption_per_time(energy_consumption_per_time),
      energy_consumption_per_distance(energy_consumption_per_distance),
      size(size),
      start_position(start_position),
      random_start_positions(random_start_positions),
      color(color),
      pointsize(pointsize),
      dist_energy(std::normal_distribution<T>(mu_energy, sigma_energy)),
      initialized(false)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::AgentClassParameters: \
    Member initialize constructor of AgentClassParameters" << std::endl;
#endif
}

template <typename T>
AgentClassParameters<T>::AgentClassParameters(const AgentClassParameters &c)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::AgentClassParameters: \
    Copy constructor of AgentClassParameters" << std::endl;
#endif

  // Copy using the operator=
  *this = c;
}

template <typename T>
AgentClassParameters<T>::AgentClassParameters(
    const AgentClassParameters &c, T dt, T space_scaling_factor)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::AgentClassParameters: \
    Copy scale constructor of AgentClassParameters" << std::endl;
#endif

  // Copy using the operator=
  *this = c;

  // Spatially scale the audibility range
  audibility *= space_scaling_factor;
  
  // Temporally scale the energy uptake rate
  energy_uptake_rate *= dt;
  
  // Spatially scale the maximal movespeed
  movespeed *= space_scaling_factor;
  
  // Temporally scale the energy consumption per time
  energy_consumption_per_time *= dt;

  // Spatially scale the energy consumption per meter
  energy_consumption_per_distance/= space_scaling_factor;
  
  // Spatially scale the size of the agents
  size *= space_scaling_factor;

  // Spatially scale c and mu of the Levy distribution
  mu_levy *= space_scaling_factor;
  c_levy *= space_scaling_factor;
}

template <typename T>
AgentClassParameters<T>& AgentClassParameters<T>::operator=(
    const AgentClassParameters &c)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::operator=: \
    Operator = of AgentClassParameters" << std::endl;
#endif

  // Check for self-assignment
  if (this == &c)
  {
    return *this;
  }

  // Set the name
  name = c.name;

  // Set the friends
  friends = c.friends;

  // Set the enemies
  enemies = c.enemies;

  // Set the food sources
  food_sources = c.food_sources;

  // Set the id
  id = c.id;

  // Set the strategy
  strategy = c.strategy;

  // Set levy parameters
  mu_levy = c.mu_levy;
  c_levy = c.c_levy;

  // Set brownian search duration 
  brownian_search_duration = c.brownian_search_duration;

  // Set movespeeds
  movespeed_search = c.movespeed_search;
  movespeed_approach_friends = c.movespeed_approach_friends;
  movespeed_avoid_enemies = c.movespeed_avoid_enemies;
  movespeed_approach_food = c.movespeed_approach_food;

  // Set the status
  status = c.status;
  
  // Set the number of agents
  num_agents = c.num_agents;

  // Set the start and stop index
  start_index = c.start_index;
  stop_index = c.stop_index;
  
  // Set the audibility range
  audibility = c.audibility;
  
  // Set the initial energy
  mu_energy = c.mu_energy;
  sigma_energy = c.sigma_energy;
  
  // Set the energy uptake rate
  energy_uptake_rate = c.energy_uptake_rate;
  
  // Set the maximal movespeed
  movespeed = c.movespeed;
  
  // Set the energy consumption per time
  energy_consumption_per_time = c.energy_consumption_per_time;
  
  // Set the energy consumption per distance unit
  energy_consumption_per_distance= c.energy_consumption_per_distance;
  
  // Set the size of the agents
  size = c.size;

  // Set start position
  start_position = c.start_position;
  random_start_positions = c.random_start_positions;

  // Set the color of the agents
  color = c.color;
  
  // Set the pointsize of the agents
  pointsize = c.pointsize;

  // Set energy distribution
  dist_energy = c.dist_energy;

  return *this;
}

template <typename T>
AgentClassParameters<T>::~AgentClassParameters()
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::~AgentClassParameters: \
    Destructor of AgentClassParameters" << std::endl;
#endif

  // Nothing to do
  friends.clear();
  enemies.clear();
  food_sources.clear();
}

template <typename T>
void AgentClassParameters<T>::setEnergyDistribution(T mu, T sigma)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setEnergyDistribution: \
    Set the energy distribution of the agents:  mu: " << mu 
    << " sigma: " << sigma << std::endl;
#endif

  dist_energy = std::normal_distribution<T>(mu, sigma);
}

template <typename T>
void AgentClassParameters<T>::save(std::string filename, 
    bool append)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::save: \
    Saving the class of the agents to file: " << filename << std::endl;
#endif

  // Open the file 
  std::ofstream file;
  if (append){
    file.open(filename, std::ios_base::app);
  } else {
    file.open(filename);
  }

  // Write the class to the file
  print(file);

  // Close the file
  file.close();
}

template <typename T>
void AgentClassParameters<T>::load(std::string filename)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::load: \
    Loading the class of the agents from file: " << filename << std::endl;
#endif

  toml::table config;

  // Check if the file exists
  try  {
    config = toml::parse_file(filename);
  } catch (const std::exception& e) {
    std::cerr << "AgentClassParameters::load: Error: " 
      << e.what() << std::endl;
    return;
  }

  // As the class name is unknown, we need to iterate over the keys to
  // get the class name
  for (const auto& entry : config) {
    // Convert the key to a string
    std::string key = entry.first.data();
    // Set the name of the class
    setName(key);

    if (entry.second.as_table()->contains("friends")) {
      std::vector<std::string> friends;
      if (toml::array* arr = config[key]["friends"].as_array()) {
        for (const auto& val : *arr) {
          friends.push_back(val.value_or(""));
        }
      setFriends(friends);
      }
    }
      

    if (entry.second.as_table()->contains("num_agents")) {
      setNumAgents(config[key]["num_agents"].value_or(0));
    }

    if (entry.second.as_table()->contains("strategy")) {
      setStrategy(config[key]["strategy"].value_or(0));
    }

    if (entry.second.as_table()->contains("mu_levy")) {
      mu_levy = config[key]["mu_levy"].value_or(0.0);
    }

    if (entry.second.as_table()->contains("c_levy")) {
      c_levy = config[key]["c_levy"].value_or(0.0);
    }

    if (entry.second.as_table()->contains("brownian_search_duration")) {
      brownian_search_duration = 
        config[key]["brownian_search_duration"].value_or(0.0);
    }

    if (entry.second.as_table()->contains("movespeed_search")) {
      movespeed_search = config[key]["movespeed_search"].value_or(0.0);
    }

    if (entry.second.as_table()->contains("movespeed_approach_friends")) {
      movespeed_approach_friends = config[key]["movespeed_approach_friends"].value_or(0.0);
    }

    if (entry.second.as_table()->contains("movespeed_avoid_enemies")) {
      movespeed_avoid_enemies = config[key]["movespeed_avoid_enemies"].value_or(0.0);
    }

    if (entry.second.as_table()->contains("movespeed_approach_food")) {
      movespeed_approach_food = config[key]["movespeed_approach_food"].value_or(0.0);
    }

    if (entry.second.as_table()->contains("status")) {
      setStatus(config[key]["status"].value_or(0));
    }

    if (entry.second.as_table()->contains("audibility")) {
      setAudibility(config[key]["audibility"].value_or(0.0));
    }

    if (entry.second.as_table()->contains("mu_energy")) {
      setMuEnergy(config[key]["mu_energy"].value_or(0.0));
    }

    if (entry.second.as_table()->contains("sigma_energy")) {
      setSigmaEnergy(config[key]["sigma_energy"].value_or(0.0));
    }

    if (entry.second.as_table()->contains("energy_uptake_rate")) {
      setEnergyUptakeRate(config[key]["energy_uptake_rate"].value_or(0.0));
    }

    if (entry.second.as_table()->contains("movespeed")) {
      setMovespeed(config[key]["movespeed"].value_or(0.0));
    }

    if (entry.second.as_table()->contains("energy_consumption_per_time")) {
      setEnergyConsumptionPerTime(
          config[key]["energy_consumption_per_time"].value_or(0.0));
    }

    if (entry.second.as_table()->contains("energy_consumption_per_distance")) {
      setEnergyConsumptionPerDistance(
          config[key]["energy_consumption_per_distance"].value_or(0.0));
    }

    if (entry.second.as_table()->contains("size")) {
      setSize(config[key]["size"].value_or(0.0));
    }

    if (entry.second.as_table()->contains("start_position")) {
      Point3D<T> start_position;
      for (int i = 0; i < 3; i++) {
        start_position[i] = config[key]["start_position"][i].value_or(0.0);
      }
      setStartPosition(start_position);
    }

    if (entry.second.as_table()->contains("random_start_positions")) {
      setRandomStartPositions(
          config[key]["random_start_positions"].value_or(false));
    }

    if (entry.second.as_table()->contains("color")) {
      Color<T> color;
      for (int i = 0; i < 3; i++) {
        color[i] = config[key]["color"][i].value_or(0.0);
      }
      setColor(color);
    }

    if (entry.second.as_table()->contains("pointsize")) {
      setPointsize(config[key]["pointsize"].value_or(0.0));
    }

    // Break after the first key
    break;
  }
}

template <typename T>
void AgentClassParameters<T>::print(std::ostream& stream) const
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::printToml: \
    Printing the class of the agents in TOML format" << std::endl;
#endif

  // Print the class of the agents in TOML format
  stream << "[" << name << "]" << std::endl;
  stream << "  id = " << id << std::endl;

  stream << "  friends = [ ";
  for (int i = 0; i < friends.size(); i++) {
    stream << " \"" << friends[i] << "\"";
    if (i < friends.size() - 1) {
      stream << ", ";
    }
  }
  stream << " ]" << std::endl;

  stream << "  enemies = [ ";
  for (int i = 0; i < enemies.size(); i++) {
    stream << " \"" << enemies[i] << "\"";
    if (i < enemies.size() - 1) {
      stream << ", ";
    }
  }
  stream << " ]" << std::endl;

  stream << "  food_sources = [ ";
  for (int i = 0; i < food_sources.size(); i++) {
    stream << " \"" << food_sources[i] << "\"";
    if (i < food_sources.size() - 1) {
      stream << ", ";
    }
  }
  stream << " ]" << std::endl;

  stream << "  strategy = " << strategy << std::endl;
  stream << "  status = " << status << std::endl;
  stream << "  num_agents = " << num_agents << std::endl;
  stream << "  audibility = " << audibility << std::endl;
  stream << "  mu_energy = " << mu_energy << std::endl;
  stream << "  sigma_energy = " << sigma_energy << std::endl;
  stream << "  energy_uptake_rate = " << energy_uptake_rate << std::endl;
  stream << "  mu_levy = " << mu_levy << std::endl;
  stream << "  c_levy = " << c_levy << std::endl;
  stream << "  brownian_search_duration = " 
    << brownian_search_duration << std::endl;
  stream << "  movespeed_search = " << movespeed_search << std::endl;
  stream << "  movespeed_approach_friends = " 
    << movespeed_approach_friends << std::endl;
  stream << "  movespeed_avoid_enemies = " 
    << movespeed_avoid_enemies << std::endl;
  stream << "  movespeed_approach_food = "
    << movespeed_approach_food << std::endl;
  stream << "  movespeed = " << movespeed << std::endl;
  stream << "  energy_consumption_per_time = " 
    << energy_consumption_per_time << std::endl;
  stream << "  energy_consumption_per_distance = " 
    << energy_consumption_per_distance << std::endl;
  stream << "  size = " << size << std::endl;
  stream << "  start_position = [ "<< start_position.toString() << " ]"
    << std::endl;
  stream << "  random_start_positions = " << random_start_positions 
    << std::endl;
  stream << "  color = [ " << color.toString() << " ]" << std::endl;
  stream << "  pointsize = " << pointsize << std::endl;
  stream << std::endl;
}

template <typename T>
void AgentClassParameters<T>::setName(std::string name)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setName: \
    Set the name of the agent class to:" << name << std::endl;
#endif
  
  this->name = name;
}

template <typename T>
std::string AgentClassParameters<T>::getName() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getName: \
    Get the name of the agent class" << std::endl;
    */
#endif

  return name;
}

template <typename T>
int AgentClassParameters<T>::getId() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getId: \
    Get the id of the agent class" << std::endl;
    */
#endif

  return id;
}

template <typename T>
void AgentClassParameters<T>::setFriends(std::vector<std::string> friends)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setFriends: \
    Set the friends of the agent class" << std::endl;
#endif

  this->friends = friends;
}

template <typename T>
std::vector<std::string> AgentClassParameters<T>::getFriends() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getFriends: \
    Get the friends of the agent class" << std::endl;
    */
#endif

  return friends;
}

template <typename T>
void AgentClassParameters<T>::setEnemies(std::vector<std::string> enemies)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setEnemies: \
    Set the enemies of the agent class" << std::endl;
#endif

  this->enemies = enemies;
}

template <typename T>
std::vector<std::string> AgentClassParameters<T>::getEnemies() const
{ 
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getEnemies: \
    Get the enemies of the agent class" << std::endl;
    */
#endif

  return enemies;
}

template <typename T>
void AgentClassParameters<T>::setFoodSources(
    std::vector<std::string> food_sources)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setFoodSources: \
    Set the food sources of the agent class" << std::endl;
#endif

  this->food_sources = food_sources;
}

template <typename T>
std::vector<std::string> AgentClassParameters<T>::getFoodSources() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getFoodSources: \
    Get the food sources of the agent class" << std::endl;
    */
#endif

  return food_sources;
}

template <typename T>
void AgentClassParameters<T>::setStrategy(int strategy)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setStrategy: \
    Set the strategy of the agent class to:" << strategy << std::endl;
#endif

  this->strategy = strategy;
}

template <typename T>
int AgentClassParameters<T>::getStrategy() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getStrategy: \
    Get the strategy of the agent class" << std::endl;
    */
#endif

  return strategy;
}

template <typename T>
void AgentClassParameters<T>::setMuLevy(T mu_levy)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setMuLevy: \
    Set the mean of the Levy distribution of the agent class to:" 
      << mu_levy << std::endl;
#endif

  this->mu_levy = mu_levy;
}

template <typename T>
T AgentClassParameters<T>::getMuLevy() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getMuLevy: \
    Get the mean of the Levy distribution of the agent class" << std::endl;
    */
#endif

  return mu_levy;
}

template <typename T>
void AgentClassParameters<T>::setCLevy(T c_levy)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setCLevy: \
    Set the scale of the Levy distribution of the agent class to:" 
      << c_levy << std::endl;
#endif

  this->c_levy = c_levy;
}

template <typename T>
T AgentClassParameters<T>::getCLevy() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getCLevy: \
    Get the scale of the Levy distribution of the agent class" << std::endl;
    */
#endif

  return c_levy;
}

template <typename T>
void AgentClassParameters<T>::setBrownianSearchDuration(
    T brownian_search_duration)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setBrownianSearchDuration: \
    Set the duration of the Brownian search of the agent class to:" 
      << brownian_search_duration << std::endl;
#endif

  this->brownian_search_duration = brownian_search_duration;
}

template <typename T>
T AgentClassParameters<T>::getBrownianSearchDuration() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getBrownianSearchDuration: \
    Get the duration of the Brownian search of the agent class" << std::endl;
    */
#endif

  return brownian_search_duration;
}

template <typename T>
void AgentClassParameters<T>::setMovespeedSearch(T movespeed_search)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setMovespeedSearch: \
    Set the movespeed of the search of the agent class to:" 
      << movespeed_search << std::endl;
#endif

  this->movespeed_search = movespeed_search;
}

template <typename T>
T AgentClassParameters<T>::getMovespeedSearch() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getMovespeedSearch: \
    Get the movespeed of the search of the agent class" << std::endl;
    */
#endif

  return movespeed_search;
}

template <typename T>
void AgentClassParameters<T>::setMovespeedApproachFriends(
    T movespeed_approach_friends)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setMovespeedApproachFriends: \
    Set the movespeed to approach friends of the agent class to:" 
      << movespeed_approach_friends << std::endl;
#endif

  this->movespeed_approach_friends = movespeed_approach_friends;
}

template <typename T>
T AgentClassParameters<T>::getMovespeedApproachFriends() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getMovespeedApproachFriends: \
    Get the movespeed to approach friends of the agent class" << std::endl;
    */
#endif

  return movespeed_approach_friends;
}

template <typename T>
void AgentClassParameters<T>::setMovespeedAvoidEnemies(
    T movespeed_avoid_enemies)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setMovespeedAvoidEnemies: \
    Set the movespeed to avoid enemies of the agent class to:"
      << movespeed_avoid_enemies << std::endl;
#endif

  this->movespeed_avoid_enemies = movespeed_avoid_enemies;
}

template <typename T>
T AgentClassParameters<T>::getMovespeedAvoidEnemies() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getMovespeedAvoidEnemies: \
    Get the movespeed to avoid enemies of the agent class" << std::endl;
    */
#endif

  return movespeed_avoid_enemies;
}

template <typename T>
void AgentClassParameters<T>::setMovespeedApproachFood(
    T movespeed_approach_food)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setMovespeedApproachFood: \
    Set the movespeed to approach food of the agent class to:"
      << movespeed_approach_food << std::endl;
#endif

  this->movespeed_approach_food = movespeed_approach_food;
}

template <typename T>
T AgentClassParameters<T>::getMovespeedApproachFood() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getMovespeedApproachFood: \
    Get the movespeed to approach food of the agent class" << std::endl;
    */
#endif

  return movespeed_approach_food;
}

template <typename T>
void AgentClassParameters<T>::setStatus(int status)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setStatus: \
    Set the status of the agent class to:" << status << std::endl;
#endif

  this->status = status;
}

template <typename T>
int AgentClassParameters<T>::getStatus() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getStatus: \
    Get the status of the agent class" << std::endl;
    */
#endif

  return (int)status;
}

template <typename T>
void AgentClassParameters<T>::setNumAgents(int num_agents)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setNumAgents: \
    Set the number of agents in the agent class to:" 
      << num_agents << std::endl;
#endif

  this->num_agents = num_agents;
}

template <typename T>
int AgentClassParameters<T>::getNumAgents() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getNumAgents: \
    Get the number of agents in the agent class" << std::endl;
    */
#endif
  
  return num_agents;
}

template <typename T>
void AgentClassParameters<T>::setAudibility(T audibility)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setAudibility: \
    Set the audibility of the agent class to:" 
      << audibility << std::endl;
#endif

  this->audibility = audibility;
}

template <typename T>
T AgentClassParameters<T>::getAudibility() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getAudibility: \
    Get the audibility of the agent class" << std::endl;
    */
#endif
  
  return audibility;
}

template <typename T>
void AgentClassParameters<T>::setMuEnergy(T mu_energy)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setMuEnergy: \
    Set the mean energy of the agent class to:" 
      << mu_energy << std::endl;
#endif
  
  this->mu_energy = mu_energy;
}

template <typename T>
T AgentClassParameters<T>::getMuEnergy() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getMuEnergy: \
    Get the mean energy of the agent class" << std::endl;
    */
#endif

  return mu_energy;
}

template <typename T>
void AgentClassParameters<T>::setSigmaEnergy(T sigma_energy)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setSigmaEnergy: \
    Set the standard deviation of the energy of the agent class to:" 
      << sigma_energy << std::endl;
#endif
  
  this->sigma_energy = sigma_energy;
}

template <typename T>
T AgentClassParameters<T>::getSigmaEnergy() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getSigmaEnergy: \
    Get the standard deviation of the energy of the agent class" << std::endl;
    */
#endif
  
  return sigma_energy;
}

template <typename T>
void AgentClassParameters<T>::setEnergyUptakeRate(T energy_uptake_rate)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setEnergyUptakeRate: \
    Set the energy uptake rate of the agent class to:" 
      << energy_uptake_rate << std::endl;
#endif

  this->energy_uptake_rate = energy_uptake_rate;
}

template <typename T>
T AgentClassParameters<T>::getEnergyUptakeRate() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getEnergyUptakeRate: \
    Get the energy uptake rate of the agent class" << std::endl;
    */
#endif

  return energy_uptake_rate;
}

template <typename T>
void AgentClassParameters<T>::setMovespeed(T movespeed)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setMovespeed: \
    Set the maximal movespeed of the agent class to:" 
      << movespeed << std::endl;
#endif

  this->movespeed = movespeed;
}

template <typename T>
T AgentClassParameters<T>::getMovespeed() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getMovespeed: \
    Get the maximal movespeed of the agent class" << std::endl;
    */
#endif

  return movespeed;
}

template <typename T>
void AgentClassParameters<T>::setEnergyConsumptionPerTime(
    T energy_consumption_per_time)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setEnergyConsumptionPerTime: \
    Set the energy consumption per time of the agent class to:" 
      << energy_consumption_per_time << std::endl;
#endif

  this->energy_consumption_per_time = energy_consumption_per_time;
}

template <typename T>
T AgentClassParameters<T>::getEnergyConsumptionPerTime() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getEnergyConsumptionPerTime: \
    Get the energy consumption per time of the agent class" << std::endl;
    */
#endif

  return energy_consumption_per_time;
}

template <typename T>
void AgentClassParameters<T>::setEnergyConsumptionPerDistance(
    T energy_consumption_per_distance)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setEnergyConsumptionPerDistance: \
    Set the energy consumption per distance of the agent class to:" 
      << energy_consumption_per_distance << std::endl;
#endif

  this->energy_consumption_per_distance = energy_consumption_per_distance;
}

template <typename T>
T AgentClassParameters<T>::getEnergyConsumptionPerDistance() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getEnergyConsumptionPerDistance: \
    Get the energy consumption per distance of the agent class" << std::endl;
    */
#endif

  return energy_consumption_per_distance;
}

template <typename T>
void AgentClassParameters<T>::setSize(T size)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setSize: \
    Set the size of the agent class to:" 
      << size << std::endl;
#endif

  this->size = size;
}

template <typename T>
T AgentClassParameters<T>::getSize() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getSize: \
    Get the size of the agent class" << std::endl;
    */
#endif

  return size;
}

template <typename T>
void AgentClassParameters<T>::setStartPosition(Point3D<T> start_position)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setStartPosition: \
    Set the start position of the agent class to:" 
      << start_position.toString() << std::endl;
#endif

  this->start_position = start_position;
}

template <typename T>
Point3D<T> AgentClassParameters<T>::getStartPosition() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getStartPosition: \
    Get the start position of the agent class" << std::endl;
    */
#endif

  return start_position;
}

template <typename T>
void AgentClassParameters<T>::setRandomStartPositions(bool random_start_positions)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setRandomStartPositions: \
    Set the random start positions of the agent class to:" 
      << random_start_positions << std::endl;
#endif

  this->random_start_positions = random_start_positions;
}

template <typename T>
bool AgentClassParameters<T>::getRandomStartPositions() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getRandomStartPositions: \
    Get the random start positions of the agent class" << std::endl;
    */
#endif
  
  return random_start_positions;
}

template <typename T>
void AgentClassParameters<T>::setColor(Color<T> color)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setColor: \
    Set the color of the agent class to:" 
      << color.toString() << std::endl;
#endif
  
  this->color = color;
}

template <typename T>
Color<T> AgentClassParameters<T>::getColor() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getColor: \
    Get the color of the agent class" << std::endl;
    */
#endif
  
  return color;
}

template <typename T>
void AgentClassParameters<T>::setPointsize(T pointsize)
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::setPointsize: \
    Set the pointsize of the agent class to:" 
      << pointsize << std::endl;
#endif

  this->pointsize = pointsize;
}

template <typename T>
T AgentClassParameters<T>::getPointsize() const
{
#ifdef DEBUG
  /*
  std::cout << "AgentClassParameters::getPointsize: \
    Get the pointsize of the agent class" << std::endl;
    */
#endif

  return pointsize;
}

template <typename T>
void AgentClassParameters<T>::init()
{
#ifdef DEBUG
  std::cout << "AgentClassParameters::init: \
    Initialize the agent class parameters" << std::endl;
#endif
  
  // Initialize the energy distribution
  dist_energy = std::normal_distribution<T>(mu_energy, sigma_energy);

  // Set the initialized flag
  initialized = true;
}

// Explicit instantiation
template class AgentClassParameters<float>;
template class AgentClassParameters<double>;


/* vim: set ts=2 sw=2 tw=0 et :*/
