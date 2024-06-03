// Code style according to google style guide:
// https://google.github.io/styleguide/cppguide.html,
// using snake_case for variables and camelCase for functions.
#include "StateMachine.h"

template <typename T>
StateMachine<T>::StateMachine(std::random_device &rd, 
    GlobalParameters<T> &p_global)
  :rd(rd), new_state(), p_global(p_global),
  restore_file(".state_backup.toml"), initialized(false), 
  log_flags{0}, gen(rd())
{
#ifdef DEBUG
  std::cout << "StateMachine::StateMachine: \
    Constructor of StateMachine" << std::endl;
#endif

  for (int i = 0; i < (int)LogFlags::NUM_LOG_FLAGS; i++){
    log_flags[i] = false;
  }

}

template <typename T>
StateMachine<T>::~StateMachine()
{
#ifdef DEBUG
  std::cout << "StateMachine::~StateMachine: \
    Destructor of StateMachine" << std::endl;
#endif

  // Save the state of the simulation
  if (states.size() > 0){
    if (states[0].getInitialized()){
      saveStateToml(states[0], restore_file);
    }
  }

  // Clear the states
  states.clear();

  // Clear the agents
  agents.clear();

  // Clear the agent classes
  agent_classes.clear();
}

template <typename T>
void StateMachine<T>::init(SimulationParameters<T> &p)
{
#ifdef DEBUG
  std::cout << "StateMachine::init: \
    Initializing the simulation" << std::endl;
#endif

  // Return if the simulation parameters are not initialized
  if (!p.simulation_parameters_initialized){
    handleError("StateMachine::init: Simulation parameters uninitialized", 
        WARNING);
    return;
  }

  // Return if the number of agents is zero
  if (p.num_agents_total == 0){
    handleError("StateMachine::init: Number of agents is zero", WARNING);
    return;
  }

  // Set the parameters of the simulation
  setParameters(p);

  // Initialize the parameters of the simulation
  try{
    initParameters();
  }catch(const std::exception &e){
    handleError(e.what(), WARNING);
    return;
  }

  // Initialize the random number generator
  initRandom();

  // Initialize the states
  initStates();

  // Initialize the state of the simulation
  initState(new_state);

  // Initialize the agents, must be called after the state is initialized
  initAgents(new_state);

  // Initialize the agent classes
  initAgentClasses();

  // Initialize the agent metrics
  initAgentMetrics();

  // Update the meta data of the simulation
  updateMeta(new_state);

  // Set the initialized flag
  new_state.setInitialized(true);

#ifdef DEBUG
  // Initialize the system metrics
  system_metrics.initialize(new_state);
#endif

  // Swap the state of the simulation, push the new state to the memory
  swapStates();

  // Print the state of the simulation
  if (p_global.getVerbosity() > 1){
    std::cout << "Unscaled simulation parameters:" << std::endl;
    printParameters();
    std::cout << std::endl;
  } 
    
  if (p_global.getVerbosity() > 2){
    std::cout << "Scaled simulation parameters:" << std::endl;
    printScaledParameters();
    std::cout << std::endl;
  }

  if (p_global.getVerbosity() > 3){
    std::cout << "Initial state of the simulation:" << std::endl;
    printState(states[0]);
    std::cout << std::endl;
  }

  // Increment the run counter
  p_global.incrementRunCounter();

  // Initialize the output directory
  p_global.initRunDirectory();

  // Save the initial state
  std::string filename = p_global.getRunDirectory() + "/initial_state.toml";
  saveStateToml(states[0], filename);
  
  //Initialize the log flags
  initLogFlags();

  // Print the agent class summary
  if (p_global.getVerbosity() > 0){
    std::cout << "Initial agent class summary:" << std::endl;
    printAgentClassSummaries();
  }

  initialized = true;
}

template <typename T>
void StateMachine<T>::reset()
{
#ifdef DEBUG
  std::cout << "StateMachine::reset: \
    Resetting the simulation" << std::endl;
#endif
  /* Careful of the order of the following functions! 
   * Do not change the order! */

  // Reset the random number generator
  initRandom();

  // Reset the iteration counter
  new_state.setIteration(0);

  // Reset the state of the simulation
  initState(new_state);

  // Initialize the agents, must be called after the state is initialized
  initAgents(new_state);

  // Initialize the agent metrics
  initAgentMetrics();

  // Update the meta data of the simulation
  updateMeta(new_state);

  // Set the initialized flag
  new_state.setInitialized(true);

#ifdef DEBUG
  // Initialize the system metrics
  system_metrics.initialize(new_state);
#endif

  // Push the new state to the memory
  swapStates();

  // Increment the run counter
  p_global.incrementRunCounter();
}

template <typename T>
void StateMachine<T>::finalize(std::string dirname)
{
#ifdef DEBUG
  std::cout << "StateMachine::finalize: \
    Finalizing the simulation" << std::endl;
#endif

#ifdef DEBUG
  // Finalize the system metrics
  if (states.size() > 0){
    system_metrics.finalize(states[0]);
  }
  if (p_global.getVerbosity() > 0){
    system_metrics.print();
  }
#endif

  // Save the final state for later resume
  std::string filename = dirname + "/final_state.toml";
  if (states.size() > 0){
    saveStateToml(states[0], filename);
  }

  // Print the agent class summary
  if (p_global.getVerbosity() > 0){
    std::cout << "Final agent class summary:" << std::endl;
    printAgentClassSummaries();
  }
}


template <typename T>
void StateMachine<T>::updateState()
{
#ifdef DEBUG
  std::cout << "StateMachine::updateState: \
    Updating the state of the simulation" << std::endl;
  // Calculate overall sum of energy
#endif

  // Return if the state is not initialized
  if (states[0].getInitialized() == false ){
    handleError("StateMachine::updateState: State not initialized", CRITICAL);
    return;
  }

  if (p_global.getVerbosity() > 1){
    std::cout << "RUN: " << p_global.getRunCounter()
      << " ITERATION: " << states[0].getIteration() << std::endl;
  }

  // Log the state of the simulation
  if (p_global.getLogLevel() > 0){
    saveState(states[0], p_global.getRunDirectory(), true); 
  }

  // Transition of the state of the simulation
  transition(states[1], states[0], new_state);

  // Update the meta data of the simulation
  updateMeta(new_state);

  // Set the initialized flag
  new_state.setInitialized(true);

  // Swap the state of the simulation
  swapStates();

#ifdef DEBUG
  if (p_global.getVerbosity() > 0){
    system_metrics.printAgentMetrics();
  }
#endif
}

template <typename T>
void StateMachine<T>::transition(const State<T> &old_state,
    const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  std::cout << "StateMachine::transition: \
    Transitioning the state of the simulation" << std::endl;
#endif

  // Update the class id of the agents
  updateClassId(state, new_state);

  // Update the strategy of the agents
  updateStrategy(state, new_state);
  
  // Update the energy uptake rate of the agents
  updateEnergyUptakeRate(state, new_state);

  // Update the energy consumption rate of the agents
  updateEnergyConsumptionRateTime(state, new_state);

  // Update the energy consumption rate of the agents
  updateEnergyConsumptionRateDistance(state, new_state);

  // Update the size of the agents
  updateSizes(state, new_state);

  // Update the movespeed of the agents
  updateMovespeed(state, new_state);

  // Update the audibility of the agents
  updateAudibility(state, new_state);

  // Update the positions of the agents
  updatePositions(state, new_state);

  /* Careful of the order of the following functions! 
   * Do not change */

  // Exchange energy between agents, call before updateEnergy
  attackFood(state, new_state);

  // Update the energy of the agents, call after attackFood and after
  // updatePositions, because the energy consumption depends on the
  // distance moved
  updateEnergy(state, new_state);

  // Update the status of the agents, always call last,
  // can override all other updates (case respawn)
  updateStatus(state, new_state);
}

