#include "Simulation.h"

template <typename T>
Simulation<T>::Simulation(
    StateMachine<T> &state_machine, 
    GlobalParameters<T> &global_params)
  : state_machine(state_machine), global_params(global_params), 
  actual_iteration_rate(0.0), iteration_rate(1.0), 
  unlimited_iteration_rate(true),
  last_iteration(0),
  status(SimulationState::UNINITIALIZED),
  output_dir(global_params.getRunDirectory()),
  run_finished(false),
  gui_flag(false)
{ 
#ifdef DEBUG
  std::cout << "Simulation::Simulation: \
    Default constructor of Simulation" << std::endl;
#endif

  // Initialize the parameters
  init(global_params.getSimulationParameters());

  // Get the status of the state machine
  if (state_machine.getInitialized()){
    status = SimulationState::INITIALIZED;
  } else {
    status = SimulationState::UNINITIALIZED;
  }
}

template <typename T>
Simulation<T>::~Simulation()
{
#ifdef DEBUG
  std::cout << "Simulation::~Simulation: \
    Destructor of Simulation" << std::endl;
#endif
}

template <typename T>
void Simulation<T>::init(SimulationParameters<T> &p)
{
#ifdef DEBUG
  std::cout << "Simulation::init: \
    Initializing the simulation" << std::endl;
  if (global_params.getVerbosity() > 1){
    p.printSimulationParameters();
    // Output directory
    std::cout << "Output directory: " 
      << global_params.getRunDirectory() << std::endl;
  }
#endif

  // Set the parameters
  sim_params = p;

  // Return if the simulation parameters are not initialized
  if (!sim_params.getSimParamsInitialized()){
    return;
  }

  // Return if the number of agents is zero
  if (sim_params.getNumAgents() == 0){
    return;
  }

  // Reset counters
  resetCounters();

  // Initialize the state machine
  state_machine.init(sim_params);
  status = SimulationState::INITIALIZED;
  global_params.initRunDirectory();
  output_dir = global_params.getRunDirectory();

#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "Simulation::init: Output directory: " 
      << output_dir << std::endl;
    std::cout << "Simulation::init: Output directory global: " 
      << global_params.getRunDirectory() << std::endl;
  }
#endif
}

template <typename T>
void Simulation<T>::load(std::string filename)
{
#ifdef DEBUG
  std::cout << "Simulation::load: \
    Loading the simulation" << std::endl;
#endif

  // Reset counters
  resetCounters();

  // Load the parameters
  sim_params.setSimulationConfigFile(filename);
  sim_params.parseSimulationConfig();

  // Load the simulation
  state_machine.loadStateToml(filename);
  status = SimulationState::INITIALIZED;
}

template <typename T>
void Simulation<T>::reset()
{
#ifdef DEBUG
  std::cout << "Simulation::reset: \
    Resetting the simulation" << std::endl;
#endif

  status = SimulationState::UNINITIALIZED;

  // Reset the state machine
  state_machine.reset();
  output_dir = global_params.getRunDirectory();
  status = SimulationState::INITIALIZED;
#ifdef DEBUG
  if (global_params.getVerbosity() > 1){
    std::cout << "Simulation::reset: Output directory: " 
      << output_dir << std::endl;
  }
#endif
  resetCounters();
}

