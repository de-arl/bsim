#include "SystemMetrics.h"

template <typename T>
SystemMetrics<T>::SystemMetrics()
  :
  initial_iteration(0),
  final_iteration(0),
  initial_energy(0.0),
  final_energy(0.0),
  netto_change_in_energy(0.0),
  energy_added_to_system(0.0),
  energy_removed_from_system(0.0),
  netto_in_out(0.0),
  energy_turnover_soll(0.0),
  energy_turnover_haben(0.0),
  netto_turnover(0.0)
{
#ifdef DEBUG
  std::cout << "SystemMetrics::SystemMetrics: Default Constructor" 
    << std::endl;
#endif
}

template <typename T>
SystemMetrics<T>::~SystemMetrics()
{
#ifdef DEBUG
  std::cout << "SystemMetrics::~SystemMetrics: Destructor" << std::endl;
#endif
}

template <typename T>
void SystemMetrics<T>::calculateInitialEnergy(const State<T> &state)
{
#ifdef DEBUG
  std::cout 
    << "SystemMetrics::calculateInitialEnergy: Calculating initial energy" 
    << std::endl;
#endif
  initial_energy = state.sumEnergy();
}

template <typename T>
void SystemMetrics<T>::calculateFinalEnergy(const State<T> &state)
{
#ifdef DEBUG
  std::cout 
    << "SystemMetrics::calculateFinalEnergy: Calculating final energy" 
    << std::endl;
#endif
  final_energy = state.sumEnergy();
}

template <typename T>
void SystemMetrics<T>::calculateEnergyAddedToSystem()
{
#ifdef DEBUG
  std::cout 
    << "SystemMetrics::calculateEnergyAddedToSystem: \
    Calculating energy added to system" 
    << std::endl;
#endif
  energy_added_to_system = agent_metrics.sumEnergyAddedToSystem();
}

template <typename T>
void SystemMetrics<T>::calculateEnergyRemovedFromSystem()
{
#ifdef DEBUG
  std::cout 
    << "SystemMetrics::calculateEnergyRemovedFromSystem: \
    Calculating energy removed from system" 
    << std::endl;
#endif
  energy_removed_from_system = agent_metrics.sumEnergyRemovedFromSystem();
}

template <typename T>
void SystemMetrics<T>::calculateEnergyTurnoverSoll()
{
#ifdef DEBUG
  std::cout 
    << "SystemMetrics::calculateEnergyTurnoverSoll: \
    Calculating energy turnover (soll)" 
    << std::endl;
#endif

  energy_turnover_soll = agent_metrics.sumEnergyTurnoverSoll();
}

template <typename T>
void SystemMetrics<T>::calculateEnergyTurnoverHaben()
{
#ifdef DEBUG
  std::cout 
    << "SystemMetrics::calculateEnergyTurnoverHaben: \
    Calculating energy turnover (haben)" 
    << std::endl;
#endif

  energy_turnover_haben = agent_metrics.sumEnergyTurnoverHaben();
}

template <typename T>
bool SystemMetrics<T>::isEnergyConserved() const
{
#ifdef DEBUG
  std::cout 
    << "SystemMetrics::isEnergyConserved: \
    Testing if energy is conserved" 
    << std::endl;
#endif

  double diff = netto_change_in_energy 
    - (energy_added_to_system - energy_removed_from_system);
  return (nearlyEqual(diff, 0.0));
}

template <typename T>
void SystemMetrics<T>::initialize(const State<T> &state)
{
#ifdef DEBUG
  std::cout 
    << "SystemMetrics::initialize: Initializing system metrics" 
    << std::endl;
#endif

  // Set the initial iteration
  initial_iteration = state.getIteration();

  // Energy in the system
  calculateInitialEnergy(state);
  netto_change_in_energy = 0.0;

  // Energy crossing the system boundaries
  energy_added_to_system = 0.0;
  energy_removed_from_system = 0.0;
  netto_in_out = 0.0;
  
  // Energy turnover
  energy_turnover_soll = 0.0;
  energy_turnover_haben = 0.0;
  netto_turnover = 0.0;
}