template <typename T>
void StateMachine<T>::updateMeta(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::updateMeta: \
    Updating the meta data of the simulation" << std::endl;
#endif

#pragma omp parallel
  {
    // Calculate the distances between agents
    calculateDistancesAgentToAgent(state);

    // Calculate the audibility of agents
    calculateAudibilityAgentToAgent(state);

    // Calculate the reachability of agents
    calculateReachabilityAgentToAgent(state);

    // Calculate the contact of agents
    calculateContactAgentToAgent(state);


    // Calculate the avg distances between agents and classes
    calculateDistancesAgentToClass(state);

    // Calculate the audibility of agents to classes
    calculateAudibilityAgentToClass(state);

    // Calculate the reachability of agents to classes
    calculateReachabilityAgentToClass(state);

    // Calculate the contact of agents to classes
    calculateContactAgentToClass(state);


    // Calculate the avg distances between classes
    calculateDistancesClassToClass(state);

    // Calculate the audibility of classes
    calculateAudibilityClassToClass(state);

    // Calculate the reachability of classes
    calculateReachabilityClassToClass(state);

    // Calculate the contact of classes
    calculateContactClassToClass(state);
  }
}

template <typename T>
void StateMachine<T>::setParameters(SimulationParameters<T> &sp)
{
#ifdef DEBUG
  std::cout << "StateMachine::setParameters: \
    Setting parameters of the simulation" << std::endl;
#endif

    // Set the parameters of the simulation
    sp_conf = sp;
}

template <typename T>
void StateMachine<T>::initAgents(const State<T> &state)
{
  // Initialize the agents
  agents.clear();
  agents.reserve(sp_scaled.num_agents_total);
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents.emplace_back(
        i, p_global.getVerbosity(), sp_scaled, agents, agent_classes, 
        system_metrics.agent_metrics, gen, dist_uni, dist_x, dist_y, dist_z);
    agents[i].init(state);
  }

  if (p_global.getVerbosity() > 3){
    std::cout << "StateMachine::initAgents: \
      Created " << agents.size() << " agents" << std::endl;
  }
}

template <typename T>
void StateMachine<T>::initAgentClasses()
{
#ifdef DEBUG
  std::cout << "StateMachine::initAgentClasses: \
    Initializing the agent classes of the simulation" << std::endl;
#endif

  // Initialize the agent classes with the parameters of the classes
  agent_classes.clear();
  agent_classes.reserve(sp_scaled.num_agent_classes);
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){
    agent_classes.emplace_back(p_global, 
        sp_scaled, sp_scaled.getAgentClass(i));
  }

  if (p_global.getVerbosity() > 1){
    std::cout << "StateMachine::initAgentClasses: \
      Created " << agent_classes.size() << " agent classes" << std::endl;
  }
}

template <typename T>
void StateMachine<T>::initAgentMetrics()
{
#ifdef DEBUG
  std::cout << "StateMachine::initAgentMetrics: \
    Initializing the agent metrics of the simulation" << std::endl;
#endif

  // Initialize the agent metrics
  system_metrics.initAgentMetrics(sp_scaled.num_agents_total);
#ifdef DEBUG
  if (p_global.getVerbosity() > 0){
    system_metrics.printAgentMetrics();
  }
#endif

}

template <typename T>
void StateMachine<T>::initStates()
{
#ifdef DEBUG
  std::cout << "StateMachine::initStates: \
    Initializing the states of the simulation" << std::endl;
  std::cout << "StateMachine::initStates: \
    Memory size: " << sp_scaled.memory_size << std::endl;
#endif
  // Allocate memory for the states
  states.clear();
  states.reserve(sp_scaled.memory_size);
  for (int i = 0; i < sp_scaled.memory_size; i++){
    states.emplace_back();
  }

  if (p_global.getVerbosity() > 1){
    std::cout << "StateMachine::initStates: \
      Created " << states.size() << " states" << std::endl;
  }
}

template <typename T>
void StateMachine<T>::initLogFlags()
{
#ifdef DEBUG
  std::cout << "StateMachine::initLogFlags: \
    Initializing the log flags" << std::endl;
#endif

  // Initialize the log flags
  for (int i = 0; i < (int)LogFlags::NUM_LOG_FLAGS; i++){
    log_flags[i] = false;
  }

  // Set the log flags
  if (p_global.getLogLevel() > 0){
    log_flags[(int)LogFlags::ENERGY] = true;
    log_flags[(int)LogFlags::POSITIONS] = true;
  }

  if (p_global.getLogLevel() > 1){
    log_flags[(int)LogFlags::ENERGY_CONSUMPTION_RATE_TIME] = true;
    log_flags[(int)LogFlags::ENERGY_CONSUMPTION_RATE_DISTANCE] = true;
    log_flags[(int)LogFlags::ENERGY_UPTAKE_RATE] = true;
    log_flags[(int)LogFlags::MOVESPEED] = true;
    log_flags[(int)LogFlags::SIZES] = true;
    log_flags[(int)LogFlags::STATUS] = true;
    log_flags[(int)LogFlags::CLASS_ID] = true;
    log_flags[(int)LogFlags::STRATEGY] = true;
    log_flags[(int)LogFlags::AUDIBILITY_THRESHOLD] = true;
  }

  if (p_global.getLogLevel() > 2){
    log_flags[(int)LogFlags::DISTANCES_CLASS_TO_CLASS] = true;
    log_flags[(int)LogFlags::AUDIBILITY_CLASS_TO_CLASS] = true;
    log_flags[(int)LogFlags::REACHABILITY_CLASS_TO_CLASS] = true;
    log_flags[(int)LogFlags::CONTACT_CLASS_TO_CLASS] = true;
  }

  if (p_global.getLogLevel() > 3){
    log_flags[(int)LogFlags::DISTANCES_AGENT_TO_CLASS] = true;
    log_flags[(int)LogFlags::AUDIBILITY_AGENT_TO_CLASS] = true;
    log_flags[(int)LogFlags::REACHABILITY_AGENT_TO_CLASS] = true;
    log_flags[(int)LogFlags::CONTACT_AGENT_TO_CLASS] = true;
  }

  if (p_global.getLogLevel() > 4){
    log_flags[(int)LogFlags::DISTANCES_AGENT_TO_AGENT] = true;
    log_flags[(int)LogFlags::AUDIBILITY_AGENT_TO_AGENT] = true;
    log_flags[(int)LogFlags::REACHABILITY_AGENT_TO_AGENT] = true;
    log_flags[(int)LogFlags::CONTACT_AGENT_TO_AGENT] = true;
  }
}


template <typename T>
void StateMachine<T>::initParameters()
{
#ifdef DEBUG
  std::cout
    << "StateMachine::initParameters: \
    Initializing parameters of the simulation" << std::endl;
#endif

  // Test if the parameters are initialized
  if (!sp_conf.simulation_parameters_initialized){
    throw std::runtime_error("Simulation parameters uninitialized");
    return;
  }

  // Scale the parameters of the simulation
  sp_scaled = sp_conf.scale();

  // Set the distributions, call after scaling
  sp_scaled.initDistributions();
}