template <typename T>
void Simulation<T>::run()
{
#ifdef DEBUG
  std::cout << "Simulation::run: \
    Running the simulation" << std::endl;
      std::cout << "Number of threads:\t"
        <<omp_get_num_threads()<<std::endl;
      std::cout << "Number of processors:\t"
        <<omp_get_num_procs()<<std::endl;
      std::cout << "Number of devices:\t"
        <<omp_get_num_devices()<<std::endl;
      std::cout << "Number of teams:\t"
        <<omp_get_num_teams()<<std::endl;
      std::cout << "Number of threads in a team:\t"
        <<omp_get_team_size(0)<<std::endl;
      std::cout << "Max threads:\t"
        <<omp_get_max_threads()<<std::endl;
      // Print thread number
      std::cout << "Thread number (simulation thread):\t"
        <<omp_get_thread_num()<<std::endl;
#endif

  last_iteration = 0;
  last_iteration_time = std::chrono::high_resolution_clock::now();
  last_iteration_time_for_actual_rate = 
    std::chrono::high_resolution_clock::now();

  // Wait until the GUI is ready
  int wait_counter = 0;
  while (!gui_flag && wait_counter++ < 100){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
#ifdef DEBUG
      std::cout << "Simulation::run: \
        GUI is not ready, waiting" << std::endl;
#endif
  }

  if (!gui_flag){
    handleError("Simulation::run: GUI couldn't be started", CRITICAL);
  }

  // Endless loop
  while (gui_flag){
#ifdef DEBUG
    if (global_params.getVerbosity() > 6){
      std::cout << "Simulation::run: \
        Running the simulation loop" << std::endl;
    }
#endif

    // If not initialized, break
    if (status == SimulationState::UNINITIALIZED){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      if (global_params.getVerbosity() > 6){
        std::cout << "Simulation::run: \
          Simulation is not initialized" << std::endl;
      }
      continue;
    }

    if (status < SimulationState::RUNNING){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      if (global_params.getVerbosity() > 6){
        std::cout << "Simulation::run: \
          Simulation is not running" << std::endl;
      }
      continue;
    }

    // Advance the state machine
    last_iteration_time = std::chrono::high_resolution_clock::now();
    state_machine.updateState();

    // Calculate the actual iteration rate
    if (state_machine.getIteration() - last_iteration > 100){
      auto current_time = std::chrono::high_resolution_clock::now();
      auto duration = 
        std::chrono::duration_cast<std::chrono::milliseconds>(
          current_time - last_iteration_time_for_actual_rate);
      actual_iteration_rate = 
        1000.0 * (state_machine.getIteration() - last_iteration) 
        / duration.count();
      last_iteration = state_machine.getIteration();
      last_iteration_time_for_actual_rate = current_time;
    }


    // If the simulation timelimit has been reached, stop the simulation
    // and save the state
    if (state_machine.getTimelimit() > 1.0 
        && state_machine.getTime() >= state_machine.getTimelimit()
        && run_finished == false
        ){
      std::cout << "Simulation::run: \
        Simulation timelimit reached" << std::endl;
      std::cout << "Simulation::run: \
        Simulation time: " << state_machine.getTime() << std::endl;
      std::cout << "Simulation::run: \
        Saving the state of the simulation" << std::endl;
      status = SimulationState::STOPPED;
      state_machine.finalize(output_dir);
      run_finished = true;
      continue;
    }

    // Continue with the next iteration if the iteration rate is unlimited
    if (unlimited_iteration_rate){
      continue;
    }

    // Wait until the the time for one iteration has passed, 
    // depending on the iteration rate
    if (iteration_rate <= 0.0){
      status = SimulationState::PAUSED;
      continue;
    }

    while (true){
      auto current_time = std::chrono::high_resolution_clock::now();
      auto duration = 
        std::chrono::duration_cast<std::chrono::milliseconds>(
          current_time - last_iteration_time);
      if (iteration_rate > 0.0){
        if (duration.count() >= 1000 / iteration_rate){
          break;
        }
      }
    } // End waiting loop

  }// Endless loop
}

template <typename T>
void Simulation<T>::run(int num_iterations, int num_repeats)
{
#ifdef DEBUG
  std::cout << "Simulation::run: \
    Running the simulation for " 
    << num_iterations << " iterations" << std::endl;
#endif


  // Set the state to running
  status = SimulationState::RUNNING;

  // Run the simulation for a given number of iterations and repeats
  for (int i = 0; i < num_repeats; i++){
    for (int j = 0; j < num_iterations; j++){
      // Advance the state machine
      state_machine.updateState();
#ifdef DEBUG
      state_machine.printState();
#endif
    }
    // Save the final state
    state_machine.finalize(output_dir);
    // Reset the state machine
    if (num_repeats > 1){
      state_machine.reset();
    }
  }
  status = SimulationState::STOPPED;
  
}

