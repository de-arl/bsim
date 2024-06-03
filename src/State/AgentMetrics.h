#ifndef METRICS_H
#define METRICS_H

#include <iostream>


template <typename T>
struct AgentMetrics
{
  // Constructor
  AgentMetrics();

  // Destructor
  ~AgentMetrics();

  // Number of agents
  int num_agents;

  // Using doubles such that overflows are less likely to occur
  // Energy added to the system
  double* energy_added_to_system;

  // Energy removed from the system
  double* energy_removed_from_system;

  // Energy turnover
  double* energy_turnover_soll;
  double* energy_turnover_haben;


  /* Public functions */
  // Sum of energy added to the system 
  double sumEnergyAddedToSystem() const;

  // Sum of energy removed from the system
  double sumEnergyRemovedFromSystem() const;

  // Sum of energy turnover
  double sumEnergyTurnoverSoll() const;
  double sumEnergyTurnoverHaben() const;

  /* Public functions */

  // Initialize the agent metrics
  void init(int num_agents);
  
  // Print the agent metrics
  void print(std::ostream &os = std::cout, std::string sep = ",") const;

  // Print energy added to the system
  void printEnergyAddedToSystem(std::ostream &os = std::cout, 
      std::string sep = ",") const;

  // Print energy removed from the system
  void printEnergyRemovedFromSystem(std::ostream &os = std::cout, 
      std::string sep = ",") const;

  // Print energy turnover
  void printEnergyTurnoverSoll(std::ostream &os = std::cout, 
      std::string sep = ",") const;
  void printEnergyTurnoverHaben(std::ostream &os = std::cout, 
      std::string sep = ",") const;

 private:
  // Allocate memory
  void allocateMemory();

  // Free memory
  void freeMemory();

  // Set all values to zero
  void setZero();
};

#endif
/* vim: set ts=2 sw=2 tw=0 et :*/
