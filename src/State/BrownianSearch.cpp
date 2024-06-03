#include "BrownianSearch.h"

template <typename T>
BrownianSearch<T>::BrownianSearch(
    int id,
    std::mt19937 &gen, 
    std::uniform_real_distribution<T> &dist_x,
    std::uniform_real_distribution<T> &dist_y,
    std::uniform_real_distribution<T> &dist_z,
    T search_duration)
  : id(id), gen(gen), 
  dist_x(dist_x), dist_y(dist_y), dist_z(dist_z), start_time(0.0),
  search_duration(search_duration)
{
#ifdef DEBUG
  /*
  std::cout 
    << "BrownianSearch::BrownianSearch: Creating a Brownian search object" 
    << std::endl;
    */
#endif
}

template <typename T>
BrownianSearch<T>::~BrownianSearch()
{
#ifdef DEBUG
  /*
  std::cout 
    << "BrownianSearch::~BrownianSearch: Destroying a Brownian search object" 
    << std::endl;
    */
#endif
}

template <typename T>
BrownianSearch<T>& BrownianSearch<T>::operator=(const BrownianSearch<T> &search)
{
#ifdef DEBUG
  /*
  std::cout 
    << "BrownianSearch::operator=: Assigning a Brownian search object" 
    << std::endl;
    */
#endif

  // Check for self-assignment
  if (this == &search){
    return *this;
  }

  // Assign the search object
  gen = search.gen;
  dist_x = search.dist_x;
  dist_y = search.dist_y;
  dist_z = search.dist_z;
  start_time = search.start_time;
  search_duration = search.search_duration;

  return *this;
}

template <typename T>
void BrownianSearch<T>::startNewSearch(const State<T> &state)
{
#ifdef DEBUG
  /*
  std::cout 
    << "BrownianSearch::startNewSearch: Starting a new Brownian search" 
    << std::endl;
    */
#endif

  // Set the start time of the search
  start_time = state.getTimestamp();
}

template <typename T>
bool BrownianSearch<T>::isFinished(const State<T> &state)
{

  T time_passed = state.getTimestamp() - start_time;
  T time_left = search_duration - time_passed;
#ifdef DEBUG
  /*
  std::cout 
    << "BrownianSearch::isFinished: Checking if the search is over" 
    << std::endl;
  std::cout 
    << "BrownianSearch::isFinished: Time passed: " 
    << time_passed 
    << std::endl;
  std::cout
    << "BrownianSearch::isFinished: Time left: "
    << time_left
    << std::endl;
    */
#endif

  // Check if the search duration has passed
  if (time_passed > search_duration){
    return true;
  }else{
    return false;
  }
}

template <typename T>
Point3D<T> BrownianSearch<T>::getDirection(const State<T> &state)
{
#ifdef DEBUG
  /*
  std::cout 
    << "BrownianSearch::getDirection: Getting a search direction" 
    << std::endl;
    */
#endif

  // Draw a random direction
  Point3D<T> direction;
  direction.x = dist_x(gen);
  direction.y = dist_y(gen);
  direction.z = dist_z(gen);

  direction.normalize();

  return direction;
}

// Explicit instantiation
template class BrownianSearch<float>;
template class BrownianSearch<double>;
  
/* vim: set ts=2 sw=2 tw=0 et :*/
