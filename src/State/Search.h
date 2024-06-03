#ifndef SEARCH_H
#define SEARCH_H

#include "Point3D.h"
#include "LevySearch.h"
#include "BrownianSearch.h"
#include "State.h"
#include "SimulationParameters.h"
#include "helpers.h"

#include <random>

enum SearchType
{
  LEVY,
  BROWNIAN,
  NUM_SEARCH_TYPES
};

template <typename T>
class Search
{
  
 public:

  // Constructor
  Search(
      int id,
      std::mt19937 &gen, 
      std::uniform_real_distribution<T> &dist_uni,
      std::uniform_real_distribution<T> &dist_x,
      std::uniform_real_distribution<T> &dist_y,
      std::uniform_real_distribution<T> &dist_z,
      T mu_levy, T c_levy, T brownian_search_duration,
      const T worldsize[3]);

  // Destructor
  ~Search();

  // Operator =
  Search<T>& operator=(const Search<T> &search);

  // Determine the search type
  void determineSearchType(const State<T> &state);

  // Get a search direction, this is the main function, 
  // calling determineSearchType and getDirection subsequently
  // will result alternatingly in a levy flight followed by a brownian search
  // if brownian_search_duration is passed, a levy step will be calculated
  // and the next brownian search will start after the levy step is finished
  // Returns a normalized direction vector of lenght 1.
  Point3D<T> getDirection(const State<T> &state);


  // Set search type
  void setSearchType(SearchType search_type);

 private:
  // Uniform distribution
  std::uniform_real_distribution<T> &dist_uni;
  std::uniform_real_distribution<T> &dist_x;
  std::uniform_real_distribution<T> &dist_y;
  std::uniform_real_distribution<T> &dist_z;

  // Random number generator
  std::mt19937 &gen;

  // Levy search
  LevySearch<T> levy_search;

  // Brownian search
  BrownianSearch<T> brownian_search;
  T brownian_search_duration;

  // Search type
  SearchType search_type;

  // Agent ID
  int id;



  /* Private methods */

};

#endif
/* vim: set ts=2 sw=2 tw=0 et :*/
