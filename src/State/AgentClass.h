#ifndef AGENTCLASS_H
#define AGENTCLASS_H

#include "AgentClassParameters.h"
#include "SimulationParameters.h"
#include "GlobalParameters.h"
#include "State.h"

/* Used to retrieve class metrics and perform class reduction operations */

template <typename T>
class AgentClass
{
 public:

  // Constructor
  AgentClass(
      const GlobalParameters<T> &global_params,
      const SimulationParameters<T> &sim_params,
      const AgentClassParameters<T> &params);

  // Destructor
  ~AgentClass();

  // Print the agent class
  void print(std::ostream &os = std::cout);

  // Get the name of the agent class
  std::string getName() const;

  // Get the id of the agent class
  int getId();

  // Reductions, using a mask, return the index of the min, max
  int findMin(T *data, int *mask);
  int findMin(T *data, int *mask, int *mask2);
  int findMin(T *data, int *mask, int *mask2, 
      const std::vector<int> &class_ids);
  int findMin(int *data, int *mask);
  int findMin(int *data, int *mask, int *mask2);
  int findMin(int *data, int *mask, int *mask2, 
      const std::vector<int> &class_ids);
  int findMax(T *data, int *mask);
  int findMax(T *data, int *mask, int *mask2);
  int findMax(int *data, int *mask);
  int findMax(int *data, int *mask, int *mask2);
  

  // Find the center of gravity, using a mask and weights
  Point3D<T> findCOG(Point3D<T> *positions, T *weights, T *mask); 

  // Find out how many agents of the class are audible
  int getNumAudible(const State<T> &state, int agent_id) const;

  // Find out wether at least one agent of the class is reachable by
  // a given agent
  int getNumReachable(const State<T> &state, int agent_id) const;

  // Find out wether at least one agent of the class is in contact with
  // a given agent
  int getNumContact(const State<T> &state, int agent_id) const;

  // Get the number of class members
  int getNumMembers() const;

  // Get start and stop indices
  int getStartIndex() const;
  int getStopIndex() const;

  // Summarize the class
  void summarize(const State<T> &state) const;

 private:

  // Global parameters
  const GlobalParameters<T> &global_params;

  // Simulation parameters
  const SimulationParameters<T> &sim_params;

  // Parameters of the agent class
  const AgentClassParameters<T> &params;

};


#endif // AGENTCLASS_H
/* vim: set ts=2 sw=2 tw=0 et :*/
