#include "AgentMetrics.h"

template <typename T>
AgentMetrics<T>::AgentMetrics()
  :num_agents(0),
  energy_added_to_system(nullptr),
  energy_removed_from_system(nullptr),
  energy_turnover_soll(nullptr),
  energy_turnover_haben(nullptr)
{
#ifdef DEBUG
  std::cout << "AgentMetrics::AgentMetrics: Default Constructor" << std::endl;
#endif

}

template <typename T>
AgentMetrics<T>::~AgentMetrics()
{
#ifdef DEBUG
  std::cout << "AgentMetrics::~AgentMetrics: Destructor" << std::endl;
#endif

  // Free memory
  freeMemory();
}

template <typename T>
void AgentMetrics<T>::init(int num_agents)
{
#ifdef DEBUG
  std::cout << "AgentMetrics::initialize: \
    initializing agent metrics for " << num_agents << " agents" << std::endl;
#endif

  // Set the number of agents
  this->num_agents = num_agents;

  // Allocate memory
  allocateMemory();

  // Set all values to zero
  setZero();
}

template <typename T>
void AgentMetrics<T>::print(std::ostream &os, std::string sep) const
{
#ifdef DEBUG
  std::cout << "AgentMetrics::print: Printing agent metrics" << std::endl;
#endif

  os << "Agent Metrics" << std::endl;
  os << "Energy added to the system: " 
    << sumEnergyAddedToSystem() << std::endl;
  os << "Energy removed from the system: " 
    << sumEnergyRemovedFromSystem() << std::endl;
  os << "Energy turnover (soll): "
    << sumEnergyTurnoverSoll() << std::endl;
  os << "Energy turnover (haben): "
    << sumEnergyTurnoverHaben() << std::endl;

  /*
  printEnergyAddedToSystem(os, sep);
  printEnergyRemovedFromSystem(os, sep);
  printEnergyTurnoverSoll(os, sep);
  printEnergyTurnoverHaben(os, sep);
  */

}

template <typename T>
void AgentMetrics<T>::printEnergyAddedToSystem(std::ostream &os, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "AgentMetrics::printEnergyAddedToSystem: \
    Printing energy added to the system" << std::endl;
#endif

  os << "Energy added to the system" << std::endl;

  for (int i = 0; i < num_agents; i++){
    os << energy_added_to_system[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void AgentMetrics<T>::printEnergyRemovedFromSystem(std::ostream &os, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "AgentMetrics::printEnergyRemovedFromSystem: \
    Printing energy removed from the system" << std::endl;
#endif

  os << "Energy removed from the system" << std::endl;

  for (int i = 0; i < num_agents; i++){
    os << energy_removed_from_system[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void AgentMetrics<T>::printEnergyTurnoverSoll(std::ostream &os, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "AgentMetrics::printEnergyTurnoverSoll: \
    Printing energy turnover (soll)" << std::endl;
#endif

  os << "Energy turnover (soll)" << std::endl;

  for (int i = 0; i < num_agents; i++){
    os << energy_turnover_soll[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void AgentMetrics<T>::printEnergyTurnoverHaben(std::ostream &os, 
    std::string sep) const
{
#ifdef DEBUG
  std::cout << "AgentMetrics::printEnergyTurnoverHaben: \
    Printing energy turnover (haben)" << std::endl;
#endif

  os << "Energy turnover (haben)" << std::endl;

  for (int i = 0; i < num_agents; i++){
    os << energy_turnover_haben[i] << sep;
  }
  os << std::endl;
}

template <typename T>
void AgentMetrics<T>::allocateMemory()
{
#ifdef DEBUG
  std::cout << "AgentMetrics::allocateMemory \
    allocating memory for metrics of " << num_agents << "agents" 
    << std::endl;
#endif

  // Free memory, if necessary
  freeMemory();

  // Alignment for SIMD compiler optimizations in bytes
  size_t alignment = 1;

  while (alignment < num_agents){
    alignment *= 2;
  }

  int size = alignment;

  // Energy added to the system
  energy_added_to_system = 
    (double *)aligned_alloc(alignment, size * sizeof(double));

  // Energy removed from the system
  energy_removed_from_system = 
    (double *)aligned_alloc(alignment, size * sizeof(double));

  // Energy turnover
  energy_turnover_soll = 
    (double *)aligned_alloc(alignment, size * sizeof(double));
  energy_turnover_haben = 
    (double *)aligned_alloc(alignment, size * sizeof(double));
  
}

template <typename T>
void AgentMetrics<T>::freeMemory()
{
#ifdef DEBUG
  std::cout << "AgentMetrics::freeMemory: \
    releasing memory" << std::endl;
#endif

  // Energy added to the system
  if (energy_added_to_system != nullptr){
    free(energy_added_to_system);
    energy_added_to_system = nullptr;
  }

  // Energy removed from the system
  if (energy_removed_from_system != nullptr){
    free(energy_removed_from_system);
    energy_removed_from_system = nullptr;
  }

  // Energy turnover
  if (energy_turnover_haben != nullptr){
    free(energy_turnover_haben);
    energy_turnover_haben = nullptr;
  }
  if (energy_turnover_soll != nullptr){
    free(energy_turnover_soll);
    energy_turnover_soll = nullptr;
  }
}

template <typename T>
void AgentMetrics<T>::setZero()
{
#ifdef DEBUG
  std::cout << "AgentMetrics::setZero: \
    setting all values to zero" << std::endl;
#endif

  // Energy added to the system
  for (int i = 0; i < num_agents; i++){
    energy_added_to_system[i] = 0.0;
  }

  // Energy removed from the system
  for (int i = 0; i < num_agents; i++){
    energy_removed_from_system[i] = 0.0;
  }

  // Energy turnover
  for (int i = 0; i < num_agents; i++){
    energy_turnover_soll[i] = 0.0;
    energy_turnover_haben[i] = 0.0;
  }
}

template <typename T>
double AgentMetrics<T>::sumEnergyAddedToSystem() const
{
#ifdef DEBUG
  std::cout << "AgentMetrics::sumEnergyAddedToSystem: \
    summing up energy added to the system" << std::endl;
#endif

  T sum = 0.0;

  for (int i = 0; i < num_agents; i++){
    sum += energy_added_to_system[i];
  }

  return sum;
}

template <typename T>
double AgentMetrics<T>::sumEnergyRemovedFromSystem() const
{
#ifdef DEBUG
  std::cout << "AgentMetrics::sumEnergyRemovedFromSystem: \
    summing up energy removed from the system" << std::endl;
#endif

  T sum = 0.0;

  for (int i = 0; i < num_agents; i++){
    sum += energy_removed_from_system[i];
  }

  return sum;
}

template <typename T>
double AgentMetrics<T>::sumEnergyTurnoverSoll() const
{
#ifdef DEBUG
  std::cout << "AgentMetrics::sumEnergyTurnoverSoll: \
    summing up energy turnover (soll)" << std::endl;
#endif

  T sum = 0.0;

  for (int i = 0; i < num_agents; i++){
    sum += energy_turnover_soll[i];
  }

  return sum;
}

template <typename T>
double AgentMetrics<T>::sumEnergyTurnoverHaben() const
{
#ifdef DEBUG
  std::cout << "AgentMetrics::sumEnergyTurnoverHaben: \
    summing up energy turnover (haben)" << std::endl;
#endif

  T sum = 0.0;

  for (int i = 0; i < num_agents; i++){
    sum += energy_turnover_haben[i];
  }

  return sum;
}


// Explicit instantation
template class AgentMetrics<float>;
template class AgentMetrics<double>;
/* vim: set ts=2 sw=2 tw=0 et :*/
