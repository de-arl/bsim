#ifndef BROWNIANSEARCH_H
#define BROWNIANSEARCH_H

#include <random>
#include "Point3D.h"
#include "State.h"

template <typename T>
class BrownianSearch
{
  
 public:

  // Constructor
  BrownianSearch(
      int id,
      std::mt19937 &gen, 
      std::uniform_real_distribution<T> &dist_x,
      std::uniform_real_distribution<T> &dist_y,
      std::uniform_real_distribution<T> &dist_z,
      T search_duration);

  // Destructor
  ~BrownianSearch();

  // Operator =
  BrownianSearch<T>& operator=(const BrownianSearch<T> &search);

  // Start new search
  void startNewSearch(const State<T> &state);

  // Find out if the search is over
  bool isFinished(const State<T> &state);

  // Get direction to the target
  Point3D<T> getDirection(const State<T> &state);

 private:
  // Uniform distribution
  std::uniform_real_distribution<T> &dist_x;
  std::uniform_real_distribution<T> &dist_y;
  std::uniform_real_distribution<T> &dist_z;

  // Random number generator
  std::mt19937 &gen;

  // Start time of the search
  double start_time;

  // Search duration
  double search_duration;

  // Agent ID
  int id;

};

#endif
/* vim: set ts=2 sw=2 tw=0 et :*/
