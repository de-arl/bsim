#ifndef LEVYSEARCH_H
#define LEVYSEARCH_H

#include <random>
#include "Point3D.h"
#include "State.h"
#include "asa241.hpp"

template <typename T>
class LevySearch
{
  
 public:

  // Constructor
  LevySearch(
      int id,
      std::mt19937 &gen, 
      std::uniform_real_distribution<T> &dist_uni,
      std::uniform_real_distribution<T> &dist_x,
      std::uniform_real_distribution<T> &dist_y,
      std::uniform_real_distribution<T> &dist_z,
      T mu_levy, T c_levy,
      const T worldsize[3]);

  // Destructor
  ~LevySearch();

  // Operator =
  LevySearch<T>& operator=(const LevySearch<T> &search);

  // Start new search
  void startNewSearch(const State<T> &state);

  // Find out if the search is finished
  bool isFinished(const State<T> &state);

  // Get direction to the target
  Point3D<T> getDirection(const State<T> &state);

 private:
  // Uniform distribution
  std::uniform_real_distribution<T> &dist_uni;
  std::uniform_real_distribution<T> &dist_x;
  std::uniform_real_distribution<T> &dist_y;
  std::uniform_real_distribution<T> &dist_z;

  // Random number generator
  std::mt19937 &gen;

  // Parameters of the Levy distribution
  T mu_levy;
  T c_levy;

  // Target position
  Point3D<T> target;

  // Agent ID
  int id;

  // Worldsize
  T worldsize[3];


  /* Private methods */

  // Draw a random number from a Levy distribution
  T rand();

  // Test if a point is within the world
  bool isWithinWorld(const Point3D<T> &point);

  // Draw random target
  void drawTarget();

};

#endif // LEVYSEARCH_H
/* vim: set ts=2 sw=2 tw=0 et :*/
