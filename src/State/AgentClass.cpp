#include "AgentClass.h"

template <typename T>
AgentClass<T>::AgentClass(
    const GlobalParameters<T> &global_params,
    const SimulationParameters<T> &sim_params, 
    const AgentClassParameters<T> &params)
  : global_params(global_params), 
    params(params), sim_params(sim_params)
{
#ifdef DEBUG
  /*
  std::cout << "AgentClass::AgentClass: \
    Creating an agent class with initialize constructor" << std::endl;
    */
#endif

}

template <typename T>
AgentClass<T>::~AgentClass()
{
#ifdef DEBUG
  /*
  std::cout << "AgentClass::~AgentClass: \
    Destroying an agent class" << std::endl;
    */
#endif
}

template <typename T>
void AgentClass<T>::print(std::ostream &os)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::print: \
      Printing the agent class" << std::endl;
  }
#endif
  params.print(os);
}

template <typename T>
std::string AgentClass<T>::getName() const
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::getName: \
      Getting the name of the agent class" << std::endl;
  }
#endif

  return params.name;
}

template <typename T>
int AgentClass<T>::getId()
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::getId: \
      Getting the id of the agent class" << std::endl;
  }
#endif

  return params.id;
}

template <typename T>
int AgentClass<T>::findMin(T *data, int *mask)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMin: \
      Finding the minimum of the data" << std::endl;
  }
#endif

  int index = -1;

  T min = std::numeric_limits<T>::max();

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i]){
      if (data[i] < min){
        index = i;
        min = data[i];
      }
    }
  }

#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMin: Returning the index: " 
      << index << std::endl;
  }
#endif

  return index;
}

template <typename T>
int AgentClass<T>::findMin(T *data, int *mask, int *mask2)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMin: \
      Finding the minimum of the data" << std::endl;
  }
#endif

  int index = -1;

  T min = std::numeric_limits<T>::max();

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i] && mask2[i]){
      if (data[i] < min){
        index = i;
        min = data[i];
      }
    }
  }

#ifdef DEBUG
  if (global_params.getVerbosity() > 2){
    std::cout << "AgentClass::findMin: Returning the index: " 
      << index << std::endl;
  }
#endif

  return index;
}

template <typename T>
int AgentClass<T>::findMin(T *data, int *mask, int *mask2, 
    const std::vector<int> &class_ids)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMin: \
      Finding the minimum of the data" << std::endl;
  }
#endif

  int index = -1;

  T min = std::numeric_limits<T>::max();

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i]){
      bool class_mask = false;
      for (int j = 0; j < class_ids.size(); j++){
        int class_id = class_ids[j];
        if (mask2[class_id*sim_params.num_agents_total + i]){
          class_mask = true;
        }
      }
      if (class_mask){
        if (data[i] < min){
          index = i;
          min = data[i];
        }
      }
    }
  }
#ifdef DEBUG
  if (global_params.getVerbosity() > 2){
    std::cout << "AgentClass::findMin: Returning the index: " 
      << index << std::endl;
  }
#endif

  return index;
}

template <typename T>
int AgentClass<T>::findMin(int *data, int *mask)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMin: \
      Finding the minimum of the data" << std::endl;
  }
#endif

  int index = -1;
  T min = std::numeric_limits<T>::max();

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i]){
      if (data[i] < min){
        index = i;
        min = data[i];
      }
    }
  }

  return index;
}

template <typename T>
int AgentClass<T>::findMin(int *data, int *mask, int *mask2)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMin: \
      Finding the minimum of the data" << std::endl;
  }
#endif

  int index = -1;
  T min = std::numeric_limits<T>::max();

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i] && mask2[i]){
      if (data[i] < min){
        index = i;
        min = data[i];
      }
    }
  }

  return index;
}

template <typename T>
int AgentClass<T>::findMin(int *data, int *mask, int *mask2, 
    const std::vector<int> &class_ids)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMin: \
      Finding the minimum of the data" << std::endl;
  }
#endif

  int index = -1;
  T min = std::numeric_limits<T>::max();

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i]){
      bool class_mask = false;
      for (int j = 0; j < class_ids.size(); j++){
        int class_id = class_ids[j];
        if (mask2[class_id*sim_params.num_agents_total + i]){
          class_mask = true;
        }
      }
      if (class_mask){
        if (data[i] < min){
          index = i;
          min = data[i];
        }
      }
    }
  }

  return index;
}


template <typename T>
int AgentClass<T>::findMax(T *data, int *mask)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMax: \
      Finding the maximum of the data" << std::endl;
  }
#endif

  int index = -1;
  T max = std::numeric_limits<T>::min();

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i]){
      if (data[i] > max){
        index = i;
        max = data[i];
      }
    }
  }

  return index;
}

template <typename T>
int AgentClass<T>::findMax(T *data, int *mask, int *mask2)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMax: \
      Finding the maximum of the data" << std::endl;
  }
#endif

  int index = -1;
  T max = std::numeric_limits<T>::min();

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i] && mask2[i]){
      if (data[i] > max){
        index = i;
        max = data[i];
      }
    }
  }

  return index;
}

template <typename T>
int AgentClass<T>::findMax(int *data, int *mask)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMax: \
      Finding the maximum of the data" << std::endl;
  }
#endif

  int index = -1;
  T max = std::numeric_limits<T>::min();

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i]){
      if (data[i] > data[index]){
        index = i;
        max = data[i];
      }
    }
  }

  return index;
}

template <typename T>
int AgentClass<T>::findMax(int *data, int *mask, int *mask2)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findMax: \
      Finding the maximum of the data" << std::endl;
  }
