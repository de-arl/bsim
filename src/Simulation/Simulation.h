#ifndef SIMULATION_H
#define SIMULATION_H

#include "StateMachine.h"
#include "State.h"
#include "SimulationParameters.h"

#include <time.h>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <cmath>
#include <omp.h>

enum class SimulationState
{
  UNINITIALIZED,
  INITIALIZED,
  STOPPED,
  PAUSED,
  RUNNING
};

// Forward declaration of SimulationImGuiHandle
template <typename T>
class SimulationImGuiHandle;

template <typename T>
class Simulation
{
 public:
 
  // Constructor
  Simulation(StateMachine<T> &state_machine, 
      GlobalParameters<T> &global_params); 
  // Destructor
  ~Simulation();
 
  // Initialize the simulation
  void init(SimulationParameters<T> &p);

  // Load the simulation
  void load(std::string filename);

  // Reset the simulation
  void reset();
 
  // Run the simulation loop for the gui 
  void run();

  // Run the simulation for a given number of iterations and repeats
  void run(int num_iterations, int num_repeats);
  void runHeadless();

  // Pause the simulation
  void pause();

  // Stop the simulation
  void stop();

  // Start the simulation
  void start();

  // Advance the simulation by one iteration
  void advance();

  // Get/set the iteration rate
  double getIterationRate();
  double getActualIterationRate();
  void setIterationRate(double rate);
  void setUnlimitedIterationRate(bool value);
  bool getUnlimitedIterationRate();

  // Get the iteration counter
  int getIteration();

  // Get the time of the simulation
  double getTime();

  // Get/set the state of the simulation
  SimulationState getState();
  void setState(SimulationState state);
  std::string getStateString();

  // Print the state of the simulation
  void printState();

  // Save the state of the simulation
  void saveState();
  void saveState(std::string dirname, bool append = false, 
      std::string sep = ",");

  // Get pointer to data
  T* getAgentClassPointer(int class_id);

  // Get the dimensions of the simulation 
  void getDims(T *dims);

  // Get/set the output directory
  void setOutputDir(std::string dir);
  std::string getOutputDir();

  // Get the run counter
  int getRunCounter();

  // Get the timestamp
  std::string getTimeString();

  // Get set the gui flag
  void setGuiFlag(bool flag);
  bool getGuiFlag();

 
 private:

  // State machine
  StateMachine<T> &state_machine;

  // Status of the simulation
  SimulationState status;

  // Simulation parameters
  SimulationParameters<T> sim_params;

  // Global parameters 
  GlobalParameters<T> &global_params;

  // Iteration rate (iterations per second)
  double iteration_rate; // value which is set by the user
  double actual_iteration_rate; // value which is calculated
  bool unlimited_iteration_rate;
  int last_iteration;

  // Last iteration time
  std::chrono::time_point<std::chrono::high_resolution_clock> 
    last_iteration_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> 
    last_iteration_time_for_actual_rate;

  // Output directory
  std::string output_dir;

  // Run finished flag
  bool run_finished;

  // Reset counters
  void resetCounters();

  // Run flag for the gui
  bool gui_flag;
 

  friend class SimulationImGuiHandle<T>;
 
};
#endif //SIMULATION_H
/* vim: set ts=2 sw=2 tw=0 et :*/