template <typename T>
void Simulation<T>::runHeadless()
{
#ifdef DEBUG
  std::cout << "Simulation::runHeadless: \
    Running the simulation" << std::endl;
#endif

  if (global_params.getVerbosity() > 2){
    sim_params.printSimulationParameters();
  }

  // Calculate the number of iterations
  T num_iter = std::ceil(sim_params.getSimulatedTime() / sim_params.getDt());
  int num_iterations = static_cast<int>(num_iter);
  int num_repeats = sim_params.getNumRuns();
  std::cout << "Simulation::runHeadless: \
    Running the simulation for " 
    << num_iterations << " iterations and " 
    << num_repeats << " repeats"
    << std::endl;
  return run(num_iterations, num_repeats);
}

template <typename T>
void Simulation<T>::stop()
{
#ifdef DEBUG
  std::cout << "Simulation::stop: \
    Stopping the simulation" << std::endl;
#endif

  status = SimulationState::STOPPED;
  state_machine.finalize(output_dir);
}

template <typename T>
void Simulation<T>::start()
{
#ifdef DEBUG
  std::cout << "Simulation::start: \
    Starting the simulation" << std::endl;
#endif

  if (status == SimulationState::UNINITIALIZED){
#ifdef DEBUG
    std::cout << "Simulation::start: \
      Simulation is not initialized" << std::endl;
#endif
    return;
  }

  status = SimulationState::RUNNING;
}

template <typename T>
void Simulation<T>::pause()
{
#ifdef DEBUG
  std::cout << "Simulation::pause: \
    Pausing the simulation" << std::endl;
#endif

  status = SimulationState::PAUSED;
}

template <typename T>
void Simulation<T>::advance()
{
#ifdef DEBUG
  std::cout << "Simulation::advance: \
    Advancing the simulation by one iteration" << std::endl;
#endif

  if (status == SimulationState::UNINITIALIZED){
    handleError("Simulation::advance: Simulation is not initialized", 
        WARNING);
    return;
  }

  state_machine.updateState();

}

template <typename T>
double Simulation<T>::getIterationRate()
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 6){
    std::cout << "Simulation::getIterationRate: \
      Getting the iteration rate of the simulation" << std::endl;
  }
#endif

  return iteration_rate;
}

template <typename T>
double Simulation<T>::getActualIterationRate()
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 6){
    std::cout << "Simulation::getActualIterationRate: \
      Getting the actual iteration rate of the simulation" << std::endl;
  }
#endif

  return actual_iteration_rate;
}

template <typename T>
void Simulation<T>::setUnlimitedIterationRate(bool unlimited)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 6){
    std::cout << "Simulation::setUnlimitedIterationRate: \
      Setting the iteration rate of the simulation to unlimited" << std::endl;
  }
#endif
  
  unlimited_iteration_rate = unlimited;
}

template <typename T>
bool Simulation<T>::getUnlimitedIterationRate()
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 6){
    std::cout << "Simulation::getUnlimitedIterationRate: \
      Getting the unlimited iteration rate of the simulation" << std::endl;
  }
#endif

  return unlimited_iteration_rate;
}

template <typename T>
void Simulation<T>::setIterationRate(double rate)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 6){
    std::cout << "Simulation::setIterationRate: \
      Setting the iteration rate of the simulation" << std::endl;
  }
#endif

  iteration_rate = rate;
}

template <typename T>
int Simulation<T>::getIteration()
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 6){
    std::cout << "Simulation::getIteration: \
      Getting the iteration of the simulation" << std::endl;
  }
#endif

  return state_machine.getIteration();
}

template <typename T>
double Simulation<T>::getTime()
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 6){
    std::cout << "Simulation::getTime: \
      Getting the time of the simulation" << std::endl;
  }
#endif

  return state_machine.getTime();
}

template <typename T>
SimulationState Simulation<T>::getState()
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 6){
    std::cout << "Simulation::getState: \
      Getting the state of the simulation" << std::endl;
  }
#endif

  return status;
}

template <typename T>
void Simulation<T>::setState(SimulationState status)
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 4){
    std::cout << "Simulation::setState: \
      Setting the status of the simulation" << std::endl;
  }
#endif
  this->status = status;
}