template <typename T>
void StateMachine<T>::initRandom()
{
#ifdef DEBUG
  std::cout << "StateMachine::initRandom: \
    Initializing the random number generator" << std::endl;
#endif

  // Seed the random number generator
  if (sp_scaled.seed == 0){
    if (p_global.getVerbosity() > 0){
      std::cout << "Using random seed" << std::endl;
    }
    gen.seed(rd());
  }else{
    if (p_global.getVerbosity() > 0){
      std::cout << "Using fixed seed: " << sp_scaled.seed << std::endl;
    }
    gen.seed(sp_scaled.seed);
  }

  // Initialize the distributions of the agent classes
  sp_scaled.initDistributions();

  // Create a uniform distribution in the interval [0, 1]
  dist_uni = std::uniform_real_distribution<T> (0.0, 1.0);

  // Set distributions over the world size
  dist_x = std::uniform_real_distribution<T>(-sp_scaled.worldsize[0], 
      sp_scaled.worldsize[0]);
  dist_y = std::uniform_real_distribution<T>(-sp_scaled.worldsize[1], 
      sp_scaled.worldsize[1]);
  dist_z = std::uniform_real_distribution<T>(-sp_scaled.worldsize[2], 
      sp_scaled.worldsize[2]);
}

template <typename T>
void StateMachine<T>::initState(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initState: \
    Initializing state of the simulation" << std::endl;
#endif
  // Set the number of agents
  state.num_agents_total = sp_scaled.num_agents_total;

  // Set the number of classes
  state.num_classes = sp_scaled.num_agent_classes;
  
  // Allocate memory for the state
  state.allocateMemory(sp_scaled.num_agents_total, 
      sp_scaled.num_agent_classes);

  // Initialize the positions of the agents
  initPositions(state);

  // Initialize the energy of the agents
  initEnergy(state);

  // Initialize the energy consumption rate of the agents
  initEnergyConsumptionRateTime(state);

  // Initialize the energy consumption rate of the agents
  initEnergyConsumptionRateDistance(state);

  // Initialize the energy uptake rate of the agents
  initEnergyUptakeRate(state);

  // Initialize the status of the agents
  initStatus(state);

  // Initialize the audibility of the agents
  initAudibility(state);

  // Initialize the movespeed of the agents
  initMovespeed(state);

  // Initialize the class id of the agents
  initClassId(state);

  // Initialize the strategy of the agents
  initStrategy(state);

  // Initialize the size of the agents
  initSizes(state);

  // Set timestamp
  state.setTimestamp(0.0);

  // Set the iteration counter
  state.setIteration(0);

  // Set the run counter
  state.setRunCounter(p_global.getRunCounter());

}

template <typename T>
void StateMachine<T>::initClassId(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initClassId: \
    Initializing class id of the agents" << std::endl;
#endif

  // Initialize the class id of the agents
  // Loop over all classes
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the class id of the agents
    for (int j = c.start_index; j < c.stop_index; j++){
        state.class_id[j] = c.getId();
    }
  }
}

template <typename T>
void StateMachine<T>::initStrategy(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initStrategy: \
    Initializing strategy of the agents" << std::endl;
#endif

  // Initialize the strategy of the agents
  // Loop over all classes
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the strategy of the agents
    for (int j = c.start_index; j < c.stop_index; j++){
        state.strategy[j] = c.strategy;
    }
  }
}

template <typename T>
void StateMachine<T>::initAudibility(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initAudibility: \
    Initializing audibility threshold of the agents" << std::endl;
#endif

  // Initialize the audibility of the agents
  // Loop over all classes
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the audibility of the agents
    for (int j = c.start_index; j < c.stop_index; j++){
        state.audibility_threshold[j] = c.audibility;
    }
  }
}

template <typename T>
void StateMachine<T>::initEnergyConsumptionRateTime(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initEnergyConsumptionRateTime: \
    Initializing energy consumption rate of the agents" << std::endl;
#endif

  // Initialize the energy consumption rate of the agents
  // Loop over all classes
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the energy consumption rate of the agents
    for (int j = c.start_index; j < c.stop_index; j++){
        state.energy_consumption_per_time[j] = c.energy_consumption_per_time;
    }
  }
}

template <typename T>
void StateMachine<T>::initEnergyConsumptionRateDistance(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initEnergyConsumptionRateDistance: \
    Initializing energy consumption rate of the agents" << std::endl;
#endif

  // Initialize the energy consumption rate of the agents
  // Loop over all classes
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the energy consumption rate of the agents
    for (int j = c.start_index; j < c.stop_index; j++){
        state.energy_consumption_per_distance[j] 
          = c.energy_consumption_per_distance;
    }
  }
}

template <typename T>
void StateMachine<T>::initEnergyUptakeRate(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initEnergyUptakeRate: \
    Initializing energy uptake rate of the agents" << std::endl;
#endif

  // Initialize the energy uptake rate of the agents
  // Loop over all classes
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the energy uptake rate of the agents
    for (int j = c.start_index; j < c.stop_index; j++){
        state.energy_uptake_rate[j] = c.energy_uptake_rate;
    }
  }
}

template <typename T>
void StateMachine<T>::initSizes(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initSize: \
    Initializing size of the agents" << std::endl;
#endif

  // Initialize the size of the agents
  // Loop over all classes
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the size of the agents
    for (int j = c.start_index; j < c.stop_index; j++){
        state.sizes[j] = c.size;
    }
  }
}

template <typename T>
void StateMachine<T>::initStatus(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initStatus: \
    Initializing status of the agents" << std::endl;
#endif

  // Initialize the status of the agents
  // Loop over all classes
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the status of the agents
    for (int j = c.start_index; j < c.stop_index; j++){
        state.status[j] = c.status;
    }
  }
}

template <typename T>
void StateMachine<T>::initMovespeed(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initMovespeed: \
    Initializing movespeed of the agents" << std::endl;
#endif

  // Initialize the movespeed of the agents
  // Loop over all classes
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the movespeed of the agents
    for (int j = c.start_index; j < c.stop_index; j++){
        state.movespeed[j] = c.movespeed;
    }
  }
}

template <typename T>
void StateMachine<T>::initPositions(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initPositions: \
    Initializing positions of the agents" << std::endl;
#endif

  // Initialize the positions of the agents per class
  for (int i=0; i<sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the positions of the agents
    for (int j=c.start_index; j<c.stop_index; j++){

      // Set the position of the agent
      if (c.random_start_positions) {

        // Uniform distribution in the world
        state.positions[j] = Point3D<T>(
            dist_x(gen),
            dist_y(gen),
            dist_z(gen));

      } else {

        // Fixed start position
        state.positions[j] = c.start_position;
     }

    }// end for agents in class
  }// end for class
}

template <typename T>
void StateMachine<T>::initEnergy(State<T> &state)
{
#ifdef DEBUG
  std::cout << "StateMachine::initEnergy: \
    Initializing energy of the agents" << std::endl;
#endif

  // Initialize the energy of the agents per class
  for (int i=0; i<sp_scaled.num_agent_classes; i++){
    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Initialize the energy of the agents
    for (int j=c.start_index; j<c.stop_index; j++){

      // Set the energy of the agent
      state.energy[j] = c.dist_energy(gen);
      if (state.energy[j] < 0.0){
        state.energy[j] = 0.0;
      }

    }// end for agents in class
  }// end for class
}

template <typename T>
void StateMachine<T>::updatePositions(const State<T> &state, 
    State<T> &new_state)
{
#ifdef DEBUG
  std::cout << "StateMachine::updatePositions: \
    Updating the positions of the agents" << std::endl;
#endif

  // Update the positions of the agents
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents.at(i).updatePosition(state, new_state);
  }
}


template <typename T>
void StateMachine<T>::updateEnergy(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::updateEnergy: \
      Updating the energy of the agents" << std::endl;
  }
