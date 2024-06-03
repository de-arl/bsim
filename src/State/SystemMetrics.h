#ifndef SYSTEM_METRICS_H
#define SYSTEM_METRICS_H

#include "AgentMetrics.h"
#include "State.h"
#include "helpers.h"

#include <iostream>
#include <string>
#include <iomanip>

template <typename T>
struct SystemMetrics
{
  // Constructor
  SystemMetrics();

  // Destructor
  ~SystemMetrics();

  // Agent metrics
  AgentMetrics<T> agent_metrics;


  /* Public functions */
  // Getters
  double getInitialEnergy() const { return initial_energy; }
  double getFinalEnergy() const { return final_energy; }
  double getNettoChangeInEnergy() const { return netto_change_in_energy; }

  double getEnergyAddedToSystem() const { return energy_added_to_system; }
  double getEnergyRemovedFromSystem() const 
  { return energy_removed_from_system; }
  double getNettoInOut() const { return netto_in_out; }

  double getEnergyTurnoverSoll() const { return energy_turnover_soll; }
  double getEnergyTurnoverHaben() const { return energy_turnover_haben; }
  double getNettoTurnover() const { return netto_turnover; }


  // Test if the energy is conserved
  bool isEnergyConserved() const; 

  // Initialize the system metrics
  void initialize(const State<T> &state);

  // Initialize the agent metrics
  void initAgentMetrics(int num_agents);

  // Finalize the system metrics
  void finalize(const State<T> &state);

  // Print the system metrics
  void print(std::ostream &os = std::cout) const;

  // Print the agent metrics
  void printAgentMetrics(std::ostream &os = std::cout) const;

  // Reset the system metrics
  void reset();

  // Summarize the system metrics
  void summarize();


 private:

  // Initial and final iteration
  int initial_iteration;
  int final_iteration;

  // Energy in the system
  double initial_energy;
  double final_energy;
  double netto_change_in_energy;

  // Energy crossing the system boundaries
  double energy_added_to_system;
  double energy_removed_from_system;
  double netto_in_out;

  // Energy turnover
  double energy_turnover_soll;
  double energy_turnover_haben;
  double netto_turnover;


  /* Private functions */
  // Calculate the initial energy of the system
  void calculateInitialEnergy(const State<T> &state);

  // Calculate the final energy of the system
  void calculateFinalEnergy(const State<T> &state);

  // Calculate the energy added to the system
  void calculateEnergyAddedToSystem();

  // Calculate the energy removed from the system
  void calculateEnergyRemovedFromSystem();

  // Calculate the energy turnover
  void calculateEnergyTurnoverSoll();
  void calculateEnergyTurnoverHaben();

};

#endif
/* vim: set ts=2 sw=2 tw=0 et :*/