template <typename T>
void SystemMetrics<T>::initAgentMetrics(int num_agents)
{
#ifdef DEBUG
  std::cout 
    << "SystemMetrics::initAgentMetrics: \
    Initializing agent metrics" 
    << std::endl;
#endif

  agent_metrics.init(num_agents);
}

template <typename T>
void SystemMetrics<T>::finalize(const State<T> &state)
{
#ifdef DEBUG
  std::cout 
    << "SystemMetrics::finalize: Finalizing system metrics" 
    << std::endl;
#endif

  // Set the final iteration
  final_iteration = state.getIteration();

  // Calculate the final energy of the system
  calculateFinalEnergy(state);
  netto_change_in_energy = final_energy - initial_energy;

  // Calculate the energy that crossed the system boundary
  calculateEnergyAddedToSystem();
  calculateEnergyRemovedFromSystem();
  netto_in_out = energy_added_to_system - energy_removed_from_system;

  // Calculate the energy turnover
  calculateEnergyTurnoverSoll();
  calculateEnergyTurnoverHaben();
  netto_turnover = energy_turnover_soll - energy_turnover_haben;
}

template <typename T>
void SystemMetrics<T>::print(std::ostream &os) const
{
#ifdef DEBUG
  os 
    << "SystemMetrics::print: Printing system metrics" 
    << std::endl;
#endif

  double total_balance = 
    netto_change_in_energy - netto_in_out + netto_turnover;
  double dissipated_energy_per_iteration = 
    total_balance / (final_iteration - initial_iteration);

  // Set to full precision
  os << "SystemMetrics::print: Sum of energy in the system: " << std::endl;
  os << "SystemMetrics::print: Initial energy: " << initial_energy << std::endl;
  os << "SystemMetrics::print: Final energy: " << final_energy << std::endl;
  os << "SystemMetrics::print: Netto change in energy: " 
    << netto_change_in_energy << std::endl;

  os << "SystemMetrics::print: Energy exchanged within agents: " << std::endl;
  os << "SystemMetrics::print: Energy turnover (soll): " << energy_turnover_soll << std::endl;
  os << "SystemMetrics::print: Energy turnover (haben): " << energy_turnover_haben << std::endl;
  os << "SystemMetrics::print: Turnover balance: " 
    << netto_turnover << std::endl;

  os << "SystemMetrics::print: Energy that crossed the system boundary: " << std::endl;
  os << "SystemMetrics::print: Energy added to system: " 
    << energy_added_to_system << std::endl;
  os << "SystemMetrics::print: Energy removed from system: " 
    << energy_removed_from_system << std::endl;
  os << "SystemMetrics::print: Netto in/out: " << netto_in_out << std::endl;

  os << "SystemMetrics::print: Total energy balance \
    (netto change in energy - netto in/out + netto turnover): " 
    << total_balance << std::endl;
  os << "SystemMetrics::print: NOTE: The balance should be zero in theory, but due to \n\
    rounding errors, the balance might be non-zero and can be interpreted \n\
    as dissipated energy, if it is reasonably small, if \n\
    not, check for errors in the code."  << std::endl;

  os << "SystemMetrics::print: Iterations measured: " 
    << final_iteration - initial_iteration << std::endl;
  os << "SystemMetrics::print: Dissipated energy (average error) per iteration: " 
    << dissipated_energy_per_iteration << std::endl;
}

template <typename T>
void SystemMetrics<T>::printAgentMetrics(std::ostream &os) const
{
#ifdef DEBUG
  os 
    << "SystemMetrics::printAgentMetrics: Printing agent metrics" 
    << std::endl;
#endif

  agent_metrics.print(os);
}

// Explicit instantiation
template struct SystemMetrics<float>;
template struct SystemMetrics<double>;
/* vim: set ts=2 sw=2 tw=0 et :*/
