#include <iostream>
#include <omp.h>

#include "StateMachine.h"
#include "Results.h"
#include "SimulationParameters.h"
#include "GlobalParameters.h"
#include "AnimationParameters.h"
#include "Simulation.h"

#define IN_LINE true
#ifdef PAPI_LL
#include "Papi.h"
#elif PAPI_HL
#include <papi.h>
#endif //PAPI_LL
// If PAPI_LL or PAPI_HL are defined, the PAPI library will be used
#if defined(PAPI_LL) || defined(PAPI_HL)
void handle_papi_error(int retval) {
  printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
  exit(1);
}
#endif
#ifdef ANIMATION
#include "Animation.h"
#endif //ANIMATION
       
// The simulation precision
// Animation only supports float
#define TYPE float 

std::string get_omp_proc_bind_string(omp_proc_bind_t bind) {
  switch (bind) {
    case omp_proc_bind_false:
      return "false";
    case omp_proc_bind_true:
      return "true";
    case omp_proc_bind_primary:
      return "primary (former master)";
    case omp_proc_bind_close:
      return "close";
    case omp_proc_bind_spread:
      return "spread";
    default:
      return "unknown";
  }
}
       
int main(int argc, char **argv) {

  /* INITIALIZATION */

  std::cout<<"--- Initialization ---"<<std::endl;

#ifdef DEBUG
  std::cout<<"<<<DEBUG ON>>>"<<std::endl;
#endif

#ifdef PAPI_LL
  std::cout<<"<<<PAPI_LL ON>>>"<<std::endl;
#endif

#ifdef PARALLEL 
  std::cout<<"<<<PARALLEL ON>>>"<<std::endl;
#ifdef DEBUG
  std::cout << "Number of threads used:\t"<<omp_get_max_threads()<<std::endl;
#endif //DEBUG
#endif //PARALLEL

#ifdef ANIMATION
  std::cout<<"<<<ANIMATION ON>>>"<<std::endl;
#ifdef DEBUG
  printf("Compiled against GLFW %i.%i.%i\n",
         GLFW_VERSION_MAJOR,
         GLFW_VERSION_MINOR,
         GLFW_VERSION_REVISION);

  int major, minor, revision;
  glfwGetVersion(&major, &minor, &revision);
  printf("Running against GLFW %i.%i.%i\n", major, minor, revision);
#endif //DEBUG
#endif //ANIMATION
       
#ifdef PAPI_LL
  /* Initialize PAPI_LL library */
  Papi papi;
#endif

  /* Initialize parameters */
  SimulationParameters<TYPE> simulation_parameters;
  AnimationParameters<TYPE> animation_parameters;
  GlobalParameters<TYPE> global_parameters(argc, argv, 
#ifdef ANIMATION
      animation_parameters,
#endif
      simulation_parameters);
  simulation_parameters.countAgents();

  /* Initialize state machine*/
  std::random_device rd;
  StateMachine<TYPE> state_machine(rd, global_parameters);

  /* Initialize simulation (handle)*/
  Simulation<TYPE> simulation(state_machine, global_parameters);


  /* Initialize results */
  //Results<TYPE> r(state_machine);


  /* SIMULATION */

  std::cout<<"--- Simulation ---"<<std::endl;


#ifdef PAPI_LL
  papi.start();
#elif PAPI_HL
  int retval;
  retval = PAPI_hl_region_begin("main");
  if (retval != PAPI_OK) handle_papi_error(retval);
#endif


  /* main simulation loop */

#ifdef ANIMATION
  if (global_parameters.getGuiFlag()) {
#pragma omp parallel sections num_threads(2) 
    {
#pragma omp section
      {
        Animation<TYPE> animation(simulation, state_machine, global_parameters);
#ifdef DEBUG
        omp_proc_bind_t bind = omp_get_proc_bind();
        std::string bind_str = get_omp_proc_bind_string(bind);
        std::cout << "Animation thread binding policy: " 
          << bind_str << std::endl;
#endif
        animation.runGui();
      }
#pragma omp section 
      {
#ifdef DEBUG
        omp_proc_bind_t bind = omp_get_proc_bind();
        std::string bind_str = get_omp_proc_bind_string(bind);
        std::cout << "Simulation thread binding policy: " 
          << bind_str << std::endl;
#endif
        simulation.run();
      }
      
    }// end parallel region
  // Jump to results section, to not run the simulation loop headless again
  goto end;
  }  // end if gui
#endif 


  /* Headless simulation */
  // Run the simulation
  simulation.runHeadless();

  /* Stop PAPI measurements */
#ifdef PAPI_LL
  papi.stop();
#elif PAPI_HL
  retval = PAPI_hl_region_end("main");
  if (retval != PAPI_OK) handle_papi_error(retval); 
#endif

  end:
  std::cout<<"--- End of simulation ---"<<std::endl;


#ifdef PAPI_LL
  std::cout<<"--- PAPI_LL measurements ---"<<std::endl;
  papi.printValues();
#endif


  return 0;
}
/* vim: set ts=2 sw=2 tw=0 et :*/
