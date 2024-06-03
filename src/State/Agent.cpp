#include "Agent.h"

template <typename T>
Agent<T>::Agent(int id, 
    int verbosity, 
    SimulationParameters<T> &sim_params,
    std::vector<Agent<T>> &agents,
    std::vector<AgentClass<T>> &agent_classes,
    AgentMetrics<T> &agent_metrics,
    std::mt19937 &gen,
    std::uniform_real_distribution<T> &dist_uni,
    std::uniform_real_distribution<T> &dist_x,
    std::uniform_real_distribution<T> &dist_y,
    std::uniform_real_distribution<T> &dist_z)
  : id(id), sim_params(sim_params), 
  agents(agents), agent_classes(agent_classes), 
  agent_metrics(agent_metrics),
  verbosity(verbosity), gen(gen), 
  dist_uni(dist_uni), dist_x(dist_x), dist_y(dist_y), dist_z(dist_z),
  search_memory(id, gen, 
      dist_uni, dist_x, dist_y, dist_z,
      0.0, // mu_levy
      0.0, // c_levy
      0.0, // brownian_search_duration
      sim_params.getWorldsize()),
  transition_state(Status::ALIVE)
{
#ifdef DEBUG
  /*
  std::cout << "Agent::Agent: \
    Creating an agent with initialize constructor" << std::endl;
    */
#endif
}

template <typename T>
Agent<T>::~Agent()
{
#ifdef DEBUG
  /*
  std::cout << "Agent::~Agent: \
    Destroying an agent" << std::endl;
    */
#endif
}

template <typename T>
void Agent<T>::init(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::init: \
      Initializing the agent" << std::endl;
  }
#endif

  // Initialize the search memory
  initSearch(state.class_id[id], sim_params, gen);
}

template <typename T>
void Agent<T>::updatePosition(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updatePosition: \
      Updating the position of the agent" << std::endl;
  }
#endif

  // If the agent is dead, no movement
  if (state.status[id] == (int)Status::DEAD){
    stay(state, new_state);
    return;
  }

  // If the agent is attacked, no movement
  if (state.status[id] == (int)Status::ATTACKED){
#ifdef DEBUG
  if (verbosity > 2){
    std::cout << "Agent::updatePosition: \
      Agent " << id << " is attacked" << std::endl;
  }
#endif
    // Make sure the agent is still under attack
    if (enemyAudible(state)){
      stay(state, new_state);
#ifdef DEBUG
  if (verbosity > 2){
      std::cout << "Agent::updatePosition: \
        Agent " << id << " not moving due to attack" << std::endl;
  }
#endif
      return;
    } else {
      transition_state = Status::ALIVE;
    }
  }

  // Create move
  Point3D<T> move = createMove(state);

  // Apply boundary control
  Point3D<T> pos = state.positions[id] + move;
  boundaryControl(pos);

#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updatePosition: \
      Agent: " << id << " Move: " << move.toString() << std::endl;
    std::cout << "Agent::updatePosition: \
      Agent: " << id << " New position: " << pos.toString() << std::endl;
  }
#endif

  // Update the position of the agent
  new_state.positions[id] = pos;
}

template <typename T>
void Agent<T>::updateMovespeed(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updateMovespeed: \
      Updating the movement speed of the agent" << std::endl;
  }
#endif

  // Update the movement speed of the agent
  new_state.movespeed[id] = state.movespeed[id];
}

template <typename T>
void Agent<T>::updateSize(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updateSize: \
      Updating the size of the agent" << std::endl;
  }
#endif

  // Update the size of the agent
  new_state.sizes[id] = state.sizes[id];
}

template <typename T>
void Agent<T>::updateClassId(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updateClassId: \
      Updating the class id of the agent" << std::endl;
  }
#endif
  /* 
   * Currently the class id is not changed. If it is, make sure to 
   * rearrange the agents in memory to keep the agents of the same class
   * together and update the agent class parameters accordingly (num agents, 
   * indices, etc). Re-initialize the search memory if the class id changes.
   */

  // Update the class id of the agent
  //T old_class_id = state.class_id[id];
  new_state.class_id[id] = state.class_id[id];

  // Refresh search memory 
  /*
  if (old_class_id != state.class_id[id]){
    initSearch(state.class_id[id], sim_params, gen);
  }
  */
}

template <typename T>
void Agent<T>::updateStrategy(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updateStrategy: \
      Updating the strategy of the agent" << std::endl;
  }
#endif

  // Update the strategy of the agent
  new_state.strategy[id] = state.strategy[id];
}

template <typename T>
void Agent<T>::updateStatus(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updateStatus: \
      Updating the status of the agent" << std::endl;
  }
#endif

  // Respawn if the agent died in the transition
  if (transition_state == Status::DEAD){
    int class_id = state.class_id[id];
    respawn(state, new_state, class_id);
  } else if (transition_state == Status::ATTACKED){
    new_state.status[id] = (int)Status::ATTACKED;
  } else {
    new_state.status[id] = state.status[id];
    transition_state = (Status)state.status[id];
  }
}

template <typename T>
void Agent<T>::updateAudibilityThreshold(const State<T> &state, 
    State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updateAudibilityThreshold: \
      Updating the audibility threshold of the agent" << std::endl;
  }
#endif
  
  // Update the audibility of the agent
  new_state.audibility_threshold[id] = state.audibility_threshold[id];
}

template <typename T>
void Agent<T>::updateEnergyConsumptionRateTime(
    const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updateEnergyConsumptionRateTime: \
      Updating the energy consumption rate per time step of the agent" 
      << std::endl;
  }
#endif

  // Update the energy consumption rate per time step of the agent
  new_state.energy_consumption_per_time[id] = 
    state.energy_consumption_per_time[id];
}

template <typename T>
void Agent<T>::updateEnergyConsumptionRateDistance(
    const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updateEnergyConsumptionRateDistance: \
      Updating the energy consumption rate per distance of the agent" 
      << std::endl;
  }
#endif

  new_state.energy_consumption_per_distance[id] = 
    state.energy_consumption_per_distance[id];
}

