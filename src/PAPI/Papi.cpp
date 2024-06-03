#include "Papi.h"

void Papi::handle_error(int error)
{
  printf("PAPI: error %d: %s\n", error, PAPI_strerror(error));
  exit(1);
}

void Papi::createEventSet() 
{
  // Make sure array size of values matches number of events
  EventSet = PAPI_NULL;
  retval = PAPI_create_eventset(&EventSet);
  if (retval != PAPI_OK) handle_error(retval);

  // Add events to event set
  retval = PAPI_add_event(EventSet, PAPI_TOT_INS);
  if (retval != PAPI_OK) handle_error(retval);
  event_added[0] = true;

  retval = PAPI_add_event(EventSet, PAPI_TOT_CYC);
  if (retval != PAPI_OK) handle_error(retval);
  event_added[1] = true;

  /* do not forget to adjust the indices if you use the commented out events
   * these are commented out because they cannot be used together with the cache misses
  retval = PAPI_add_event(EventSet, PAPI_SP_OPS);
  if (retval != PAPI_OK) handle_error(retval);
  event_added[2] = true;

  retval = PAPI_add_event(EventSet, PAPI_VEC_SP);
  if (retval != PAPI_OK) handle_error(retval);
  event_added[3] = true;
  */

  // Add L1 cache misses
  retval = PAPI_add_event(EventSet, PAPI_L1_DCM);
  if (retval != PAPI_OK) handle_error(retval);
  event_added[2] = true;

  // Add L2 cache misses
  retval = PAPI_add_event(EventSet, PAPI_L2_DCM);
  if (retval != PAPI_OK) handle_error(retval);
  event_added[3] = true;

  // Add L3 cache misses
  retval = PAPI_add_event(EventSet, PAPI_L3_TCM);
  if (retval != PAPI_OK) handle_error(retval);
  event_added[4] = true;
  
}

void Papi::init()
{
  int retval;
  int version;
  retval = PAPI_library_init(PAPI_VER_CURRENT);
  if (retval!=PAPI_VER_CURRENT) {
    handle_error(retval);
  }

  // Print PAPI version
  fprintf(stdout, "PAPI: PAPI Version: %d.%d.%d\n", 
      PAPI_VERSION_MAJOR(PAPI_VERSION), 
      PAPI_VERSION_MINOR(PAPI_VERSION), 
      PAPI_VERSION_REVISION(PAPI_VERSION));
  createEventSet();
}


void Papi::clean()
{
  retval = PAPI_cleanup_eventset(EventSet);
  if (retval != PAPI_OK) handle_error(retval);
  retval = PAPI_destroy_eventset(&EventSet);
  if (retval != PAPI_OK) handle_error(retval);
}

Papi::Papi()
{
  init();
}

Papi::~Papi()
{
  clean();
}

void Papi::printValues()
{
  double wall_time = (double)(end_usec - start_usec) / 1000000.0;
  double ipc = (double)values[0]/(double)values[1];
  double flops = (double)values[2]/wall_time;

  std::cout
    <<std::fixed<<std::setprecision(6)<<std::setfill(' ')
    <<std::left<<"PAPI: Wall clock in seconds:  "
    <<std::right<<std::setw(10) << wall_time <<std::endl
    <<std::left<<"PAPI: Instructions per cycle: "
    <<std::right<<std::setw(10)<< ipc <<std::endl
    <<std::left<<"PAPI: MFLOPS:                 "
    <<std::fixed<<std::setprecision(1)<<std::setfill(' ')
    <<std::right<<std::setw(10)<<flops/1000000<<std::endl

    <<std::left<<"PAPI: PAPI_TOT_INS"
    <<std::right<<std::setw(22)<<values[0]<<std::endl
    <<std::left<<"PAPI: PAPI_TOT_CYC"
    <<std::right<<std::setw(22)<<values[1]<<std::endl
    
    /* do not forget to adjust the indices if you use the commented out events
    <<std::left<<"PAPI: PAPI_FP_OPS "
    <<std::right<<std::setw(22)<<values[2]<<std::endl
    <<std::left<<"PAPI: PAPI_VEC_SP "
    <<std::right<<std::setw(22)<<values[2]<<std::endl
    */
    <<std::left<<"PAPI: PAPI_L1_DCM "
    <<std::right<<std::setw(22)<<values[2]<<std::endl
    <<std::left<<"PAPI: PAPI_L2_DCM "
    <<std::right<<std::setw(22)<<values[3]<<std::endl
    <<std::left<<"PAPI: PAPI_L3_TCM "
    <<std::right<<std::setw(22)<<values[4]
  <<std::endl;
}

void Papi::start()
{
  retval = PAPI_start(EventSet);
  if (retval != PAPI_OK) handle_error(retval);
  start_usec = PAPI_get_real_usec();
}

void Papi::stop()
{
  retval = PAPI_stop(EventSet, values);
  if (retval != PAPI_OK) handle_error(retval);
  end_usec = PAPI_get_real_usec();
}