#endif

  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].updateEnergy(state, new_state);
  }

  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].energyExchangePartTwoDistribute(state, new_state);
  }
}

template <typename T>
void StateMachine<T>::attackFood(const State<T> &state, 
    State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::attackFood: \
      Registering at food sources for consumption" << std::endl;
  }
#endif

  // All agents register the energy to be exchanged
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].energyExchangePartOneRegister(state, new_state);
  }

}

template <typename T>
void StateMachine<T>::updateEnergyConsumptionRateTime(const State<T> &state, 
    State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::updateEnergyConsumptionRateTime: \
      Updating the energy consumption rate of the agents" << std::endl;
  }
#endif

  // Update the energy consumption rate of the agents
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].updateEnergyConsumptionRateTime(state, new_state);
  }
}

template <typename T>
void StateMachine<T>::updateEnergyConsumptionRateDistance(
    const State<T> &state, 
    State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::updateEnergyConsumptionRateDistance: \
      Updating the energy consumption rate of the agents" << std::endl;
  }
#endif

  // Update the energy consumption rate of the agents
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].updateEnergyConsumptionRateDistance(state, new_state);
  }
}

template <typename T>
void StateMachine<T>::updateEnergyUptakeRate(const State<T> &state, 
    State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::updateEnergyUptakeRate: \
      Updating the energy uptake rate of the agents" << std::endl;
  }
#endif

  // Update the energy uptake rate of the agents
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].updateEnergyUptakeRate(state, new_state);
  }
}

template <typename T>
void StateMachine<T>::updateAudibility(const State<T> &state, 
    State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::updateAudibility: \
      Updating the audibility of the agents" << std::endl;
  }
#endif

  // Update the audibility of the agents
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].updateAudibilityThreshold(state, new_state);
  }
}

template <typename T>
void StateMachine<T>::updateMovespeed(const State<T> &state, 
    State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::updateMovespeed: \
      Updating the movespeed of the agents" << std::endl;
  }
#endif

  // Update the movespeed of the agents
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].updateMovespeed(state, new_state);
  }
}

template <typename T>
void StateMachine<T>::updateSizes(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::updateSizes: \
      Updating the size of the agents" << std::endl;
  }
#endif

  // Update the size of the agents
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].updateSize(state, new_state);
  }
}

template <typename T>
void StateMachine<T>::updateClassId(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::updateClassId: \
      Updating the class id of the agents" << std::endl;
  }
#endif

  // Update the class id of the agents
  // Placeholder for now
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].updateClassId(state, new_state);
  }
}

template <typename T>
void StateMachine<T>::updateStrategy(const State<T> &state, 
    State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::updateStrategy: \
      Updating the strategy of the agents" << std::endl;
  }
#endif

  // Update the strategy of the agents
  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].updateStrategy(state, new_state);
  }
}

template <typename T>
void StateMachine<T>::updateStatus(const State<T> &state, State<T> &new_state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::updateStatus: \
      Updating the status of the agents" << std::endl;
  }
#endif

  for (int i = 0; i < sp_scaled.num_agents_total; i++){
    agents[i].updateStatus(state, new_state);
  }
}

template <typename T>
void StateMachine<T>::calculateAudibilityAgentToAgent(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateAudibilityAgentToAgent: \
      Calculating audibility of agents" << std::endl;
  }
#endif

  // Calculate the audibility of agents
#pragma omp for 
  for (int i=0; i<state.num_agents_total; i++){
    for (int j=0; j<state.num_agents_total; j++){
      if (state.distances_agent_to_agent[i*state.num_agents_total+j] 
          < state.audibility_threshold[j]){
        state.audibility[i*state.num_agents_total+j] = 1;
      } else{
        state.audibility[i*state.num_agents_total+j] = 0;
      }
      if (i == j){
        state.audibility[i*state.num_agents_total+j] = 0;
      }
    }
  }
  
} 

template <typename T>
void StateMachine<T>::calculateAudibilityAgentToClass(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateAudibilityAgentToClass: \
      Calculating audibility of agents to classes" << std::endl;
  }
#endif
  
  if (state.num_classes != sp_scaled.num_agent_classes){
    handleError("StateMachine::calculateAudibilityAgentToClass: \
      Number of classes mismatch", WARNING);
    return;
  }
  // Memory layout of audibility_agent_to_class:
  // a0c0, a1c0, ..., ajc0, ...
  // a0c1, a1c1, ..., ajc1, ...
  // a0ci, a1ci, ..., ajci, ...
  // how many agents in class i can agent j hear

  // Calculate the audibility of agents to classes
#pragma omp for  
  for (int j = 0; j < state.num_agents_total; j++){
    for (int i = 0; i < sp_scaled.num_agent_classes; i++){
      // Get the class
      AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);
      // Calculate indices
      const int start_index = j * state.num_agents_total + c.start_index; 
      const int stop_index = j * state.num_agents_total + c.stop_index;
      // Number of audible agents in class
      int sum = 0;
      for (int k = start_index; k < stop_index; k++){
        sum += state.audibility[k];
      }
      state.audibility_agent_to_class[i*state.num_agents_total + j] = sum;
    }
  }
}

template <typename T>
void StateMachine<T>::calculateAudibilityClassToClass(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateAudibilityClassToClass: \
      Calculating audibility of classes" << std::endl;
  }
#endif

  // Calculate the audibility of classes
  // Memory layout of audibility_class_to_class:
  // c0c0, c1c0, ..., cjci, ...
  // c0c1, c1c1, ..., cjci, ...
  // c0cj, c1cj, ..., cjci, ...

  // Calculate the audibility of classes
#pragma omp for 
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){

    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Loop over all classes
    for (int j = 0; j < sp_scaled.num_agent_classes; j++){

      // Get the class
      AgentClassParameters<T> &d = sp_scaled.getAgentClass(j);

      // Calculate indices
      int start_index = i * state.num_agents_total + d.start_index;
      int stop_index = i * state.num_agents_total + d.stop_index;

      // Number of agents in class i that can hear at least 
      // one agent of class j
      state.countTrue(
        state.audibility_agent_to_class,
        start_index, stop_index, 
        state.audibility_class_to_class[i*sp_scaled.num_agent_classes + j]);
    }
  }
}

template <typename T>
void StateMachine<T>::calculateDistancesAgentToAgent(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateDistancesAgentToAgent: \
      Calculating distances between agents" << std::endl;
  }
#endif

  // Calculate the distances between agents
#pragma omp for 
  for (int i = 0; i < sp_scaled.num_agents_total; i++){

    // Loop over all agents
    for (int j = 0; j < sp_scaled.num_agents_total; j++){
      // Calculate the distance between agents
      state.distances_agent_to_agent[i*sp_scaled.num_agents_total + j] 
        = state.positions[i].distance(state.positions[j]);
    }
  }
}

template <typename T>
void StateMachine<T>::calculateDistancesAgentToClass(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateDistancesAgentToClass: \
      Calculating distances between agents and classes" << std::endl;
  }
#endif

  // Calculate the distances between agents and classes
  // Matrix of distances between agents and classes
  // da0c0, da1c0, ..., daic0, ...
  // da0c1, da1c1, ..., daic1, ...
  // ...  
  // da0cj, da1cj, ..., daicj, ...
  //


    // Loop over all agents