template <typename T>
void Agent<T>::updateEnergyUptakeRate(
    const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updateEnergyUptakeRate: \
      Updating the energy uptake rate of the agent" << std::endl;
  }
#endif

  // Update the energy uptake rate of the agent
  new_state.energy_uptake_rate[id] = state.energy_uptake_rate[id];
}

template <typename T>
void Agent<T>::updateEnergy(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::updateEnergy: \
      Updating the energy of the agent" << std::endl;
  }
#endif

  // Calculate the energy consumed by time
  T dt = sim_params.getDt();
  T dE_t = dt * state.energy_consumption_per_time[id];

  // Calculate the energy consumed by distance
  // ATTENTION: This is uses the new position, thus make sure to call
  // updatePosition before updateEnergy
  T ds = state.positions[id].distance(new_state.positions[id]);
  T dE_s = ds * state.energy_consumption_per_distance[id];

  // Update the energy of the agent
  T new_energy = state.energy[id] - dE_t - dE_s;

  // Update the energy of the agent
  new_state.energy[id] = new_energy;

#ifdef DEBUG
  // Sum up delta energy
  T dE = dE_t + dE_s;

  // Keep track of dissipated energy
  agent_metrics.energy_removed_from_system[id] += dE;

  if (verbosity > 3){
    std::cout << "Agent::updateEnergy: \
      Agent " << id << " consumed " << dE << " energy" << std::endl;
    std::cout << "Agent::updateEnergy: \
      Agent " << id << " consumed " << dE_t << " energy by time" << std::endl;
    std::cout << "Agent::updateEnergy: \
      Agent " << id << " consumed " << dE_s << " energy by distance" 
      << std::endl;
    std::cout << "Agent::updateEnergy: \
      Agent " << id << " new energy: " << state.energy[id] - dE << std::endl;
  }
#endif

  if (new_energy < DEATH_THRESHOLD){
    // Agent died
    energy_exchange_register.clear();
    transition_state = Status::DEAD;
#ifdef DEBUG
    agent_metrics.energy_removed_from_system[id] += new_energy;
#endif
  } 

}

template <typename T>
void Agent<T>::energyExchangePartOneRegister(const State<T> &state, 
    State<T> &new_state)
{
  // Return if no food is reachable
  if ( !( foodReachable(state) || foodContact(state) ) ) {
    return;
  }

#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::energyExchangePartOneRegister: \
      Registering for energy exchange" << std::endl;
  }
#endif

  // Get vector of food source class ids
  std::vector<int> food_source_ids 
    = sim_params.getFoodSourceIds(state.class_id[id]);

  // Drops out after first successful registration, this way,
  // food sources are prioritized in the order they are given in
  // the parameters.

  // Loop over food classes
  for (int i = 0; i < food_source_ids.size(); i++){
    // Get the id of the closest food source
    if (numFoodContact(state) > 0){
      int food_source_id = findClosestContact(state, food_source_ids[i]);
      if (food_source_id > -1){
        // Register for energy exchange at the food source
        agents[food_source_id].registerForEnergyExchange(id, 
            state.energy_uptake_rate[id]);
        break;
      }
    } else {
      int food_source_id = findClosestReachable(state, food_source_ids[i]);
      if (food_source_id > -1){
        // Set the status of the food source to attacked, 
        // this will immobilize the food source
        agents[food_source_id].setTransitionState(Status::ATTACKED);
        break;
      }
    }
  }
}

template <typename T>
void Agent<T>::energyExchangePartTwoDistribute(const State<T> &state, 
    State<T> &new_state)
{
  // Return if the energy exchange register is empty
  if (energy_exchange_register.empty()){
    return;
  }

#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::energyExchangePartTwoDistribute: \
      Distributing energy" << std::endl;
  }
#endif

  // Calculate the sum of requested energy
  T sum_requested_energy = 0.0;
  for (int i = 0; i < energy_exchange_register.size(); i++){
    sum_requested_energy += energy_exchange_register[i].second;
  }

  // Make sure the sum of requested energy is positive
  if (nearlyEqual(sum_requested_energy, (T)0.0) 
      || sum_requested_energy < 0.0){
    return;
  }

  // Calculate the ratio of requested energy to available energy
  double ratio = 1.0;
  if (state.energy[id] < sum_requested_energy){
    ratio = state.energy[id] / sum_requested_energy;
#ifdef DEBUG
    if (verbosity > 3){
      std::cout << "Agent::energyExchangePartTwoDistribute: \
        Agent " << id << " has less energy than requested" << std::endl;
      std::cout << "Agent::energyExchangePartTwoDistribute: \
        Ratio: " << ratio << std::endl;
    }
#endif
  }

  // Distribute the energy
  for (int i = 0; i < energy_exchange_register.size(); i++){
    int other_id = energy_exchange_register[i].first;
    double amount = (double)energy_exchange_register[i].second;
    amount *= ratio;
#ifdef DEBUG
    if (verbosity > 3){
      std::cout << "Agent::energyExchangePartTwoDistribute: \
        Agent " << id << " giving " << amount << 
        " energy to agent " << other_id << std::endl;
    }
#endif
    giveEnergy(state, new_state, other_id, amount);
  }

  // Test if the agent died
  if (new_state.energy[id] < DEATH_THRESHOLD){
    transition_state = Status::DEAD;
#ifdef DEBUG
    agent_metrics.energy_removed_from_system[id] += new_state.energy[id];
#endif
  } 

  // Clear the energy exchange register
  energy_exchange_register.clear();
}

template <typename T>
void Agent<T>::registerForEnergyExchange(int &other_id, T &amount)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::registerForEnergyExchange: \
      Agent " << id << " registering exchange of " 
      << amount << " energy to agent " << other_id << std::endl;
  }
#endif
  energy_exchange_register.push_back(std::make_pair(other_id, amount));
}



template <typename T>
void Agent<T>::printId(std::ostream &os)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::printId: \
      Printing the id of the agent" << std::endl;
  }
#endif

  os << "Agent id: " << id << std::endl;
}