template <typename T>
std::string Simulation<T>::getStateString()
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 6){
    std::cout << "Simulation::getStateString: \
      Getting the state of the simulation as a string" << std::endl;
  }
#endif

  switch (status){
    case SimulationState::UNINITIALIZED:
      return "Uninitialized";
    case SimulationState::INITIALIZED:
      return "Initialized";
    case SimulationState::STOPPED:
      return "Stopped";
    case SimulationState::PAUSED:
      return "Paused";
    case SimulationState::RUNNING:
      return "Running";
    default:
      return "Uninitialized";
  }
}

template <typename T>
void Simulation<T>::printState()
{
#ifdef DEBUG
  std::cout << "Simulation::printState: \
    Printing the state of the simulation" << std::endl;
#endif

  state_machine.printState();
}


template <typename T>
void Simulation<T>::saveState()
{
#ifdef DEBUG
  std::cout << "Simulation::saveState: \
    Saving the state of the simulation" << std::endl;
#endif

  bool append = true;
  saveState(global_params.getRunDirectory(), append, ",");
}

template <typename T>
void Simulation<T>::saveState(std::string dirname, bool append, std::string sep)
{
#ifdef DEBUG
  std::cout << "Simulation::saveState: \
    Saving the state of the simulation" << std::endl;
#endif

  state_machine.saveState(dirname, append, sep);
}

template <typename T>
T* Simulation<T>::getAgentClassPointer(int class_id)
{
#ifdef DEBUG
  /*
  std::cout << "Simulation::getAgentClassPointer: \
    Getting the agent class pointer" << std::endl;
    */
#endif

  return state_machine.getAgentClassPointer(class_id);
}

template <typename T>
void Simulation<T>::getDims(T *dims)
{
#ifdef DEBUG
  std::cout << "Simulation::getDims: \
    Getting the dimensions of the simulation" << std::endl;
#endif

  return state_machine.getDims(dims); 
} 

template <typename T>
void Simulation<T>::setOutputDir(std::string dir)
{
#ifdef DEBUG
  std::cout << "Simulation::setOutputDir: \
    Setting the output directory: " << dir <<std::endl;
#endif
  
  global_params.setOutputDirectory(dir);
  output_dir = global_params.getRunDirectory();
}

template <typename T>
std::string Simulation<T>::getOutputDir()
{
#ifdef DEBUG
  if (global_params.getVerbosity() > 6){
    std::cout << "Simulation::getOutputDir: Output directory: " 
      << output_dir << std::endl;
  }
#endif

  return output_dir;
}

template <typename T>
int Simulation<T>::getRunCounter()
{
  /*
#ifdef DEBUG
  std::cout << "Simulation::getRunCounter: \
    Getting the run counter of the simulation" << std::endl;
#endif
*/

  return state_machine.getRunCounter();
}

template <typename T>
std::string Simulation<T>::getTimeString()
{
  /*
#ifdef DEBUG
  std::cout << "Simulation::getTimeString: \
    Getting the time string of the simulation" << std::endl;
#endif
*/

  return global_params.getTimeString();
}

template <typename T>
void Simulation<T>::setGuiFlag(bool flag)
{
#ifdef DEBUG
  std::cout << "Simulation::setGuiFlag: \
    Setting the GUI flag of the simulation" << std::endl;
#endif

  gui_flag = flag;
}

template <typename T>
bool Simulation<T>::getGuiFlag()
{
#ifdef DEBUG
  std::cout << "Simulation::getGuiFlag: \
    Getting the GUI flag of the simulation" << std::endl;
#endif

  return gui_flag;
}

template <typename T>
void Simulation<T>::resetCounters()
{
#ifdef DEBUG
  std::cout << "Simulation::resetCounters: \
    Resetting the counters of the simulation" << std::endl;
#endif
  iteration_rate = 1.0;
  actual_iteration_rate = 0.0;
  unlimited_iteration_rate = true;
  last_iteration = 0;
  run_finished = false;
}

// Explicit instantiation of the Simulation class
template class Simulation<float>;
template class Simulation<double>;

/* vim: set ts=2 sw=2 tw=0 et :*/