#pragma omp for 
    for (int i = 0; i < sp_scaled.num_agents_total; i++){

      // Loop over all classes
      for (int j = 0; j < sp_scaled.num_agent_classes; j++){

        // Get the class
        AgentClassParameters<T> &c = sp_scaled.getAgentClass(j);

        // Calculate indices
        int start_index = i * state.num_agents_total + c.start_index; 
        int stop_index = i * state.num_agents_total + c.stop_index;

        // Average distance to class
        // Calculates the average of the distances between agent i and 
        // all members of class j
        state.calculateAverage(
          state.distances_agent_to_agent,
          start_index, stop_index, 
          state.distances_agent_to_class[j*state.num_agents_total + i]);
    }
  }
}

template <typename T>
void StateMachine<T>::calculateDistancesClassToClass(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateDistancesClassToClass: \
      Calculating distances between classes" << std::endl;
  }
#endif

  int num_classes = sp_scaled.num_agent_classes;

  // Calculate the distances between classes
#pragma omp for 
  for (int i = 0; i < num_classes; i++){

    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Loop over all classes
    for (int j = 0; j < num_classes; j++){

      // Get the class
      AgentClassParameters<T> &d = sp_scaled.getAgentClass(j);

      // Calculate indices
      int start_index = i * state.num_agents_total + d.start_index;
      int stop_index = i * state.num_agents_total + d.stop_index;

      // Average distance to class
      state.calculateAverage(
        state.distances_agent_to_class,
        start_index, stop_index, 
        state.distances_class_to_class[i*num_classes + j]);
    }
  }
}

template <typename T>
void StateMachine<T>::calculateReachabilityAgentToAgent(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateReachabilityAgentToAgent: \
      Calculating reachability of agents" << std::endl;
  }
#endif

  // Calculate the reachability of agents
#pragma omp for 
  for (int i=0; i<state.num_agents_total; i++){
    for (int j=0; j<state.num_agents_total; j++){
      if (state.distances_agent_to_agent[i*state.num_agents_total+j] 
          < state.movespeed[i]){
        state.reachability[i*state.num_agents_total+j] = 1;
      } else{
        state.reachability[i*state.num_agents_total+j] = 0;
      }
      if (i == j){
        state.reachability[i*state.num_agents_total+j] = 0;
      }
    }
  }
}

template <typename T>
void StateMachine<T>::calculateReachabilityAgentToClass(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateReachabilityAgentToClass: \
      Calculating reachability of agents to classes" << std::endl;
  }
#endif

  // Calculate the reachability of agents to classes
  // Memory layout of reachability_agent_to_class:
  // a0c0, a1c0, ..., ajc0, ...
  // a0c1, a1c1, ..., ajc1, ...
  // a0ci, a1ci, ..., ajci, ...

  // Calculate the reachability of agents to classes
#pragma omp for 
  for (int j = 0; j < state.num_agents_total; j++){
    for (int i = 0; i < sp_scaled.num_agent_classes; i++){
      // Calculate indices
      // Get the class
      AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);
      const int start_index = j * state.num_agents_total + c.start_index; 
      const int stop_index = j * state.num_agents_total + c.stop_index;
      // Number of reachable agents in class
      int sum = 0;
      for (int k = start_index; k < stop_index; k++){
        sum += state.reachability[k];
      }
      state.reachability_agent_to_class[i*state.num_agents_total + j] = sum;
    }
  }
}

template <typename T>
void StateMachine<T>::calculateReachabilityClassToClass(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateReachabilityClassToClass: \
      Calculating reachability of classes" << std::endl;
  }
#endif

  // Calculate the reachability of classes
  // Memory layout of reachability_class_to_class:
  // c0c0, c1c0, ..., cjci, ...
  // c0c1, c1c1, ..., cjci, ...
  // c0cj, c1cj, ..., cjci, ...

  // Calculate the reachability of classes
#pragma omp for 
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){

    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Loop over all classes
    for (int j = 0; j < sp_scaled.num_agent_classes; j++){

      // Get the class
      AgentClassParameters<T> &d = sp_scaled.getAgentClass(j);

      // Calculate indices
      int start_index = i * state.num_agents_total + d.start_index;
      int stop_index = i * state.num_agents_total + d.stop_index;

      // Number of agents of class i that can reach at least
      // one agent of class j 
      state.countTrue(
        state.reachability_agent_to_class,
        start_index, stop_index, 
        state.reachability_class_to_class[i*sp_scaled.num_agent_classes + j]);
    }
  } 
}

template <typename T>
void StateMachine<T>::calculateContactAgentToAgent(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateContactAgentToAgent: \
      Calculating contact of agents" << std::endl;
  }
#endif

  // Calculate the contact of agents
#pragma omp for  
  for (int i=0; i<state.num_agents_total; i++){
    for (int j=0; j<state.num_agents_total; j++){
      if (state.distances_agent_to_agent[i*state.num_agents_total+j] 
          < 0.5 *(state.sizes[i]+state.sizes[j])){
        state.contact[i*state.num_agents_total+j] = 1;
      } else{
        state.contact[i*state.num_agents_total+j] = 0;
      }
      if (i == j){
        state.contact[i*state.num_agents_total+j] = 0;
      }
    }
  }
}

template <typename T>
void StateMachine<T>::calculateContactAgentToClass(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateContactAgentToClass: \
      Calculating contact of agents to classes" << std::endl;
  }
#endif

  // Calculate the contact of agents to classes
  // Memory layout of contact_agent_to_class:
  // a0c0, a1c0, ..., ajc0, ...
  // a0c1, a1c1, ..., ajc1, ...
  // a0ci, a1ci, ..., ajci, ...

  // Calculate the contact of agents to classes
#pragma omp for 
  for (int j = 0; j < state.num_agents_total; j++){
    for (int i = 0; i < sp_scaled.num_agent_classes; i++){
      // Get the class
      AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

      // Calculate indices
      const int start_index = j * state.num_agents_total + c.start_index; 
      const int stop_index = j * state.num_agents_total + c.stop_index;
      // Number of contacts with agents in class
      int sum = 0;
      for (int k = start_index; k < stop_index; k++){
        sum += state.contact[k];
      }
      state.contact_agent_to_class[i*state.num_agents_total + j] = sum;
    }
  }
}

template <typename T>
void StateMachine<T>::calculateContactClassToClass(State<T> &state)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::calculateContactClassToClass: \
      Calculating contact of classes" << std::endl;
  }
#endif

  // Calculate the contact of classes
  // Memory layout of contact_class_to_class:
  // c0c0, c1c0, ..., cjci, ...
  // c0c1, c1c1, ..., cjci, ...
  // c0cj, c1cj, ..., cjcies
#pragma omp for 
  for (int i = 0; i < sp_scaled.num_agent_classes; i++){

    // Get the class
    AgentClassParameters<T> &c = sp_scaled.getAgentClass(i);

    // Loop over all classes
    for (int j = 0; j < sp_scaled.num_agent_classes; j++){

      // Get the class
      AgentClassParameters<T> &d = sp_scaled.getAgentClass(j);

      // Calculate indices
      int start_index = i * state.num_agents_total + d.start_index;
      int stop_index = i * state.num_agents_total + d.stop_index;

      // Number of agents of class i that are in contact with at least
      // one agent of class j
      state.countTrue(
        state.contact_agent_to_class,
        start_index, stop_index, 
        state.contact_class_to_class[i*sp_scaled.num_agent_classes + j]);
    }
  }
}

template <typename T>
void StateMachine<T>::printState(const State<T> &state, 
    std::ostream &stream, bool append, std::string sep, int verbosity) const
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 2){
    std::cout << "StateMachine::printState: \
      Printing the state of the simulation" << std::endl;
  }
#endif

  // Print the state of the simulation
  return state.printState(stream, append, sep, verbosity);
}

template <typename T>
void StateMachine<T>::printState() const
{
  return printState(states[0], std::cout, false, ",", p_global.getVerbosity());
}