template <typename T>
void Agent<T>::printState(const State<T> &state, std::ostream &os)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::printState: \
      Printing the state of the agent" << std::endl;
  }
#endif

  os << "Agent id: " << id << std::endl;
  os << "Agent position: " << state.positions[id].toString() << std::endl;
  os << "Agent energy: " << state.energy[id] << std::endl;
  os << "Agent movespeed: " << state.movespeed[id] << std::endl;
  os << "Agent size: " << state.sizes[id] << std::endl;
  os << "Agent class id: " << state.class_id[id] << std::endl;
  os << "Agent strategy: " << state.strategy[id] << std::endl;
  os << "Agent status: " << state.status[id] << std::endl;
  os << "Agent audibility threshold: " << state.audibility_threshold[id] 
    << std::endl;
  os << "Agent energy consumption per time: " 
    << state.energy_consumption_per_time[id] << std::endl;
  os << "Agent energy consumption per distance: " 
    << state.energy_consumption_per_distance[id] << std::endl;
  os << "Agent energy uptake rate: " << state.energy_uptake_rate[id] 
    << std::endl;
}

template <typename T>
Point3D<T> Agent<T>::createMove(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::createMove: \
      Creating a move for the agent" << std::endl;
  }
#endif

  // Return if the movespeed is zero
  if (nearlyEqual(state.movespeed[id], (T)0.0)) {
    return (Point3D<T>(0.0, 0.0, 0.0));
  }

  switch (state.strategy[id])
  {
    case 0:
      return moveStrategy0(state);

    case 1:
      return moveStrategy1(state);

    case 2:
      return moveStrategy2(state);

    case 3:
      return moveStrategy3(state);

    case 4:
      return moveStrategy4(state);

    case 5:
      return moveStrategy5(state);

    case 6:
      return moveStrategy6(state);

    case 7:
      return moveStrategy7(state);

    case 8:
      return moveStrategy8(state);

    default:
      std::cerr << "Agent::createMove: \
        Move strategy " << state.strategy[id] << " not implemented" 
        << std::endl;
      return Point3D<T>(0.0, 0.0, 0.0);
  }
}

template <typename T>
Point3D<T> Agent<T>::moveStrategy0(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::moveStrategy0: \
      Moving agent " << id << " according to strategy 0" << std::endl;
  }
#endif

  // No movement
  return (Point3D<T>(0.0, 0.0, 0.0));
}

template <typename T>
Point3D<T> Agent<T>::moveStrategy1(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::moveStrategy1: \
      Moving agent " << id << " according to strategy 1" << std::endl;
  }
#endif

  /* No communication, only search and approach food */

  // If at food source, stay
  if (foodContact(state)){
    return (Point3D<T>(0.0, 0.0, 0.0));
  }

  // Approach food sources, if some are known
  if (foodAudible(state)){
    return approachClosestFoodSource(state);
  }

  // Search, if no other strategy applies
  return search(state);
  
}

template <typename T>
Point3D<T> Agent<T>::moveStrategy2(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::moveStrategy2: \
      Moving agent " << id << " according to strategy 2" << std::endl;
  }
#endif

  /* Communicate food sources, search and approach food */

  // If at food source, stay
  if (foodContact(state)){
#ifdef DEBUG
    std::cout << "Agent::moveStrategy2: \
      Agent " << id << " is at food source" << std::endl;
#endif
    return (Point3D<T>(0.0, 0.0, 0.0));
  }

  // Approach food sources, if some are known
  if (foodAudible(state)){
#ifdef DEBUG
    std::cout << "Agent::moveStrategy2: \
      Agent " << id << " is approaching food source" << std::endl;
#endif
    return approachClosestFoodSource(state);
  }

  // If no food sources are known, approach friends which hear food
  if (friendAudible(state)){
    Point3D<T> move;
    move = approachClosestFriendsHearingFood(state);
    if (!move.isZero()){
#ifdef DEBUG
    std::cout << "Agent::moveStrategy2: \
      Agent " << id << " is approaching friends" << std::endl;
#endif
      return move;
    }
  }

  // Search, if no other strategy applies
#ifdef DEBUG
  std::cout << "Agent::moveStrategy2: \
    Agent " << id << " is searching" << std::endl;
#endif
  return search(state);

}

template <typename T>
Point3D<T> Agent<T>::moveStrategy3(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::moveStrategy3: \
      Moving agent " << id << " according to strategy 3" << std::endl;
  }
#endif

  // Avoid enemies, communicate food sources, search and approach food

  // If attacked by enemy, no movement
  if (attacked(state)){
    return (Point3D<T>(0.0, 0.0, 0.0));
  } 
  
  // If enemy is audible, avoid
  if (enemyAudible(state)){
    return avoidClosestEnemies(state);
  }

  // If no enemies are audible, move according to strategy 2
  return moveStrategy2(state);

}

template <typename T>
Point3D<T> Agent<T>::moveStrategy4(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::moveStrategy4: \
      Moving agent " << id << " according to strategy 4" << std::endl;
  }
#endif

  // Not yet fully implemented, needs to compare old and new state to
  // avoid infinite loops

  // If at food source, stay
  if (foodContact(state)){
    return (Point3D<T>(0.0, 0.0, 0.0));
  }

  // Approach food sources, if some are known
  Point3D<T> move(0.0, 0.0, 0.0);
  if (foodAudible(state)){
    move = approachClosestFoodSource(state);
    return (move);
  }
  
  // If friends are audible
  if (friendAudible(state)){

    // Approach friends which hear food
    move = approachClosestFriendsHearingFood(state);

    // If no friends hear food
    if (move.isZero()){

      // If friends are reachable, search
      if (friendReachable(state)){
        move = search(state);

      } else {

      // If no friends are reachable, approach audible friends
        move = approachClosestFriends(state);
      }

    }

    return move;
  }

  // If no friends are audible, search
  move = search(state);
  return move;
  
}

template <typename T>
Point3D<T> Agent<T>::moveStrategy5(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::moveStrategy5: \
      Moving agent " << id << " according to strategy 5" << std::endl;
  }