#endif

  int index = -1;
  T max = std::numeric_limits<T>::min();

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i] && mask2[i]){
      if (data[i] > data[index]){
        index = i;
        max = data[i];
      }
    }
  }

  return index;
}

template <typename T>
Point3D<T> AgentClass<T>::findCOG(Point3D<T> *positions, T *weights, T *mask)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::findCOGMass: \
      Finding the center of gravity of the data" << std::endl;
  }
#endif

  Point3D<T> cog(0.0, 0.0, 0.0);

  T sum_weights = 0.0;

  // Iterate over the data
  for (int i = params.start_index; i < params.stop_index; i++){
    if (mask[i]){
      cog += positions[i] * weights[i];
      sum_weights += weights[i];
    }
  }

  return cog /= sum_weights;
}

template <typename T>
int AgentClass<T>::getNumAudible(const State<T> &state, int agent_id) const
{
  // Check audibility
  int index = params.id * sim_params.num_agents_total + agent_id;
  int num = state.audibility_agent_to_class[index];
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::getNumAudible: \
      Checking how many agents of class " << params.id <<
      " are audible to " << agent_id << ": " << num << std::endl;
  }
#endif
  return num;
}

template <typename T>
int AgentClass<T>::getNumReachable(const State<T> &state, int agent_id) const
{
  // Check reachability
  int index = params.id * sim_params.num_agents_total + agent_id;
  int num = state.reachability_agent_to_class[index];
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::getNumReachable: \
      Checking how many agents of class " << params.id <<
      " are reachable by " << agent_id << ": " << num << std::endl;
  }
#endif
  return num;
}

template <typename T>
int AgentClass<T>::getNumContact(const State<T> &state, int agent_id) const
{
  // Check contact
  int index = params.id * sim_params.num_agents_total + agent_id;
  int num = state.contact_agent_to_class[index];
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::getNumContact \
      Checking the number of contacts between agents of class " << params.id <<
      " and agent " << agent_id << ": " << num << std::endl;
  }
#endif
  return num;
}

template <typename T>
int AgentClass<T>::getNumMembers() const
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::getNumMembers: \
      Getting the number of members of the class" << std::endl;
  }
#endif

  return params.num_agents;
}

template <typename T>
int AgentClass<T>::getStartIndex() const
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::getStartIndex: \
      Getting the start index of the class" << std::endl;
  }
#endif

  return params.start_index;
}

template <typename T>
int AgentClass<T>::getStopIndex() const
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::getStopIndex: \
      Getting the stop index of the class" << std::endl;
  }
#endif

  return params.stop_index;
}

template <typename T>
void AgentClass<T>::summarize(const State<T> &state) const
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "AgentClass::summarize: \
      Summary of class " << getName() << std::endl;
  }
#endif

  // Print header
  std::cout << "SUMMARY OF CLASS:     " << getName() << std::endl;

  // Get the inverse space scaling factor
  T inv_space_scaling_factor = sim_params.getInverseSpaceScalingFactor();

  // Calculate the inverse temporal scaling factor
  T inv_temporal_scaling_factor = 1.0 / sim_params.getDt();

  // Get the number of agents
  int num_agents = getNumMembers();
  std::cout << "  Number of agents:   " << num_agents << std::endl;
  
  // Get the start and stop indices
  int start_index = getStartIndex();
  int stop_index = getStopIndex();


  // Calculate the average energy
  T avg_energy;
  state.calculateAverage(state.energy, start_index, stop_index, avg_energy);
  std::cout << "  Average energy:     " << avg_energy << std::endl;

  /*
   * The following code is commented out because the values currently
   * do not change during the simulation
   */
  /*
  // Calculate the average movespeed
  T avg_speed;
  state.calculateAverage(state.movespeed, start_index, stop_index, avg_speed);
  avg_speed *= inv_space_scaling_factor;
  std::cout << "  Average movespeed:  " << avg_speed << std::endl;

  // Calculate the average size 
  T avg_size;
  state.calculateAverage(state.sizes, start_index, stop_index, avg_size);
  avg_size *= inv_space_scaling_factor;
  std::cout << "  Average size:       " << avg_size << std::endl;

  // Calculate the average audibility
  T avg_audibility;
  state.calculateAverage(state.audibility_threshold, 
      start_index, stop_index, avg_audibility);
  avg_audibility *= inv_space_scaling_factor;
  std::cout << "  Average audibility: " << avg_audibility << std::endl;

  // Calculate the average energy consumption per time
  T avg_energy_consumption_per_time;
  state.calculateAverage(state.energy_consumption_per_time, 
      start_index, stop_index, avg_energy_consumption_per_time);
  avg_energy_consumption_per_time *= inv_temporal_scaling_factor;
  std::cout << "  Average energy consumption per time: " 
    << avg_energy_consumption_per_time << std::endl;

  // Calculate the average energy consumption per distance
  T avg_energy_consumption_per_distance;
  state.calculateAverage(state.energy_consumption_per_distance, 
      start_index, stop_index, avg_energy_consumption_per_distance);
  avg_energy_consumption_per_distance *= sim_params.getSpaceScalingFactor();
  std::cout << "  Average energy consumption per distance: "
    << avg_energy_consumption_per_distance << std::endl;

  // Calculate the average energy uptake rate
  T avg_energy_uptake_rate;
  state.calculateAverage(state.energy_uptake_rate, 
      start_index, stop_index, avg_energy_uptake_rate);
  avg_energy_uptake_rate *= inv_temporal_scaling_factor;
  std::cout << "  Average energy uptake rate: " 
    << avg_energy_uptake_rate << std::endl;

  */
}



// Explicit instantiation
template class AgentClass<float>;
template class AgentClass<double>;


/* vim: set ts=2 sw=2 tw=0 et :*/