template <typename T>
void StateMachine<T>::saveState(const State<T> &state,
    std::string dirname, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveState: \
    Saving the state of the simulation to: " 
    << dirname 
    << std::endl;
#endif

  // Test if the directory exists
  if (!std::filesystem::exists(dirname)){
    try{
      std::filesystem::create_directories(dirname);
    }catch(const std::exception& e){
      std::cerr << e.what() << '\n';
      throw std::runtime_error("Could not create directory");
    }
  }

  if (log_flags[(int)LogFlags::POSITIONS]){
    // Save the x-position of the agents to a file
    std::string filename = dirname + "/pos_x.csv";
    savePositions(0, state, filename, append, sep);

    // Save the y-position of the agents to a file
    filename = dirname + "/pos_y.csv";
    savePositions(1, state, filename, append, sep);

    // Save the z-position of the agents to a file
    filename = dirname + "/pos_z.csv";
    savePositions(2, state, filename, append, sep);
  }

  // Save the energy of the agents to a file
  if (log_flags[(int)LogFlags::ENERGY]){
    std::string filename = dirname + "/energy.csv";
    saveEnergy(state, filename, append, sep);
  }

  // Save the energy consumption rate of the agents to a file
  if (log_flags[(int)LogFlags::ENERGY_CONSUMPTION_RATE_TIME]){
    std::string filename = dirname + "/energy_consumption_rate_time.csv";
    saveEnergyConsumptionRateTime(state, filename, append, sep);
  }

  // Save the energy consumption rate of the agents to a file
  if (log_flags[(int)LogFlags::ENERGY_CONSUMPTION_RATE_DISTANCE]){
    std::string filename = dirname + "/energy_consumption_rate_distance.csv";
    saveEnergyConsumptionRateDistance(state, filename, append, sep);
  }

  // Save the energy uptake rate of the agents to a file
  if (log_flags[(int)LogFlags::ENERGY_UPTAKE_RATE]){
    std::string filename = dirname + "/energy_uptake_rate.csv";
    saveEnergyUptakeRate(state, filename, append, sep);
  }

  // Save the status of the agents to a file
  if (log_flags[(int)LogFlags::STATUS]){
    std::string filename = dirname + "/status.csv";
    saveStatus(state, filename, append, sep);
  }

  // Save the strategy of the agents to a file
  if (log_flags[(int)LogFlags::STRATEGY]){
    std::string filename = dirname + "/strategy.csv";
    saveStrategy(state, filename, append, sep);
  }

  // Save the class id of the agents to a file
  if (log_flags[(int)LogFlags::CLASS_ID]){
    std::string filename = dirname + "/class_id.csv";
    saveClassId(state, filename, append, sep);
  }

  // Save the audibility threshold of the agents to a file
  if (log_flags[(int)LogFlags::AUDIBILITY_THRESHOLD]){
    std::string filename = dirname + "/audibility_threshold.csv";
    saveAudibilityThreshold(state, filename, append, sep);
  }
  
  // Save the average distance between agents to a file
  if (log_flags[(int)LogFlags::DISTANCES_AGENT_TO_AGENT]){
    std::string filename = dirname + "/distances_agent_to_agent.csv";
    saveDistances(state, filename, append, sep);
  }

  // Save the average distance between agents and classes to a file
  if (log_flags[(int)LogFlags::DISTANCES_AGENT_TO_CLASS]){
    std::string filename = dirname + "/distances_agent_to_class.csv";
    saveDistancesAgentToClass(state, filename, append, sep);
  }

  // Save the average distance between classes to a file
  if (log_flags[(int)LogFlags::DISTANCES_CLASS_TO_CLASS]){
    std::string filename = dirname + "/distances_class_to_class.csv";
    saveDistancesClassToClass(state, filename, append, sep);
  }

  // Save the audibility of agents to a file
  if (log_flags[(int)LogFlags::AUDIBILITY_AGENT_TO_AGENT]){
    std::string filename = dirname + "/audibility_agent_to_agent.csv";
    saveAudibility(state, filename, append, sep);
  }

  // Save the audibility of agents to classes to a file
  if (log_flags[(int)LogFlags::AUDIBILITY_AGENT_TO_CLASS]){
    std::string filename = dirname + "/audibility_agent_to_class.csv";
    saveAudibilityAgentToClass(state, filename, append, sep);
  }

  // Save the audibility of classes to a file
  if (log_flags[(int)LogFlags::AUDIBILITY_CLASS_TO_CLASS]){
    std::string filename = dirname + "/audibility_class_to_class.csv";
    saveAudibilityClassToClass(state, filename, append, sep);
  }

  // Save the reachability of agents to a file
  if (log_flags[(int)LogFlags::REACHABILITY_AGENT_TO_AGENT]){
    std::string filename = dirname + "/reachability.csv";
    saveReachability(state, filename, append, sep);
  }

  // Save the reachability of agents to classes to a file
  if (log_flags[(int)LogFlags::REACHABILITY_AGENT_TO_CLASS]){
    std::string filename = dirname + "/reachability_agent_to_class.csv";
    saveReachabilityAgentToClass(state, filename, append, sep);
  }

  // Save the reachability of classes to a file
  if (log_flags[(int)LogFlags::REACHABILITY_CLASS_TO_CLASS]){
    std::string filename = dirname + "/reachability_class_to_class.csv";
    saveReachabilityClassToClass(state, filename, append, sep);
  }

  // Save agent to agent contact to a file
  if (log_flags[(int)LogFlags::CONTACT_AGENT_TO_AGENT]){
    std::string filename = dirname + "/contact_agent_to_agent.csv";
    saveContactAgentToAgent(state, filename, append, sep);
  }

  // Save agent to class contact to a file
  if (log_flags[(int)LogFlags::CONTACT_AGENT_TO_CLASS]){
    std::string filename = dirname + "/contact_agent_to_class.csv";
    saveContactAgentToClass(state, filename, append, sep);
  }

  // Save class to class contact to a file
  if (log_flags[(int)LogFlags::CONTACT_CLASS_TO_CLASS]){
    std::string filename = dirname + "/contact_class_to_class.csv";
    saveContactClassToClass(state, filename, append, sep);
  }

}

template <typename T>
void StateMachine<T>::saveState(std::string dirname, bool append, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveState: \
    Saving the state of the simulation" << std::endl;
#endif

  // Save the state of the simulation
  saveState(states[0], dirname, append, sep);
}

template <typename T>
void StateMachine<T>::saveStateToml(const State<T> &state, 
    std::string filename) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveStateToml: \
    Saving the state of the simulation to: " 
    << filename 
    << std::endl;
#endif

  // Open the file for writing
  std::ofstream file;
  openFile(file, filename, false);

  // Save the state of the simulation
  sp_conf.printSimulationParameters(file);
  file << "[simulation_state]" << std::endl;
  state.printStateToml(file);

  // Close the file
  file.close();
}

template <typename T>
void StateMachine<T>::saveStateToml(std::string filename) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveStateToml: \
    Saving the state of the simulation" << std::endl;
#endif
  return saveStateToml(states[0], filename);
}