#endif

  // Avoid enemies only 
  if (enemyAudible(state)){
    return avoidClosestEnemies(state);
  } else {
    return Point3D<T>(0.0, 0.0, 0.0);
  }
}

template <typename T>
Point3D<T> Agent<T>::moveStrategy6(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::moveStrategy6: \
      Moving agent " << id << " according to strategy 6" << std::endl;
  }
#endif

  // Avoid enemies, search food
  if (enemyAudible(state)){
    return avoidClosestEnemies(state);
  } else {
    return moveStrategy1(state);
  }
}

template <typename T>
Point3D<T> Agent<T>::moveStrategy7(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::moveStrategy7: \
      Moving agent " << id << " according to strategy 7" << std::endl;
  }
#endif

  // Move randomly
  return randomMove(state);
}

template <typename T>
Point3D<T> Agent<T>::moveStrategy8(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::moveStrategy8: \
      Moving agent " << id << " according to strategy 8" << std::endl;
  }
#endif

  // Move randomly, using search memory
  return search(state);
}

template <typename T>
Point3D<T> Agent<T>::approachClosestFoodSource(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::approachClosestFoodSource: \
      Approaching food sources" << std::endl;
  }
#endif
  // Get vector of food sources
  std::vector<int> food_source_ids 
    = sim_params.getFoodSourceIds(state.class_id[id]);


  // Approach the closest food source
  T scale = getSpeedFactorApproachFood(state);
  Point3D<T> move = approachClosest(state, food_source_ids, scale);
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::approachClosestFoodSource: \
      Agent: " << id << " Move: " << move.toString() << std::endl;
  }
#endif
  return move;
}

template <typename T>
Point3D<T> Agent<T>::approachClosestFriends(const State<T> &state)
{
  // Get vector of friends
  std::vector<int> friend_ids 
    = sim_params.getFriendIds(state.class_id[id]);

  // Approach friends
  T scale = getSpeedFactorApproachFriends(state);
  Point3D<T> move = approachClosest(state, friend_ids, scale);
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::approachClosestFriends: \
      Approaching friends" << std::endl;
    std::cout << "Agent::approachClosestFriends: \
      Agent: " << id << " Move: " << move.toString() << std::endl;
  }
#endif
  return move;
}

template <typename T>
Point3D<T> Agent<T>::approachClosestFriendsHearingFood(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::approachClosestFriendsHearingFood: \
      Approaching friends which hear food" << std::endl;
  }
#endif

  // Get vector of friends
  std::vector<int> friend_ids 
    = sim_params.getFriendIds(state.class_id[id]);

  // Get vector of food sources
  std::vector<int> food_source_ids 
    = sim_params.getFoodSourceIds(state.class_id[id]);

  // Approach friends
  T scale = getSpeedFactorApproachFriends(state);
  Point3D<T> move = approachClosest(state, friend_ids, food_source_ids, scale);
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::approachClosestFriendsHearingFood: \
      Agent: " << id << " Move: " << move.toString() << std::endl;
  }
#endif
  return move;
}

template <typename T>
Point3D<T> Agent<T>::avoidClosestEnemies(const State<T> &state)
{
  // Get vector of enemies
  std::vector<int> enemy_ids 
    = sim_params.getEnemyIds(state.class_id[id]);

  // Avoid enemies
  T scale = getSpeedFactorAvoidEnemies(state);
  Point3D<T> move = avoidClosest(state, enemy_ids, scale);
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::avoidClosestEnemies: \
      Avoiding enemies" << std::endl;
    std::cout << "Agent::avoidClosestEnemies: \
      Agent: " << id << " Move: " << move.toString() << std::endl;
  }
#endif
  return move;
}

template <typename T>
Point3D<T> Agent<T>::search(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::search: \
      Searching" << std::endl;
  }
#endif
  
  // Continue search
  search_memory.determineSearchType(state);
  Point3D<T> move = search_memory.getDirection(state);
  move *= state.movespeed[id] * getSpeedFactorSearch(state);
  return move;
}

template <typename T>
Point3D<T> Agent<T>::stay()
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::stay: \
      Staying" << std::endl;
  }
#endif

  return Point3D<T>(0.0, 0.0, 0.0);
}

template <typename T>
void Agent<T>::stay(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::stay: \
      Staying" << std::endl;
  }
#endif

  new_state.positions[id] = state.positions[id];
}

template <typename T>
Point3D<T> Agent<T>::randomMove(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::randomMove: \
      Moving randomly" << std::endl;
  }
#endif

  // Creates a random move with equal probability in all directions
  // of random length between 0 and the movespeed of the agent
  T x = dist_x(gen);
  T y = dist_y(gen);
  T z = dist_z(gen);

  Point3D<T> move(x, y, z);
  move.normalize();
  move *= state.movespeed[id] * dist_uni(gen);
  return move;
}


template<typename T>
T Agent<T>::getSpeedFactorApproachFriends(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::getSpeedFactorApproachFriends: \
      getting scale factor for approaching friends" << std::endl;
  }
#endif

  return sim_params.agent_class_params[state.class_id[id]].
                                  getMovespeedApproachFriends();
}

template<typename T>
T Agent<T>::getSpeedFactorApproachFood(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "agent::getSpeedFactorApproachFood: \
      getting scale factor for approaching food" << std::endl;
  }
#endif

  return sim_params.agent_class_params[state.class_id[id]].
                                  getMovespeedApproachFood();
}

template<typename T>
T Agent<T>::getSpeedFactorAvoidEnemies(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "agent::getSpeedFactorAvoidEnemies: \
      getting scale factor for avoiding enemies" << std::endl;
  }
#endif

  return sim_params.agent_class_params[state.class_id[id]].
                                  getMovespeedAvoidEnemies();
}

template<typename T>
T Agent<T>::getSpeedFactorSearch(const State<T> &state)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::getSpeedFactorSearch: \
      getting scale factor for searching" << std::endl;
  }
#endif

  return sim_params.agent_class_params[state.class_id[id]].
                                  getMovespeedSearch();
}

