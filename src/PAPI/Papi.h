#ifndef Papi_H
#define Papi_H
#include <papi.h> // see https://github.com/icl-utk-edu/papi/wiki
//#include "papi.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <ios>
#include <stdio.h>
#include <stdlib.h>
class Papi
{
 private: 
  /* Private member variables */
  int start_usec;
  int end_usec;
  int EventSet;
  int OptEventSet;
  int retval;
  long long values[10];
  long long opt_values[10];
  bool event_added[10] = 
  {false, false, false, false, false, false, false, false, false, false};

  /* Private member functions*/
  void handle_error(int error);
  void handle_event_error(std::string event);
  void createEventSet();
  void init();
  void clean();

 public:
  /* Public member functions*/
  Papi();
  ~Papi();
  void printValues();
  void start();
  void stop();
};
#endif //Papi_H