template <typename T>
void StateMachine<T>::loadStateToml(State<T> &state, std::string filename)
{
#ifdef DEBUG
  std::cout << "StateMachine::loadStateToml: \
    Loading the state of the simulation from toml" << std::endl;
#endif

  // Load the parameters of the simulation from a file
  sp_conf.parseSimulationConfig(filename);
  sp_conf.init();
  try{
    initParameters();
  }catch(const std::exception &e){
    handleError(e.what(), WARNING);
    return;
  }

  // Initialize random number generator
  initRandom();

  // Initialize the agent class and state objects
  initStates(); // this clears the states vector, if loadStateToml 
                // function is called on a state of the vector
                // a segfault will occur

  // Load the state of the simulation from a file
  state.loadStateToml(filename);
  if (state.num_agents_total == 0){
    handleError("StateMachine::loadStateToml: \
      State not initialized", WARNING);
    return;
  }
  initAgents(state); //must be called after state.loadStateToml
  initAgentClasses();
  initAgentMetrics();
  updateMeta(state);

#ifdef DEBUG
  // Initialize the system metrics
  system_metrics.initialize(state);
#endif

  // Set iteration and run counter, must be done before swapStates
  p_global.setRunCounter(state.getRunCounter());
  swapStates();

  // Create a copy of state[0] because the transition function needs two 
  // old states
  states[1] = states[0];

#ifdef DEBUG
  if (p_global.getVerbosity() > 4){
    printState(state);
  }
#endif

  // Set global parameters
  p_global.setRunCounter(state.getRunCounter());
  p_global.initRunDirectory();
  initialized = true;
  if (p_global.getVerbosity() > 4){
    printState(state);
  }

}

template <typename T>
void StateMachine<T>::loadStateToml(std::string filename)
{
  if (states.size() == 0){
    states.emplace_back();
  }
  return StateMachine<T>::loadStateToml(new_state, filename);
}

template <typename T>
std::string StateMachine<T>::getRestoreFile()
{
#ifdef DEBUG
  std::cout << "StateMachine::getRestoreFile: \
    Getting the restore file: "<< restore_file << std::endl;
#endif
  return restore_file;
}

template <typename T>
void StateMachine<T>::setRestoreFile(std::string filename)
{
#ifdef DEBUG
  std::cout << "StateMachine::setRestoreFile: \
    Setting the restore file: "<< filename << std::endl;
#endif
  restore_file = filename;
}

template <typename T>
void StateMachine<T>::saveParameters(std::string dirname)
{
#ifdef DEBUG
  std::cout << "StateMachine::saveParameters: \
    Saving the parameters of the simulation" << std::endl;
#endif

  // Save the parameters of the simulation to a file
  std::string filename = dirname + "/simulation_parameters.csv";
  std::ofstream file;
  openFile(file, filename, false);
  file << "Runtime: " << p_global.getTimeString() << std::endl;
  file << "Run: " << p_global.getRunCounter() << std::endl;
  sp_conf.printSimulationParameters(file);
  file.close();
}