template <typename T>
int Agent<T>::getNumContact(const State<T> &state, 
    const std::vector<int> &class_ids) const
{
  int sum = 0;
  // Sum up the number of contacts of all given classes
  for (int i=0; i<class_ids.size(); i++){
    int index = class_ids[i];
    sum += agent_classes.at(index).getNumContact(state, id);
  }
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::getNumContact: \
      found " << sum << " contacts" << std::endl;
  }
#endif
  return sum;
}

template <typename T>
int Agent<T>::getNumReachable(const State<T> &state,
    const std::vector<int> &class_ids) const
{
  int sum = 0;
  // Sum up the number of reachable agents of all given classes
  for (int i=0; i<class_ids.size(); i++){
    int index = class_ids[i];
    sum += agent_classes.at(index).getNumReachable(state, id);
  }
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::getNumReachable: \
      found " << sum << " reachable agents" << std::endl;
  }
#endif
  return sum;
}

template <typename T>
int Agent<T>::getNumAudible(const State<T> &state, 
    const std::vector<int> &class_ids) const
{
  int sum = 0;
  // Sum up the number of audible agents of all given classes
  for (int i=0; i<class_ids.size(); i++){
    int index = class_ids[i];
    sum += agent_classes.at(index).getNumAudible(state, id);
  }
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::getNumAudible: \
      found " << sum << " audible agents" << std::endl;
  }
#endif
  return sum;
}

template <typename T>
bool Agent<T>::foodContact(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::foodContact: \
      Checking if the agent is in contact with food" << std::endl;
  }
#endif

  // Check if the agent is in contact with food
  return numFoodContact(state) > 0;
}

template <typename T>
bool Agent<T>::friendContact(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::friendContact: \
      Checking if the agent is in contact with friends" << std::endl;
  }
#endif
  // Check if the agent is in contact with friends
  return numFriendContact(state) > 0;
}

template <typename T>
bool Agent<T>::enemyContact(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::enemyContact: \
      Checking if the agent is in contact with enemies" << std::endl;
  }
#endif
  // Check if the agent is in contact with enemies
  return numEnemyContact(state) > 0;
}

template <typename T>
bool Agent<T>::foodReachable(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::foodReachable: \
      Checking if the agent is in reach of food" << std::endl;
  }
#endif
  // Check if the agent is in reach of food
  return numFoodReachable(state) > 0;
}

template <typename T>
bool Agent<T>::friendReachable(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::friendReachable: \
      Checking if the agent is in reach of friends" << std::endl;
  }
#endif

  // Check if the agent is in reach of friends
  return numFriendReachable(state) > 0;
}

template <typename T>
bool Agent<T>::enemyReachable(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::enemyReachable: \
      Checking if the agent is in reach of enemies" << std::endl;
  }
#endif

  // Check if the agent is in reach of enemies
  return numEnemyReachable(state) > 0;
}

template <typename T>
bool Agent<T>::foodAudible(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::foodAudible: \
      Checking if the agent can hear food" << std::endl;
  }
#endif
  // Check if the agent can hear food
  return numFoodAudible(state) > 0;
}

template <typename T>
bool Agent<T>::friendAudible(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::friendAudible: \
      Checking if the agent can hear friends" << std::endl;
  }
#endif

  // Check if the agent can hear friends
  return numFriendAudible(state) > 0;
}

template <typename T>
bool Agent<T>::enemyAudible(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::enemyAudible: \
      Checking if the agent can hear enemies" << std::endl;
  }
#endif

  // Check if the agent can hear enemies
  return numEnemyAudible(state) > 0;
}

template <typename T>
bool Agent<T>::attacked(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::attacked: \
      Checking if the agent is attacked" << std::endl;
  }
#endif

  // Check if the agent is attacked
  return state.status[id] == (int)Status::ATTACKED;
}

template <typename T>
int Agent<T>::numFoodContact(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::numFoodContact: \
      Counting the number of food sources in contact" << std::endl;
  }
#endif

  // Get vector of food sources
  std::vector<int> food_source_ids 
    = sim_params.getFoodSourceIds(state.class_id[id]);

  // Count the number of food sources in contact
  return getNumContact(state, food_source_ids);
}

template <typename T>
int Agent<T>::numFriendContact(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::numFriendContact: \
      Counting the number of friends in contact" << std::endl;
  }
#endif

  // Get vector of friends
  std::vector<int> friend_ids 
    = sim_params.getFriendIds(state.class_id[id]);

  // Count the number of friends in contact
  return getNumContact(state, friend_ids);
}

template <typename T>
int Agent<T>::numEnemyContact(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::numEnemyContact: \
      Counting the number of enemies in contact" << std::endl;
  }
#endif

  // Get vector of enemies
  std::vector<int> enemy_ids 
    = sim_params.getEnemyIds(state.class_id[id]);

  // Count the number of enemies in contact
  return getNumContact(state, enemy_ids);
}

template <typename T>
int Agent<T>::numFoodReachable(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::numFoodReachable: \
      Counting the number of food sources in reach" << std::endl;
  }
#endif

  // Get vector of food sources
  std::vector<int> food_source_ids 
    = sim_params.getFoodSourceIds(state.class_id[id]);

  // Count the number of food sources in reach
  return getNumReachable(state, food_source_ids);
}

template <typename T>
int Agent<T>::numFriendReachable(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::numFriendReachable: \
      Counting the number of friends in reach" << std::endl;
  }
#endif

  // Get vector of friends
  std::vector<int> friend_ids 
    = sim_params.getFriendIds(state.class_id[id]);

  // Count the number of friends in reach
  return getNumReachable(state, friend_ids);
}

template <typename T>
int Agent<T>::numEnemyReachable(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::numEnemyReachable: \
      Counting the number of enemies in reach" << std::endl;
  }
#endif

  // Get vector of enemies
  std::vector<int> enemy_ids 
    = sim_params.getEnemyIds(state.class_id[id]);

  // Count the number of enemies in reach
  return getNumReachable(state, enemy_ids);
}

template <typename T>
int Agent<T>::numFoodAudible(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::numFoodAudible: \
      Counting the number of food sources audible" << std::endl;
  }
#endif

  // Get vector of food sources
  std::vector<int> food_source_ids 
    = sim_params.getFoodSourceIds(state.class_id[id]);

  // Count the number of food sources audible
  return getNumAudible(state, food_source_ids);
}

template <typename T>
int Agent<T>::numFriendAudible(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::numFriendAudible: \
      Counting the number of friends audible" << std::endl;
  }
#endif

  // Get vector of friends
  std::vector<int> friend_ids 
    = sim_params.getFriendIds(state.class_id[id]);

  // Count the number of friends audible
  return getNumAudible(state, friend_ids);
}

template <typename T>
int Agent<T>::numEnemyAudible(const State<T> &state) const
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::numEnemyAudible: \
      Counting the number of enemies audible" << std::endl;
  }
#endif

  // Get vector of enemies
  std::vector<int> enemy_ids 
    = sim_params.getEnemyIds(state.class_id[id]);

  // Count the number of enemies audible
  return getNumAudible(state, enemy_ids);
}

template <typename T>
int Agent<T>::findClosestContact(const State<T> &state, int &class_id)
{

  // Find the closest agent of the given class in contact
  if (agent_classes.at(class_id).getNumContact(state, id) > 0){
    int target_id = agent_classes[class_id].findMin(
        &state.distances_agent_to_agent[id * sim_params.num_agents_total], 
        &state.contact[id * sim_params.num_agents_total]
        );
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::findClosestContact: \
      Finding the closest target of class " << class_id << 
      " in contact with agent "<< id << std::endl;
    std::cout << "Agent::findClosestContact: \
      Target id: " << target_id << std::endl;
  }
#endif
    return target_id;
  }

  // Return -1 if no agent is in contact
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::findClosestContact: Agent: " << id <<
      " No target found" << std::endl;
  }
#endif
  return (-1);
}

template <typename T>
int Agent<T>::findClosestReachable(const State<T> &state, int &class_id)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::findClosestReachable: \
      Finding the closest target of class " << class_id << 
      " reachable by agent "<< id << std::endl;
  }
#endif

  // Find the closest agent of the given class in reach
  if (agent_classes.at(class_id).getNumReachable(state, id) > 0){
    return agent_classes[class_id].findMin(
        &state.distances_agent_to_agent[id * sim_params.num_agents_total], 
        &state.reachability[id * sim_params.num_agents_total]
        );
  }

  // Return -1 if no agent was found
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::findClosestReachable: Agent: " << id <<
      " No target found" << std::endl;
  }
#endif
  return (-1);
}

template <typename T>
int Agent<T>::findClosestAudible(const State<T> &state, int &class_id)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::findClosestAudible: \
      Finding the closest target of class " << class_id << 
      " audible by agent "<< id << std::endl;
  }
#endif

  // Find the closest agent of the given class in audible range
  if (agent_classes.at(class_id).getNumAudible(state, id) > 0){
    return agent_classes[class_id].findMin(
        &state.distances_agent_to_agent[id * sim_params.num_agents_total], 
        &state.audibility[id * sim_params.num_agents_total]
        );
  }

  // Return -1 if no agent was found
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::findClosestAudible: Agent: " << id <<
      " No target found" << std::endl;
  }
#endif
  return (-1);
}

template <typename T>
int Agent<T>::findClosestAware(const State<T> &state, int class_id)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::findClosestAware: \
      Finding the closest target of class " << class_id << 
      " aware by agent "<< id << std::endl;
  }
#endif
  int target_id = -1;

  // Try to find the closest agent in reach
  target_id = findClosestReachable(state, class_id);
  if (target_id > -1){
    return target_id;
  }

  // Try to find the closest agent in audible range
  target_id = findClosestAudible(state, class_id);
  if (target_id > -1){
    return target_id;
  }

  // Return -1 if no agent was found
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::findClosestAware: Agent: " << id <<
      " No target found" << std::endl;
  }
#endif
  return (-1);
}

template <typename T>
int Agent<T>::findClosest(const State<T> &state, 
    const std::vector<int> &target_classes)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::findClosest: \
      Finding the closest target" << std::endl;
  }
#endif

  std::vector<int> target_ids;
  return findClosest(state, target_classes, target_ids);

}

template <typename T>
int Agent<T>::findClosestContact(const State<T> &state, int &class_id,
    const std::vector<int> &audible_classes)
{

  // Find the closest agent of the given class in contact
  if (agent_classes.at(class_id).getNumContact(state, id) > 0){
    int target_id = agent_classes[class_id].findMin(
        &state.distances_agent_to_agent[id * sim_params.num_agents_total], 
        &state.contact[id * sim_params.num_agents_total],
        &state.audibility_agent_to_class[0], audible_classes
        );
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::findClosestContact: \
      (2masks) Finding the closest target of class " << class_id << 
      " in contact with agent "<< id << std::endl;
    std::cout << "Agent::findClosestContact: \
      Target id: " << target_id << std::endl;
  }
#endif
    return target_id;
  }

  // Return -1 if no agent is in contact
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::findClosestContact: Agent: " << id <<
      " No target found" << std::endl;
  }
#endif
  return (-1);
}

template <typename T>
int Agent<T>::findClosestReachable(const State<T> &state, int &class_id, 
    const std::vector<int> &audible_classes)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::findClosestReachable: \
      (audibility-metamask) Finding the closest target of class " 
      << class_id << " reachable by agent "<< id << std::endl;
  }
#endif

  // Find the closest agent of the given class in reach
  if (agent_classes.at(class_id).getNumReachable(state, id) > 0){
    return agent_classes[class_id].findMin(
        &state.distances_agent_to_agent[id * sim_params.num_agents_total], 
        &state.reachability[id * sim_params.num_agents_total],
        &state.audibility_agent_to_class[0], audible_classes);
  }

  // Return -1 if no agent was found
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::findClosestReachable: Agent: " << id <<
      " No target found" << std::endl;
  }
#endif
  return (-1);
}

template <typename T>
int Agent<T>::findClosestAudible(const State<T> &state, int &class_id,
    const std::vector<int> &audible_classes)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::findClosestAudible: \
      (2masks) Finding the closest target of class " << class_id << 
      " audible by agent "<< id << std::endl;
  }