template <typename T>
void StateMachine<T>::saveEnergy(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveEnergy: \
    Saving the energy of the agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the energy of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printEnergy(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveEnergyConsumptionRateTime(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveEnergyConsumptionRateTime: \
    Saving the energy consumption rate of the agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the energy consumption rate of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printEnergyConsumptionPerTime(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveEnergyConsumptionRateDistance(
    const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveEnergyConsumptionRateDistance: \
    Saving the energy consumption rate of the agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the energy consumption rate of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printEnergyConsumptionPerDistance(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveEnergyUptakeRate(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveEnergyUptakeRate: \
    Saving the energy uptake rate of the agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the energy uptake rate of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printEnergyUptakeRate(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveStatus(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveStatus: \
    Saving the status of the agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the status of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printStatus(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveStrategy(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveStrategy: \
    Saving the strategy of the agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the strategy of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printStrategy(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveClassId(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveClassId: \
    Saving the class id of the agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the class id of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printClassId(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveAudibilityThreshold(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveAudibilityThreshold: \
    Saving the audibility threshold of the agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the audibility threshold of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printAudibilityThreshold(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::savePositions(int dim, const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::savePositions: \
    Saving the positions of the agents" << std::endl;
#endif

  // Save the positions of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  if (!append){
    file << "Positions" << std::endl;
  }
  state.printPositions(file, dim, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveDistances(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveDistances: \
    Saving the distances between agents" << std::endl;
#endif

  // Save the distances between agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printDistances(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveDistancesAgentToClass(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveDistancesAgentToClass: \
    Saving the distances between agents and classes" << std::endl;
#endif

  // Save the distances between agents and classes to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printDistancesAgentToClass(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveDistancesClassToClass(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveDistancesClassToClass: \
    Saving the distances between classes" << std::endl;
#endif

  // Save the distances between classes to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printDistancesClassToClass(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveAudibility(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveAudibility: \
    Saving the audibility of the agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the audibility of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printAudibility(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveAudibilityAgentToClass(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveAudibilityAgentToClass: \
    Saving the audibility of the agents to classes" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the audibility of the agents to classes to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printAudibilityAgentToClass(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveAudibilityClassToClass(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveAudibilityClassToClass: \
    Saving the audibility of the classes" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the audibility of the classes to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printAudibilityClassToClass(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveReachability(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveReachability: \
    Saving the reachability of the agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the reachability of the agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printReachability(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveReachabilityAgentToClass(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveReachabilityAgentToClass: \
    Saving the reachability of the agents to classes" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the reachability of the agents to classes to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printReachabilityAgentToClass(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveReachabilityClassToClass(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveReachabilityClassToClass: \
    Saving the reachability of the classes" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the reachability of the classes to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printReachabilityClassToClass(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveContactAgentToAgent(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveContactAgentToAgent: \
    Saving the contact between agents" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the contact between agents to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printContact(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveContactAgentToClass(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveContactAgentToClass: \
    Saving the contact between agents and classes" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the contact between agents and classes to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printContactAgentToClass(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::saveContactClassToClass(const State<T> &state, 
    std::string filename, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::saveContactClassToClass: \
    Saving the contact between classes" << std::endl;
#endif

  // Test if file exists
  if (std::filesystem::exists(filename)){
    append = true;
  }

  // Save the contact between classes to a file
  std::ofstream file;
  openFile(file, filename, append);
  state.printContactClassToClass(file, append, sep);
  file.close();
}

template <typename T>
void StateMachine<T>::openFile(std::ofstream &file, std::string filename,
    bool append) const
{
#ifdef DEBUG
  std::cout << "StateMachine::openFile: \
    Opening file for writing: " << filename << std::endl;
#endif

  // Open the file for writing
  if (append){
    file.open(filename, std::ios_base::app);
  }else{
    file.open(filename);
  }
}


template <typename T>
void StateMachine<T>::printDistances(const State<T> &state, 
    std::ostream &stream, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::printDistances: \
    Printing the distances between agents" << std::endl;
#endif

  // Print the distances between agents
  state.printDistances(stream, append, sep);
}

template <typename T>
void StateMachine<T>::printDistancesAgentToClass(const State<T> &state, 
    std::ostream &stream, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::printDistancesAgentToClass: \
    Printing the distances between agents and classes" << std::endl;
#endif

  // Print the distances between agents and classes

  state.printDistancesAgentToClass(stream, append, sep);
}

template <typename T>
void StateMachine<T>::printDistancesClassToClass(const State<T> &state, 
    std::ostream &stream, bool append, std::string sep) const
{
#ifdef DEBUG
  std::cout << "StateMachine::printDistancesClassToClass: \
    Printing the distances between classes" << std::endl;
#endif

  // Print the distances between classes
  state.printDistancesClassToClass(stream, append, sep);
}

template <typename T>
void StateMachine<T>::printParameters(std::ostream &stream)
{
#ifdef DEBUG
  std::cout << "StateMachine::printParameters: \
    Printing the parameters of the simulation" << std::endl;
#endif

  // Print the parameters of the simulation
  sp_conf.printSimulationParameters(stream);
}

template <typename T>
void StateMachine<T>::printScaledParameters()
{
#ifdef DEBUG
  std::cout << "StateMachine::printScaledParameters: \
    Printing the scaled parameters of the simulation" << std::endl;
#endif

  // Print the scaled parameters of the simulation
  sp_scaled.printSimulationParameters();
}

template <typename T>
SimulationParameters<T> StateMachine<T>::getParameters()
{
#ifdef DEBUG
  std::cout << "StateMachine::getParameters: \
    Getting the parameters of the simulation" << std::endl;
#endif
  
  // Return the parameters of the simulation
  return sp_conf;
}

template <typename T>
SimulationParameters<T> StateMachine<T>::getScaledParameters()
{
#ifdef DEBUG
  std::cout << "StateMachine::getScaledParameters: \
    Getting the scaled parameters of the simulation" << std::endl;
#endif
  
  // Return the scaled parameters of the simulation
  return sp_scaled;
}

template <typename T>
State<T> &StateMachine<T>::getState()
{
#ifdef DEBUG
  std::cout << "StateMachine::getState: \
    Getting the state of the simulation" << std::endl;
#endif

  // Return the state of the simulation
  return states[0];
}

template <typename T>
void StateMachine<T>::getDims(T* dims)
{
#ifdef DEBUG
  std::cout << "StateMachine::getDims: \
    Getting the dimensions of the simulation" << std::endl;
#endif

  // Get the dimensions of the simulation
  sp_scaled.getWorldsize(dims);
}

template <typename T>
int StateMachine<T>::getIteration()
{
  /*
#ifdef DEBUG
  std::cout << "StateMachine::getIteration: \
    Getting the iteration of the simulation" << std::endl;
#endif
*/

  // Return the iteration counter
  return states[0].getIteration();
}

template <typename T>
double StateMachine<T>::getTime()
{
  /*
#ifdef DEBUG
  std::cout << "StateMachine::getTime: \
    Getting the time of the simulation" << std::endl;
#endif
*/

  // Return the time of the simulation
  return states[0].getTimestamp();
}

template <typename T>
double StateMachine<T>::getTimelimit()
{
  /*
#ifdef DEBUG
  std::cout << "StateMachine::getTimelimit: \
    Getting the time limit of the simulation" << std::endl;
#endif
*/

  // Return the time limit of the simulation
  return sp_scaled.getSimulatedTime();
}

template <typename T>
int StateMachine<T>::getRunCounter()
{
  /*
#ifdef DEBUG
  std::cout << "StateMachine::getRunCounter: \
    Getting the run counter of the simulation" << std::endl;
#endif
*/

  // Return the run counter
  return p_global.getRunCounter();
}

template <typename T>
void StateMachine<T>::setLogLevel(int level)
{
#ifdef DEBUG
  std::cout << "StateMachine::setLogLevel: \
    Setting the log level of the simulation" << std::endl;
#endif

  // Set the log level
  p_global.setLogLevel(level);
  initLogFlags();
}

template <typename T>
int StateMachine<T>::getLogLevel()
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 4){
    std::cout << "StateMachine::getLogLevel: \
      Getting the log level of the simulation" << std::endl;
  }
#endif

  // Return the log level
  return p_global.getLogLevel();
}

template <typename T>
bool StateMachine<T>::getInitialized()
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 4){
    std::cout << "StateMachine::getInitialized: \
      Getting the initialization status of the simulation" << std::endl;
  }
#endif

  // Return the initialization status
  return initialized;
}

template <typename T>
void StateMachine<T>::printAgentClassSummaries() const
{
#ifdef DEBUG
  std::cout << "StateMachine::printAgentClassSummaries: \
    Printing the summaries of the agent classes" << std::endl;
#endif

  // Print the summaries of the agent classes
  for (int i = 0; i < agent_classes.size(); i++){
    agent_classes[i].summarize(states[0]);
  }
}


template <typename T>
T* StateMachine<T>::getAgentClassPointer(int class_id)
{
  return getAgentClassPointer(states[0], class_id);
}

template <typename T>
T* StateMachine<T>::getAgentClassPointer(State<T> &state, int class_id)
{
#ifdef DEBUG
  if (p_global.getVerbosity() > 6){
    std::cout << "StateMachine::getAgentClassPointer: \
      Getting the pointer to the agent class" << std::endl;
  }
#endif

  // Return if the class id is out of bounds
  if (class_id >= sp_scaled.num_agent_classes || class_id < 0){
    /*
    handleError("StateMachine::getAgentClassPointer: Class id out of bounds", 
        INFO);
        */
    return NULL;
  }

  // Return if the state is not initialized
  if (state.getInitialized() == false){
    handleError("StateMachine::getAgentClassPointer: State not initialized", 
        WARNING);
    if (p_global.getVerbosity() > 4) printState(state);
    return NULL;
  }

  try{
    // Get the pointer to the class 
    T *c = 
      &state.positions[sp_scaled.agent_class_params[class_id].start_index][0];

    // Get the pointer to the class
    return c;
  } catch (const std::exception &e){
    handleError(e.what(), WARNING);
    return NULL;
  }
}

template <typename T>
void StateMachine<T>::swapStates()
{
#ifdef DEBUG
  std::cout << "StateMachine::swapStates: \
    Swapping the state of the simulation" << std::endl;
  /*
  std::cout << "StateMachine::swapStates: \
    Size of states: " << states.size() << std::endl;
    */
#endif

  // Return if the state is not initialized, this way 
  // all states in the memory vector are initialized
  if (new_state.getInitialized() == false){
    handleError("StateMachine::swapStates: State not initialized", WARNING);
    return;
  }

  // Swap the state of the simulation
  for (int i = sp_scaled.memory_size-1; i > 0; i--){
    states[i] = std::move(states[i-1]);
#ifdef DEBUG
    /*
    if (p_global.getVerbosity() > 4){
      std::cout << "StateMachine::swapStates: \
        Swapping state " << i << std::endl;
      std::cout << "StateMachine::swapStates: \
        Size of state " << i << ": " << states[i].num_agents_total << std::endl;
      std::cout << "StateMachine::swapStates: \
        State " << i << " initialized: " << states[i].getInitialized() 
        << std::endl;
      std::cout << "StateMachine::swapStates: \
        State " << i << " positions: " << std::endl;
      states[i].printPositions(std::cout, 3, ",");
    }
    */
#endif
  }
  states[0] = std::move(new_state);
#ifdef DEBUG
  /*
  if (p_global.getVerbosity() > 4){
    std::cout << "StateMachine::swapStates: \
      Swapping state 0" << std::endl;
    std::cout << "StateMachine::swapStates: \
      Size of state 0: " << states[0].num_agents_total << std::endl;
    std::cout << "StateMachine::swapStates: \
      State 0 initialized: " << states[0].getInitialized() << std::endl;
    std::cout << "StateMachine::swapStates: \
      State 0 positions: " << std::endl;
    states[0].printPositions(std::cout, 3, ",");
  }
  */
#endif
  new_state.allocateMemory(sp_scaled.num_agents_total, 
      sp_scaled.num_agent_classes);
  int iteration = states[0].getIteration();
  iteration++;
  new_state.setIteration(iteration);
  new_state.setRunCounter(p_global.getRunCounter());
  double timestamp = iteration * sp_scaled.getDt();
  new_state.setTimestamp(timestamp);
  new_state.setVerbosity(p_global.getVerbosity());

}


// Explicit instantiation
template class StateMachine<float>;
template class StateMachine<double>;

/* vim: set ts=2 sw=2 tw=0 et :*/