#endif

  // Find the closest agent of the given class in audible range
  if (agent_classes.at(class_id).getNumAudible(state, id) > 0){
    return agent_classes[class_id].findMin(
        &state.distances_agent_to_agent[id * sim_params.num_agents_total], 
        &state.audibility[id * sim_params.num_agents_total],
        &state.audibility_agent_to_class[0], audible_classes
        );
  }

  // Return -1 if no agent was found
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::findClosestAudible: Agent: " << id <<
      " No target found" << std::endl;
  }
#endif
  return (-1);
}

template <typename T>
int Agent<T>::findClosestAware(const State<T> &state, int class_id,
    const std::vector<int> &audible_classes)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::findClosestAware: \
      (audibility-metamask) Finding the closest target of class " 
      << class_id << 
      " aware by agent "<< id << std::endl;
  }
#endif
  int target_id = -1;

  // Try to find the closest agent in reach
  target_id = findClosestReachable(state, class_id, audible_classes);
  if (target_id > -1){
    return target_id;
  }

  // Try to find the closest agent in audible range
  target_id = findClosestAudible(state, class_id, audible_classes);
  if (target_id > -1){
    return target_id;
  }

  // Return -1 if no agent was found
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::findClosestAware (2masks): Agent: " << id <<
      " No target found" << std::endl;
  }
#endif
  return (-1);
}

template <typename T>
int Agent<T>::findClosest(const State<T> &state, 
    const std::vector<int> &target_classes, 
    const std::vector<int> &audible_classes,
    bool two_masks)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::findClosest: \
      (2masks) Finding the closest target of the given classes, \
      that hears at least one of the given audible classes" << std::endl;
  }

  if (verbosity > 4){
    // Print target classes
    std::cout << "Agent::findClosest: \
      Target classes: ";
    for (int i = 0; i < target_classes.size(); i++){
      std::cout << target_classes[i] << " ";
    }
    std::cout << std::endl;
    // Print audible classes
    std::cout << "Agent::findClosest: \
      Audible classes: ";
    for (int i = 0; i < audible_classes.size(); i++){
      std::cout << audible_classes[i] << " ";
    }
    std::cout << std::endl;
  }
#endif

  // Set up a minimizer
  int target_id_class[target_classes.size()];
  for (int i = 0; i < target_classes.size(); i++){
    target_id_class[i] = -1;
  }

  // Find the closest target of each given class
  for (int i = 0; i < target_classes.size(); i++){
    if (!two_masks){
      target_id_class[i] = findClosestAware(state, target_classes[i]);
    } else {
      target_id_class[i] = 
        findClosestAware(state, target_classes[i], audible_classes);
    }
  }

  // Find the overall closest target
  int target_id = -1;
  for (int i = 0; i < target_classes.size(); i++){

    // Test if an agent of the target class was found
    if (target_id_class[i] < 0){
      continue;
    }

    // Set the target id if not set
    if (target_id < 0){
      target_id = target_id_class[i];
    }

    // Compare the distance of the target and the current food source
    int index_current = 
      id * sim_params.num_agents_total + target_id_class[i];
    int index_target = 
      id * sim_params.num_agents_total + target_id;
    if (state.distances_agent_to_agent[index_current]
        <
        state.distances_agent_to_agent[index_target]){
      target_id = target_id_class[i];
    }
  }

  // Return the target id
  return target_id;
}

template <typename T>
Point3D<T> Agent<T>::approachClosest(const State<T> &state, 
    const std::vector<int> &target_classes, T scale)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::approachClosest: \
      Approaching the closest target of the given classes" << std::endl;
  }
#endif

  // Find the closest target
  int target_id = findClosest(state, target_classes);

  // Test if a target was found
  if (target_id < 0){
#ifdef DEBUG
  if (verbosity > 2){
    std::cout << "Agent::approachClosest: \
      No target found" << std::endl;
  }
#endif
    return (Point3D<T>(0.0, 0.0, 0.0));
  }

  // Calculate the direction to the target
  Point3D<T> direction = 
    state.positions[id].normalizedDirection(state.positions[target_id]);

  // Get the distance to the target
  T distance = 
    state.positions[id].distance(state.positions[target_id]);

  // Make sure the agent does not overshoot the target
  T max_move_distance = state.movespeed[id] * scale;
  if (distance < max_move_distance){
    direction *= distance;
  } else {
    direction *= max_move_distance;
  }

  // Simulation parameters
  return (direction);
}

template <typename T>
Point3D<T> Agent<T>::approachClosest(const State<T> &state, 
    const std::vector<int> &target_classes, 
    const std::vector<int> &audible_classes, T scale)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::approachClosest: \
      (2masks)Approaching the closest target of the given classes" 
      << std::endl;
  }
#endif

  // Find the closest target
  int target_id = findClosest(state, target_classes, audible_classes, true);

  // Test if a target was found
  if (target_id < 0){
#ifdef DEBUG
  if (verbosity > 2){
    std::cout << "Agent::approachClosest: \
      No target found" << std::endl;
  }
#endif
    return (Point3D<T>(0.0, 0.0, 0.0));
  }

  // Calculate the direction to the target
  Point3D<T> direction = 
    state.positions[id].normalizedDirection(state.positions[target_id]);

  // Get the distance to the target
  T distance = 
    state.positions[id].distance(state.positions[target_id]);

  // Make sure the agent does not overshoot the target
  T max_move_distance = state.movespeed[id] * scale;
  if (distance < max_move_distance){
    direction *= distance;
  } else {
    direction *= max_move_distance;
  }

  // Simulation parameters
  return (direction);
}

template <typename T>
Point3D<T> Agent<T>::avoidClosest(const State<T> &state, 
    const std::vector<int> &target_classes, T scale)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::avoidClosest: \
      Avoiding the closest target of the given classes" << std::endl;
  }
#endif

  // Find the closest target
  int target_id = findClosest(state, target_classes);

#ifdef DEBUG
  if (verbosity > 2){
    std::cout << "Agent::avoidClosest: \
      Agent: " << id << " Target id: " << target_id << std::endl;
  }
#endif

  // Test if a target was found
  if (target_id < 0){
#ifdef DEBUG
    std::cout << "Agent::avoidClosest: \
      No target found" << std::endl;
#endif
    return (Point3D<T>(0.0, 0.0, 0.0));
  }

  // Calculate the direction to the target
  Point3D<T> direction = 
    state.positions[id].normalizedDirection(state.positions[target_id]);

  // Invert the direction
  direction *= -1.0;

  // Scale the direction to the movespeed
  direction *= state.movespeed[id] * scale;

  return (direction);
}

template <typename T>
void Agent<T>::boundaryControl(Point3D<T> &pos)
{
#ifdef DEBUG
  if (verbosity > 1){
    std::cout << "Agent::boundaryControl: \
      Keeping agent within boundaries" << std::endl;
  }
#endif

  // Test if the agent is within the boundaries
  if (sim_params.isInsideWorld(pos)){
    return;
  }

  // Restrict the agent to the boundaries
  const T* worldsize = sim_params.getWorldsize();
  for (int i = 0; i < 3; i++){
    if (pos[i] > worldsize[i]){
      pos[i] = worldsize[i] - (pos[i] - worldsize[i]);
    }
    if (pos[i] < -worldsize[i]){
      pos[i] = -worldsize[i] - (pos[i] + worldsize[i]);
    }
  }
}

template <typename T>
void Agent<T>::initSearch(
    int class_id,
    SimulationParameters<T> &sim_params,
    std::mt19937 &gen)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::initSearch: \
      Initializing the search object" << std::endl;
  }
#endif
  T mu_levy = sim_params.agent_class_params[class_id].getMuLevy();
  T c_levy = sim_params.agent_class_params[class_id].getCLevy();
  T brownian_search_duration = 
    sim_params.agent_class_params[class_id].getBrownianSearchDuration();
  T worldsize[3]; 
  sim_params.getWorldsize(worldsize);

  // Initialize the search object
  search_memory = Search<T>(
      id, gen, 
      dist_uni, dist_x, dist_y, dist_z,
      mu_levy, c_levy, brownian_search_duration,
      worldsize);
}

template <typename T>
void Agent<T>::giveEnergy(const State<T> &state, State<T> &new_state, int &other_id, 
    double amount)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::giveEnergy: \
      Agent " << id << " giving " << amount << " energy to agent "
      << other_id << std::endl;
  }
#endif

  if (nearlyEqual(amount, 0.0)){
    handleError("Agent::giveEnergy: \
      Negative energy transfer is not allowed", CRITICAL);
    return;
  }

  // Update the energy of the agent
  double new_energy = (double) new_state.energy[id] - amount;
  new_state.energy[id] = new_energy;

  // Update the energy of the other agent
  double new_energy_other = 
    (double) new_state.energy[other_id] + amount;
  new_state.energy[other_id] = new_energy_other;

#ifdef DEBUG
  // Keep track of the energy exchanged
#pragma omp atomic
  agent_metrics.energy_turnover_soll[id] += amount;
#pragma omp atomic
  agent_metrics.energy_turnover_haben[other_id] += amount;
#endif

}

template <typename T>
void Agent<T>::respawn(const State<T> &state, State<T> &new_state, 
    const int &class_id)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::respawn: \
      Respawning the agent" << std::endl;
  }
#endif

  // Set the class id of the new agent
  new_state.class_id[id] = class_id;

  // Get the class parameters of the new agent
  AgentClassParameters<T> &p = 
    sim_params.agent_class_params[class_id];


  /* Init the new agent */

  // Set the new position
  // Set to random, else predators will wait at the roost if they find it
  new_state.positions[id] = Point3D<T>(
      dist_x(gen), dist_y(gen), dist_z(gen));
  /*
  if (p.random_start_positions){
    new_state.positions[id] = Point3D<T>(
        dist_x(gen), dist_y(gen), dist_z(gen));
  } else {
    new_state.positions[id] = p.start_position;
  }
  */
  
  // Set the new energy
  new_state.energy[id] = p.dist_energy(gen);
  if (new_state.energy[id] < 0.0){
    new_state.energy[id] = 0.0; // Totgeburt
  }

#ifdef DEBUG
  // Keep track of the energy added to the system
  agent_metrics.energy_added_to_system[id] += new_state.energy[id];
#endif

  // Clear the energy exchange register
  energy_exchange_register.clear();

  // Set the new movespeed
  new_state.movespeed[id] = p.movespeed;

  // Set the new size
  new_state.sizes[id] = p.size;

  // Set the new strategy
  new_state.strategy[id] = p.strategy;

  // Set the new status
  new_state.status[id] = p.status;

  // Set the new audibility threshold
  new_state.audibility_threshold[id] = p.audibility;

  // Set the new energy consumption per time
  new_state.energy_consumption_per_time[id] = p.energy_consumption_per_time;

  // Set the new energy consumption per distance
  new_state.energy_consumption_per_distance[id] = 
    p.energy_consumption_per_distance;

  // Set the new energy uptake rate
  new_state.energy_uptake_rate[id] = p.energy_uptake_rate;

  // Refresh search memory
  initSearch(state.class_id[id], sim_params, gen);

  // Set the transition state to alive
  transition_state = Status::ALIVE;

}

template <typename T>
void Agent<T>::setTransitionState(Status status)
{
#ifdef DEBUG
  if (verbosity > 3){
    std::cout << "Agent::setStatus: \
      Setting the transition state to ";
    switch (status){
      case Status::ALIVE:
        std::cout << "ALIVE" << std::endl;
        break;
      case Status::DEAD:
        std::cout << "DEAD" << std::endl;
        break;
      case Status::ATTACKED:
        std::cout << "ATTACKED" << std::endl;
        break;
      default:
        std::cout << "UNKNOWN" << std::endl;
        break;
    }
  }
#endif
  transition_state = status;
}


// Explicit instantiations
template class Agent<float>;
template class Agent<double>;
/* vim: set ts=2 sw=2 tw=0 et :*/
